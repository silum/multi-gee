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
 * @brief Multi-gee Frame Grabber Library definition
 */
#include <cclass/xmalloc.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <asm/types.h> /* needed for videodev2.h */
#include <linux/videodev2.h> /* struct v4l2_buffer */

#include "fg_util.h"
#include "log.h"
#include "mg_device.h"
#include "mg_frame.h"
#include "multi-gee.h" /* class implemented */
#include "sllist.h"
#include "tv_util.h"

USE_XASSERT

/**
 * @brief Synchronisation status
 */
enum sync_status
{
	SYNC_FATAL = -1,  /**< fatal loss of sync */
	SYNC_OK, /**< frames in sync */
	SYNC_FAIL /**< frames not in sync */
};

/**
 * @brief Find device in list given the file descriptor
 *
 * @param list  device list object handle
 * @param fd  file descriptor
 *
 * @return device handle, or 0 if device not in list
 */
static mg_device_t
find_device_fd(sllist_t list,
	       int fd);

/**
 * @brief Find a frame in a list given the capture device
 *
 * @param list  frame list object handle
 * @param device  object handle
 *
 * @return frame handle, or 0 if frame not in list
 */
static mg_frame_t
find_frame_device(sllist_t list,
		  mg_device_t device);

/**
 * @brief Find device in list with given its device number
 *
 * @param list  device list object handle
 * @param devno  device number
 *
 * @return device handle, or 0 if device not in list
 */
static mg_device_t
find_device_number(sllist_t list,
		   dev_t devno);

/**
 * @brief Create a list of device frames
 *
 * makes a list containing frames from devices in the device list.  the
 * frame list is taken as a starting point, and frames for devices not
 * in the device list are removed.
 *
 * @param frame  frame list object handle
 * @param device  device list object handle
 *
 * @return a frame list only containing frames from devices in the
 * device list
 */
static
sllist_t
add_frame(sllist_t frame,
	  sllist_t device);

/**
 * @brief Enqueue old frame, dequeue new frame
 *
 * swaps current scratch frame with frame filled by capture device
 *
 * @param multi_gee  object handle
 * @param device  object handle
 *
 * @return \c true of the frame was successfully swapped, \c false if an
 * error occurred
 */
static bool
swap_frame(multi_gee_t multi_gee,
	   mg_device_t device);

/**
 * @brief Tests frame list for sync
 *
 * all frames are in sync when the maximum difference in time stamps are
 * less than TV_IN_SYNC.  if the time elapsed since the previous sync
 * condition was more than TV_NO_SYNC, or when the maximum difference in
 * time stamps are more than TV_NO_SYNC, a fatal condition exists
 *
 * @param multi_gee  object handle
 *
 * @return sync status
 */
static enum sync_status
sync_test(multi_gee_t multi_gee);

/**
 * @brief Monitors all devices for capture events
 *
 * performs a select on the fd_set.  if any activity on the set occurs
 * within a timeout period of TV_NO_SYNC, the function returns with a
 * non-fatal sync status.  If the select fails, or times out, a fatal
 * condition status is returned.
 *
 * @param multi_gee  device object
 * @param fds  fd_set to test for availability of a captured frame
 *
 * @return sync status
 */
static enum sync_status
sync_select(multi_gee_t multi_gee,
	    fd_set *fds);

/**
 * @brief Multi-gee object structure
 */
CLASS(multi_gee, multi_gee_t)
{
	bool busy; /**< \c true while mg_capture() in progress */
	bool halt; /**< \c true if mg_capture_halt() called */
	bool refresh; /**< Need to refresh device list? */

	void (*callback)(multi_gee_t, sllist_t); /**< Pointer to user
						   defined callback
						   function */

	sllist_t frame; /**< List of frames */
	sllist_t device; /**< List of devices */

	struct timeval last_sync; /**< Time stamp when last in sync */

	log_t log; /**< Log object handle */

	struct timeval TV_IN_SYNC; /**< Frames in sync criterion */
	struct timeval TV_NO_SYNC; /**< Failure to achieve sync criterion */
	struct timeval TV_SUB; /**< Capture start offset */

	unsigned int num_bufs; /** Number of capture buffers */
};

multi_gee_t
mg_create(char* log_file)
{
	multi_gee_t multi_gee;
	NEWOBJ(multi_gee);

	multi_gee->busy = false;
	multi_gee->halt = false;
	multi_gee->refresh = false;

	multi_gee->callback = 0;

	multi_gee->frame = 0;
	multi_gee->device = 0;

	timerclear(&multi_gee->last_sync);

	multi_gee->log = lg_create("multi-gee", log_file);

	timerset(&multi_gee->TV_IN_SYNC, 0, 21000); /* 55% of frame rate */
	timerset(&multi_gee->TV_NO_SYNC, 0, 168000); /* 4 frames + 5% */
	timerset(&multi_gee->TV_SUB, 0, 84000); /* 2 frames + 5% */

	multi_gee->num_bufs = 3;

	lg_log(multi_gee->log, "startup");

	return multi_gee;
}

multi_gee_t
mg_create_special(char* log_file,
		  struct timeval tv_in_sync,
		  struct timeval tv_no_sync,
		  struct timeval tv_sub,
		  unsigned int num_bufs)
{
	multi_gee_t multi_gee = mg_create(log_file);

	if (multi_gee) {
		multi_gee->TV_IN_SYNC = tv_in_sync;
		multi_gee->TV_NO_SYNC = tv_no_sync;
		multi_gee->TV_SUB = tv_sub;
		multi_gee->num_bufs = num_bufs;
	}
	return multi_gee;
}

multi_gee_t
mg_destroy(multi_gee_t multi_gee)
{
	VERIFYZ(multi_gee) {
		while (multi_gee->device) {
			int id = mg_device_fd(sll_data(multi_gee->device));
			mg_deregister_device(multi_gee, id);
		}
		multi_gee->device = sll_empty(multi_gee->device);
		multi_gee->frame = add_frame(multi_gee->frame, 0);
		multi_gee->log = lg_destroy(multi_gee->log);
		FREEOBJ(multi_gee);
	}

	return 0;
}

#if DEBUG_SELECT
/**
 * @brief Print struct timeval value
 *
 * struct timeval values always maintain the tv_usec member as positive.
 * The actual value is tv_sec + tv_usec / 1000000.  Thus -0.1, for
 * instance, is internally represented by {-1, 900000}.
 *
 * @param str  string to prefix to value -- may be ""
 * @param tv  timeval to print
 */
static void
print_tv(char *str, struct timeval tv)
{
	if (tv.tv_sec == -1) {
		printf("%s%10s.%06ld", str, "-0", 1000000 - tv.tv_usec);
	} else if (tv.tv_sec < 0) {
		printf("%s%10ld.%06ld", str, tv.tv_sec - 1, 1000000 - tv.tv_usec);
	} else {
		printf("%s%10ld.%06ld", str, tv.tv_sec, tv.tv_usec);
	}
}
#endif

enum mg_RETURN
mg_capture(multi_gee_t multi_gee,
	   int n)
{
	int ret = RET_UNDEF;

	VERIFY(multi_gee) {
		int count = 0;
		enum sync_status sync = SYNC_OK;
		bool done = false;

		if (multi_gee->busy) {
			done = true;
			ret = RET_BUSY;
		} else {
			multi_gee->busy = true;
		}

		/* update sync time to now */
		gettimeofday(&multi_gee->last_sync, 0);
		timersub(&multi_gee->last_sync,
			 &multi_gee->TV_SUB,
			 &multi_gee->last_sync);

#if DEBUG_SELECT
		print_tv("capture start:   ", multi_gee->last_sync); printf("\n");
#endif

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
				/* OK, so we're not done yet */
				done = false;

				fd_set fds;
				sync = sync_select(multi_gee, &fds);

				for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
					mg_device_t dev = sll_data(d);
					if (FD_ISSET(mg_device_fd(dev), &fds)) {
						if (!swap_frame(multi_gee, dev)) {
							sync = SYNC_FATAL;
							break;
						}

#if DEBUG_SELECT
						struct timeval tv;
						gettimeofday(&tv, 0);
						printf("--select--\n");
						print_tv("now:             ", tv); printf("\n");

						mg_frame_t f = find_frame_device(multi_gee->frame, dev);
						struct timeval f_tv = mg_frame_timestamp(f);
						timersub(&f_tv, &tv, &tv);

						print_tv("frame timestamp: ", f_tv); printf("\n");
						print_tv("frame now diff:  ", tv); printf("\n");
						printf(  "frame device:    %17s\n", mg_device_name(dev));
						printf(  "frame index:     %17d\n", mg_frame_index(f));
						printf(  "frame sequence:  %17d\n", mg_frame_sequence(f));
						printf("--/select--\n");
#endif

						sync = sync_test(multi_gee);

						if (sync == SYNC_FATAL)
							break;

						if (sync == SYNC_OK) {
							multi_gee->callback(multi_gee, multi_gee->frame);
							count++;
						}
					}
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

int
mg_deregister_device(multi_gee_t multi_gee,
		     int id)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t device = find_device_fd(multi_gee->device, id);
		if (device) {
			multi_gee->refresh = true;
			multi_gee->device = sll_remove_data(multi_gee->device, device);

			/* ignore failures */
			fg_stop_capture(device, multi_gee->log);
			fg_uninit_device(device, multi_gee->log);

			mg_device_destroy(device);
			ret = id;
		}
	}

	return ret;
}

multi_gee_t
mg_register_callback(multi_gee_t multi_gee,
		     void (*callback)(multi_gee_t, sllist_t))
{
	multi_gee_t p = 0;

	VERIFY(multi_gee) {
		if (callback)
			multi_gee->callback = callback;
		p = multi_gee;
	}

	return p;
}

int
mg_register_device(multi_gee_t multi_gee,
		   char *name)
{
	fflush(0);
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t dev = mg_device_create(name,
						   multi_gee->num_bufs,
						   multi_gee->log);

		/* can device be registered? */
		if (mg_device_number(dev) == makedev(-1, -1)) {
			dev = mg_device_destroy(dev);
			ret = -1;
		} else {
			/* is device already registered? */
			mg_device_t dup =
				find_device_number(multi_gee->device,
						   mg_device_number(dev));
			if (dup) {
				dev = mg_device_destroy(dev);
				ret = mg_device_fd(dup);
			}
		}

		/* try to start capture on device */
		if (dev) {
			ret = mg_device_open(dev);
			if (-1 != ret) {
				ret = mg_device_fd(dev);
			}

			if (-1 != ret) {
				if (!fg_init_device(dev, multi_gee->log)
				    || !fg_start_capture(dev, multi_gee->log))
					ret = -1;
			}

			if (-1 != ret) {
				/* everything OK, add it to device list */
				multi_gee->refresh = true;
				multi_gee->device = sll_insert_data(multi_gee->device, dev);
			} else {
				/* fatal error */
				mg_device_destroy(dev);
			}
		}
	}

	return ret;
}

sllist_t
add_frame(sllist_t frame,
	  sllist_t device)
{
	sllist_t list = 0;

	for (sllist_t d = device; d; d = sll_next(d)) {
		mg_device_t dev = sll_data(d);

		mg_frame_t old_frame = find_frame_device(frame, dev);
		if (old_frame) {
			frame = sll_remove_data(frame, old_frame);
			list = sll_insert_data(list, old_frame);
		} else {
			mg_frame_t new_frame = mg_frame_create(dev, 0);
			list = sll_insert_data(list, new_frame);
		}
	}

	for (sllist_t f = frame; f; f = sll_next(f))
		mg_frame_destroy(sll_data(f));

	frame = sll_empty(frame);
	return list;
}

mg_device_t
find_device_fd(sllist_t list,
	       int fd)
{
	for (sllist_t d = list; d; d = sll_next(d)) {
		mg_device_t device = sll_data(d);
		if (mg_device_fd(device) == fd)
			return device;
	}

	return 0;
}

mg_device_t
find_device_number(sllist_t list,
		   dev_t devno)
{
	for (sllist_t d = list; d; d = sll_next(d)) {
		mg_device_t device = sll_data(d);
		if (mg_device_number(device) == devno)
			return device;
	}

	return 0;
}

mg_frame_t
find_frame_device(sllist_t list,
		  mg_device_t device)
{
	for (sllist_t f = list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		if (mg_frame_device(frame) == device)
			return frame;
	}

	return 0;
}

bool
swap_frame(multi_gee_t multi_gee,
	   mg_device_t dev)
{
	int fd = mg_device_fd(dev);

	struct v4l2_buffer buf;
	if (!fg_dequeue(fd, &buf, multi_gee->log))
		return false;

	mg_buffer_t dev_buf = mg_device_buffer(dev);
	xassert(buf.index < mg_buffer_number(dev_buf)) {
		for (sllist_t f = multi_gee->frame; f; f = sll_next(f)) {
			mg_frame_t frame = sll_data(f);
			if (dev == mg_frame_device(frame)) {
				int index = mg_frame_index(frame);
				if (index >= 0)
					if (!fg_enqueue(mg_device_fd(dev), index, multi_gee->log))
						return false;

				multi_gee->frame =
					sll_remove_data(multi_gee->frame, frame);

				mg_frame_destroy(frame);

				frame = mg_frame_create(dev, &buf);
				multi_gee->frame =
					sll_insert_data(multi_gee->frame, frame);

				return true;
			}
		}
	}

	return false;
}

enum sync_status
sync_select(multi_gee_t multi_gee,
	    fd_set *fds)
{
	if (multi_gee->refresh) {
		multi_gee->frame =
			add_frame(multi_gee->frame, multi_gee->device);
		multi_gee->refresh = false;
	}

	enum sync_status sync = SYNC_FAIL;
	while (SYNC_FATAL != sync) {
		FD_ZERO(fds);

		int max_fd = -1;
		for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
			int fd = mg_device_fd(sll_data(d));
			max_fd = (fd > max_fd) ? fd : max_fd;
			FD_SET(fd, fds);
		}
		max_fd += 1;

		struct timeval timeout = multi_gee->TV_NO_SYNC;
		int ret = select(max_fd, fds, NULL, NULL, &timeout);

		if (-1 == ret) {
			if (EINTR == errno)
				continue;

			lg_log(multi_gee->log, "select");
			sync = SYNC_FATAL;
		}

		if (0 == ret) {
			/* select timeout */
			lg_log(multi_gee->log, "wait too long for frame");
			sync = SYNC_FATAL;
		}

		break;
	}

	return sync;
}

enum sync_status
sync_test(multi_gee_t multi_gee)
{
	enum sync_status sync = SYNC_FAIL;

	VERIFY(multi_gee) {

		struct timeval now;
		gettimeofday(&now, 0);

		struct timeval tv_diff;
		timersub(&now, &multi_gee->last_sync, &tv_diff);
		if (timercmp(&multi_gee->TV_NO_SYNC, &tv_diff, <)) {
			lg_log(multi_gee->log,
			       "too long since last sync: %ld.%06ld",
			       tv_diff.tv_sec,
			       tv_diff.tv_usec);

			sync = SYNC_FATAL;
		} else if (multi_gee->frame) {
			mg_frame_t frame = sll_data(multi_gee->frame);
			struct timeval tv_max = mg_frame_timestamp(frame);
			struct timeval tv_min = tv_max;

			bool ready = true;
			bool old = true; /* delayed startup flag */

			/* repeat first frame */
			for (sllist_t f = multi_gee->frame; f; f = sll_next(f)) {
				frame = sll_data(f);

				struct timeval tv = mg_frame_timestamp(frame);
				if (timercmp(&tv, &multi_gee->last_sync, <)) {
					old = true;
					mg_frame_set_used(frame);
				} else {
					if (timercmp(&tv_min, &tv, >))
						tv_min = tv;
					if (timercmp(&tv_max, &tv, <))
						tv_max = tv;
				}

				ready &= !mg_frame_used(frame);
			}

			timersub(&tv_max, &tv_min, &tv_diff);
			if (ready
			    && timercmp(&multi_gee->TV_IN_SYNC, &tv_diff, >)) {
				for (sllist_t f = multi_gee->frame; f; f = sll_next(f))
					mg_frame_set_used(sll_data(f));
				multi_gee->last_sync = now;
				sync = SYNC_OK;
			} else if (!old
				   && timercmp(&multi_gee->TV_NO_SYNC, &tv_diff, <)) {
				lg_log(multi_gee->log,
				       "fatal loss of sync: %ld.%06ld\n",
				       tv_diff.tv_sec,
				       tv_diff.tv_usec);
				sync = SYNC_FATAL;
			}
		}
	}
	return sync;
}

#ifdef DEBUG_MULTI_GEE

#include <cclass/xassert.h>
#include <cclass/xmalloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void
process_images(multi_gee_t mg, sllist_t frame_list)
{
	static struct timeval then = {0, 0};
	static int count = 0;

	struct timeval now;
	gettimeofday(&now, 0);
	printf("now: %10ld.%06ld\n", now.tv_sec, now.tv_usec);

	struct timeval diff;
	timersub(&now, &then, &diff);
	printf("  then now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	then = now;

	printf("  count   : %d\n", count++);

	for (sllist_t f = frame_list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		printf("dev: %s\n", mg_device_name(mg_frame_device(frame)));
		struct timeval tv = mg_frame_timestamp(frame);

		printf(" tv: %10ld.%06ld\n",  tv.tv_sec,  tv.tv_usec);

		timersub(&now, &tv, &diff);
		printf("  tv   now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);
		printf("  sequence: %d\n", mg_frame_sequence(frame));

	}
	printf("\n");

	return;

	static int dev_id = -1;
	if (count % 1 == 0) {
		static bool flag = true;
		if (flag) {
			dev_id = mg_register_device(mg, "/dev/video1");
		} else {
			dev_id = mg_deregister_device(mg, dev_id);
		}
		flag = !flag;
	}
}

void
multi_gee()
{
	multi_gee_t mg = mg_create("stderr");

	mg_register_callback(mg, process_images);

	printf("dev id = %d\n", mg_register_device(mg, "/dev/video0"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video1"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video2"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video3"));

	for (int i = 0; i < 5; i++) {

		printf("sleep a while\n");
		sleep(1);

		int ret = mg_capture(mg, 5);
		printf("capture ret = %d\n", ret);

		// handle return value
		switch (ret) {
			case RET_UNDEF    : printf("should not happen\n"); break;
			case RET_CALLBACK : printf("no callback registered\n"); break;
			case RET_SYNC     : printf("sync lost\n"); break;
			case RET_BUSY     : printf("multiple call to capture\n"); break;
			case RET_DEVICE   : printf("no devices registered\n"); break;
			case RET_HALT     : printf("capture_halt called\n"); break;
			default           : printf("captured %d frames\n", ret); break;
		}

	}

	mg_destroy(mg);
}

int
main()
{
	for (int i = 0; i < 5; i++) {
		int ret = debug_test(multi_gee);
		if (ret != EXIT_SUCCESS)
			return ret;
	}
	return EXIT_SUCCESS;
}

#endif /* DEBUG_MULTI_GEE */
