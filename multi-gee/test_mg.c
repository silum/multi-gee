#include <stdlib.h>
#include <stdio.h>

#include "multi-gee.h"
#include "mg_frame.h"
#include "xmalloc.h"

#include "debug_xassert.h"

int DEBUG=1;

void
mg()
{
	multi_gee_t mg;
	mg = mg_create();

	/* test double destroy */
	mg = mg_destroy(mg);
	mg = mg_destroy(mg);
}

int
main()
{
	return debug_test(mg);
}

