#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"

#include "sllist.h"

USE_XASSERT;

int DEBUG = 0;
bool ERR  = false;


/*
 * user defined assertion failure report
 */
void
report_xassert(char *filename, int line)
{
	printf(" ** xassert: %s-%d ", filename, line);
	ERR = true;
	if (DEBUG) {
		printf("(Press Enter) ");
		while ('\n' != getchar()) {
			/* empty */
		}
	} else {
		printf("\n");
	}
}

int
main()
{
	int testnum = 10;

	sllist_t sll = 0;
	LOOP(testnum) {
		sll = sll_insert_data(sll, (void*)loop);
	} ENDLOOP;

	sllist_t list = sll;
	while (list) {
		printf("data: %d\n", sll_data(list));
		list = sll_next(list);
	}

	LOOP(testnum) {
		sll = sll_remove_data(sll, (void*)loop);
		xassert(sll);
	} ENDLOOP;

	xwalkheap();

	if (ERR)
		return EXIT_FAILURE;

	printf("\nAll tests passed\n");
	return EXIT_SUCCESS;
}
