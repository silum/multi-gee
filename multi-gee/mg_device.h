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
 * @file mg_device.h
 * @brief Multi-gee device declaration
 */

#ifndef DSM_MG_DEVICE_H
#define DSM_MG_DEVICE_H

#include <multi-gee/log.h>
#include <multi-gee/mg_buffer.h>

__BEGIN_DECLS

/**
 * @brief Multi-gee capture device object handle
 */
NEWHANDLE(mg_device_t);

/**
 * @brief Create device object
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
 * @brief Destroy device object
 *
 * @param device  handle of object to be destroyed
 *
 * @return 0
 */
mg_device_t
mg_device_destroy(mg_device_t device);

/**
 * @brief Device name accessor
 *
 * @param device  object handle
 *
 * @return device file name
 */
char *
mg_device_name(mg_device_t device);

/**
 * @brief Open device
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
 * @brief Device file descriptor accessor
 *
 * @param device  object handle
 *
 * @return device file descriptor or -1 if device not open
 */
int
mg_device_fd(mg_device_t device);

/**
 * @brief Device major number accessor
 *
 * @param device  object handle
 *
 * @return device major number or -1 if not a device
 */
int
mg_device_major(mg_device_t device);

/**
 * @brief Device minor number accessor
 *
 * @param device  object handle
 *
 * @return device minor number or -1 if not a device
 */
int
mg_device_minor(mg_device_t device);

/**
 * @brief Device buffer container accessor
 *
 * @param device  object handle
 *
 * @return buffer object handle
 */
mg_buffer_t
mg_device_buffer(mg_device_t device);

__END_DECLS

#endif /* DSM_MG_DEVICE_H */
