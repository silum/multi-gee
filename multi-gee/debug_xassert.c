#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "debug_xassert.h"
#include "xmalloc.h"

USE_XASSERT;

bool XASSERT_INTERACTIVE = true;
bool XASSERT_FAILURE = false;

void
report_xassert(const char *file_name, int line)
{
	printf(" ** xassert: %s-%d ", file_name, line);
	XASSERT_FAILURE = true;
	if (XASSERT_INTERACTIVE) {
		printf("(Press Enter) ");
		while ('\n' != getchar()) {
			/* empty */
		}
	} else {
		printf("\n");
	}
}

int
debug_test(void (*test_func)())
{
	XASSERT_FAILURE = false;
	XASSERT_INTERACTIVE = false;

	test_func();

	XASSERT(xwalkheap() == 0);

	if (XASSERT_FAILURE)
		return EXIT_FAILURE;

	printf("\nAll tests passed\n");
	return EXIT_SUCCESS;
}

