/* $Id$ */
/*
 * Singly linked-list
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_SLLIST_H
#define DSM_SLLIST_H 1

#include <multi-gee/classdef.h>

__BEGIN_DECLS

NEWHANDLE(sllist_t); /* singly linked list object handle */

/* list operations */

/**
 * @brief insert data into list, after current position
 *
 * @param list  object handle
 * @param data  pointer to data
 *
 * @return handle to modified list
 */
sllist_t
sll_insert_data(sllist_t,
		void *);

/**
 * @brief remove the first occurence of data in list
 *
 * @param list  object handle
 * @param data  pointer to data
 *
 * @return handle to modified list
 */
sllist_t
sll_remove_data(sllist_t,
		void *);

/**
 * @brief retrieve data from list item
 *
 * @param item  object handle
 *
 * @return pointer to data
 */
void *
sll_data(sllist_t);

/**
 * @brief find next list item
 *
 * @param item  object handle
 *
 * @return pointer to next item
 */
sllist_t
sll_next(sllist_t);

/**
 * @brief empty out list, destroying all items
 *
 * data items are not destroyed, only list items
 *
 * @param list  object handle
 *
 * @return 0
 */
sllist_t
sll_empty(sllist_t);

__END_DECLS

#endif /* ndef DSM_SLLIST_H */
