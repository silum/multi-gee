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
#include "xmalloc.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

static void
errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	exit(EXIT_FAILURE);
}

static int
xioctl(int fd, int request, void *arg)
{
	int r;

	do
		r = ioctl(fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}

static void
process_image(const void *p)
{
	struct timeval tv;
	gettimeofday(&tv, 0);

	static long then = 0;

	long now = (tv.tv_sec) * 1000000 + tv.tv_usec;
	long diff = now - then;
	then = now;

	printf("%10d.%06d\n", tv.tv_sec, tv.tv_usec);
	printf("  %d\n", diff);
	//	fflush(stdout);
}

static int
read_frame(mg_device_t dev)
{
	unsigned int i;
	int fd = mg_device_fd(dev);
	printf("fd = %d\n", fd);

	struct v4l2_buffer buf;
	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
		}
	}

	assert(buf.index < mg_device_num_bufs(dev));

	process_image(mg_device_buffer(dev)[buf.index].start);

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");

	return 1;
}

static void
mainloop(sllist_t list)
{
	unsigned int count;

	count = 100;

	while (count-- > 0) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;
			int max_fd = -1;

			FD_ZERO(&fds);

			sllist_t l;
			for (l = list; l; l = sll_next(l)) {
				int fd = mg_device_fd(sll_data(l));
				max_fd = (fd > max_fd) ? fd : max_fd;
				FD_SET(fd, &fds);
			}

			/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select(max_fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno)
					continue;

				errno_exit("select");
			}

			if (0 == r) {
				fprintf(stderr, "select timeout\n");
				exit(EXIT_FAILURE);
			}

			r = 0;
			for (l = list; l; l = sll_next(l)) {
				mg_device_t dev = sll_data(l);
				if (FD_ISSET(mg_device_fd(dev), &fds))
					if (read_frame(dev))
						r = 1;
			}
			if (r) break;

			/* EAGAIN - continue select loop. */
		}
	}
}

static void
stop_capturing(mg_device_t dev)
{
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");

}

static void
start_capturing(mg_device_t dev)
{
	unsigned int i;
	enum v4l2_buf_type type;
	int fd = mg_device_fd(dev);
	unsigned int n_buffers = mg_device_num_bufs(dev);

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
}

static void
uninit_device(mg_device_t dev)
{
	unsigned int i;
	int fd = mg_device_fd(dev);
	unsigned int n_buffers = mg_device_num_bufs(dev);
	struct buffer *buffers = mg_device_buffer(dev);

	for (i = 0; i < n_buffers; ++i)
		if (-1 ==
		    munmap(buffers[i].start, buffers[i].length))
			errno_exit("munmap");

//	free(buffers);
}

static void
init_read(unsigned int buffer_size,
	  mg_device_t dev)
{
	struct buffer *buffers = mg_device_buffer(dev);
	buffers = calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

static void
init_mmap(mg_device_t dev)
{
	struct v4l2_requestbuffers req;
	int fd = mg_device_fd(dev);
	char *dev_name = mg_device_file_name(dev);
	struct buffer *buffers = mg_device_buffer(dev);
	unsigned int n_buffers = mg_device_num_bufs(dev);

	CLEAR(req);

	req.count = 4;
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

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	// buffers = calloc(req.count, sizeof(*buffers));
	buffers = mg_device_alloc_buffer(dev, req.count);

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */ ,
						buf.length,
						PROT_READ | PROT_WRITE
						/* required */ ,
						MAP_SHARED
						/* recommended */ ,
						fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void
init_device(mg_device_t dev)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;
	int fd = mg_device_fd(dev);
	char *dev_name = mg_device_file_name(dev);

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

	/* Select video input, video standard and tune here. */

	int index = 1;
	if (-1 == xioctl(fd, VIDIOC_S_INPUT, &index))
		errno_exit("VIDIOC_S_INPUT");

	v4l2_std_id std = V4L2_STD_PAL;
	if (-1 == xioctl(fd, VIDIOC_S_STD, &std))
		errno_exit("VIDIOC_S_STD");

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		/* Errors ignored. */
	}

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

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// fmt.fmt.pix.width = 768;
	// fmt.fmt.pix.height = 576;
	fmt.fmt.pix.width = 320;
	fmt.fmt.pix.height = 200;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	init_mmap(dev);
}

static void
open_device(mg_device_t dev)
{
	char *dev_name = mg_device_file_name(dev);
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

	int fd = mg_device_open(dev);
	printf("open device: fd = %d\n", fd);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void
usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Options:\n"
		"-h | --help          Print this message\n"
		"", argv[0]);
}

static const char short_options[] = "d:hmru";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{0, 0, 0, 0}
};

int
main(int argc, char **argv)
{
	sllist_t list = 0;
	mg_device_t dev;

	dev = mg_device_create("/dev/video0");
	list = sll_insert_data(list, dev);

	dev = mg_device_create("/dev/video1");
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
				usage(stdout, argc, argv);
				exit(EXIT_SUCCESS);

			default:
				usage(stderr, argc, argv);
				exit(EXIT_FAILURE);
		}
	}

	sllist_t l;
	for (l = list; l; l = sll_next(l))
		open_device(sll_data(l));

	for (l = list; l; l = sll_next(l))
		init_device(sll_data(l));

	for (l = list; l; l = sll_next(l))
		start_capturing(sll_data(l));

	mainloop(list);

	for (l = list; l; l = sll_next(l))
		stop_capturing(sll_data(l));

	for (l = list; l; l = sll_next(l))
		uninit_device(sll_data(l));

	for (l = list; l; l = sll_next(l))
		mg_device_destroy(sll_data(l));

	list = sll_empty(list);

	xwalkheap();

	exit(EXIT_SUCCESS);

	return 0;
}

