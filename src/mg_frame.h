/* $Id$ */
/*
 * multi-gee Frame Internals
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_FRAME_H
#define DSM_MG_FRAME_H 1

#include "classdef.h"
#include "multi-gee.h"

__BEGIN_DECLS

/* multi-gee frame */

/* create object */
mg_frame_t /* new handle */
mg_frame_create(multi_gee_t, /* capture object */
		int device_id,
		const unsigned char ** image,
		struct timespec timestamp,
		int sequence);

/* destroy object */
mg_frame_t
mg_frame_destroy(mg_frame_t /* object to destroy */);

mg_frame_t
mg_frame_set_next(mg_frame_t,
		  mg_frame_t next);

__END_DECLS

#endif /* ndef DSM_MULTI_GEE_H */
