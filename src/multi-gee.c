/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include <assert.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "multi-gee.h"

#include "fg_util.h"
#include "mg_device.h"
#include "mg_frame.h"
#include "sllist.h"
#include "tv_util.h"

#include "xmalloc.h"

static struct timeval TV_IN_SYNC = {0, 22000}; /* 55% of framerate */
static struct timeval TV_NO_SYNC = {0, 120000}; /* 3 frames */

USE_XASSERT;

CLASS(multi_gee, multi_gee_t)
{
	bool busy;
	bool halt;
	bool refresh;

	void (*callback)(sllist_t frame);

	sllist_t frame;
	sllist_t device;
};

/**
 * @brief enqueue old frame, dequeue new frame
 */
static sllist_t
swap_frame(sllist_t frame_list,
	   mg_device_t dev)
{
	int fd = mg_device_fd(dev);
	// printf("fd = %d\n", fd);

	struct v4l2_buffer buf;
	if (!fg_dequeue(fd, &buf))
		return frame_list;

	mg_buffer_t dev_buf = mg_device_buffer(dev);
	assert(buf.index < mg_buffer_number(dev_buf));

	for (sllist_t f = frame_list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		if (dev == mg_frame_device(frame)) {
			int index = mg_frame_index(frame);
			if (index >= 0)
				if (!fg_enqueue(mg_device_fd(dev), index))
					return frame_list;

			frame_list = sll_remove_data(frame_list, frame);
			frame = mg_frame_destroy(frame);
			frame = mg_frame_create(dev, &buf);
			frame_list = sll_insert_data(frame_list, frame);

			break;
		}
	}

	return frame_list;
}

/**
 * @brief create a list of all device frames
 *
 * @desc frames without corresponding devices are removed while frames
 * for devices without correspoding frames are added
 *
 * @param frame  a list of all previously added frames
 * @param device  a list of devices
 */
static sllist_t
add_frame(sllist_t frame,
	  sllist_t device)
{
	sllist_t list = 0;

	for (sllist_t d = device; d; d = sll_next(d)) {
		bool found = false;
		mg_device_t dev = sll_data(d);

		for (sllist_t f = frame; f; f = sll_next(f)) {
			if (mg_frame_device(sll_data(f)) == dev) {
				list = sll_insert_data(list, f);
				found = true;
				break;
			}
		}

		if (!found) {
			mg_frame_t new_frame = mg_frame_create(dev, 0);
			list = sll_insert_data(list, new_frame);
		}
	}

	for (sllist_t f = frame; f; f = sll_next(f))
		mg_frame_destroy(sll_data(f));

	frame = sll_empty(frame);
	return list;
}

enum sync_status
{
	SYNC_FATAL = -1,
	SYNC_OK,
	SYNC_FAIL
};


/*
	TODO
 */
static enum sync_status
sync_test(sllist_t frame_list)
{
	enum sync_status sync = SYNC_FAIL;

	static struct timeval last_sync = {0, 0};

	if (last_sync.tv_sec == 0
	    && last_sync.tv_usec == 0)
		gettimeofday(&last_sync, 0);

	struct timeval now;
	gettimeofday(&now, 0);

	if (tv_lt(TV_NO_SYNC, tv_abs_diff(last_sync, now))) {
		sync = SYNC_FATAL;
	} else if (frame_list) {
		mg_frame_t frame = sll_data(frame_list);
		struct timeval tv_max = mg_frame_timestamp(frame);
		struct timeval tv_min = mg_frame_timestamp(frame);

		bool ready = true;
		ready &= !mg_frame_used(frame);

		for (sllist_t f = sll_next(frame_list); f; f = sll_next(f)) {
			frame = sll_data(f);
			struct timeval tv = mg_frame_timestamp(frame);
			if (tv_lt(tv, tv_min))
				tv_min = tv;
			if (tv_lt(tv_max, tv))
				tv_max = tv;
			ready &= !mg_frame_used(frame);
		}

		struct timeval tv_diff = tv_abs_diff(tv_min, tv_max);
		if (ready && tv_lt(tv_diff, TV_IN_SYNC)) {
			for (sllist_t f = frame_list; f; f = sll_next(f))
				mg_frame_set_used(sll_data(f));
			last_sync = now;
			sync = SYNC_OK;
		} else if (tv_lt(TV_NO_SYNC, tv_diff)) {
			sync = SYNC_FATAL;
		}
	}
	return sync;
}

/*
 */
static
mg_device_t
find_device_id(sllist_t list,
	       int seek)
{
	mg_device_t found = 0;
	while (list) {
		found = sll_data(list);
		if (mg_device_fd(found) == seek)
			break;

		list = sll_next(list);
	}

	return found;
}

static enum sync_status
sync_capture(multi_gee_t multi_gee)
{
	bool sync = SYNC_FAIL;

	VERIFY(multi_gee) {

		if (multi_gee->refresh) {
			multi_gee->frame = add_frame(multi_gee->frame,
						     multi_gee->device);
			multi_gee->refresh = false;
		}

		for (;;) {
			fd_set fds;

			FD_ZERO(&fds);

			int max_fd = -1;
			for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
				int fd = mg_device_fd(sll_data(d));
				max_fd = (fd > max_fd) ? fd : max_fd;
				FD_SET(fd, &fds);
			}
			max_fd += 1;

			struct timeval timeout = TV_NO_SYNC;
			int ret = select(max_fd, &fds, NULL, NULL, &timeout);

			if (-1 == ret) {
				if (EINTR == errno)
					continue;

				ferrno(stderr, "select");
				return false;
			}

			if (0 == ret) {
				fprintf(stderr, "select timeout\n");
				return false;
			}

			for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
				mg_device_t dev = sll_data(d);
				if (FD_ISSET(mg_device_fd(dev), &fds)) {
					multi_gee->frame = swap_frame(multi_gee->frame, dev);
					sync = sync_test(multi_gee->frame);
				}
			}
			if (sync != SYNC_FAIL)
				break;

			/* EAGAIN - continue select loop. */
		}
	}
	return sync;
}

multi_gee_t
mg_create()
{
	multi_gee_t multi_gee;
	NEWOBJ(multi_gee);

	multi_gee->busy = false;
	multi_gee->halt = false;
	multi_gee->refresh = false;

	multi_gee->callback = 0;

	multi_gee->frame = 0;
	multi_gee->device = 0;

	return multi_gee;
}

multi_gee_t
mg_destroy(multi_gee_t multi_gee)
{
	VERIFYZ(multi_gee) {
		for (sllist_t d = multi_gee->device; d; d = sll_next(d))
			mg_device_destroy(sll_data(d));
		multi_gee->device = sll_empty(multi_gee->device);
		multi_gee->frame = add_frame(multi_gee->frame, 0);
		FREEOBJ(multi_gee);
	}

	return 0;
}

enum mg_RETURN {
	RET_CALLBACK = -5,
	RET_SYNC,
	RET_BUSY,
	RET_HALT,
	RET_DEVICE
};

int
mg_capture(multi_gee_t multi_gee,
	   int n)
{
	int ret = -3;

	VERIFY(multi_gee) {
		int count = 0;
		bool sync = SYNC_OK;
		bool done = false;

		if (multi_gee->busy) {
			done = true;
			ret = RET_BUSY;
		} else {
			multi_gee->busy = true;
		}

		while (!done) {
			/* assume we are done */
			done = true;

			/* find a reason to be done */
			if (!multi_gee->callback)
				ret = RET_CALLBACK;
			else if (multi_gee->halt)
				ret = RET_HALT;
			else if (!multi_gee->device)
				ret = RET_DEVICE;
			else if (n >= 0 && count >= n)
				ret = count;
			else if (sync == SYNC_FATAL)
				ret = RET_SYNC;
			else {
				/* ok, so we're not done yet */
				done = false;

				sync = sync_capture(multi_gee);
				if (sync == SYNC_OK) {
					multi_gee->callback(multi_gee->frame);
					count++;
				}
			}
		}
		multi_gee->busy = false;
	}
	return ret;
}

void
mg_capture_halt(multi_gee_t multi_gee)
{
	VERIFY(multi_gee) {
		multi_gee->halt = true;
	}
}

multi_gee_t
mg_register_callback(multi_gee_t multi_gee,
		     void (*callback)(const sllist_t))
{
	multi_gee_t p = 0;

	VERIFY(multi_gee) {
		if (callback)
			multi_gee->callback = callback;
		p = multi_gee;
	}

	return p;
}


static
mg_device_t
find_device(sllist_t list,
	    mg_device_t seek)
{
	int major = mg_device_major(seek);
	int minor = mg_device_minor(seek);

	for (sllist_t d = list; d; d = sll_next(d)) {
		mg_device_t found = sll_data(d);

		if (mg_device_major(found) == major
		    && mg_device_minor(found) == minor)
			return found;
	}

	return 0;
}

int
mg_register_device(multi_gee_t multi_gee,
		   char *name)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t dev = mg_device_create(name);

		/* can device be registered? */
		if (mg_device_major(dev) == -1
		    || mg_device_minor(dev) == -1) {
			dev = mg_device_destroy(dev);
			ret = -1;
		} else {
			/* is device already registered? */
			mg_device_t dup = find_device(multi_gee->device, dev);
			if (dup) {
				dev = mg_device_destroy(dev);
				ret = mg_device_fd(dup);
			}
		}
		
		/* everything ok, add it to list */
		if (dev) {
			multi_gee->refresh = true;

			multi_gee->device = sll_insert_data(multi_gee->device, dev);

			mg_device_open(dev);
			ret = mg_device_fd(dev);

			if (-1 != ret) {
				if (!fg_init_device(dev)
				    || !fg_start_capture(dev))
					ret = -1;
			}
		}
	}

	return ret;
}

int
mg_deregister_device(multi_gee_t multi_gee,
		     int id)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t device = find_device_id(multi_gee->device, id);
		if (device) {
			multi_gee->device = sll_remove_data(multi_gee->device, device);

			/* ignore failures */
			fg_stop_capture(device);
			fg_uninit_device(device);

			mg_device_destroy(device);
			ret = id;

			multi_gee->refresh = true;
		}
	}

	return ret;
}

#ifdef DEBUG_MULTI_GEE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "xmalloc.h"
#include "debug_xassert.h"

static void
process_images(sllist_t frame_list)
{
	static struct timeval then = {0, 0};
	static int count = 0;

	struct timeval now;
	gettimeofday(&now, 0);
	printf("now: %10ld.%06ld\n", now.tv_sec, now.tv_usec);

	struct timeval diff = diff = tv_abs_diff(then, now);
	printf("  then now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	then = now;

	printf("  count   : %d\n", count++);

	for (sllist_t f = frame_list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		printf("dev: %s\n", mg_device_name(mg_frame_device(frame)));
		struct timeval tv = mg_frame_timestamp(frame);

		printf(" tv: %10ld.%06ld\n",  tv.tv_sec,  tv.tv_usec);


		diff = tv_abs_diff(tv, now);
		printf("  tv   now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);
		printf("  sequence: %d\n", mg_frame_sequence(frame));

	}
	printf("\n");
}

void
multi_gee()
{
	multi_gee_t mg = 0;

	mg = mg_create();
	XASSERT(mg);

	mg_register_callback(mg, process_images);

	printf("dev id = %d\n", mg_register_device(mg, "/dev/video0"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video1"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video2"));

	printf("capture ret = %d\n", mg_capture(mg, 3));

	// mg_deregister_device(mg, dev_id);

	mg_destroy(mg);
}

int
main()
{
	return debug_test(multi_gee);
}

#endif /* def DEBUG_MULTI_GEE */
