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
 * @file sllist.h
 * @brief Singly linked-list declaration
 */

#ifndef DSM_SLLIST_H
#define DSM_SLLIST_H

#include <classdef.h>

__BEGIN_DECLS

/**
 * @brief Singly linked list object handle
 */
NEWHANDLE(sllist_t);

/* list operations */

/**
 * @brief Insert data into list, after current position
 *
 * @param list  object handle
 * @param data  pointer to data
 *
 * @return handle to modified list
 */
sllist_t
sll_insert_data(sllist_t list,
		void *data);

/**
 * @brief Remove the first occurrence of data in list
 *
 * @param list  object handle
 * @param data  pointer to data
 *
 * @return handle to modified list
 */
sllist_t
sll_remove_data(sllist_t list,
		void *data);

/**
 * @brief Retrieve data from list item
 *
 * @param item  object handle
 *
 * @return pointer to data
 */
void *
sll_data(sllist_t item);

/**
 * @brief Find next list item
 *
 * @param item  object handle
 *
 * @return pointer to next item
 */
sllist_t
sll_next(sllist_t item);

/**
 * @brief Empty out list, destroying all items
 *
 * data items are not destroyed, only list items
 *
 * @param list  object handle
 *
 * @return 0
 */
sllist_t
sll_empty(sllist_t list);

__END_DECLS

#endif /* DSM_SLLIST_H */
