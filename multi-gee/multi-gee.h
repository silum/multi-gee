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
 * @file
 * @brief Multi-gee Frame Grabber Library declaration
 */
#ifndef ITL_MULTI_GEE_MULTI_GEE_H
#define ITL_MULTI_GEE_MULTI_GEE_H

#include <cclass/classdef.h>

#include <multi-gee/sllist.h>
#include <multi-gee/mg_buffer.h>
#include <multi-gee/mg_device.h>
#include <multi-gee/mg_frame.h>

__BEGIN_DECLS

/**
 * @mainpage multi-gee Frame Capture Library
 *
 * \section phil_sec Philosophy
 *
 * This library makes use of a C style object implementation where the
 * members of the object is unknown, and not directly accessible, to the
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
 * constraints, where you cannot wait extended periods of time between
 * acquired images, and you have a rather stiff timing constraint on you
 * image capture software.
 *
 * The multi-gee image capture library was written with tight time
 * constraints in mind, as well as failing gracefully in the event that
 * images cannot be acquired within a reasonable time period.  All images
 * that are passed to the user are guaranteed to be in sync.  \em In \em
 * sync sync is defined as all the maximum difference of time stamps of
 * all frames being less than half of the frame rate, give or take 5 per
 * cent.  At PAL frame rate the frame rate is 25 frames a second, thus
 * consecutive frames are 40 milliseconds (ms) apart.  A set of frames
 * are therefore in sync if the time stamp spread is less than 22 ms.
 *
 * If captured frames are not in sync for more than 3 frames, plus 5 per
 * cent (126 ms) then the capture function fails with an appropriate
 * return value.
 *
 * \section usage_sec Usage
 *
 * Below is some example code showing how the basic use of the image
 * capture library.  It is written in C-99 format.
 *
 * @include multi-gee.c
 */

/**
 * @brief Multi_gee object handle
 */
NEWHANDLE(multi_gee_t);

/**
 * @brief Create multi-gee object
 *
 * @param file_name  log file name,
 *   - can be 0 for no log file,
 *   - "stdout" for standard output stream
 *   - "stderr" for standard error stream
 *   - a file name: logs will be appended to this file
 *
 * @return a newly created Multi-gee object handle
 */
multi_gee_t
mg_create(char *file_name);

/**
 * @brief Create specialised multi-gee object
 *
 * @param file_name  log file name,
 *   - can be 0 for no log file,
 *   - "stdout" for standard output stream
 *   - "stderr" for standard error stream
 *   - a file name: logs will be appended to this file
 * @param tv_in_sync  in sync criterion
 * @param tv_no_sync  sync failure criterion
 * @param tv_sub  offset from capture start to accept frames
 * @param num_bufs  number of captyre buffers
 *
 * @return a newly created Multi-gee object handle
 */
multi_gee_t
mg_create_special(char *file_name,
		  struct timeval tv_in_sync,
		  struct timeval tv_no_sync,
		  struct timeval tv_sub,
		  unsigned int num_bufs);

/**
 * @brief Destroy multi-gee object
 *
 * @param multi_gee  handle of object to be destroyed
 *
 * @return 0
 */
multi_gee_t
mg_destroy(multi_gee_t multi_gee);

/**
 * @brief Error return values for mg_capture
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
 * @brief Start capture loop
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
 * @brief Halt capture loop
 *
 * @param multi_gee  object handle
 */
void
mg_capture_halt(multi_gee_t multi_gee);

/**
 * @brief Deregister capture device
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

/**
 * @brief Register callback function
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
 * @brief Register capture device
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

__END_DECLS

#endif /* ITL_MULTI_GEE_MULTI_GEE_H */
