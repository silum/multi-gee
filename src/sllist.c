/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "sllist.h"
#include "xmalloc.h"
#include "debug_xassert.h"

USE_XASSERT;

/* set flag to true if sll can be verified */
#define VERIFY_SLL_FLAG(sll, flag) \
	bool flag = false; { sllist_t sllist = sll; VERIFY(sllist)  { flag = true; } }
#define VERIFYZ_SLL_FLAG(sll, flag) \
	bool flag = false; { sllist_t sllist = sll; VERIFYZ(sllist) { flag = true; } }

CLASS(sllist, sllist_t)
{
	void *data;
	sllist_t next;
};

/* create object */
static
sllist_t /* new handle */
sll_create(void *data);

/* destroy object */
static
sllist_t
sll_destroy(sllist_t /* object to destroy */);

/* insert into list after current position */
static
sllist_t
sll_insert(sllist_t /* current position */,
	   sllist_t /* item to insert */ );

/* remove object from list, but do not destroy object */
static
sllist_t
sll_remove(sllist_t,
	   sllist_t /* object to remove */ );

static
sllist_t
sll_create(void *data)
{
	sllist_t sllist;
	NEWOBJ(sllist);

	sllist->data = data;
	sllist->next = 0;

	return sllist;
}

static
sllist_t
sll_destroy(sllist_t sllist)
{
	VERIFYZ(sllist) {
		FREEOBJ(sllist);
	}

	return 0;
}

/*
 * if the list exists, insert item after first item in list, else item
 * becomes list
 */
static
sllist_t
sll_insert(sllist_t list_0,
	   sllist_t list_1)
{
	sllist_t sllist = 0;

	VERIFYZ_SLL_FLAG(list_0, flag_0);
	VERIFYZ_SLL_FLAG(list_1, flag_1);

	if (flag_0 && flag_1) {
		sllist_t l0_next = list_0->next; /* next item in list_0 */
		sllist_t l1_last = list_1; /* last item in list_1 */

		if (l1_last)
			while (l1_last->next)
				l1_last = l1_last->next;

		list_0->next = list_1;
		l1_last->next = l0_next;

		sllist = list_0;

	} else if (flag_0 && !flag_1) {
		sllist = list_0;
	} else if (!flag_0 && flag_1) {
		sllist = list_1;
	}

	return sllist;
}

static
sllist_t
sll_remove(sllist_t list,
	   sllist_t item)
{
	sllist_t sllist = 0;

	VERIFYZ_SLL_FLAG(list, list_f);
	VERIFYZ_SLL_FLAG(item, item_f);

	if (list_f)
		sllist = list;

	if (list_f && item_f) {
		if (list == item) {
			sllist = list->next;
			list->next = 0;
		} else while (list->next) {
			if (list->next == item) {
				sllist_t next = list->next->next;
				list->next->next = 0;
				list->next = next;
				break;
			}
			list = list->next;
		}
	}

	return sllist;
}

sllist_t
sll_insert_data(sllist_t sllist,
		void *data)
{
	sllist_t list = 0;

	VERIFYZ(sllist) {
		list = sllist;
	}

	sllist_t sll = sll_create(data);
	list = sll_insert(sll, list);

	return list;
}

sllist_t
sll_remove_data(sllist_t sllist,
		void *data)
{
	sllist_t list = 0;
	VERIFY(sllist) {
		sllist_t p;
		for (p = sllist;
		     p;
		     p = p->next) {
			if (p->data == data) {
				sll_remove(sllist, p);
				sll_destroy(p);
				break;
			}
		}
		list = sllist;
	}

	return list;
}

void *
sll_data(sllist_t sllist)
{
	VERIFY(sllist) {
		return sllist->data;
	}

	return 0;
}

sllist_t
sll_next(sllist_t sllist)
{
	VERIFY(sllist) {
		return sllist->next;
	}

	return 0;
}


sllist_t
sll_empty(sllist_t sllist)
{
	VERIFYZ(sllist) {
		while (sllist) {
			sllist = sll_remove(sllist, sllist);
		}
	}

	return 0;
}

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include "debug_xassert.h"

/*
 * test singly linked list
 *
 * see sllist_test.txt for meaning of test numbers
 */

void
sllist()
{
	/* 0.0 */
	printf("insert NULL into NULL list\n");
	sllist_t sll = sll_insert(0, 0);
	xassert(sll == 0);

	/* 0.1 */
	printf("insert item into NULL list\n");
	sllist_t item_1 = sll_create((void*)1);
	sll = sll_insert(0, item_1);
	xassert(sll == item_1
		&& !sll_next(sll));

	/* 0.3 */
	printf("insert NULL into 1 item list\n");
	sll = sll_insert(item_1, 0);
	xassert(sll == item_1
		&& !sll_next(sll));

	/* 0.5 */
	printf("insert item into 1 item list\n");
	sllist_t item_2 = sll_create((void*)2);
	sll = sll_insert(sll, item_2);
	xassert(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.7 */
	printf("insert item into 2 item list\n");
	sllist_t item_3 = sll_create((void*)3);
	sll = sll_insert(sll, item_3);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 0.2 */
	printf("insert list into NULL item\n");
	sll = sll_insert(0, sll);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 0.4 */
	printf("insert NULL item into list\n");
	sll = sll_insert(0, sll);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-a */
	printf("remove first item from 3 item list\n");
	sll = sll_remove(sll, item_1);
	xassert(sll == item_3
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.6 */
	printf("insert 2 item list into 1 item list\n");
	sll = sll_insert(item_1, sll);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-c */
	printf("remove last item from 3 item list\n");
	sll = sll_remove(sll, item_2);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& !sll_next(sll_next(sll)));

	/* 1.7-c */
	printf("remove last item from 2 item list\n");
	sll = sll_remove(sll, item_3);
	xassert(sll == item_1
		&& !sll_next(sll));

	/* 0.5 */
	printf("insert item into 1 item list\n");
	sll = sll_insert(sll, item_2);
	xassert(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.7 */
	printf("insert item into 2 item list\n");
	sll = sll_insert(sll, item_3);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.4 */
	printf("remove NULL item from 3 item list\n");
	sll = sll_remove(sll, 0);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-b */
	printf("remove middle item from 3 item list\n");
	sll = sll_remove(sll, item_3);
	xassert(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 1.7-c */
	printf("remove last item from 2 item list\n");
	sll = sll_remove(sll, item_2);
	xassert(sll == item_1
		&& !sll_next(sll));

	/* 1.3 */
	printf("remove NULL item from 1 item list\n");
	sll = sll_remove(sll, 0);
	xassert(sll == item_1
		&& !sll_next(sll));

	/* 1.5 */
	printf("remove last item from 1 item list\n");
	sll = sll_remove(sll, item_1);
	xassert(!sll);

	/* 1.1 */
	printf("remove NULL item from empty list\n");
	sll = sll_remove(sll, 0);
	xassert(!sll);

	/* 1.0 */
	printf("remove item from empty list\n");
	sll = sll_remove(sll, item_1);
	xassert(!sll);


	/* prep for 0.8 */
	printf("create 2 element list\n");
	sll = sll_insert(sll, item_1);
	sll = sll_insert(sll, item_2);
	xassert(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* prep for 0.8 */
	printf("create another 2 element list\n");
	sllist_t item_4 = sll_create((void*)4);
	sllist_t sll_1 = sll_insert(0, item_3);
	sll_1 = sll_insert(sll_1, item_4);
	xassert(sll_1 == item_3
		&& sll_next(sll_1) == item_4
		&& !sll_next(sll_next(sll_1)));

	/* 0.8 */
	printf("insert a list into a list\n");
	sll = sll_insert(sll, sll_1);
	xassert(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_4
		&& sll_next(sll_next(sll_next(sll))) == item_2
		&& !sll_next(sll_next(sll_next(sll_next(sll)))));


	/* destroy items */
	sll_destroy(item_1);
	sll_destroy(item_2);
	sll_destroy(item_3);
	sll_destroy(item_4);
}

int
main()
{
	return debug_test(sllist);
}

#endif
