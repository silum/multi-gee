/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "xmalloc.h"
#include "mg_buffer.h"

USE_XASSERT;

CLASS(mg_buffer, mg_buffer_t)
{
	bool used;
	struct timespec timestamp;
	unsigned int sequence;
	void **start;
	size_t *length;
	unsigned int number; /* of alloced buffers */
};

mg_buffer_t
mg_buffer_create()
{
	mg_buffer_t mg_buffer;
	NEWOBJ(mg_buffer);

	mg_buffer->used = false;

	mg_buffer->timestamp.tv_sec = 0;
	mg_buffer->timestamp.tv_nsec = 0;

	mg_buffer->sequence = 0;

	mg_buffer->start = 0;
	mg_buffer->length = 0;
	mg_buffer->number = 0;

	return mg_buffer;
}

mg_buffer_t
mg_buffer_destroy(mg_buffer_t mg_buffer)
{
	VERIFYZ(mg_buffer) {
		xfree(mg_buffer->start);
		xfree(mg_buffer->length);

		FREEOBJ(mg_buffer);
	}

	return 0;
}

int
mg_buffer_number(mg_buffer_t mg_buffer)
{
	int number = 0;

	VERIFY(mg_buffer) {
		number = mg_buffer->number;
	}

	return number;
}

mg_buffer_t
mg_buffer_alloc_buffer(mg_buffer_t mg_buffer,
		       unsigned int n)
{
	mg_buffer_t p = 0;

	VERIFY(mg_buffer) {
		if (!mg_buffer->number) {
			mg_buffer->start = xmalloc(n * sizeof(mg_buffer->start));
			mg_buffer->length = xmalloc(n * sizeof(mg_buffer->length));
			mg_buffer->number = n;
		}
		p = mg_buffer;
	}

	return p;
}

mg_buffer_t
mg_buffer_set_buffer(mg_buffer_t mg_buffer,
		     int n,
		     void *start,
		     size_t length)
{
	mg_buffer_t p = 0;

	VERIFY(mg_buffer) {
		if (mg_buffer->number > n
		    && mg_buffer->start[n] == 0
		    && mg_buffer->length[n] == 0) {
			mg_buffer->start[n] = start;
			mg_buffer->length[n] = length;
		}
		p = mg_buffer;
	}

	return p;
}

void *
mg_buffer_start(mg_buffer_t mg_buffer,
	       	int n)
{
	void *p = 0;

	VERIFY(mg_buffer) {
		if (mg_buffer->number > n)
			p = mg_buffer->start[n];
	}

	return p;
}


size_t /* length */
mg_buffer_length(mg_buffer_t mg_buffer,
		 int n)
{
	size_t s = 0;

	VERIFY(mg_buffer) {
		if (mg_buffer->number > n)
			s = mg_buffer->length[n];
	}

	return s;
}

mg_buffer_t
mg_buffer_set_timestamp(mg_buffer_t mg_buffer,
			unsigned int seq,
			struct timespec ts)
{
	mg_buffer_t p = 0;

	VERIFY(mg_buffer) {
		mg_buffer->sequence = seq;

		mg_buffer->timestamp.tv_sec = ts.tv_sec;
		mg_buffer->timestamp.tv_nsec = ts.tv_nsec;

		mg_buffer->used = false;
		p = mg_buffer;
	}

	return p;
}

unsigned int
mg_buffer_sequence(mg_buffer_t mg_buffer)
{
	unsigned int seq = 0;

	VERIFY(mg_buffer) {
		seq = mg_buffer->sequence;
	}

	return seq;
}

struct timespec
mg_buffer_timestamp(mg_buffer_t mg_buffer)
{
	struct timespec ts = {0, 0};

	VERIFY(mg_buffer) {
		 ts.tv_sec =  mg_buffer->timestamp.tv_sec;
		 ts.tv_nsec = mg_buffer->timestamp.tv_nsec;
	}

	return ts;
}

mg_buffer_t
mg_buffer_set_used(mg_buffer_t mg_buffer)
{
	mg_buffer_t p = 0;

	VERIFY(mg_buffer) {
		mg_buffer->used = true;
		p = mg_buffer;
	}

	return p;
}

bool
mg_buffer_used(mg_buffer_t mg_buffer)
{
	bool b = false;

	VERIFY(mg_buffer) {
		b = mg_buffer->used;
	}

	return b;
}

#ifdef DEBUG_BUFFER

#include <stdlib.h>
#include <stdio.h>

#include "debug_xassert.h"

void
verify_buffer(mg_buffer_t buffer,
	      int n,
	      unsigned int number,
	      int sequence,
	      struct timespec ts,
	      void *start,
	      size_t length,
	      bool used)
{
	struct timespec buffer_ts = mg_buffer_timestamp(buffer);
	xassert(buffer_ts.tv_sec == ts.tv_sec);
	xassert(buffer_ts.tv_nsec == ts.tv_nsec);

	xassert(mg_buffer_number(buffer) == number);

	xassert(mg_buffer_sequence(buffer) == sequence);
	xassert(mg_buffer_start(buffer, n) == start);
	xassert(mg_buffer_length(buffer, n) == length);
	xassert(mg_buffer_used(buffer) == used);
}

void
test_buffer(struct timespec ts,
	    unsigned int sequence,
	    void *start_0,
	    size_t length_0,
	    void *start_1,
	    size_t length_1)
{
	struct timespec ts_zero = {0, 0};

	/* create */
	mg_buffer_t buffer = mg_buffer_create();
	verify_buffer(buffer, 0, 0, 0, ts_zero, 0, 0, false);

	/* allocate buffers */
	buffer = mg_buffer_alloc_buffer(buffer, 2);
	verify_buffer(buffer, 0, 2, 0, ts_zero, 0, 0, 0);
	verify_buffer(buffer, 1, 2, 0, ts_zero, 0, 0, 0);
	verify_buffer(buffer, 2, 2, 0, ts_zero, 0, 0, 0);

	/* set start and length of buffers */
	buffer = mg_buffer_set_buffer(buffer, 0, start_0, length_0);
	buffer = mg_buffer_set_buffer(buffer, 1, start_1, length_1);
	buffer = mg_buffer_set_buffer(buffer, 2, start_1, length_1);
	verify_buffer(buffer, 0, 2, 0, ts_zero, start_0, length_0, false);
	verify_buffer(buffer, 1, 2, 0, ts_zero, start_1, length_1, false);
	verify_buffer(buffer, 2, 2, 0, ts_zero, 0, 0, false);

	/* try to reset start and length of a buffer */
	buffer = mg_buffer_set_buffer(buffer, 1, start_0, length_0);
	verify_buffer(buffer, 1, 2, 0, ts_zero, start_1, length_1, false);

	/* set the used state */
	buffer = mg_buffer_set_used(buffer);
	verify_buffer(buffer, 0, 2, 0, ts_zero, start_0, length_0, true);
	verify_buffer(buffer, 1, 2, 0, ts_zero, start_1, length_1, true);
	verify_buffer(buffer, 2, 2, 0, ts_zero, 0, 0, true);

	/* set the timestamp */
	buffer = mg_buffer_set_timestamp(buffer, sequence, ts);
	verify_buffer(buffer, 0, 2, sequence, ts, start_0, length_0, false);
	verify_buffer(buffer, 1, 2, sequence, ts, start_1, length_1, false);
	verify_buffer(buffer, 2, 2, sequence, ts, 0, 0, false);

	buffer = mg_buffer_destroy(buffer);
	xassert(buffer == 0);
}

void
mg_buffer()
{
	struct timespec ts_0 = {1000, 2000};
	struct timespec ts_1 = {3000, 4000};

	test_buffer(ts_0, 6666,
		    (void *) 0xdeadbeef, 7777,
		    (void *) 0xffffffff, 8888);
}

int
main()
{
	return debug_test(mg_buffer);
}

#endif /* def DEBUG_BUFFER */
