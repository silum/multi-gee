
#include <stdarg.h>    /* vfprintf */
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "log.h"
#include "xmalloc.h"

USE_XASSERT;

CLASS(log, log_t) {
	char *name;
	FILE *file;
};

static FILE *
open_log(const char *file);

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
		if (log->file) {
			struct timeval tv;
			gettimeofday(&tv, 0);

			char tm_buf[30];
			strftime(tm_buf, sizeof(tm_buf), "%F %H:%M:%S", localtime(&tv.tv_sec));

			fprintf(log->file, "%s: %s: ", log->name, tm_buf);
			vfprintf(log->file, format, ap);
			fprintf(log->file, "\n");
			fflush(log->file);
		}
		else
			vfprintf(stderr, format, ap);

		va_end(ap);
	}
}

/**
 * @brief print error message corresponding to errno
 */
void
lg_errno(log_t log, const char *s)
{
	lg_log(log, "%s error %d, %s", s, errno, strerror(errno));
}

FILE*
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

