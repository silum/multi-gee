/* $Id$ */
/*
 * frame grabber interface utilities
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
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
 * @param dev  device object handle
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to init device
 */
bool
fg_init_device(mg_device_t,
	       log_t);

/**
 * @brief uninitialise frame capture device
 *
 * @param dev  device to uninit
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to uninit device
 */
bool
fg_uninit_device(mg_device_t,
		 log_t);

/**
 * @brief enqueue a capture buffer for filling by the driver
 *
 * @param fd  file descriptor
 * @param i   buffer index
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to enqueue buffer
 */
bool
fg_enqueue(int,
	   int,
	   log_t);

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
fg_dequeue(int,
	   struct v4l2_buffer *,
	   log_t);

/**
 * @brief start streaming capturing on device
 *
 * @param dev  device to start streaming
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to start capture
 */
bool
fg_start_capture(mg_device_t,
		 log_t);

/**
 * @brief stop streaming capturing on device
 *
 * @param dev  device to stop streaming
 * @param log  to log possible errors to
 *
 * @return true on success, false on failure to stop capture
 */
bool
fg_stop_capture(mg_device_t,
		log_t);

__END_DECLS

#endif /* ndef DSM_FG_UTIL_H */
