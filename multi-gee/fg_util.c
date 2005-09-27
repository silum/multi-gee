/* $Id$
 * Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
 * Modified 2005-06-14 by Neil Muller <neil@tlabs.ac.za> (Minor tweak)
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
 * @brief Frame grabber interface utility definition
 */
#include <cclass/xmalloc.h>

#include <errno.h> /* errno */
#include <string.h> /* memset */

#include <sys/mman.h> /* mmap */
#include <sys/ioctl.h> /* ioctl */

#include <stdlib.h> /* struct timeval, needed for videodev2.h */
#include <asm/types.h> /* needed for videodev2.h */
#include <linux/videodev2.h>

#include "fg_util.h" /* declarations implemented */
#include "log.h"
#include "mg_buffer.h"
#include "mg_device.h"

/**
 * @brief Initialise memory area to zeros
 */
#define CLEAR(x) memset (&(x), 0, sizeof (x))

/**
 * @brief Initialise memory mapping
 *
 * @param fd  file descriptor
 * @param name  device name
 * @param buffer  device buffer
 * @param log  to log possible errors to
 * @param num_bufs  number of capture buffers
 */
static bool
init_mmap(int fd,
	  char *name,
	  mg_buffer_t buffer,
	  unsigned int num_bufs,
	  log_t log);

/**
 * @brief Select video input and video standard
 *
 * @param fd  file descriptor
 * @param log  to log possible errors to
 */
static bool
select_input(int fd,
	     log_t log);

/**
 * @brief Reset cropping
 *
 * @param fd  file descriptor
 */
static void
set_crop(int fd);

/**
 * @brief Set capture format
 *
 * set the following parameters:
 *  - height
 *  - width
 *  - depth
 *  - interlacing
 *
 * @param fd  file descriptor
 * @param log  to log possible errors to
 */
static bool
set_format(int fd,
	   log_t log);

/**
 * @brief Test device capabilities
 *
 * will not return if insufficient capabilities is detected
 *
 * @param fd  file descriptor
 * @param name  device name, for logging
 * @param log  to log possible errors to
 */
static bool
test_capability(int fd,
		char *name,
		log_t log);

/**
 * @brief Retry ioctl until it happens
 *
 * @param fd  file descriptor
 * @param req  ioctl request
 * @param arg  ioctl argument
 */
static int
xioctl(int fd,
       int req,
       void *arg);

bool
fg_dequeue(int fd,
	   struct v4l2_buffer *buf,
	   log_t log)
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
				lg_errno(log, "VIDIOC_DQBUF");
				return false;
		}
	}

	return buf;
}

bool
fg_enqueue(int fd,
	   int i,
	   log_t log)
{
	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = i;

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		lg_errno(log, "VIDIOC_QBUF");
		return false;
	}

	return true;
}

bool
fg_init_device(mg_device_t dev,
	       log_t log)
{
	int fd = mg_device_get_fd(dev);
	char *dev_name = mg_device_get_name(dev);

	if (!test_capability(fd, dev_name, log))
		return false;

	if (!select_input(fd, log))
		return false;

	set_crop(fd);

	if (!set_format(fd, log))
		return false;

	if (!init_mmap(fd,
		       dev_name,
		       mg_device_get_buffer(dev),
		       mg_device_get_no_bufs(dev),
		       log))
		return false;

	return true;
}

bool
fg_start_capture(mg_device_t dev,
		 log_t log)
{
	enum v4l2_buf_type type;
	int fd = mg_device_get_fd(dev);
	unsigned int bufs = mg_buffer_get_number(mg_device_get_buffer(dev));

	for (unsigned int i = 0; i < bufs; ++i)
		fg_enqueue(fd, i, log);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
		lg_errno(log, "VIDIOC_STREAMON");
		return false;
	}

	return true;
}

bool
fg_stop_capture(mg_device_t dev,
		log_t log)
{
	enum v4l2_buf_type type;
	int fd = mg_device_get_fd(dev);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
		lg_errno(log, "VIDIOC_STREAMOFF");
		return false;
	}

	return true;
}

bool
fg_uninit_device(mg_device_t dev,
		 log_t log)
{
	mg_buffer_t dev_buf = mg_device_get_buffer(dev);
	unsigned int bufs = mg_buffer_get_number(dev_buf);

	for (unsigned int i = 0; i < bufs; ++i) {
		if (-1 == munmap(mg_buffer_get_start(dev_buf, i),
				 mg_buffer_get_length(dev_buf, i))) {
			lg_errno(log, "munmap");
			return false;
		}
	}

	return true;
}

bool
init_mmap(int fd,
	  char *dev_name,
	  mg_buffer_t dev_buf,
	  unsigned int req_bufs,
	  log_t log)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = req_bufs;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			lg_log(log, "%s does not support memory "
			       "mapping", dev_name);
			return false;
		} else {
			lg_errno(log, "VIDIOC_REQBUFS");
			return false;
		}
	}

	if (req.count < req_bufs) {
		lg_log(log, "Insufficient buffer memory on %s",
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
			lg_errno(log, "VIDIOC_QUERYBUF");
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
			lg_errno(log, "mmap");
			return false;
		}
	}
	return true;
}

bool
select_input(int fd,
	     log_t log)
{
	int index = 0; /* Changed to television input 2005-06-14 - NM */
	if (-1 == xioctl(fd, VIDIOC_S_INPUT, &index)) {
		lg_errno(log, "VIDIOC_S_INPUT");
		return false;
	}

	v4l2_std_id std = V4L2_STD_PAL;
	if (-1 == xioctl(fd, VIDIOC_S_STD, &std)) {
		lg_errno(log, "VIDIOC_S_STD");
		return false;
	}

	return true;
}

void
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

bool
set_format(int fd,
	   log_t log)
{
	struct v4l2_format fmt;

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 768;
	fmt.fmt.pix.height = 576;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
		lg_errno(log, "VIDIOC_S_FMT");
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

bool
test_capability(int fd,
		char *dev_name,
		log_t log)
{
	struct v4l2_capability cap;
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			lg_log(log, "%s is no V4L2 device",
				dev_name);
			return false;
		} else {
			lg_errno(log, "VIDIOC_QUERYCAP");
			return false;
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		lg_log(log, "%s is no video capture device",
			dev_name);
		return false;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		lg_log(log,
			"%s does not support streaming i/o",
			dev_name);
		return false;
	}

	return true;
}

int
xioctl(int fd, int req, void *arg)
{
	int ret;

	do
		ret = ioctl(fd, req, arg);
	while (-1 == ret && EINTR == errno);

	return ret;
}

