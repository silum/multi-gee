/* $Id$
 * Copyright (C) 2006 Deneys S. Maartens <dsm@tlabs.ac.za>
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
  */
#ifndef ITL_DEVICE_H
#define ITL_DEVICE_H

#include <cclass/classdef.h>
#include <sys/time.h>

__BEGIN_DECLS

/**
 * @brief Device object handle
 */
NEWHANDLE(device_t);

/**
 * @brief Create device object
 *
 * @param id  device identifier
 * @param tv  time value
 * @param diff  difference time value
 * @param seq  sequence number
 *
 * @return a newly created device object handle
 */
device_t
dev_create(const char *id,
	   struct timeval tv,
	   struct timeval diff,
	   int seq);

/**
 * @brief Destroy device object
 *
 * @param device  handle of object to be destroyed
 *
 * @return 0
 */
device_t
dev_destroy(device_t device);

/**
 * @brief Get device id
 *
 * @param device  object handle
 *
 * @return
 * - 0 on invalid object handle
 * - the device identifier
 */
const char *
dev_id(device_t device);

/**
 * @brief Get time value
 *
 * @param device  object handle
 *
 * @return
 * - {0,0} time value on invalid object handle
 * - the time value
 */
struct timeval
dev_tv(device_t device);

/**
 * @brief Get time value difference
 *
 * @param device  object handle
 *
 * @return
 * - {0,0} time value on invalid object handle
 * - the time value
 */
struct timeval
dev_diff(device_t device);

/**
 * @brief Get sequence number
 *
 * @param device  object handle
 *
 * @return
 * - 0 on invalid object handle
 * - the sequence number
 */
int
dev_seq(device_t device);

__END_DECLS

#endif // ITL_DEVICE_H
