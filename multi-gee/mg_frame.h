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

#include <stdint.h> /* uint32_t */
#include <stdbool.h> /* bool */

#include <multi-gee/mg_device.h>

struct v4l2_buffer;

__BEGIN_DECLS

/* multi-gee frame */

NEWHANDLE(mg_frame_t); /* multi-gee frame object handle */

/* create object */
mg_frame_t /* new handle */
mg_frame_create(mg_device_t /* capture device */,
		struct v4l2_buffer * /* capture buffer */);

/* destroy object */
mg_frame_t
mg_frame_destroy(mg_frame_t /* object to destroy */);

int /* index */
mg_frame_index(mg_frame_t);

mg_device_t /* device */
mg_frame_device(mg_frame_t);

void * /* image */
mg_frame_image(mg_frame_t);

struct timeval /* time stamp */
mg_frame_timestamp(mg_frame_t);

uint32_t /* sequence number */
mg_frame_sequence(mg_frame_t);

bool /* old frame */
mg_frame_used(mg_frame_t);

mg_frame_t
mg_frame_set_used(mg_frame_t);

__END_DECLS

#endif /* ndef DSM_MG_FRAME_H */
