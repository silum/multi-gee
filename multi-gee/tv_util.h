/* $Id$ */
/*
 * timeval structure utility functions
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_TV_UTIL_H
#define DSM_TV_UTIL_H 1

#include <stdbool.h>
#include <sys/time.h>

__BEGIN_DECLS

/**
 * @brief test equality between to timeval structures
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
 * @brief strict less than test between to timeval structures
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
 * @brief finds the absolute difference between two timeval structures
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

#endif /* ndef DSM_TV_UTIL_H */
