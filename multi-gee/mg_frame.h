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
 * @brief Multi-gee Frame declaration
 */
#ifndef ITL_MULTI_GEE_MG_FRAME_H
#define ITL_MULTI_GEE_MG_FRAME_H

#include <stdint.h> /* uint32_t */
#include <stdbool.h> /* bool */

#include <multi-gee/mg_device.h>

struct v4l2_buffer;

__BEGIN_DECLS

/**
 * @brief Multi-gee frame object handle
 */
NEWHANDLE(mg_frame_t);

/**
 * @brief Create frame object
 *
 * takes a v4l2 buffer and extracts pertinent information such as
 *  - a pointer to image data
 *  - the size of the image data
 *  - the time stamp
 *  - the sequence number
 *  - the buffer index
 *
 * @param device  capture device
 * @param buffer  video4linux2 capture buffer
 *
 * @return a newly created frame object handle
 */
mg_frame_t
mg_frame_create(mg_device_t device,
		struct v4l2_buffer *buffer);

/**
 * @brief Destroy frame object
 *
 * @param frame  handle of object to be destroyed
 *
 * @return 0
 */
mg_frame_t
mg_frame_destroy(mg_frame_t frame);

/**
 * @brief Capture device accessor
 *
 * @param frame  object handle
 *
 * @return the capture device object handle
 */
mg_device_t
mg_frame_get_device(mg_frame_t frame);

/**
 * @brief Image data accessor
 *
 * @param frame  object handle
 *
 * @return a pointer to the image data, or 0 if no image data exists
 */
void *
mg_frame_get_image(mg_frame_t frame);

/**
 * @brief Buffer index accessor
 *
 * the image pointer is read from the v4l2 buffer.  if frame was
 * constructed without a valid v4l2 buffer it is -1.
 *
 * @param frame  object handle
 *
 * @return the index number, or -1 if not a valid frame
 */
int
mg_frame_get_index(mg_frame_t frame);

/**
 * @brief Sequence number accessor
 *
 * the sequence number is read from the v4l2 buffer.  if frame was
 * constructed without a valid v4l2 buffer it is -1.
 *
 * @param frame  object handle
 *
 * @return the sequence number
 */
uint32_t
mg_frame_get_sequence(mg_frame_t frame);

/**
 * @brief Time stamp accessor
 *
 * the time stamp is read from the v4l2 buffer.  if frame was
 * constructed without a valid v4l2 buffer it is the time when the frame
 * was created.
 *
 * @param frame  object handle
 *
 * @return the time stamp
 */
struct timeval
mg_frame_get_timestamp(mg_frame_t frame);

/**
 * @brief Old frame indicator
 *
 * indicator to show whether the frame is considered to be
 * old/used.  if frame was constructed without a valid v4l2 buffer it is
 * considered to be already used.
 *
 * @param frame  object handle
 *
 * @return \c true if it has been used, else \c false
 */
bool
mg_frame_get_used(mg_frame_t frame);

/**
 * @brief User defined pointer accessor
 *
 * @param frame  object handle
 *
 * @return user defined pointer, or 0 on failure
 */
void *
mg_frame_get_userptr(mg_frame_t frame);

/**
 * @brief Old frame indicator accessor
 *
 * @param frame  object handle
 *
 * @return the object handle
 */
mg_frame_t
mg_frame_set_used(mg_frame_t frame);

__END_DECLS

#endif /* ITL_MULTI_GEE_MG_FRAME_H */
