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
#include "mg_device.h"

__BEGIN_DECLS

/* multi-gee frame */

NEWHANDLE(mg_frame_t); /* multi-gee frame object handle */

/* create object */
mg_frame_t /* new handle */
mg_frame_create(multi_gee_t, /* capture object */
		mg_device_t device,
		void *image,
		struct timespec timestamp,
		int sequence);

/* destroy object */
mg_frame_t
mg_frame_destroy(mg_frame_t /* object to destroy */);

multi_gee_t /* capture object */
mg_frame_multi_gee(mg_frame_t);

mg_device_t /* device */
mg_frame_device(mg_frame_t);

void * /* image */
mg_frame_image(mg_frame_t);

struct timespec /* time stamp */
mg_frame_timestamp(mg_frame_t);

int /* sequence number */
mg_frame_sequence(mg_frame_t);

bool /* old frame */
mg_frame_used(mg_frame_t);

mg_frame_t
mg_frame_set_used(mg_frame_t);

__END_DECLS

#endif /* ndef DSM_MG_FRAME_H */
