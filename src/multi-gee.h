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
#include "sllist.h"

__BEGIN_DECLS

/* object handle */

NEWHANDLE(multi_gee_t); /* multi-gee object handle */


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
       -2 - duplicate call to mg_capture,
       -3 - no callback registered */
mg_capture(multi_gee_t,
	   int n /* number of frames to capture */ );

/* halt capture loop */
void
mg_capture_halt(multi_gee_t);

/* register callback function */
multi_gee_t
mg_register_callback(multi_gee_t,
		     void (*)(const sllist_t) /* callback function */ );

/* register capture device */
int /* status value:
       -1 - failed to register device,
       value >= 0 - device identifier */
mg_register_device(multi_gee_t,
		   char *file_name /* device to register */ );

/* deregister capture device */
int /* status value:
       -1 - failed to deregister device,
       value >= 0 - device identifier */
mg_deregister_device(multi_gee_t,
		     int device_id /* device identifier */ );

__END_DECLS

#endif /* ndef DSM_MULTI_GEE_H */
