#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "debug_xassert.h"
#include "xmalloc.h"

USE_XASSERT;

bool XASSERT_INTERACTIVE = true;
bool XASSERT_FAILURE = false;

/*
 * user defined assertion failure report
 *
 * sets XASSERT_FAILURE true when called, prompts for user input when
 * XASSERT_INTERACTIVE is true
 */
void
report_xassert(char *file_name, int line)
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

/*
 * debug test programme framework function
 *
 * calls the user supplied test function and checks for assertion fail
 * and memory leaks
 *
 * returns
 *   EXIT_SUCCESS if everything is ok,
 *   EXIT_FAILURE if an assertion failed or a memory leak was detected
 */
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

