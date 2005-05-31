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
#include <xassert.h>
#include <xmalloc.h>

#include "sllist.h"

USE_XASSERT

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

