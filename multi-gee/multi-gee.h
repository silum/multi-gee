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

/**
 * @mainpage multi-gee Frame Capture Library
 *
 * \section phil_sec Philosophy
 *
 * This library makes use of a C style object implementation where the
 * members of the object is unknown, and not directly accessable, to the
 * user of the library.  The magic is built on object handles, or
 * structure pointers if you will.  All the methods for an object rely
 * on receiving a handle to perform it's work on.  The philosophy of
 * this implementation style is discussed in length in ``Writing
 * Bug-Free C Code -- A Programming Style That Automatically Detects
 * Bugs in C Code'' by Jerry Jongerius, January 1995.  It is available
 * as an online resource at http://www.duckware.com/bugfreec/.
 *
 * \section intro_sec Introduction
 *
 * Full frame rate video capture and processing needs a low latency
 * video capture solution.  Couple this with high reliability
 * contraints, where you cannot wait extended periods of time between
 * acquired images, and you have a rather stiff timing constraint on you
 * image capture software.
 *
 * The multi-gee image capture library was written with tight time
 * constraints in mind, as well as failing gracefully in the event that
 * images cannot be aquired within a reasonable time period.  All images
 * that are passed to the user are guaranteed to be in sync.  \em In \em
 * sync sync is defined as all the maximum difference of time stamps of
 * all frames being less than half of the frame rate, give or take 5 per
 * cent.  At PAL frame rate the frame rate is 25 frames a second, thus
 * consecutive frames are 40 milliseconds (ms) appart.  A set of frames
 * are therefore in sync if the time stamp spread is less than 22 ms.
 *
 * If captured frames are not in sync for more than 3 frames, plus 5 per
 * cent (126 ms) then the capture function fails with an appropridate
 * return value.
 *
 * \section usage_sec Usage
 *
 * Below is some example code showing how the basic use of the image
 * capture library.  It is written in C-99 format.
 *
 * @include multi-gee/multi-gee.c

 * \subsection step1 Step 1: Opening the box
 *
 */

/**
 * @class multi_gee_t multi-gee.h multi-gee.h
 * @brief multi_gee object handle
 *
 * Class like implementation of a multi-gee frame grabber interface.
 */

NEWHANDLE(multi_gee_t);

/**
 * @brief create multi-gee object
 *
 * @param file_name  logfile name,
 *   - can be 0 for no log file,
 *   - "stdout" for standard output stream
 *   - "stderr" for standard error stream
 *   - a file name: logs will be appended to this file
 *
 * @return a newly created multi-gee object handle
 */
multi_gee_t
mg_create(char *file_name);

/**
 * @brief destroy multi-gee object
 *
 * @param multi_gee  handle of object to be destroyed
 *
 * @return 0
 */
multi_gee_t
mg_destroy(multi_gee_t multi_gee);

/**
 * @brief error return values for mg_capture
 */
enum mg_RETURN {
	RET_UNDEF = -6, /**< undefined return value, should never occur */
	RET_CALLBACK,   /**< -5 -- no callback registered */
	RET_SYNC,       /**< -4 -- sync lost */
	RET_BUSY,       /**< -3 -- multiple call to capture */
	RET_DEVICE,     /**< -2 -- no devices registered */
	RET_HALT,       /**< -1 -- capture_halt called */
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
mg_capture(multi_gee_t multi_gee,
	   int n);

/**
 * @brief halt capture loop
 *
 * @param multi_gee  object handle
 */
void
mg_capture_halt(multi_gee_t multi_gee);

/**
 * @brief register callback function
 *
 * @param multi_gee  object handle
 * @param callback   user defined callback function
 *
 * @return object handle
 */
multi_gee_t
mg_register_callback(multi_gee_t multi_gee,
		     void (*callback)(multi_gee_t, sllist_t));

/**
 * @brief register capture device
 *
 * @param multi_gee  object handle
 * @param device_name  device to register
 *
 * @return status value:
 *   -1 - failed to register device,
 *   value >= 0 - device identifier
 */
int
mg_register_device(multi_gee_t multi_gee,
		   char *device_name);

/**
 * @brief deregister capture device
 *
 * @param multi_gee  object handle
 * @param device_id  device identifier
 *
 * @return status value:
 *   -1 - failed to register device,
 *   value >= 0 - device identifier of deregistered device
 */
int
mg_deregister_device(multi_gee_t multi_gee,
		     int device_id);

__END_DECLS;

#endif /* ndef DSM_MULTI_GEE_H */