/* $Id$
 * Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file sllist.c
 * @brief Singly linked-list definition
 */

#include <xassert.h>
#include <xmalloc.h>

#include "sllist.h"

USE_XASSERT;

/** @brief Set flag to true if sll can be verified */
#define VERIFY_SLL_FLAG(sll, flag) \
	bool flag = false; { sllist_t sllist = sll; VERIFY(sllist)  { flag = true; } }
/** @brief Set flag to true if sll can be verified, or is zero */
#define VERIFYZ_SLL_FLAG(sll, flag) \
	bool flag = false; { sllist_t sllist = sll; VERIFYZ(sllist) { flag = true; } }

/**
 * @brief Create sllist object
 *
 * @param data  pointer to list item data
 *
 * @return a newly created log object handle
 */
static sllist_t
sll_create(void *data);

/**
 * @brief Destroy sllist object
 *
 * @param list  handle of object to be destroyed
 *
 * @return 0
 */
static sllist_t
sll_destroy(sllist_t list);

/**
 * @brief Insert item into list after current position
 *
 * inserts an item, or a list, after the current position.  if the list
 * exists, insert item after first item in list, else item becomes list
 * if a list is inserted the last item of the inserted list will be
 * modified to point to the item after the current position.
 *
 * care must be taken not to insert an item of a specific list into the
 * same list.  this will cause a infinite list.... bad, very bad.
 *
 * @param list  current position
 * @param item  to be inserted
 *
 * @return handle to modified list
 */
static sllist_t
sll_insert(sllist_t list,
	   sllist_t item);

/**
 * @brief Remove item from list, but do not destroy object
 *
 * @param list  head of list
 * @param item  to be removed
 *
 * @return handle to modified list
 */
static sllist_t
sll_remove(sllist_t list,
	   sllist_t item);

/**
 * @brief Sllist object structure
 */
CLASS(sllist, sllist_t)
{
	void *data;
	sllist_t next;
};

static sllist_t
sll_create(void *data)
{
	sllist_t sllist;
	NEWOBJ(sllist);

	sllist->data = data;
	sllist->next = 0;

	return sllist;
}

static sllist_t
sll_destroy(sllist_t sllist)
{
	VERIFYZ(sllist) {
		FREEOBJ(sllist);
	}

	return 0;
}

static sllist_t
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

static sllist_t
sll_remove(sllist_t list,
	   sllist_t item)
{
	sllist_t sllist = 0;

	VERIFYZ_SLL_FLAG(list, list_f);
	VERIFYZ_SLL_FLAG(item, item_f);

	if (list_f) {
		sllist = list;

		if (item_f) {
			if (list == item) {
				sllist = list->next;
				item->next = 0;
			} else while (list->next) {
				if (list->next == item) {
					list->next = list->next->next;
					item->next = 0;
					break;
				}
				list = list->next;
			}
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
		for (sllist_t p = sllist; p; p = p->next) {
			if (p->data == data) {
				sllist = sll_remove(sllist, p);
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
			sllist_t item = sllist;
			sllist = sll_remove(sllist, item);
			sll_destroy(item);
		}
	}

	return 0;
}

#ifdef DEBUG_SLLIST

#include <stdlib.h>
#include <stdio.h>
#include <xassert.h>
#include <xmalloc.h>

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
	XASSERT(sll == 0);

	/* 0.1 */
	printf("insert item into NULL list\n");
	sllist_t item_1 = sll_create((void*)1);
	sll = sll_insert(0, item_1);
	XASSERT(sll == item_1
		&& !sll_next(sll));

	/* 0.3 */
	printf("insert NULL into 1 item list\n");
	sll = sll_insert(item_1, 0);
	XASSERT(sll == item_1
		&& !sll_next(sll));

	/* 0.5 */
	printf("insert item into 1 item list\n");
	sllist_t item_2 = sll_create((void*)2);
	sll = sll_insert(sll, item_2);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.7 */
	printf("insert item into 2 item list\n");
	sllist_t item_3 = sll_create((void*)3);
	sll = sll_insert(sll, item_3);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 0.2 */
	printf("insert list into NULL item\n");
	sll = sll_insert(0, sll);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 0.4 */
	printf("insert NULL item into list\n");
	sll = sll_insert(0, sll);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-a */
	printf("remove first item from 3 item list\n");
	sll = sll_remove(sll, item_1);
	XASSERT(sll == item_3
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.6 */
	printf("insert 2 item list into 1 item list\n");
	sll = sll_insert(item_1, sll);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-c */
	printf("remove last item from 3 item list\n");
	sll = sll_remove(sll, item_2);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& !sll_next(sll_next(sll)));

	/* 1.7-c */
	printf("remove last item from 2 item list\n");
	sll = sll_remove(sll, item_3);
	XASSERT(sll == item_1
		&& !sll_next(sll));

	/* 0.5 */
	printf("insert item into 1 item list\n");
	sll = sll_insert(sll, item_2);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 0.7 */
	printf("insert item into 2 item list\n");
	sll = sll_insert(sll, item_3);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.4 */
	printf("remove NULL item from 3 item list\n");
	sll = sll_remove(sll, 0);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_3
		&& sll_next(sll_next(sll)) == item_2
		&& !sll_next(sll_next(sll_next(sll))));

	/* 1.7-b */
	printf("remove middle item from 3 item list\n");
	sll = sll_remove(sll, item_3);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* 1.7-c */
	printf("remove last item from 2 item list\n");
	sll = sll_remove(sll, item_2);
	XASSERT(sll == item_1
		&& !sll_next(sll));

	/* 1.3 */
	printf("remove NULL item from 1 item list\n");
	sll = sll_remove(sll, 0);
	XASSERT(sll == item_1
		&& !sll_next(sll));

	/* 1.5 */
	printf("remove last item from 1 item list\n");
	sll = sll_remove(sll, item_1);
	XASSERT(!sll);

	/* 1.1 */
	printf("remove NULL item from empty list\n");
	sll = sll_remove(sll, 0);
	XASSERT(!sll);

	/* 1.0 */
	printf("remove item from empty list\n");
	sll = sll_remove(sll, item_1);
	XASSERT(!sll);


	/* prep for 0.8 */
	printf("create 2 element list\n");
	sll = sll_insert(sll, item_1);
	sll = sll_insert(sll, item_2);
	XASSERT(sll == item_1
		&& sll_next(sll) == item_2
		&& !sll_next(sll_next(sll)));

	/* prep for 0.8 */
	printf("create another 2 element list\n");
	sllist_t item_4 = sll_create((void*)4);
	sllist_t sll_1 = sll_insert(0, item_3);
	sll_1 = sll_insert(sll_1, item_4);
	XASSERT(sll_1 == item_3
		&& sll_next(sll_1) == item_4
		&& !sll_next(sll_next(sll_1)));

	/* 0.8 */
	printf("insert a list into a list\n");
	sll = sll_insert(sll, sll_1);
	XASSERT(sll == item_1
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

#endif /* def DEBUG_SLLIST */
