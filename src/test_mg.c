#include <stdlib.h>
#include <stdio.h>

#include "multi-gee.h"
#include "mg_frame.h"
#include "xmalloc.h"

int DEBUG=1;

/*
 * user defined assertion failure report
 */
void
report_xassert(char *filename, int line)
{
	printf("xassert: %s-%d (Press Enter) ", filename, line);
	if (DEBUG)
		while ('\n' != getchar()) {
			/* empty */
		}
}

int
main()
{
	multi_gee_t mg;
	mg = mg_create();

	mg = mg_destroy(mg);
	mg = mg_destroy(mg);

	xwalkheap();
	return EXIT_SUCCESS;
}
