/* $Id$ */
/*
 * multi-gee Frame Internals
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_DEVICE_H
#define DSM_MG_DEVICE_H 1

#include <multi-gee/log.h>
#include <multi-gee/mg_buffer.h>

__BEGIN_DECLS

NEWHANDLE(mg_device_t); /* multi-gee device object handle */

/**
 * @brief create device object
 *
 * sets major an minor number to actual device major and minor.  If
 * there is a problem, i.e. the device does not exist or the file is not
 * a device, the major and minor is both set to -1.
 *
 * @param file  device file name
 * @param log  object handle, to log error messages to during creation
 *
 * @return a newly created device object handle
 */
mg_device_t
mg_device_create(char *file,
		 log_t log);

/**
 * @brief destroy device object
 *
 * @param device  handle of object to be destroyed
 *
 * @return 0
 */
mg_device_t
mg_device_destroy(mg_device_t device);

/**
 * @brief device name accessor
 *
 * @param device  object handle
 *
 * @return device file name
 */
char *
mg_device_name(mg_device_t device);

/**
 * @brief open device
 *
 * an unopened device's file descriptor is -1.  This call opens a new
 * file descriptor for the device, if it does not have one yet.
 *
 * @param device  object handle
 *
 * @return status value:
 *  - 1: device already open,
 *  - 0: action successful,
 *  - -1: failed to open device
 */
int
mg_device_open(mg_device_t device);

/**
 * @brief device file descriptor accessor
 *
 * @param device  object handle
 *
 * @return device file descriptor or -1 if device not open
 */
int
mg_device_fd(mg_device_t device);

/**
 * @brief device major number accessor
 *
 * @param device  object handle
 *
 * @return device major number or -1 if not a device
 */
int
mg_device_major(mg_device_t device);

/**
 * @brief device minor number accessor
 *
 * @param device  object handle
 *
 * @return device minor number or -1 if not a device
 */
int
mg_device_minor(mg_device_t device);

/**
 * @brief device buffer container accessor
 *
 * @param device  object handle
 *
 * @return buffer object handle
 */
mg_buffer_t
mg_device_buffer(mg_device_t device);

__END_DECLS

#endif /* ndef DSM_MG_DEVICE_H */
