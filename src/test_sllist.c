#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include "debug_xassert.h"

#include "sllist.h"

USE_XASSERT;

void
sllist()
{
	int testnum = 15;

	/*-forward-*/
	{
		sllist_t sll = 0;
		for (int i = 0; i <= testnum; ++i) {
			sll = sll_insert_data(sll, (void*)i);
		}

		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sll_data(list));
			list = sll_next(list);
		}

		for (int i = 0; i <= testnum; ++i) {
			sll = sll_remove_data(sll, (void*)i);
			if (i != testnum) {
				XASSERT(sll);
			} else {
				XASSERT(sll == 0);
			}
		}
	}

	/*-reverse-*/
	{
		sllist_t sll = 0;
		for (int i = 0; i <= testnum; ++i) {
			sll = sll_insert_data(sll, (void*)i);
		}

		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sll_data(list));
			list = sll_next(list);
		}

		for (int i = testnum; i >= 0; --i) {
			sll = sll_remove_data(sll, (void*)i);
			if (i) {
				XASSERT(sll);
			} else {
				XASSERT(sll == 0);
			}
		}
	}
}


int
main()
{
	return debug_test(sllist);
}

