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

#include "classdef.h"

__BEGIN_DECLS

NEWHANDLE(sllist_t); /* singly linked list object handle */

/* list operations */

/* insert data into list, after current position */
sllist_t
sll_insert_data(sllist_t,
		void *data);

/* remove the first occurence of data in list */
sllist_t
sll_remove_data(sllist_t,
		void *data);

/* retrieve data from list item */
void *
sll_data(sllist_t);

/* find next list item */
sllist_t
sll_next(sllist_t);

/* empty out list */
sllist_t
sll_empty(sllist_t);

__END_DECLS

#endif /* ndef DSM_MULTI_GEE_H */
