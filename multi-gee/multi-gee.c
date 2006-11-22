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
 * @brief Create a list of device frames
 *
 * Makes a list containing frames from devices in the device list.  The
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
 * @brief Call callback with a set of in-sync frames
 *
 * Searches through the frame list and does a call to the callback
 * function when an in-sync set of frames are found.  If the count
 * pointer is not NULL, the value it pointing to is incremented every
 * time the callback function is called.  Technically a maximum of 1
 * call the the callback function is possible per call to this function.
 *
 * @param multi_gee  object handle
 * @param [in,out]count  callback call counter
 *
 * @return a frame list only containing frames from devices in the
 * device list
 */
static
enum sync_status
capture_frameset(multi_gee_t multi_gee,
		 int *count);

/**
 * @brief Find device in list given the file descriptor
 *
 * @param list  device list object handle
 * @param fd  file descriptor
 *
 * @return device handle, or 0 if device not in list
 */
static
mg_device_t
find_device_fd(sllist_t list,
	       int fd);

/**
 * @brief Find device in list with given its device number
 *
 * @param list  device list object handle
 * @param devno  device number
 *
 * @return device handle, or 0 if device not in list
 */
static
mg_device_t
find_device_number(sllist_t list,
		   dev_t devno);

/**
 * @brief Find a frame in a list given the capture device
 *
 * @param list  frame list object handle
 * @param device  object handle
 *
 * @return frame handle, or 0 if frame not in list
 */
static
mg_frame_t
find_frame_device(sllist_t list,
		  mg_device_t device);

/**
 * @brief Enqueue old frame, dequeue new frame
 *
 * Swaps current scratch frame with frame filled by capture device
 *
 * @param multi_gee  object handle
 * @param device  object handle
 *
 * @return \c true of the frame was successfully swapped, \c false if an
 * error occurred
 */
static
bool
swap_frame(multi_gee_t multi_gee,
	   mg_device_t device);

/**
 * @brief Monitors all devices for capture events
 *
 * Performs a select on the fd_set.  If any activity on the set occurs
 * within a timeout period of TV_NO_SYNC, the function returns with a
 * non-fatal sync status.  If the select fails, or times out, a fatal
 * condition status is returned.
 *
 * @param multi_gee  device object
 * @param fds  fd_set to test for availability of a captured frame
 *
 * @return sync status
 */
static
enum sync_status
sync_select(multi_gee_t multi_gee,
	    fd_set *fds);

/**
 * @brief Tests frame list for sync
 *
 * All frames are in sync when the maximum difference in time stamps are
 * less than TV_IN_SYNC.  If the time elapsed since the previous sync
 * condition was more than TV_NO_SYNC, or when the maximum difference in
 * time stamps are more than TV_NO_SYNC, a fatal condition exists.
 *
 * @param multi_gee  object handle
 *
 * @return sync status
 */
static
enum sync_status
sync_test(multi_gee_t multi_gee);

/**
 * @brief Multi-gee object structure
 */
CLASS(multi_gee, multi_gee_t)
{
	bool busy; /**< \c true while mg_capture() in progress */
	bool halt; /**< \c true if mg_capture_halt() called */

	void (*callback)(multi_gee_t, sllist_t); /**< Pointer to user
						   defined callback
						   function */

	sllist_t frame; /**< List of frames */
	sllist_t device; /**< List of devices */

	struct timeval last_sync; /**< Time stamp when last in sync */

	log_t log; /**< Log object handle */

	struct timeval TV_IN_SYNC; /**< Frames in sync criterion */
	struct timeval TV_NO_SYNC; /**< Failure to achieve sync criterion */

	unsigned int num_bufs; /**< Number of capture buffers */
};

multi_gee_t
mg_create(char* log_file)
{
	multi_gee_t multi_gee;
	NEWOBJ(multi_gee);

	multi_gee->busy = false;
	multi_gee->halt = false;

	multi_gee->callback = 0;

	multi_gee->frame = 0;
	multi_gee->device = 0;

	timerclear(&multi_gee->last_sync);

	multi_gee->log = lg_create("multi-gee", log_file);

	timerset(&multi_gee->TV_IN_SYNC, 0, 21000); /* 55% of frame rate */
	timerset(&multi_gee->TV_NO_SYNC, 0, 168000); /* 4 frames + 5% */

	multi_gee->num_bufs = 3;

	lg_log(multi_gee->log, "startup");

	return multi_gee;
}

multi_gee_t
mg_create_special(char* log_file,
		  struct timeval tv_in_sync,
		  struct timeval tv_no_sync,
		  unsigned int num_bufs)
{
	multi_gee_t multi_gee = mg_create(log_file);

	if (multi_gee) {
		multi_gee->TV_IN_SYNC = tv_in_sync;
		multi_gee->TV_NO_SYNC = tv_no_sync;
		multi_gee->num_bufs = num_bufs;
	}
	return multi_gee;
}

multi_gee_t
mg_destroy(multi_gee_t multi_gee)
{
	VERIFYZ(multi_gee) {
		while (multi_gee->device) {
			int id = mg_device_get_fd(sll_data(multi_gee->device));
			mg_deregister_device(multi_gee, id);
		}
		multi_gee->device = sll_empty(multi_gee->device);
		multi_gee->frame = add_frame(multi_gee->frame, 0);
		multi_gee->log = lg_destroy(multi_gee->log);
		FREEOBJ(multi_gee);
	}

	return 0;
}

#ifdef DEBUG_SELECT
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
static
void
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

static
void
debug_print_frame(multi_gee_t multi_gee,
		   mg_device_t dev)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	printf("--select--\n");
	print_tv("now:             ", tv); printf("\n");

	mg_frame_t f = find_frame_device(multi_gee->frame, dev);
	struct timeval f_tv = mg_frame_get_timestamp(f);
	timersub(&f_tv, &tv, &tv);

	print_tv("frame timestamp: ", f_tv); printf("\n");
	print_tv("frame now diff:  ", tv); printf("\n");
	printf(  "frame device:    %17s\n", mg_device_get_name(dev));
	printf(  "frame index:     %17d\n", mg_frame_get_index(f));
	printf(  "frame sequence:  %17d\n", mg_frame_get_sequence(f));
	printf("--/select--\n");
}

void
debug_print_tv(struct timeval tv)
{
	print_tv("capture start:   ", tv);
	printf("\n");
}
#else
#define debug_print_frame(arg0,arg1)
#define debug_print_tv(arg)
#endif

enum sync_status
capture_frameset(multi_gee_t multi_gee,
		 int *count)
{
	fd_set fds;
	enum sync_status sync = sync_select(multi_gee, &fds);

	for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
		mg_device_t dev = sll_data(d);
		if (FD_ISSET(mg_device_get_fd(dev), &fds)) {
			bool swap_ok = swap_frame(multi_gee, dev);
			debug_print_frame(multi_gee, dev);

			if (swap_ok) {
				sync = sync_test(multi_gee);
			} else {
				sync = SYNC_FATAL;
			}

			if (SYNC_OK == sync) {
				multi_gee->callback(multi_gee,
						    multi_gee->frame);
				if (count) {
					(*count)++;
				}
			} else if (SYNC_FATAL == sync) {
				break;
			}
		}
	}

	return sync;
}
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

		debug_print_tv(multi_gee->last_sync);

		while (!done) {
			/* assume we are done */
			done = true;

			/* find a reason to be done */
			if (!multi_gee->callback) {
				ret = RET_CALLBACK;
			} else if (multi_gee->halt) {
				ret = RET_HALT;
			} else if (!multi_gee->device) {
				ret = RET_DEVICE;
			} else if (0 <= n && n <= count) {
				ret = count;
			} else if (SYNC_FATAL == sync) {
				ret = RET_SYNC;
			} else {
				/* OK, so we're not done yet */
				done = false;
				sync = capture_frameset(multi_gee,
							&count);
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
			/* remove device and frame from lists */
			multi_gee->device =
				sll_remove_data(multi_gee->device,
						device);
			multi_gee->frame =
				add_frame(multi_gee->frame,
					  multi_gee->device);

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
		if (callback) {
			multi_gee->callback = callback;
		}
		p = multi_gee;
	}

	return p;
}

int
mg_register_device(multi_gee_t multi_gee,
		   char *name,
		   void *userptr)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t dev = mg_device_create(name,
						   multi_gee->num_bufs,
						   multi_gee->log,
						   userptr);

		/* can device be registered? */
		if (mg_device_get_devno(dev) == makedev(-1, -1)) {
			dev = mg_device_destroy(dev);
			ret = -1;
		} else {
			/* is device already registered? */
			mg_device_t dup =
				find_device_number(multi_gee->device,
						   mg_device_get_devno(dev));
			if (dup) {
				dev = mg_device_destroy(dev);
				ret = mg_device_get_fd(dup);
			}
		}

		/* try to start capture on device */
		if (dev) {
			ret = mg_device_open(dev);
			if (-1 != ret) {
				ret = mg_device_get_fd(dev);
			}

			if (-1 != ret) {
				if (!fg_init_device(dev,
						    multi_gee->log)) {
					ret = -1;
				} else if (!fg_start_capture(dev,
							     multi_gee->log)) {
					ret = -1;
				}
			}

			if (-1 != ret) {
				/* everything OK, add it to device list */
				multi_gee->device = sll_insert_data(multi_gee->device, dev);
				multi_gee->frame =
					add_frame(multi_gee->frame,
						  multi_gee->device);
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

	for (sllist_t f = frame; f; f = sll_next(f)) {
		mg_frame_destroy(sll_data(f));
	}

	frame = sll_empty(frame);
	return list;
}

mg_device_t
find_device_fd(sllist_t list,
	       int fd)
{
	for (sllist_t d = list; d; d = sll_next(d)) {
		mg_device_t device = sll_data(d);
		if (mg_device_get_fd(device) == fd) {
			return device;
		}
	}

	return 0;
}

mg_device_t
find_device_number(sllist_t list,
		   dev_t devno)
{
	for (sllist_t d = list; d; d = sll_next(d)) {
		mg_device_t device = sll_data(d);
		if (mg_device_get_devno(device) == devno) {
			return device;
		}
	}

	return 0;
}

mg_frame_t
find_frame_device(sllist_t list,
		  mg_device_t device)
{
	for (sllist_t f = list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		if (mg_frame_get_device(frame) == device) {
			return frame;
		}
	}

	return 0;
}

bool
swap_frame(multi_gee_t multi_gee,
	   mg_device_t dev)
{
	int fd = mg_device_get_fd(dev);

	struct v4l2_buffer buf;
	if (!fg_dequeue(fd, &buf, multi_gee->log)) {
		return false;
	}

	mg_buffer_t dev_buf = mg_device_get_buffer(dev);
	XASSERT(buf.index < mg_buffer_get_number(dev_buf)) {
		for (sllist_t f = multi_gee->frame; f; f = sll_next(f)) {
			mg_frame_t frame = sll_data(f);
			if (mg_frame_get_device(frame) == dev) {
				int index = mg_frame_get_index(frame);
				if (0 <= index) {
					if (!fg_enqueue(mg_device_get_fd(dev), index, multi_gee->log)) {
						return false;
					}
				}

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
	enum sync_status sync = SYNC_FAIL;
	while (SYNC_FATAL != sync) {
		FD_ZERO(fds);

		int max_fd = -1;
		for (sllist_t d = multi_gee->device; d; d = sll_next(d)) {
			int fd = mg_device_get_fd(sll_data(d));
			max_fd = (fd > max_fd) ? fd : max_fd;
			FD_SET(fd, fds);
		}
		max_fd += 1;

		struct timeval timeout = multi_gee->TV_NO_SYNC;
		int ret = select(max_fd, fds, NULL, NULL, &timeout);

		if (-1 == ret) {
			if (EINTR == errno) {
				continue;
			}

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
			struct timeval tv_max = mg_frame_get_timestamp(frame);
			struct timeval tv_min = tv_max;

			bool ready = true;

			/* repeat first frame */
			for (sllist_t f = multi_gee->frame; f; f = sll_next(f)) {
				frame = sll_data(f);

				struct timeval tv = mg_frame_get_timestamp(frame);
				if (timercmp(&tv, &multi_gee->last_sync, <)) {
					mg_frame_set_used(frame);
				} else {
					if (timercmp(&tv_min, &tv, >)) {
						tv_min = tv;
					}
					if (timercmp(&tv_max, &tv, <)) {
						tv_max = tv;
					}
				}

				ready &= !mg_frame_get_used(frame);
			}

			timersub(&tv_max, &tv_min, &tv_diff);
			if (ready
			    && timercmp(&multi_gee->TV_IN_SYNC, &tv_diff, >)) {
				for (sllist_t f = multi_gee->frame; f; f = sll_next(f)) {
					mg_frame_set_used(sll_data(f));
				}
				multi_gee->last_sync = now;
				sync = SYNC_OK;
			} else if (timercmp(&multi_gee->TV_NO_SYNC, &tv_diff, <)) {
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static
int
register_device(multi_gee_t mg,
		int i)
{
	char file[] = "/dev/video_";
	snprintf(file + 10, 2, "%d", i);
	printf("register %s\n", file);
	int id = mg_register_device(mg, file, 0);
	printf("dev id = %d\n", id);
	return id;
}

static
void
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
		printf("dev: %s\n", mg_device_get_name(mg_frame_get_device(frame)));
		struct timeval tv = mg_frame_get_timestamp(frame);

		printf(" tv: %10ld.%06ld\n",  tv.tv_sec,  tv.tv_usec);

		timersub(&now, &tv, &diff);
		printf("  tv   now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);
		printf("  sequence: %d\n", mg_frame_get_sequence(frame));

	}
	printf("\n");

	static int dev_id = -1;
	if (count % 1 == 0) {
		static bool flag = true;
		if (flag) {
			printf("deregister %d\n",
			       mg_deregister_device(mg, dev_id));
			dev_id = register_device(mg, 3);
		} else {
			printf("deregister %d\n",
			       mg_deregister_device(mg, dev_id));
			dev_id = register_device(mg, 4);
		}
		flag = !flag;
	}
}

static
void
multi_gee()
{
	multi_gee_t mg = mg_create("stderr");

	mg_register_callback(mg, process_images);

	for (int i = 0; i < 3; i++) {
		register_device(mg, i);
	}

	for (int i = 0; i < 5; i++) {
		printf("sleep a while\n");
		sleep(1);

		int ret = mg_capture(mg, 5);
		printf("capture ret = %d\n", ret);

		// handle return value
		switch (ret) {
		case RET_UNDEF:
			printf("should not happen\n");
			break;
		case RET_CALLBACK:
			printf("no callback registered\n");
			break;
		case RET_SYNC:
			printf("sync lost\n");
			break;
		case RET_BUSY:
			printf("multiple call to capture\n");
			break;
		case RET_DEVICE:
			printf("no devices registered\n");
			break;
		case RET_HALT:
			printf("capture_halt called\n");
			break;
		default:
			printf("captured %d frames\n", ret);
			break;
		}
	}

	mg_destroy(mg);
}

int
main()
{
	for (int i = 0; i < 5; i++) {
		int ret = cclass_assert_test(multi_gee);
		if (ret != EXIT_SUCCESS) {
			return ret;
		}
	}
	return EXIT_SUCCESS;
}

#endif /* DEBUG_MULTI_GEE */
