/* $Id$ */
/*
 * multi-gee capture buffer container
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_BUFFER_H
#define DSM_MG_BUFFER_H 1

#include <multi-gee/classdef.h>

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
mg_buffer_destroy(mg_buffer_t);

/**
 * @brief number of buffers accessor
 *
 * @param buffer  object handle
 *
 * @return the number of buffers allocated
 */
unsigned int
mg_buffer_number(mg_buffer_t);

/**
 * @brief allocate buffers
 *
 * @param buffer  object handle
 * @paran n  number of buffers to allocate
 *
 * @return object handled
 */
mg_buffer_t
mg_buffer_alloc(mg_buffer_t,
		unsigned int);

/**
 * @brief set the start address and length of a buffer
 *
 * @param buffer  object handle
 * @param i  buffer index
 * @param start  buffer start pointer
 * @param length  size of buffer, in bytes
 *
 * @return object handled
 */
mg_buffer_t
mg_buffer_set(mg_buffer_t,
	      unsigned int,
	      void *,
	      size_t);

/**
 * @brief buffer start address accessor
 *
 * @param buffer  object handle
 * @param i  buffer index
 *
 * @return buffer start pointer
 */
void *
mg_buffer_start(mg_buffer_t,
		unsigned int);

/**
 * @brief buffer length accessor
 *
 * @param buffer  object handle
 * @param i  buffer index
 *
 * @return length of buffer
 */
size_t
mg_buffer_length(mg_buffer_t,
		 unsigned int);

__END_DECLS

#endif /* ndef DSM_MG_BUFFER_H */
