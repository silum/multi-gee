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
 * @file tv_util.c
 * @brief Timeval structure utility definition
 */

#include "tv_util.h"

bool
tv_eq(struct timeval tv_0,
      struct timeval tv_1)
{
	return (tv_0.tv_sec == tv_1.tv_sec
		&& tv_0.tv_usec == tv_1.tv_usec);
}

bool
tv_lt(struct timeval tv_0,
      struct timeval tv_1)
{
	if (tv_0.tv_sec == tv_1.tv_sec) {
		if (tv_0.tv_usec < tv_1.tv_usec)
			return true;
		else
			return false;
	} else if (tv_0.tv_sec < tv_1.tv_sec)
		return true;
	else /* tv_1.tv_sec < tv_0.tv_sec */
		return false;
}

struct timeval
tv_abs_diff(struct timeval tv_0,
	    struct timeval tv_1)
{
	struct timeval result = {0, 0};

	if (tv_eq(tv_0, tv_1))
		return result;

	if (tv_lt(tv_0, tv_1)) {
		result.tv_sec = tv_1.tv_sec - tv_0.tv_sec;
		result.tv_usec = tv_1.tv_usec - tv_0.tv_usec;
	} else {
		result.tv_sec = tv_0.tv_sec - tv_1.tv_sec;
		result.tv_usec = tv_0.tv_usec - tv_1.tv_usec;
	}

	if (result.tv_usec < 0) {
		result.tv_sec--;
		result.tv_usec += 1000000;
	}

	return result;
}

#ifdef DEBUG_TV_UTIL

#include <stdio.h>

#include <xassert.h>
#include <xmalloc.h>

USE_XASSERT;

void
test_tv_util()
{
	struct timeval result;
	struct timeval zero = {0, 0};
	struct timeval expect = {0, 20};
	struct timeval tv_sml = {0, 40};
	struct timeval tv_lrg = {0, 60};

	printf("sml = %ld.%06ld\n", tv_sml.tv_sec, tv_sml.tv_usec);
	printf("lrg = %ld.%06ld\n", tv_lrg.tv_sec, tv_lrg.tv_usec);

	printf("test: less than\n");
	XASSERT(tv_lt(tv_sml, tv_lrg));

	printf("test: equality\n");
	XASSERT(tv_eq(zero, zero));
	XASSERT(tv_eq(tv_sml, tv_sml));

	result = tv_abs_diff(tv_sml, tv_lrg);
	printf("test: tv_abs_diff(sml, lrg)\n");
	printf("|sml - lrg| = %ld.%06ld\n", result.tv_sec, result.tv_usec);
	XASSERT(tv_eq(result, expect));

	result = tv_abs_diff(tv_lrg, tv_sml);
	printf("test: tv_abs_diff(lrg, sml)\n");
	printf("|lrg - sml| = %ld.%06ld\n", result.tv_sec, result.tv_usec);
	XASSERT(tv_eq(result, expect));

	result = tv_abs_diff(tv_lrg, tv_lrg);
	printf("test: tv_abs_diff(lrg, lrg)\n");
	printf("|lrg - lrg| = %ld.%06ld\n", result.tv_sec, result.tv_usec);
	XASSERT(tv_eq(result, zero));

	result = tv_abs_diff(tv_sml, tv_sml);
	printf("test: tv_abs_diff(sml, sml)\n");
	printf("|sml - sml| = %ld.%06ld\n", result.tv_sec, result.tv_usec);
	XASSERT(tv_eq(result, zero));
}

int
main()
{
	return debug_test(test_tv_util);
}

#endif /* def DEBUG_TV_UTIL */
