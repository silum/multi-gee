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

#include <multi_gee/mg_device.h>

__BEGIN_DECLS

struct v4l2_buffer;

/**
 * @brief initialise frame capture device
 *
 * @param dev  device to initialise
 *
 * @return true on success, false on failure to init device
 */
bool
fg_init_device(mg_device_t dev);

/**
 * @brief uninitialise frame capture device
 *
 * @param dev  device to uninit
 *
 * @return true on success, false on failure to uninit device
 */
bool
fg_uninit_device(mg_device_t dev);

/**
 * @brief enqueue a capture buffer for filling by the driver
 *
 * @param fd  file descriptor
 * @param i   buffer index
 *
 * @return true on success, false on failure to enqueue buffer
 */
bool
fg_enqueue(int fd, int i);

/**
 * @brief dequeue a buffer for user processing
 *
 * @param fd  file descriptor
 * @param buffer  video4linux2 buffer to dequeue
 *
 * @return true on success, false on failure to dequeue buffer
 */
bool
fg_dequeue(int fd, struct v4l2_buffer *);

/**
 * @brief start streaming capturing on device
 *
 * @param dev  device to start streaming
 *
 * @return true on success, false on failure to start capture
 */
bool
fg_start_capture(mg_device_t dev);

/**
 * @brief stop streaming capturing on device
 *
 * @param dev  device to stop streaming
 *
 * @return true on success, false on failure to stop capture
 */
bool
fg_stop_capture(mg_device_t dev);

__END_DECLS

#endif /* ndef DSM_FG_UTIL_H */
