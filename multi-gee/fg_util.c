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

#define FIELD         V4L2_FIELD_INTERLACED
#define MEMORY        V4L2_MEMORY_MMAP
#define PIXELFORMAT   V4L2_PIX_FMT_GREY
#define STANDARD      V4L2_STD_PAL
#define STREAMING     V4L2_CAP_STREAMING
#define TYPE          V4L2_BUF_TYPE_VIDEO_CAPTURE
#define VIDEO_CAPTURE V4L2_CAP_VIDEO_CAPTURE

#define WIDTH         768
#define HEIGHT        576

/**
 * @brief Initialise memory mapping
 *
 * @param fd  file descriptor
 * @param name  device name
 * @param buffer  device buffer
 * @param log  to log possible errors to
 * @param num_bufs  number of capture buffers
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
bool
init_mmap(int fd,
	  const char *name,
	  mg_buffer_t buffer,
	  unsigned int num_bufs,
	  log_t log);

/**
 * @brief Call Initiate Memory Mapping IOCTL
 *
 * @param fd  file descriptor
 * @param name  device name
 * @param log  to log possible errors to
 * @param num_bufs  number of capture buffers
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
bool
request_buffers(int fd,
		const char *name,
		unsigned int req_bufs,
		log_t log);

/**
 * @brief Select video input and video standard
 *
 * @param fd  file descriptor
 * @param log  to log possible errors to
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
bool
select_input(int fd,
	     log_t log);

/**
 * @brief Reset cropping
 *
 * @param fd  file descriptor
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
void
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
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
bool
set_format(int fd,
	   log_t log);

/**
 * @brief Test device capabilities
 *
 * This function will not return if insufficient capabilities is
 * detected.
 *
 * @param fd  file descriptor
 * @param name  device name, for logging
 * @param log  to log possible errors to
 *
 * @return
 * - @c false on any failure, else
 * - @c true
 */
static
bool
test_capability(int fd,
		const char *name,
		log_t log);

/**
 * @brief Retry ioctl until it happens
 *
 * @param fd  file descriptor
 * @param req  ioctl request
 * @param arg  ioctl argument
 */
static
int
xioctl(int fd,
       int req,
       void *arg);

bool
fg_dequeue(int fd,
	   struct v4l2_buffer *buf,
	   log_t log)
{
	CLEAR(*buf);

	buf->type = TYPE;
	buf->memory = MEMORY;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, buf)) {
		switch (errno) {
		case EAGAIN:
			return false;
		case EIO:
			/* Could also ignore EIO, see V4L2 spec. */
			/* fall */
		default:
			lg_errno(log, "VIDIOC_DQBUF on fd %d", fd);
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

	buf.type = TYPE;
	buf.memory = MEMORY;
	buf.index = i;

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		lg_errno(log, "VIDIOC_QBUF on fd %d", fd);
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

	if (!test_capability(fd, dev_name, log)) {
		return false;
	}

	if (!select_input(fd, log)) {
		return false;
	}

	set_crop(fd);

	if (!set_format(fd, log)) {
		return false;
	}

	if (!init_mmap(fd,
		       dev_name,
		       mg_device_get_buffer(dev),
		       mg_device_get_no_bufs(dev),
		       log)) {
		return false;
	}

	return true;
}

bool
fg_start_capture(mg_device_t dev,
		 log_t log)
{
	enum v4l2_buf_type type;
	int fd = mg_device_get_fd(dev);
	unsigned int bufs = mg_buffer_get_number(mg_device_get_buffer(dev));

	for (unsigned int i = 0; i < bufs; i++) {
		fg_enqueue(fd, i, log);
	}

	type = TYPE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
		lg_errno(log, "VIDIOC_STREAMON on fd %d", fd);
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

	type = TYPE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
		lg_errno(log, "VIDIOC_STREAMOFF on fd %d", fd);
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

	for (unsigned int i = 0; i < bufs; i++) {
		if (-1 == munmap(mg_buffer_get_start(dev_buf, i),
				 mg_buffer_get_length(dev_buf, i))) {
			lg_errno(log, "munmap");
			return false;
		}
	}

	return true;
}

static
bool
query_buffer(int fd,
	     log_t log,
	     mg_buffer_t dev_buf,
	     int index)
{
	struct v4l2_buffer buf;
	CLEAR(buf);

	buf.type = TYPE;
	buf.memory = MEMORY;
	buf.index = index;

	if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
		lg_errno(log, "VIDIOC_QUERYBUF on fd %d", fd);
		return false;
	}

	void *start = mmap(NULL /* start anywhere */,
			   buf.length,
			   PROT_READ | PROT_WRITE /* required */,
			   MAP_SHARED /* recommended */,
			   fd,
			   buf.m.offset);

	if (MAP_FAILED == start) {
		lg_errno(log, "mmap");
		return false;
	}

	if (!mg_buffer_set(dev_buf, index, start, buf.length)) {
		return false;
	}

	return true;
}

bool
init_mmap(int fd,
	  const char *dev_name,
	  mg_buffer_t dev_buf,
	  unsigned int req_bufs,
	  log_t log)
{
	if (!request_buffers(fd, dev_name, req_bufs, log)) {
		return false;
	}

	dev_buf = mg_buffer_alloc(dev_buf, req_bufs);
	if (!dev_buf) {
		return false;
	}

	for (unsigned int i = 0; i < req_bufs; i++) {
		if (!query_buffer(fd, log, dev_buf, i)) {
			return false;
		}
	}

	return true;
}

bool
request_buffers(int fd,
		const char *dev_name,
		unsigned int req_bufs,
		log_t log)
{
	struct v4l2_requestbuffers req;
	CLEAR(req);

	req.count = req_bufs;
	req.type = TYPE;
	req.memory = MEMORY;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			lg_log(log, "%s does not support memory "
			       "mapping", dev_name);
			return false;
		} else {
			lg_errno(log, "VIDIOC_REQBUFS on fd %d", fd);
			return false;
		}
	}

	if (req.count < req_bufs) {
		lg_log(log, "Insufficient buffer memory on %s",
		       dev_name);
		return false;
	}

	return true;
}

bool
select_input(int fd,
	     log_t log)
{
	int index = 0; /* Changed to television input 2005-06-14 - NM */
	if (-1 == xioctl(fd, VIDIOC_S_INPUT, &index)) {
		lg_errno(log, "VIDIOC_S_INPUT on fd %d", fd);
		return false;
	}

	v4l2_std_id std = STANDARD;
	if (-1 == xioctl(fd, VIDIOC_S_STD, &std)) {
		lg_errno(log, "VIDIOC_S_STD on fd %d", fd);
		return false;
	}

	return true;
}

void
set_crop(int fd)
{
	struct v4l2_cropcap cropcap;
	cropcap.type = TYPE;

	if (-1 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		/* Errors ignored. */
	}

	struct v4l2_crop crop;
	crop.type = TYPE;
	crop.c = cropcap.defrect; /* reset to default */

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

	fmt.type = TYPE;
	fmt.fmt.pix.width = WIDTH;
	fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.pixelformat = PIXELFORMAT;
	fmt.fmt.pix.field = FIELD;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
		lg_errno(log, "VIDIOC_S_FMT on fd %d", fd);
		return false;
	}

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	unsigned int min;
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min) {
		fmt.fmt.pix.bytesperline = min;
	}
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min) {
		fmt.fmt.pix.sizeimage = min;
	}

	return true;
}

bool
test_capability(int fd,
		const char *dev_name,
		log_t log)
{
	struct v4l2_capability cap;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			lg_log(log, "%s is no V4L2 device",
				dev_name);
			return false;
		} else {
			lg_errno(log, "VIDIOC_QUERYCAP on fd %d", fd);
			return false;
		}
	}

	if (!(cap.capabilities & VIDEO_CAPTURE)) {
		lg_log(log, "%s is no video capture device",
			dev_name);
		return false;
	}

	if (!(cap.capabilities & STREAMING)) {
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
