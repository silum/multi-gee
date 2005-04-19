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
 * @file mg_buffer.c
 * @brief Multi-gee capture buffer definition
 */

#include <xmalloc.h>

#include "mg_buffer.h"

USE_XASSERT

/**
 * @brief Buffer object structure
 */
CLASS(mg_buffer, mg_buffer_t)
{
	void **start; /**< Pointer to first byte of buffer memory */
	size_t *length; /**< Size of buffer memory area */
	unsigned int number; /**< Number of allocated buffers */
};

mg_buffer_t
mg_buffer_create()
{
	mg_buffer_t mg_buffer;
	NEWOBJ(mg_buffer);

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

mg_buffer_t
mg_buffer_alloc(mg_buffer_t mg_buffer,
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

size_t
mg_buffer_length(mg_buffer_t mg_buffer,
		 unsigned int n)
{
	size_t s = 0;

	VERIFY(mg_buffer) {
		if (mg_buffer->number > n)
			s = mg_buffer->length[n];
	}

	return s;
}

unsigned int
mg_buffer_number(mg_buffer_t mg_buffer)
{
	int number = 0;

	VERIFY(mg_buffer) {
		number = mg_buffer->number;
	}

	return number;
}

mg_buffer_t
mg_buffer_set(mg_buffer_t mg_buffer,
	      unsigned int n,
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
		unsigned int n)
{
	void *p = 0;

	VERIFY(mg_buffer) {
		if (mg_buffer->number > n)
			p = mg_buffer->start[n];
	}

	return p;
}

#ifdef DEBUG_BUFFER

#include <stdlib.h>
#include <stdio.h>

#include <xassert.h>

void
verify_buffer(mg_buffer_t buffer,
	      unsigned int n,
	      unsigned int number,
	      void *start,
	      size_t length)
{
	xassert(mg_buffer_number(buffer) == number) {}
	xassert(mg_buffer_start(buffer, n) == start) {}
	xassert(mg_buffer_length(buffer, n) == length) {}
}

void
test_buffer(void *start_0,
	    size_t length_0,
	    void *start_1,
	    size_t length_1)
{
	/* create */
	mg_buffer_t buffer = mg_buffer_create();
	verify_buffer(buffer, 0, 0, 0, 0);

	/* allocate buffers */
	buffer = mg_buffer_alloc(buffer, 2);
	verify_buffer(buffer, 0, 2, 0, 0);
	verify_buffer(buffer, 1, 2, 0, 0);
	verify_buffer(buffer, 2, 2, 0, 0);

	/* set start and length of buffers */
	buffer = mg_buffer_set(buffer, 0, start_0, length_0);
	buffer = mg_buffer_set(buffer, 1, start_1, length_1);
	buffer = mg_buffer_set(buffer, 2, start_1, length_1);
	verify_buffer(buffer, 0, 2, start_0, length_0);
	verify_buffer(buffer, 1, 2, start_1, length_1);
	verify_buffer(buffer, 2, 2, 0, 0);

	/* try to reset start and length of a buffer */
	buffer = mg_buffer_set(buffer, 1, start_0, length_0);
	verify_buffer(buffer, 1, 2, start_1, length_1);

	/* destroy */
	buffer = mg_buffer_destroy(buffer);
	xassert(buffer == 0) {}
}

void
mg_buffer()
{
	test_buffer(0, 0, 0, 0);

	test_buffer((void *) 0xdeadbeef, 7777,
		    (void *) 0xffffffff, 8888);
}

int
main()
{
	return debug_test(mg_buffer);
}

#endif /* def DEBUG_BUFFER */
