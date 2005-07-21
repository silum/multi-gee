/* $Id$
 * Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file
 * @brief Logging object definition
 */
#include <cclass/xmalloc.h>
#include <errno.h>
#include <stdarg.h> /* vfprintf */
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "log.h" /* declarations implemented */

USE_XASSERT

/**
 * @brief Open log file
 *
 * open named log file.  if name is \c stdout, or \c stderr the output
 * is directed onto the named stream.  if the file name pointer is \e
 * NULL log output goes to \c /dev/null
 *
 * @param log  object handle
 * @param file  log file name, can be 0, "stdout", "stderr" or a file
 * name.  Output is appended to the log file.  If the log file cannot be
 * opened output defaults to \c stderr, and the user is informed.
 */
static void
open_log(log_t log,
	 const char *file);

/**
 * @brief Output the header string to file
 *
 * the header string is the name, followed by a colon, the date and the
 * time, followed by a colon
 *
 * @param file  the output file pointer
 * @param name  the name to print
 */
static void
put_header(FILE *file,
	   char *name);

/**
 * @brief Prints the current date and time to buffer
 *
 * @param buffer  the destination
 * @param size  the length of the buffer
 */
static void
put_time(char *buffer,
	 size_t size);

/**
 * @brief Log object structure
 */
CLASS(log, log_t) {
	char *name; /**< Log file name */
	FILE *file; /**< Log file object pointer */
	bool close; /**< Close the file when log object is destroyed? */
};

log_t
lg_create(const char *name,
	  const char *file)
{
	log_t log;

	NEWOBJ(log);

	log->name = xstrdup(name, __FILE__, __LINE__);
	log->file = 0;
	log->close = true;

	open_log(log, file);

	return log;
}

log_t
lg_destroy(log_t log)
{
	VERIFYZ(log) {
		xfree(log->name);
		if (log->close)
			fclose(log->file);

		FREEOBJ(log);
	}

	return 0;
}

void
lg_errno(log_t log,
	 const char *format,
	 ...)
{
	VERIFYZ(log) {
		va_list ap;
		va_start(ap, format);

		FILE* out = stderr;
		if (log->file) {
			out = log->file;
			put_header(out, log->name);
		}

		vfprintf(out, format, ap);
		fprintf(out, " error %d, %s\n", errno, strerror(errno));
		fflush(out);

		va_end(ap);
	}
}

void
lg_log(log_t log,
       const char *format,
       ...)
{
	VERIFYZ(log) {
		va_list ap;
		va_start(ap, format);

		FILE* out = stderr;
		if (log->file) {
			out = log->file;
			put_header(out, log->name);
		}

		vfprintf(out, format, ap);
		fprintf(out, "\n");
		fflush(out);

		va_end(ap);
	}
}

void
open_log(log_t log,
	 const char *file)
{
	VERIFY(log) {
		if (log->file
		    && log->close) {
			fclose(log->file);
			log->file = 0;
		}

		if (!file) {
			log->file = 0;
			return;
		}

		if (0 == strcmp(file, "stderr")) {
			log->file = stderr;
			log->close = false;
			return;
		}

		if (0 == strcmp(file, "stdout")) {
			log->file = stdout;
			log->close = false;
			return;
		}

		log->file = fopen(file, "a");
		log->close = true;
		if (!log->file) {
			fprintf(stderr, "could not open %s for appending, using"
				" `stderr' instead\n", file);
			log->file = stderr;
			log->close = false;
		}
	}
}

void
put_header(FILE *file,
	   char *name)
{
	char tm_buf[30];
	put_time(tm_buf, sizeof(tm_buf));

	fprintf(file, "%s: %s: ", name, tm_buf);
}

void
put_time(char *buffer,
	 size_t size)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	strftime(buffer, size, "%F %H:%M:%S", localtime(&tv.tv_sec));
}

