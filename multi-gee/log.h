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
 *
 *
 * logging object
 */

#ifndef DSM_LOG_H
#define DSM_LOG_H 1

#include <classdef.h>

__BEGIN_DECLS

/**
 * @class log_t multi-gee/log.h
 * @brief log file writer
 */
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
lg_create(const char *string,
	  const char *file);

/**
 * @brief destroy log object
 *
 * @param log  handle of object to be destroyed
 *
 * @return 0
 */
log_t
lg_destroy(log_t log);

/**
 * @brief write a log entry
 *
 * @param log  the log object
 * @param format  a printf like format log message
 * @param ...  arguments for the format string
 */
void
lg_log(log_t log,
       const char *format,
       ...);

/**
 * @brief write a log entry for the current errno
 *
 * @param log  the log object
 * @param format  a printf like format description of error
 * @param ...  arguments for the format string
 */
void
lg_errno(log_t log,
	 const char *format,
	 ...);

__END_DECLS

#endif /* DSM_LOG_H */
