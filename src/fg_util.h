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

#include "mg_device.h"

__BEGIN_DECLS

struct v4l2_buffer;

/**
 * @brief initialise frame capture device
 */
bool
fg_init_device(mg_device_t dev);

/**
 * @brief unmap memmapped memory
 */
bool
fg_uninit_device(mg_device_t dev);

/**
 * @brief enqueue a capture buffer for filling by the driver
 */
bool
fg_enqueue(int fd, int i);

/**
 * @brief dequeue a buffer for user processing
 */
bool
fg_dequeue(int fd, struct v4l2_buffer *);

/**
 * @brief start streaming capturing on device
 */
bool
fg_start_capture(mg_device_t dev);

/**
 * @brief stop streaming capturing on device
 */
bool
fg_stop_capture(mg_device_t dev);

__END_DECLS

#endif /* ndef DSM_FG_UTIL_H */
