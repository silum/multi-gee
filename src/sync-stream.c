/* $Id: capture.c 18 2004-11-30 12:55:40Z dsm $ */
/*
 *  V4L2 video capture example
 *
 *  http://v4l2spec.bytesex.org/v4l2spec/capture.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>		/* getopt_long() */

#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>		/* needed for videodev2.h */
#include "linux/videodev2.h"

#include "sllist.h"
#include "mg_device.h"
#include "mg_frame.h"
#include "xmalloc.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

static const unsigned int REQ_BUFS = 3;

static struct timeval TV_IN_SYNC = {0, 22000}; /* 55% of framerate */
static struct timeval TV_NO_SYNC = {0, 120000}; /* 3 frames */


/*
static void
peruse_frame(mg_frame_t frame)
{
	printf("  peruse frame\n");
	printf("  index      : %d\n", mg_frame_index(frame));
	printf("  device     : %p\n", mg_frame_device(frame));
	printf("  image      : %p\n", mg_frame_image(frame));
	printf("  timestamp  : %ld.%06ld\n",
	       mg_frame_timestamp(frame).tv_sec,
	       mg_frame_timestamp(frame).tv_usec);
	printf("  used       : %d\n", mg_frame_used(frame));
}

static void
peruse_frame_list(sllist_t frame)
{
	printf("peruse frame list\n");
	for (sllist_t f = frame; f; f = sll_next(f)) {
		printf("item = %p\n", f);
		printf("next = %p\n", sll_next(f));
		printf("data = %p\n", sll_data(f));
		peruse_frame(sll_data(f));
	}
	printf("\n");
}
*/

static bool
tv_eq(struct timeval tv_0,
      struct timeval tv_1)
{
	return (tv_0.tv_sec == tv_1.tv_sec
		&& tv_0.tv_usec == tv_1.tv_usec);
}

static bool
tv_lt(struct timeval tv_0,
      struct timeval tv_1)
{
	if (tv_0.tv_sec == tv_1.tv_sec) {
		if (tv_0.tv_usec < tv_1.tv_usec)
			return true;
		else
			return false;
	} else if (tv_0.tv_sec < tv_1.tv_sec)
		return true;
	else /* tv_1.tv_sec < tv_0.tv_sec */
		return false;
}

static struct timeval
tv_abs_diff(struct timeval tv_0,
	    struct timeval tv_1)
{
	struct timeval tv = {0, 0};

	if (tv_eq(tv_0, tv_1))
		return tv;

	if (tv_lt(tv_0, tv_1)) {
		tv.tv_sec = tv_1.tv_sec - tv_0.tv_sec;
		tv.tv_usec = tv_1.tv_usec - tv_0.tv_usec;
	} else {
		tv.tv_sec = tv_0.tv_sec - tv_1.tv_sec;
		tv.tv_usec = tv_0.tv_usec - tv_1.tv_usec;
	}

	if (tv.tv_usec < 0) {
		tv.tv_sec--;
		tv.tv_usec += 1000000;
	}

	return tv;
}

/**
 * @brief print error message corresponding to errno and exit
 */
static void
errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	exit(EXIT_FAILURE);
}

/**
 * @brief retry ioctl until it happens
 */
static int
xioctl(int fd, int request, void *arg)
{
	int ret;

	do
		ret = ioctl(fd, request, arg);
	while (-1 == ret && EINTR == errno);

	return ret;
}

static void
process_image(const void *p,
	      struct timeval tv)
{
	(void) p;
	struct timeval now;
	gettimeofday(&now, 0);

	printf("now: %10ld.%06ld\n", now.tv_sec, now.tv_usec);
	printf(" tv: %10ld.%06ld\n",  tv.tv_sec,  tv.tv_usec);
	fflush(0);

	static struct timeval then = {0, 0};
	static struct timeval diff = {0, 0};

	diff = tv_abs_diff(tv, now);
	printf("  tv   now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);

	diff = tv_abs_diff(then, now);
	printf("  then now diff: %10ld.%06ld\n", diff.tv_sec, diff.tv_usec);

	then = now;
}

/**
 * @brief enqueue a capture buffer for filling by the driver
 */
static void
enqueue(int fd, int i)
{
	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = i;

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");
}

/**
 * @brief dequeue a buffer for user processing
 */
static bool
dequeue(int fd, struct v4l2_buffer *buf)
{
	CLEAR(*buf);

	buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, buf)) {
		switch (errno) {
			case EAGAIN:
				return false;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
		}
	}

	return buf;
}

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
	if (!dequeue(fd, &buf))
		return frame_list;

	mg_buffer_t dev_buf = mg_device_buffer(dev);
	assert(buf.index < mg_buffer_number(dev_buf));

	for (sllist_t f = frame_list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		if (dev == mg_frame_device(frame)) {
			int index = mg_frame_index(frame);
			if (index >= 0)
				enqueue(mg_device_fd(dev), index);

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

static bool
sync_test(sllist_t frame_list)
{
	struct timeval tv_max;
	struct timeval tv_min;
	bool ready = true;
	bool processed = false;

	if (frame_list) {
		mg_frame_t frame = sll_data(frame_list);
		tv_max = mg_frame_timestamp(frame);
		tv_min = mg_frame_timestamp(frame);
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

		struct timeval tv_diff= tv_abs_diff(tv_min, tv_max);
		if (ready && tv_lt(tv_diff, TV_IN_SYNC)) {
			for (sllist_t f = frame_list; f; f = sll_next(f)) {
				process_image(mg_frame_image(sll_data(f)),
					      mg_frame_timestamp(sll_data(f)));
				mg_frame_set_used(sll_data(f));
				processed = true;
			}
			printf("\n");
		} else if (tv_lt(TV_NO_SYNC, tv_diff)) {
			fprintf(stderr, "sync lost\n");
			exit(EXIT_FAILURE);
		}
	}
	return processed;
}

/**
 * @brief main capture loop
 */
static void
mainloop(sllist_t device, int n)
{
	int count = 0;

	/* add frames for all devices */
	sllist_t frame = add_frame(0 , device);
	// peruse_frame_list(frame);

	bool done = false;

	while (n && !done) {
		for (;;) {
			fd_set fds;

			FD_ZERO(&fds);

			int max_fd = -1;
			for (sllist_t d = device; d; d = sll_next(d)) {
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

				errno_exit("select");
			}

			if (0 == ret) {
				fprintf(stderr, "select timeout\n");
				exit(EXIT_FAILURE);
			}

			for (sllist_t d = device; d; d = sll_next(d)) {
				mg_device_t dev = sll_data(d);
				if (FD_ISSET(mg_device_fd(dev), &fds)) {
					frame = swap_frame(frame, dev);
					done = sync_test(frame);
				}
			}
			if (done)
				break;

			/* EAGAIN - continue select loop. */
		}
		// usleep(39500);
		// usleep(38500);
		done = (0 < n && n <= ++count) ? true : false;
	}
	frame = add_frame(frame, 0);
}

/**
 * @brief start streaming capturing on device
 */
static void
start_capturing(mg_device_t dev)
{
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);
	unsigned int bufs = mg_buffer_number(mg_device_buffer(dev));

	for (unsigned int i = 0; i < bufs; ++i)
		enqueue(fd, i);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
}

/**
 * @brief stop streaming capturing on device
 */
static void
stop_capturing(mg_device_t dev)
{
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");
}

/**
 * @brief intialise memory mapping
 */
static void
init_mmap(mg_device_t dev)
{
	int fd = mg_device_fd(dev);
	char *dev_name = mg_device_name(dev);

	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = REQ_BUFS;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < REQ_BUFS) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	mg_buffer_t dev_buf = mg_device_buffer(dev);
	dev_buf = mg_buffer_alloc(dev_buf, req.count);

	for (unsigned int i = 0; i < req.count; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		void *start = mmap(NULL /* start anywhere */ ,
				   buf.length,
				   PROT_READ | PROT_WRITE
				   /* required */ ,
				   MAP_SHARED
				   /* recommended */ ,
				   fd, buf.m.offset);

		mg_buffer_set(dev_buf, i, start, buf.length);

		if (MAP_FAILED == start)
			errno_exit("mmap");
	}
}

/**
 * @brief unmap memmapped memory
 */
static void
uninit_mmap(mg_device_t dev)
{
	mg_buffer_t dev_buf = mg_device_buffer(dev);
	unsigned int bufs = mg_buffer_number(dev_buf);

	for (unsigned int i = 0; i < bufs; ++i) {
		if (-1 == munmap(mg_buffer_start(dev_buf, i),
				 mg_buffer_length(dev_buf, i)))
			errno_exit("munmap");
	}
}

/**
 * @brief test device capabilities
 *
 * @desc will not return if insufficient capabilities is detected
 */
static void
test_capability(int fd,
		char *dev_name)
{
	struct v4l2_capability cap;
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
				dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr,
			"%s does not support streaming i/o\n",
			dev_name);
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief select video input and video standard
 */
static void
select_input(int fd)
{
	int index = 1;
	if (-1 == xioctl(fd, VIDIOC_S_INPUT, &index))
		errno_exit("VIDIOC_S_INPUT");

	v4l2_std_id std = V4L2_STD_PAL;
	if (-1 == xioctl(fd, VIDIOC_S_STD, &std))
		errno_exit("VIDIOC_S_STD");
}

/**
 * @brief reset cropping
 */
static void
set_crop(int fd)
{
	struct v4l2_cropcap cropcap;
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		/* Errors ignored. */
	}

	struct v4l2_crop crop;
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect;	/* reset to default */

	if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
		switch (errno) {
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
		}
	}
}

/**
 * @brief set capture format
 *
 * @desc set the following parameters:
 *  - height
 *  - width
 *  - depth
 *  - interlacing
 */
static void
set_format(int fd)
{
	struct v4l2_format fmt;

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 768;
	fmt.fmt.pix.height = 576;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	unsigned int min;
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;
}

/**
 * @brief initialise frame capture device
 */
static void
init_device(mg_device_t dev)
{
	int fd = mg_device_fd(dev);
	char *dev_name = mg_device_name(dev);

	test_capability(fd, dev_name);

	select_input(fd);

	set_crop(fd);

	set_format(fd);
}

/**
 * @brief open device for reading and writing
 *
 * @desc will not return if a problem is detected
 */
static void
open_device(mg_device_t dev)
{
	char *dev_name = mg_device_name(dev);
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	if (-1 == mg_device_open(dev)) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief programme usage message
 */
static void
usage(FILE *fp, char **argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Options:\n"
		"-h | --help          Print this message\n"
		"", argv[0]);
}

static const char short_options[] = "h";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{0, 0, 0, 0}
};

int
main(int argc, char *argv[])
{
	sllist_t list = 0;
	mg_device_t dev;

	dev = mg_device_create("/dev/video0");
	list = sll_insert_data(list, dev);

	dev = mg_device_create("/dev/video1");
	list = sll_insert_data(list, dev);

	dev = mg_device_create("/dev/video2");
	list = sll_insert_data(list, dev);

	dev = mg_device_create("/dev/video3");
	list = sll_insert_data(list, dev);

	dev = 0;

	for (;;) {
		int index;
		int c;

		c = getopt_long(argc, argv,
				short_options, long_options, &index);

		if (-1 == c)
			break;

		switch (c) {
			case 0:	/* getopt_long() flag */
				break;

			case 'h':
				usage(stdout, argv);
				exit(EXIT_SUCCESS);

			default:
				usage(stderr, argv);
				exit(EXIT_FAILURE);
		}
	}

	for (sllist_t l = list; l; l = sll_next(l)) {
		mg_device_t dev = sll_data(l);

		open_device(dev);
		init_device(dev);
		init_mmap(dev);
		start_capturing(dev);
	}

	// usleep(TV_NO_SYNC.tv_usec);
	// usleep(20000);
	usleep(TV_IN_SYNC.tv_usec);
	// mainloop(list, 250);
	mainloop(list, -1);

	for (sllist_t l = list; l; l = sll_next(l)) {
		mg_device_t dev = sll_data(l);

		stop_capturing(dev);
		uninit_mmap(dev);
		mg_device_destroy(dev);
	}

	list = sll_empty(list);

	xwalkheap();

	exit(EXIT_SUCCESS);

	return 0;
}

