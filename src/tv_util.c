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
	struct timeval tv = {0, 0};

	if (tv_eq(tv_0, tv_1))
		return tv;

	if (tv_lt(tv_0, tv_1)) {
		tv.tv_sec = tv_1.tv_sec - tv_0.tv_sec;
		tv.tv_usec = tv_1.tv_usec - tv_0.tv_usec;
	} else {
		tv.tv_sec = tv_0.tv_sec - tv_1.tv_sec;
		tv.tv_usec = tv_0.tv_usec - tv_1.tv_usec;
	}

	if (tv.tv_usec < 0) {
		tv.tv_sec--;
		tv.tv_usec += 1000000;
	}

	return tv;
}

