/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#include "xmalloc.h"
#include "mg_device.h"

USE_XASSERT;

CLASS(mg_device, mg_device_t)
{
	int fd;
	char *file_name;
	int major;
	int minor;
	mg_buffer_t buffer;
	unsigned int num_bufs;
};

mg_device_t
mg_device_create(char *file_name)
{
	mg_device_t mg_device;
	NEWOBJ(mg_device);

	mg_device->fd = -1;
	mg_device->file_name = xstrdup(file_name, __FILE__, __LINE__);

	struct stat buf;
	int ret = stat(file_name, &buf);
	if (S_ISCHR(buf.st_mode)) {
		mg_device->major = major(buf.st_rdev);
		mg_device->minor = minor(buf.st_rdev);
	} else {
		mg_device->major = -1;
		mg_device->minor = -1;
	}

	mg_device->buffer = mg_buffer_create();

	return mg_device;
}

mg_device_t
mg_device_destroy(mg_device_t mg_device)
{
	VERIFYZ(mg_device) {
		if (mg_device->fd != -1)
			close(mg_device->fd);

		mg_buffer_destroy(mg_device->buffer);

		FREEOBJ(mg_device);
	}

	return 0;
}

char *
mg_device_file_name(mg_device_t mg_device)
{
	char * file_name = 0;
	VERIFY(mg_device) {
		file_name = mg_device->file_name;
	}

	return file_name;
}

int
mg_device_open(mg_device_t mg_device)
{
	VERIFY(mg_device) {
		if (mg_device->fd == -1)
			mg_device->fd = open(mg_device->file_name,
					     O_RDWR | O_SYNC |
					     O_NONBLOCK);
	}

	return mg_device->fd;
}

int
mg_device_fd(mg_device_t mg_device)
{
	int fd = -1;
	VERIFY(mg_device) {
		fd = mg_device->fd;
	}

	return fd;
}

int
mg_device_major(mg_device_t mg_device)
{
	int major = -1;
	VERIFY(mg_device) {
		major = mg_device->major;
	}

	return major;
}

int
mg_device_minor(mg_device_t mg_device)
{
	int minor = -1;
	VERIFY(mg_device) {
		minor = mg_device->minor;
	}

	return minor;
}

int
mg_device_num_bufs(mg_device_t mg_device)
{
	int num_buf = 0;
	VERIFY(mg_device) {
		num_buf = mg_device->num_bufs;
	}

	return num_buf;
}

mg_buffer_t
mg_device_buffer(mg_device_t mg_device)
{
	mg_buffer_t p = 0;
	VERIFY(mg_device) {
		p = mg_device->buffer;
	}

	return p;
}

#ifdef DEBUG_DEVICE

#include <stdlib.h>
#include <stdio.h>

#include "debug_xassert.h"

bool
test_device(char *file_name,
	    int major,
	    int minor)
{
	/* create */
	mg_device_t dev;
	dev = mg_device_create(file_name);

	xassert(mg_device_fd(dev) == -1);
	xassert(mg_device_major(dev) == major);
	xassert(mg_device_minor(dev) == minor);

	/* device still valid */
	xassert(dev);

	int fd = mg_device_open(dev);
	xassert(fd == mg_device_fd(dev));

	/* device still valid */
	xassert(dev);

	printf("%s file descriptor = %d\n", file_name, fd);
	xassert(mg_device_major(dev) == major);
	xassert(mg_device_minor(dev) == minor);

	/* device still valid */
	xassert(dev);

	/* destroy */
	dev = mg_device_destroy(dev);

	/* device still valid */
	xassert(dev == 0);
}

void
mg_device()
{
	test_device("/dev/no_such_device", -1, -1);
	test_device("/bin/ls", -1, -1);
	test_device("/dev/null", 1, 3);
	test_device("/dev/zero", 1, 5);
}

int
main()
{
	return debug_test(mg_device);
}

#endif /* def DEBUG_DEVICE */
