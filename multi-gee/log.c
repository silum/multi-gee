
#include <stdarg.h>    /* vfprintf */
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "log.h"
#include "xmalloc.h"

USE_XASSERT;

/**
 * @brief open log file
 *
 * open named log file.  if name is stdout, or stderr the output is
 * directed onto the named stream.  if the file name pointer is NULL log
 * output goes to /dev/null
 *
 * @param file  log file name, can be 0, "stdout", "stderr" or a file
 * name.  output is appened to the logfile.  if the log file cannot be
 * opened output defaults to stderr, and the user is informed.
 *
 * @return file pointer
 */
static FILE *
open_log(const char *);

/**
 * @brief prints the current date and time to buffer
 *
 * @param buffer  the destination
 * @param size  the length of the buffer
 */
static void
put_time(char *,
	 size_t);

/**
 * @brief output the header string to file
 *
 * the header string is the name, followed by a colon, the date and the
 * time, followed by a colon
 *
 * @param file  the output file pointer
 * @param name  the name to print
 */
static void
put_header(FILE *,
	   char *);

CLASS(log, log_t) {
	char *name;
	FILE *file;
};

log_t
lg_create(const char *name,
	  const char *file)
{
	log_t log;

	NEWOBJ(log);

	log->name = xstrdup(name, __FILE__, __LINE__);
	log->file = open_log(file);

	return log;
}

log_t
lg_destroy(log_t log)
{
	VERIFYZ(log) {
		xfree(log->name);
		fclose(log->file);

		FREEOBJ(log);
	}

	return 0;
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

/**
 * @brief print error message corresponding to errno
 */
void
lg_errno(log_t log, const char *format, ...)
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

static FILE*
open_log(const char *file)
{
	FILE* log = 0;

	if (!file)
		return 0;

	if (0 == strcmp(file, "stderr"))
		return stderr;

	if (0 == strcmp(file, "stdout"))
		return stdout;

	log = fopen(file, "a");
	if (!log) {
		fprintf(stderr, "could not open %s for appending, using"
			" `stderr' instead\n", file);
		log = stderr;
	}

	return log;
}

static void
put_time(char *buffer,
	 size_t size)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	strftime(buffer, size, "%F %H:%M:%S", localtime(&tv.tv_sec));
}

static void
put_header(FILE *file,
	   char *name)
{
	char tm_buf[30];
	put_time(tm_buf, sizeof(tm_buf));

	fprintf(file, "%s: %s: ", name, tm_buf);
}

