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
 * @file
 * @brief Singly linked-list test program
 */
#include <cclass/xassert.h>
#include <cclass/xmalloc.h>
#include <stdlib.h>
#include <stdio.h>

#include <multi-gee/sllist.h>

USE_XASSERT

void
sllist()
{
	int testnum = 15;

	{
		printf("insert %d items into list\n", testnum);
		sllist_t sll = 0;
		for (int i = 0; i <= testnum; ++i) {
			sll = sll_insert_data(sll, (void*)i);
		}

		printf("display items\n");
		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sll_data(list));
			list = sll_next(list);
		}

		printf("forward remove\n");
		for (int i = 0; i <= testnum; ++i) {
			sll = sll_remove_data(sll, (void*)i);
			if (i != testnum) {
				XASSERT(sll);
			} else {
				XASSERT(sll == 0);
			}
		}
	}

	{
		printf("insert %d items into list\n", testnum);
		sllist_t sll = 0;
		for (int i = 0; i <= testnum; ++i) {
			sll = sll_insert_data(sll, (void*)i);
		}

		printf("display items\n");
		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sll_data(list));
			list = sll_next(list);
		}

		printf("reverse remove\n");
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
	exit(xassert_test(sllist));
}

