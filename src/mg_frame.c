/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "mg_frame.h"
#include "multi-gee.h"

#include "xmalloc.h"

USE_XASSERT;

CLASS(mg_frame, mg_frame_t)
{
	multi_gee_t multi_gee;
	int device_id;
	unsigned char const **image;
	struct timespec timestamp;
	int sequence;
};

mg_frame_t
mg_frame_create(multi_gee_t multi_gee,
		int device_id,
		const unsigned char **image,
		struct timespec timestamp,
		int sequence)
{
	mg_frame_t mg_frame = 0;
	NEWOBJ(mg_frame);

	mg_frame->multi_gee = multi_gee;
	mg_frame->device_id = device_id;
	mg_frame->image = image;
	mg_frame->timestamp = timestamp;
	mg_frame->sequence = sequence;

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

int
mg_frame_device_id(mg_frame_t mg_frame)
{
	int device_id = -1;

	VERIFY(mg_frame) {
		device_id = mg_frame->device_id;
	}

	return device_id;
}

const unsigned char **
mg_frame_image(mg_frame_t mg_frame)
{
	const unsigned char **image = 0;

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

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include "debug_xassert.h"
#include "multi-gee.h"

void
test_frame(multi_gee_t multi_gee,
	   int device_id,
	   unsigned char const ** image,
	   struct timespec timestamp,
	   int sequence)
{

	mg_frame_t frame = mg_frame_create(multi_gee, device_id, image, timestamp, sequence);

	xassert(mg_frame_multi_gee(frame) == multi_gee);
	xassert(mg_frame_device_id(frame) == device_id);
	xassert(mg_frame_image(frame) == image);

	struct timespec ts = mg_frame_timestamp(frame);
	xassert(ts.tv_sec == timestamp.tv_sec);
	xassert(ts.tv_nsec == timestamp.tv_nsec);

	xassert(mg_frame_sequence(frame) == sequence);

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

	test_frame((multi_gee_t) 1, 2, (unsigned char const **) 3, timestamp, 4);
}

int
main()
{
	return debug_test(mg_frame);
}

#endif /* def DEBUG */
