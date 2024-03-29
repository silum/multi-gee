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
 * @brief Multi-gee Frame definition
 */
#include <sys/time.h> /* gettimeofday */

#include <asm/types.h> /* needed for videodev2.h */
#include <linux/videodev2.h> /* struct v4l2_buffer */

#include <config.h>

#include "mg_frame.h" /* class implemented */
#include "mg_device.h"
#include "multi-gee.h"

USE_XASSERT

/**
 * @brief Frame object structure
 */
CLASS(mg_frame, mg_frame_t)
{
	mg_device_t device; /**< Device object handle */
	unsigned int index; /**< Device buffer index */
	struct timeval timestamp; /**< Frame time stamp */
	uint32_t sequence; /**< Frame sequence number */
	bool used; /**< Frame already processed by user? */
};

mg_frame_t
mg_frame_create(mg_device_t mg_device,
		struct v4l2_buffer *buf)
{
	mg_frame_t mg_frame = 0;
	NEWOBJ(mg_frame);

	mg_frame->device = mg_device;

	if (buf) {
		mg_frame->index = buf->index;
		mg_frame->timestamp = buf->timestamp;
#ifdef ENABLE_SYNC_HACK
         // Slow device hack, to avoid fatal sync errors on ctpsg system
         if ((mg_frame->timestamp.tv_sec == 0) && (mg_frame->timestamp.tv_usec == 0))
         {
            gettimeofday(&mg_frame->timestamp, 0);
         }
#endif
		mg_frame->sequence = buf->sequence;
	} else {
		mg_frame->index = -1;
		gettimeofday(&mg_frame->timestamp, 0);
		mg_frame->sequence = -1;
	}

	mg_frame->used = (buf) ? false : true;

	return mg_frame;
}

mg_frame_t
mg_frame_destroy(mg_frame_t mg_frame)
{
	VERIFYZ(mg_frame) {
		FREEOBJ(mg_frame);
	}

	return 0;
}

mg_device_t
mg_frame_get_device(mg_frame_t mg_frame)
{
	mg_device_t device = 0;

	VERIFY(mg_frame) {
		device = mg_frame->device;
	}

	return device;
}

void *
mg_frame_get_image(mg_frame_t mg_frame)
{
	void *image = 0;

	VERIFY(mg_frame) {
		mg_buffer_t buf = mg_device_get_buffer(mg_frame->device);
		image = mg_buffer_get_start(buf, mg_frame->index);
	}

	return image;
}

int
mg_frame_get_index(mg_frame_t mg_frame)
{
	int index = -1;

	VERIFY(mg_frame) {
		index = mg_frame->index;
	}

	return index;
}

uint32_t
mg_frame_get_sequence(mg_frame_t mg_frame)
{
	int sequence = -1;

	VERIFY(mg_frame) {
		sequence = mg_frame->sequence;
	}

	return sequence;
}

struct timeval
mg_frame_get_timestamp(mg_frame_t mg_frame)
{
	struct timeval timestamp = {0, 0};

	VERIFY(mg_frame) {
		timestamp = mg_frame->timestamp;
	}
	return timestamp;
}

bool
mg_frame_get_used(mg_frame_t mg_frame)
{
	bool used = false;

	VERIFY(mg_frame) {
		used = mg_frame->used;
	}

	return used;
}

void *
mg_frame_get_userptr(mg_frame_t mg_frame)
{
	void *userptr = 0;

	VERIFY(mg_frame) {
		userptr = mg_device_get_userptr(mg_frame->device);
	}

	return userptr;
}

mg_frame_t
mg_frame_set_used(mg_frame_t mg_frame)
{
	mg_frame_t frame = 0;

	VERIFY(mg_frame) {
		mg_frame->used = true;
		frame = mg_frame;
	}

	return frame;
}

#ifdef TEST_MULTI_GEE_MG_FRAME

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "multi-gee.h"

void
test_frame(mg_device_t device,
	   void *image,
	   struct timeval timestamp,
	   uint32_t sequence)
{
	printf("%s(%p, %p, {%ld, %ld}, %d)\n",
	       __func__,
	       device,
	       image,
	       timestamp.tv_sec,
	       timestamp.tv_usec,
	       sequence);

	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(buf));
	buf.index = 0;
	buf.timestamp = timestamp;
	buf.sequence = sequence;

	mg_frame_t frame = mg_frame_create(device, &buf);

	XASSERT(mg_frame_get_device(frame) == device) {
		/* empty */
	}
	XASSERT(mg_frame_get_image(frame) == image) {
		/* empty */
	}

	struct timeval tv = mg_frame_get_timestamp(frame);
	XASSERT(tv.tv_sec == timestamp.tv_sec) {
		/* empty */
	}
	XASSERT(tv.tv_usec == timestamp.tv_usec) {
		/* empty */
	}

	XASSERT(mg_frame_get_sequence(frame) == sequence) {
		/* empty */
	}

	XASSERT(mg_frame_get_used(frame) == false) {
		/* empty */
	}
	frame = mg_frame_set_used(frame);
	XASSERT(mg_frame_get_used(frame) == true) {
		/* empty */
	}
	frame = mg_frame_set_used(frame);
	XASSERT(mg_frame_get_used(frame) == true) {
		/* empty */
	}

	XASSERT(mg_frame_get_userptr(frame) ==
		mg_device_get_userptr(device)) {
		/* empty */
	}

	mg_frame_destroy(frame);
}

void
mg_frame()
{
	log_t log = lg_create("mg_frame", "stderr");
	struct timeval timestamp = {0 , 0};
	mg_device_t mg_device = mg_device_create("/dev/null", 3, log,
						 (void *) 0xdeadbeef);
	mg_buffer_t mg_buffer = mg_device_get_buffer(mg_device);
	mg_buffer_alloc(mg_buffer, 1);

	test_frame(mg_device, 0, timestamp, 0);

	timestamp.tv_sec = 300;
	timestamp.tv_usec = 400;

	test_frame(mg_device, 0, timestamp, 0);

	mg_buffer_set(mg_buffer, 0, (void *) 3, 0);

	test_frame(mg_device, (void *)3, timestamp, 4);

	mg_device = mg_device_destroy(mg_device);

	log = lg_destroy(log);
}

int
main()
{
	exit(cclass_assert_test(mg_frame));
}

#endif /* DEBUG_MG_FRAME */
