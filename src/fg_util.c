/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#include <getopt.h>		/* getopt_long() */

//#include <fcntl.h>		/* low-level i/o */
//#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
//#include <sys/types.h>
//#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>		/* needed for videodev2.h */
#include "linux/videodev2.h"

#include "xmalloc.h"

#include "mg_buffer.h"
#include "mg_device.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

static const unsigned int REQ_BUFS = 3;

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

/**
 * @brief intialise memory mapping
 */
static bool
init_mmap(int fd,
	  char *dev_name,
	  mg_buffer_t dev_buf)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = REQ_BUFS;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"memory mapping\n", dev_name);
			return false;
		} else {
			ferrno(stderr, "VIDIOC_REQBUFS");
			return false;
		}
	}

	if (req.count < REQ_BUFS) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			dev_name);
		return false;
	}

	dev_buf = mg_buffer_alloc(dev_buf, req.count);

	for (unsigned int i = 0; i < req.count; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
			ferrno(stderr, "VIDIOC_QUERYBUF");
			return false;
		}

		void *start = mmap(NULL /* start anywhere */ ,
				   buf.length,
				   PROT_READ | PROT_WRITE
				   /* required */ ,
				   MAP_SHARED
				   /* recommended */ ,
				   fd, buf.m.offset);

		mg_buffer_set(dev_buf, i, start, buf.length);

		if (MAP_FAILED == start) {
			ferrno(stderr, "mmap");
			return false;
		}
	}
	return true;
}

/**
 * @brief test device capabilities
 *
 * @desc will not return if insufficient capabilities is detected
 */
static bool
test_capability(int fd,
		char *dev_name)
{
	struct v4l2_capability cap;
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
				dev_name);
			return false;
		} else {
			ferrno(stderr, "VIDIOC_QUERYCAP");
			return false;
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",
			dev_name);
		return false;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr,
			"%s does not support streaming i/o\n",
			dev_name);
		return false;
	}

	return true;
}

/**
 * @brief select video input and video standard
 */
static bool
select_input(int fd)
{
	int index = 1;
	if (-1 == xioctl(fd, VIDIOC_S_INPUT, &index)) {
		ferrno(stderr, "VIDIOC_S_INPUT");
		return false;
	}

	v4l2_std_id std = V4L2_STD_PAL;
	if (-1 == xioctl(fd, VIDIOC_S_STD, &std)) {
		ferrno(stderr, "VIDIOC_S_STD");
		return false;
	}

	return true;
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
static bool
set_format(int fd)
{
	struct v4l2_format fmt;

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 768;
	fmt.fmt.pix.height = 576;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
		ferrno(stderr, "VIDIOC_S_FMT");
		return false;
	}

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	unsigned int min;
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	return true;
}

/**
 * @brief initialise frame capture device
 */
bool
fg_init_device(mg_device_t dev)
{
	int fd = mg_device_fd(dev);
	char *dev_name = mg_device_name(dev);

	if (!test_capability(fd, dev_name))
		return false;

	if (!select_input(fd))
		return false;

	set_crop(fd);

	if (!set_format(fd))
		return false;

	if (!init_mmap(fd, dev_name, mg_device_buffer(dev)))
		return false;

	return true;
}

/**
 * @brief unmap memmapped memory
 */
bool
fg_uninit_device(mg_device_t dev)
{
	mg_buffer_t dev_buf = mg_device_buffer(dev);
	unsigned int bufs = mg_buffer_number(dev_buf);

	for (unsigned int i = 0; i < bufs; ++i) {
		if (-1 == munmap(mg_buffer_start(dev_buf, i),
				 mg_buffer_length(dev_buf, i))) {
			ferrno(stderr, "munmap");
			return false;
		}
	}

	return true;
}

/**
 * @brief enqueue a capture buffer for filling by the driver
 */
bool
fg_enqueue(int fd, int i)
{
	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = i;

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		ferrno(stderr, "VIDIOC_QBUF");
		return false;
	}

	return true;
}

/**
 * @brief dequeue a buffer for user processing
 */
bool
fg_dequeue(int fd, struct v4l2_buffer *buf)
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
				ferrno(stderr, "VIDIOC_DQBUF");
				return false;
		}
	}

	return buf;
}

/**
 * @brief start streaming capturing on device
 */
bool
fg_start_capture(mg_device_t dev)
{
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);
	unsigned int bufs = mg_buffer_number(mg_device_buffer(dev));

	for (unsigned int i = 0; i < bufs; ++i)
		fg_enqueue(fd, i);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
		ferrno(stderr, "VIDIOC_STREAMON");
		return false;
	}

	return true;
}

/**
 * @brief stop streaming capturing on device
 */
bool
fg_stop_capture(mg_device_t dev)
{
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
		ferrno(stderr, "VIDIOC_STREAMOFF");
		return false;
	}

	return true;
}

