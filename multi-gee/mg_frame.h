/* $Id$ */
/*
 * multi-gee Frame Internals
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_FRAME_H
#define DSM_MG_FRAME_H 1

#include <stdint.h> /* uint32_t */
#include <stdbool.h> /* bool */

// #include <multi-gee/classdef.h>
#include <multi-gee/mg_device.h>

struct v4l2_buffer;

__BEGIN_DECLS

/* multi-gee frame */

NEWHANDLE(mg_frame_t); /* multi-gee frame object handle */

/**
 * @brief create frame object
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
 * @return a newly created frane object handle
 */
mg_frame_t
mg_frame_create(mg_device_t device,
		struct v4l2_buffer *buffer);

/**
 * @brief destroy frame object
 *
 * @param frame  handle of object to be destroyed
 *
 * @return 0
 */
mg_frame_t
mg_frame_destroy(mg_frame_t frame);

/**
 * @brief buffer index accessor
 *
 * the image pointer is read from the v4l2 buffer.  if frame was
 * constructed without a valid v4l2 buffer it is 0.
 *
 * @param frame  object handle
 *
 * @return the index number, or -1 if not a valid frame
 */
int
mg_frame_index(mg_frame_t frame);

/**
 * @brief capture device accessor
 *
 * @param frame  object handle
 *
 * @return the capture device object handle
 */
mg_device_t
mg_frame_device(mg_frame_t frame);

/**
 * @brief image data accessor
 *
 * @param frame  object handle
 *
 * @return a pointer to the image data, or 0 if no image data exists
 */
void *
mg_frame_image(mg_frame_t frame);

/**
 * @brief time stamp accessor
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
mg_frame_timestamp(mg_frame_t frame);

/**
 * @brief sequence number accessor
 *
 * the sequence number is read from the v4l2 buffer.  if frame was
 * constructed without a valid v4l2 buffer it is -1.
 *
 * @param frame  object handle
 *
 * @return the sequence number
 */
uint32_t
mg_frame_sequence(mg_frame_t frame);

/**
 * @brief old frame indicator
 *
 * indicator to show whether the frame is considdered to be
 * old/used.  if frame was constructed without a valid v4l2 buffer it is
 * considdered to be already used.
 *
 * @param frame  object handle
 *
 * @return true if it has been used, else false
 */
bool
mg_frame_used(mg_frame_t frame);

/**
 * @brief old frame indicator accessor
 *
 * @param frame  object handle
 *
 * @return the object handle
 */
mg_frame_t
mg_frame_set_used(mg_frame_t frame);

__END_DECLS

#endif /* DSM_MG_FRAME_H */
