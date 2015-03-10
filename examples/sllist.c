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
		for (size_t i = 0; i <= (size_t)testnum; ++i) {
			sll = sllist_insert_data(sll, (void*)i);
		}

		printf("display items\n");
		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sllist_data(list));
			list = sllist_next(list);
		}

		printf("forward remove\n");
		for (size_t i = 0; i <= (size_t)testnum; ++i) {
			sll = sllist_remove_data(sll, (void*)i);
			if (i != (size_t)testnum) {
				XASSERT(sll) {
					/* empty */
				}
			} else {
				XASSERT(sll == 0) {
					/* empty */
				}
			}
		}
	}

	{
		printf("insert %d items into list\n", testnum);
		sllist_t sll = 0;
		for (size_t i = 0; i <= (size_t)testnum; ++i) {
			sll = sllist_insert_data(sll, (void*)i);
		}

		printf("display items\n");
		sllist_t list = sll;
		while (list) {
			printf("data: %p\n", sllist_data(list));
			list = sllist_next(list);
		}

		printf("reverse remove\n");
      // Loop runs from testnum+1 to 1 to avoid tripping up over a
      // always true i > 0 check
		for (size_t i = testnum+1; i > 1; --i) {
			sll = sllist_remove_data(sll, (void*)(i-1));
			if (i > 1) {
				XASSERT(sll) {
					/* empty */
				}
			} else {
				XASSERT(sll == 0) {
					/* empty */
				}
			}
		}
	}
}

int
main()
{
	exit(cclass_assert_test(sllist));
}
