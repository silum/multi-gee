#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include "debug_xassert.h"

#include "sllist.h"

USE_XASSERT;

void
sllist()
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
}


int
main()
{
	return debug_test(sllist);
}

