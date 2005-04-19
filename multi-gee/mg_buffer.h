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
 * @file mg_buffer.h
 * @brief Multi-gee capture buffer declaration
 */

#ifndef DSM_MG_BUFFER_H
#define DSM_MG_BUFFER_H

#include <classdef.h>

__BEGIN_DECLS

/**
 * @brief Multi-gee capture buffer object handle
 */
NEWHANDLE(mg_buffer_t);

/**
 * @brief Create buffer object
 *
 * @return a newly created buffer object handle
 */
mg_buffer_t
mg_buffer_create();

/**
 * @brief Destroy buffer object
 *
 * @param buffer  handle of object to be destroyed
 *
 * @return 0
 */
mg_buffer_t
mg_buffer_destroy(mg_buffer_t buffer);

/**
 * @brief Allocate buffers
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
 * @brief Buffer length accessor
 *
 * @param buffer  object handle
 * @param index  buffer index
 *
 * @return length of buffer
 */
size_t
mg_buffer_length(mg_buffer_t buffer,
		 unsigned int index);

/**
 * @brief Number of buffers accessor
 *
 * @param buffer  object handle
 *
 * @return the number of buffers allocated
 */
unsigned int
mg_buffer_number(mg_buffer_t buffer);

/**
 * @brief Buffer start address accessor
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
 * @brief Set the start address and length of a buffer
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

__END_DECLS

#endif /* DSM_MG_BUFFER_H */
