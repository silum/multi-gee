/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "mg_frame.h"
#include "mg_device.h"
#include "multi-gee.h"

#include "xmalloc.h"

USE_XASSERT;

CLASS(mg_frame, mg_frame_t)
{
	multi_gee_t multi_gee;
	mg_device_t device;
	void *image;
	struct timespec timestamp;
	int sequence;
	bool used;
};

mg_frame_t
mg_frame_create(multi_gee_t multi_gee,
		mg_device_t mg_device,
		void *image,
		struct timespec timestamp,
		int sequence)
{
	mg_frame_t mg_frame = 0;
	NEWOBJ(mg_frame);

	mg_frame->multi_gee = multi_gee;
	mg_frame->device = mg_device;
	mg_frame->image = image;
	mg_frame->timestamp = timestamp;
	mg_frame->sequence = sequence;
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

multi_gee_t
mg_frame_multi_gee(mg_frame_t mg_frame)
{
	multi_gee_t multi_gee = 0;

	VERIFY(mg_frame) {
		multi_gee = mg_frame->multi_gee;
	}

	return multi_gee;
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
		image = mg_frame->image;
	}

	return image;
}

struct timespec
mg_frame_timestamp(mg_frame_t mg_frame)
{
	struct timespec timestamp = { 0, 0 };

	VERIFY(mg_frame) {
		timestamp = mg_frame->timestamp;
	}

	return timestamp;
}

int
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

#include "xmalloc.h"
#include "debug_xassert.h"
#include "multi-gee.h"

void
test_frame(multi_gee_t multi_gee,
	   mg_device_t device,
	   void *image,
	   struct timespec timestamp,
	   int sequence)
{

	mg_frame_t frame = mg_frame_create(multi_gee, device, image, timestamp, sequence);

	xassert(mg_frame_multi_gee(frame) == multi_gee);
	xassert(mg_frame_device(frame) == device);
	xassert(mg_frame_image(frame) == image);

	struct timespec ts = mg_frame_timestamp(frame);
	xassert(ts.tv_sec == timestamp.tv_sec);
	xassert(ts.tv_nsec == timestamp.tv_nsec);

	xassert(mg_frame_sequence(frame) == sequence);

	xassert(mg_frame_used(frame) == false);
	frame = mg_frame_set_used(frame);
	xassert(mg_frame_used(frame) == true);
	frame = mg_frame_set_used(frame);
	xassert(mg_frame_used(frame) == true);

	mg_frame_destroy(frame);
}

void
mg_frame()
{
	struct timespec timestamp = {0 , 0};
	test_frame(0, 0, 0, timestamp, 0);

	timestamp.tv_sec = 300;
	timestamp.tv_nsec = 400;

	test_frame(0, 0, 0, timestamp, 0);

	test_frame((multi_gee_t) 1, (mg_device_t) 2, (void *)3, timestamp, 4);
}

int
main()
{
	return debug_test(mg_frame);
}

#endif /* def DEBUG_FRAME */
