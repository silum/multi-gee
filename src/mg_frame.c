/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include <stdint.h>
#include <sys/time.h>

#include "mg_frame.h"
#include "mg_device.h"
#include "multi-gee.h"

#include "xmalloc.h"

USE_XASSERT;

CLASS(mg_frame, mg_frame_t)
{
	mg_device_t device;
	unsigned int index; /* device buffer index */
	struct timeval timestamp;
	uint32_t sequence;
	bool used;
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
		mg_frame->sequence = buf->sequence;
	} else {
		mg_frame->index = -1;
		struct timeval timestamp = {0, 0};
		gettimeofday(&timestamp, 0);
		mg_frame->timestamp = timestamp;
		mg_frame->sequence = 0;
	}

	mg_frame->used = false;

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

int
mg_frame_index(mg_frame_t mg_frame)
{
	int index = -1;
	VERIFY(mg_frame) {
		index = mg_frame->index;
	}

	return index;
}

mg_device_t
mg_frame_device(mg_frame_t mg_frame)
{
	mg_device_t device = 0;

	VERIFY(mg_frame) {
		device = mg_frame->device;
	}

	return device;
}

void *
mg_frame_image(mg_frame_t mg_frame)
{
	void *image = 0;

	VERIFY(mg_frame) {
		mg_buffer_t buf = mg_device_buffer(mg_frame->device);
		image = mg_buffer_start(buf, mg_frame->index);
	}

	return image;
}

struct timeval
mg_frame_timestamp(mg_frame_t mg_frame)
{
	struct timeval timestamp = {0, 0};

	VERIFY(mg_frame) {
		timestamp = mg_frame->timestamp;
	}

	return timestamp;
}

uint32_t
mg_frame_sequence(mg_frame_t mg_frame)
{
	int sequence = -1;

	VERIFY(mg_frame) {
		sequence = mg_frame->sequence;
	}

	return sequence;
}

bool
mg_frame_used(mg_frame_t mg_frame)
{
	bool used = false;
	VERIFY(mg_frame) {
		used = mg_frame->used;
	}

	return used;
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

#ifdef DEBUG_FRAME

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xmalloc.h"
#include "debug_xassert.h"
#include "multi-gee.h"

void
test_frame(mg_device_t device,
	   void *image,
	   struct timeval timestamp,
	   uint32_t sequence)
{

	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(buf));
	buf.index = 0;
	buf.timestamp = timestamp;
	buf.sequence = sequence;

	mg_frame_t frame = mg_frame_create(device, &buf);

	XASSERT(mg_frame_device(frame) == device);
	XASSERT(mg_frame_image(frame) == image);

	struct timeval tv = mg_frame_timestamp(frame);
	XASSERT(tv.tv_sec == timestamp.tv_sec);
	XASSERT(tv.tv_usec == timestamp.tv_usec);

	XASSERT(mg_frame_sequence(frame) == sequence);

	XASSERT(mg_frame_used(frame) == false);
	frame = mg_frame_set_used(frame);
	XASSERT(mg_frame_used(frame) == true);
	frame = mg_frame_set_used(frame);
	XASSERT(mg_frame_used(frame) == true);

	mg_frame_destroy(frame);
}

void
mg_frame()
{
	struct timeval timestamp = {0 , 0};
	mg_device_t mg_device = mg_device_create("/dev/null");
	mg_buffer_t mg_buffer = mg_device_buffer(mg_device);
	mg_buffer_alloc(mg_buffer, 1);

	test_frame(mg_device, 0, timestamp, 0);

	timestamp.tv_sec = 300;
	timestamp.tv_usec = 400;

	test_frame(mg_device, 0, timestamp, 0);

	mg_buffer_set(mg_buffer, 0, (void *) 3, 0);

	test_frame(mg_device, (void *)3, timestamp, 4);

	mg_device = mg_device_destroy(mg_device);
}

int
main()
{
	return debug_test(mg_frame);
}

#endif /* def DEBUG_FRAME */
