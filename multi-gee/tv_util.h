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
 * @brief timeval structure utility function declarations
 */

#ifndef DSM_TV_UTIL_H
#define DSM_TV_UTIL_H

#include <stdbool.h> /* bool */
#include <sys/time.h> /* struct timeval */

__BEGIN_DECLS

/**
 * @brief Test equality between to timeval structures
 *
 * @param tv_0  a timeval
 * @param tv_1  another timeval
 *
 * @returns true if tv_0 == tv_1, otherwise returns false
 */
bool
tv_eq(struct timeval tv_0,
      struct timeval tv_1);

/**
 * @brief Strict less than test between to timeval structures
 *
 * @param tv_0  a timeval
 * @param tv_1  another timeval
 *
 * @returns true if tv_0 < tv_1, otherwise returns false
 */
bool
tv_lt(struct timeval tv_0,
      struct timeval tv_1);

/**
 * @brief Finds the absolute difference between two timeval structures
 *
 * @param tv_0  a timeval
 * @param tv_1  another timeval
 *
 * @returns the absolute difference as a timeval
 */
struct timeval
tv_abs_diff(struct timeval tv_0,
	    struct timeval tv_1);

__END_DECLS

#endif /* DSM_TV_UTIL_H */
