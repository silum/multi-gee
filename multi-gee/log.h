/* $Id$ */
/*
 * logging object
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_LOG_H
#define DSM_LOG_H 1

#include <multi-gee/classdef.h>

__BEGIN_DECLS

NEWHANDLE(log_t);

/**
 * @brief create log object
 *
 * @param string  programme/system name, to appear in logfile
 * @param file    logfile name,
 *                - can be 0 for no log file,
 *                - "stdout" for standard output stream
 *                - "stderr" for standard error stream
 *                - a file name: logs will be appended to this file
 *
 * @return a newly created log object handle
 */
log_t
lg_create(const char *,
	  const char *);

/**
 * @brief destroy log object
 *
 * @param log  handle of object to be destroyed
 *
 * @return 0
 */
log_t
lg_destroy(log_t);

/**
 * @brief write a log entry
 *
 * @param log  the log object
 * @param format  a printf like format
 * @param ...  arguments for the format string
 */
void
lg_log(log_t,
       const char*,
       ...);

/**
 * @brief write a log entry for the current errno
 *
 * @param log  the log object
 * @param string  description of error
 */
void
lg_errno(log_t,
	 const char*,
	 ...);

__END_DECLS

#endif /* ndef DSM_LOG_H */
