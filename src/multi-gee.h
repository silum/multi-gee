/* $Id$ */
/*
 * multi-gee Frame Grabber Library
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MULTI_GEE_H
#define DSM_MULTI_GEE_H 1

#include <time.h>

#include "classdef.h"

__BEGIN_DECLS

/* object handles */

NEWHANDLE(multi_gee_t); /* multi-gee object handle */
NEWHANDLE(mg_frame_t); /* multi-gee frame object handle */


/* multi-gee frame */

int /* device identifier */
mg_frame_device_id(mg_frame_t);

const unsigned char ** /* image */
mg_frame_image(mg_frame_t);

multi_gee_t /* capture object */
mg_frame_mg_object(mg_frame_t);

struct timespec /* time stamp */
mg_frame_timestamp(mg_frame_t);

int /* sequence number */
mg_frame_sequence(mg_frame_t);

mg_frame_t /* next frame in list */
mg_frame_next(mg_frame_t);


/* multi-gee */

/* create object */
multi_gee_t /* new handle */
mg_create();

/* destroy object */
multi_gee_t
mg_destroy(multi_gee_t /* object to destroy */ );


/* start capture loop */
int /* status value:
       2 - mg_capture_halt() called
       1 - no capture devices registered
       0 - requested frames successfully acquired,
       -1 - sync failure,
       -2 - duplicate call to mg_capture */
mg_capture(multi_gee_t,
	   int n /* number of frames to capture */ );

/* halt capture loop */
void
mg_capture_halt(multi_gee_t);

/* register callback function */
multi_gee_t
mg_register_callback(void (*)(const mg_frame_t) /* callback function */ );

/* register capture device */
int /* status value:
       -1 - failed to register device,
       value >= 0 - device identifier */
mg_register_device(multi_gee_t,
		   const char * filename /* device to register */ );

/* deregister capture device */
int /* status value:
       -1 - failed to deregister device,
       value >= 0 - device identifier */
mg_deregister_device(multi_gee_t,
		     int device_id /* device identifier */ );

__END_DECLS

#endif /* ndef DSM_MULTI_GEE_H */
