/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mg_device.h"
#include "xmalloc.h"

USE_XASSERT;

CLASS(mg_device, mg_device_t)
{
	int fd;
	char *name;
	int major;
	int minor;
	mg_buffer_t buffer;
	unsigned int num_bufs;
};

mg_device_t
mg_device_create(char *name)
{
	mg_device_t mg_device;
	NEWOBJ(mg_device);

	mg_device->fd = -1;
	mg_device->major = -1;
	mg_device->minor = -1;

	mg_device->name = xstrdup(name, __FILE__, __LINE__);

	struct stat st;
	if (0 != stat(name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			name, errno, strerror(errno));
	} else if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", name);
	} else {
		mg_device->major = major(st.st_rdev);
		mg_device->minor = minor(st.st_rdev);
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

		xfree(mg_device->name);
		mg_buffer_destroy(mg_device->buffer);

		FREEOBJ(mg_device);
	}

	return 0;
}

char *
mg_device_name(mg_device_t mg_device)
{
	char * name = 0;
	VERIFY(mg_device) {
		name = mg_device->name;
	}

	return name;
}

int
mg_device_open(mg_device_t mg_device)
{
	VERIFY(mg_device) {
		if (mg_device->fd == -1)
			mg_device->fd = open(mg_device->name,
					     O_RDWR | O_SYNC |
					     O_NONBLOCK);
		if (-1 == mg_device->fd) {
			fprintf(stderr, "Cannot open '%s': %d, %s\n",
				mg_device->name, errno, strerror(errno));
		}
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

void
test_device(char *name,
	    int major,
	    int minor)
{
	/* create */
	mg_device_t dev;
	dev = mg_device_create(name);

	XASSERT(mg_device_fd(dev) == -1);
	XASSERT(mg_device_major(dev) == major);
	XASSERT(mg_device_minor(dev) == minor);

	/* device still valid */
	XASSERT(dev);

	int fd = mg_device_open(dev);
	XASSERT(fd == mg_device_fd(dev));

	/* device still valid */
	xassert(dev) { }

	printf("%s file descriptor = %d\n", name, fd);
	XASSERT(mg_device_major(dev) == major);
	XASSERT(mg_device_minor(dev) == minor);

	/* device still valid */
	XASSERT(dev);

	/* destroy */
	dev = mg_device_destroy(dev);

	/* device still valid */
	XASSERT(dev == 0);
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