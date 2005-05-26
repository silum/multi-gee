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
 * @file tv_util.h
 * @brief Timeval structure utility declaration
 */

#ifndef DSM_TV_UTIL_H
#define DSM_TV_UTIL_H

#include <sys/time.h> /* struct timeval */

__BEGIN_DECLS

#define timerset(tvp, sec, usec) \
	do { (tvp)->tv_sec = sec;  (tvp)->tv_usec = usec; } while (0)

#define timerabs(tvp) \
	do { \
		if ((tvp)->tv_sec < 0) (tvp)->tv_sec = -(tvp)->tv_sec; \
		if ((tvp)->tv_usec < 0) (tvp)->tv_usec = -(tvp)->tv_usec; \
	} while (0)

#define timernorm(tvp) \
	do { \
		struct timeval zero; \
		timerclear(&zero); \
		while ((tvp)->tv_usec < 0) \
			timersub(tvp, &zero, tvp); \
		while ((tvp)->tv_usec > 1000000) \
			timeradd(tvp, &zero, tvp); \
	} while (0)

__END_DECLS

#endif /* DSM_TV_UTIL_H */
