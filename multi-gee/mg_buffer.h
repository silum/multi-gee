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
 *
 *
 * multi-gee capture buffer container
 */

#ifndef DSM_MG_BUFFER_H
#define DSM_MG_BUFFER_H 1

#include <classdef.h>

__BEGIN_DECLS

NEWHANDLE(mg_buffer_t); /* multi-gee capture buffer object handle */

/**
 * @brief create buffer object
 *
 * @return a newly created buffer object handle
 */
mg_buffer_t
mg_buffer_create();

/**
 * @brief destroy buffer object
 *
 * @param buffer  handle of object to be destroyed
 *
 * @return 0
 */
mg_buffer_t
mg_buffer_destroy(mg_buffer_t buffer);

/**
 * @brief number of buffers accessor
 *
 * @param buffer  object handle
 *
 * @return the number of buffers allocated
 */
unsigned int
mg_buffer_number(mg_buffer_t buffer);

/**
 * @brief allocate buffers
 *
 * @param buffer  object handle
 * @param n  number of buffers to allocate
 *
 * @return object handled
 */
mg_buffer_t
mg_buffer_alloc(mg_buffer_t buffer,
		unsigned int n);

/**
 * @brief set the start address and length of a buffer
 *
 * @param buffer  object handle
 * @param index  buffer index
 * @param start  buffer start pointer
 * @param length  size of buffer, in bytes
 *
 * @return object handled
 */
mg_buffer_t
mg_buffer_set(mg_buffer_t buffer,
	      unsigned int index,
	      void *start,
	      size_t length);

/**
 * @brief buffer start address accessor
 *
 * @param buffer  object handle
 * @param index  buffer index
 *
 * @return buffer start pointer
 */
void *
mg_buffer_start(mg_buffer_t buffer,
		unsigned int index);

/**
 * @brief buffer length accessor
 *
 * @param buffer  object handle
 * @param index  buffer index
 *
 * @return length of buffer
 */
size_t
mg_buffer_length(mg_buffer_t buffer,
		 unsigned int index);

__END_DECLS

#endif /* DSM_MG_BUFFER_H */
