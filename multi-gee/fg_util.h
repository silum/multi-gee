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
 *
 *
 * frame grabber interface utilities
 */

#ifndef DSM_FG_UTIL_H
#define DSM_FG_UTIL_H 1

#include <stdbool.h>
#include <multi-gee/mg_device.h>

__BEGIN_DECLS

/* pre-declaration of video4linux2 buffer type */
struct v4l2_buffer;

/**
 * @brief initialise frame capture device
 *
 * does the following
 *  - test capabilities
 *  - select input source
 *  - reset the cropping
 *  - set the capture format
 *  - initialise the memory-mapping
 *
 * @param device  device object handle
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to init device
 */
bool
fg_init_device(mg_device_t device,
	       log_t log);

/**
 * @brief uninitialise frame capture device
 *
 * @param device  device handle to uninit
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to uninit device
 */
bool
fg_uninit_device(mg_device_t device,
		 log_t log);

/**
 * @brief enqueue a capture buffer for filling by the driver
 *
 * @param fd  file descriptor
 * @param index   buffer index
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to enqueue buffer
 */
bool
fg_enqueue(int fd,
	   int index,
	   log_t log);

/**
 * @brief dequeue a buffer for user processing
 *
 * @param fd  file descriptor
 * @param buffer  video4linux2 buffer to dequeue
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to dequeue buffer
 */
bool
fg_dequeue(int fd,
	   struct v4l2_buffer *buffer,
	   log_t log);

/**
 * @brief start streaming capturing on device
 *
 * @param device  device to start streaming
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to start capture
 */
bool
fg_start_capture(mg_device_t device,
		 log_t log);

/**
 * @brief stop streaming capturing on device
 *
 * @param device  device to stop streaming
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to stop capture
 */
bool
fg_stop_capture(mg_device_t device,
		log_t log);

__END_DECLS

#endif /* DSM_FG_UTIL_H */
