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

#include <sys/cdefs.h>

#include <multi-gee/classdef.h>
#include <multi-gee/sllist.h>
#include <multi-gee/mg_buffer.h>
#include <multi-gee/mg_device.h>
#include <multi-gee/mg_frame.h>

__BEGIN_DECLS;

/* object handle */

NEWHANDLE(multi_gee_t); /* multi-gee object handle */

/**
 * @brief multi-gee object
 *
 * @param file  logfile name,
 *   - can be 0 for no log file,
 *   - "stdout" for standard output stream
 *   - "stderr" for standard error stream
 *   - a file name: logs will be appended to this file
 *
 * @return a newly created multi-gee object handle
 */
multi_gee_t
mg_create(char *);

/**
 * @brief destroy multi-gee object
 *
 * @param multi_gee  handle of object to be destroyed
 *
 * @return 0
 */
multi_gee_t
mg_destroy(multi_gee_t);

/**
 * @brief error return values for mg_capture
 */
enum mg_RETURN {
	RET_UNDEF = -6, /**< undefined return value, should never occur */
	RET_CALLBACK,   /**< -5 - no callback registered */
	RET_SYNC,       /**< -4 - sync lost */
	RET_BUSY,       /**< -3 - multiple call to capture */
	RET_DEVICE,     /**< -2 - no devices registered */
	RET_HALT,       /**< -1 - capture_halt called */
};

/**
 * @brief start capture loop
 *
 * @param multi_gee  object handle
 * @param n  number of frames to capture: -1 => capture forever
 *
 * @return mg_RETURN status value, or the number of images captured
 */
enum mg_RETURN
mg_capture(multi_gee_t,
	   int);

/**
 * @brief halt capture loop
 *
 * @param multi_gee  object handle
 */
void
mg_capture_halt(multi_gee_t);

/**
 * @brief register callback function
 *
 * @param multi_gee  object handle
 * @param callback   user defined callback function
 *
 * @return object handle
 */
multi_gee_t
mg_register_callback(multi_gee_t,
		     void (*callback)(multi_gee_t, sllist_t));

/**
 * @brief register capture device
 *
 * @param multi_gee  object handle
 * @param file_name  device to register
 *
 * @return status value:
 *   -1 - failed to register device,
 *   value >= 0 - device identifier
 */
int
mg_register_device(multi_gee_t,
		   char *);

/**
 * @brief deregister capture device
 *
 * @param multi_gee  object handle
 * @param id  device identifier
 *
 * @return status value:
 *   -1 - failed to register device,
 *   value >= 0 - device identifier of deregistered device
 */
int
mg_deregister_device(multi_gee_t,
		     int);

__END_DECLS;

#endif /* ndef DSM_MULTI_GEE_H */
