/* $Id$ */
/*
 * multi-gee capture buffer
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_BUFFER_H
#define DSM_MG_BUFFER_H 1

#include "classdef.h"

#include "multi-gee.h"

__BEGIN_DECLS

NEWHANDLE(mg_buffer_t); /* multi-gee capture buffer object handle */

/* create object */
mg_buffer_t /* new handle */
mg_buffer_create();

/* destroy object */
mg_buffer_t
mg_buffer_destroy(mg_buffer_t /* object to destroy */);

int /* number of buffers */
mg_buffer_number(mg_buffer_t);

mg_buffer_t
mg_buffer_alloc_buffer(mg_buffer_t, unsigned int);

mg_buffer_t
mg_buffer_set_buffer(mg_buffer_t,
		     int /* index */,
		     void* /* start */,
		     size_t /* length */);

void * /* start */
mg_buffer_start(mg_buffer_t, int);

size_t /* length */
mg_buffer_length(mg_buffer_t, int);

mg_buffer_t
mg_buffer_set_timestamp(mg_buffer_t,
			unsigned int /* sequence */,
			struct timespec /* timestamp */);

unsigned int /* sequence */
mg_buffer_sequence(mg_buffer_t);

struct timespec /* timestamp */
mg_buffer_timestamp(mg_buffer_t);

mg_buffer_t
mg_buffer_set_used(mg_buffer_t);

bool /* state */
mg_buffer_used(mg_buffer_t);

__END_DECLS

#endif /* ndef DSM_MG_BUFFER_H */
