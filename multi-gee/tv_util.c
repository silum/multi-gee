/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
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

#include "classdef.h"
#include "debug_xassert.h"

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
