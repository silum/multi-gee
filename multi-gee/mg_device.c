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
 * @brief Multi-gee device definition
 */
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mg_device.h" /* class implemented */

USE_XASSERT

/**
 * @brief Device object structure
 */
CLASS(mg_device, mg_device_t)
{
	int fd; /**< Device file descriptor */
	char *name; /**< Device file name */
	dev_t devno; /**< Device number */
	mg_buffer_t buffer; /**< Frame buffer object handle */
	unsigned int no_bufs; /**< Number of capture buffers */
	void *userptr; /**< User defined pointer */
};

mg_device_t
mg_device_create(const char *name,
		 unsigned int no_bufs,
		 log_t log,
		 void *userptr)
{
	mg_device_t mg_device;
	NEWOBJ(mg_device);

	mg_device->fd = -1;
	mg_device->devno = -1;

	STRDUP(mg_device->name, name);

	struct stat st;
	if (0 != stat(name, &st)) {
		lg_errno(log, "cannot identify '%s':", name);
	} else if (!S_ISCHR(st.st_mode)) {
		lg_log(log, "%s is no device", name);
	} else {
		mg_device->devno = st.st_rdev;
	}

	mg_device->no_bufs = no_bufs;
	mg_device->buffer = mg_buffer_create();
	mg_device->userptr = userptr;

	return mg_device;
}

mg_device_t
mg_device_destroy(mg_device_t mg_device)
{
	VERIFYZ(mg_device) {
		if (-1 != mg_device->fd) {
			if (close(mg_device->fd)) {
				warn("close(%d)", mg_device->fd);
			}
		}

		FREEOBJ(mg_device->name);
		mg_buffer_destroy(mg_device->buffer);

		FREEOBJ(mg_device);
	}

	return 0;
}

mg_buffer_t
mg_device_get_buffer(mg_device_t mg_device)
{
	mg_buffer_t p = 0;
	VERIFY(mg_device) {
		p = mg_device->buffer;
	}

	return p;
}

dev_t
mg_device_get_devno(mg_device_t mg_device)
{
	dev_t devno = makedev(-1,-1);
	VERIFY(mg_device) {
		devno = mg_device->devno;
	}

	return devno;
}

int
mg_device_get_fd(mg_device_t mg_device)
{
	int fd = -1;
	VERIFY(mg_device) {
		fd = mg_device->fd;
	}

	return fd;
}

char *
mg_device_get_name(mg_device_t mg_device)
{
	char * name = 0;
	VERIFY(mg_device) {
		name = mg_device->name;
	}

	return name;
}

unsigned int
mg_device_get_no_bufs(mg_device_t mg_device)
{
	unsigned int no_bufs = 0;
	VERIFY(mg_device) {
		no_bufs = mg_device->no_bufs;
	}

	return no_bufs;
}

void *
mg_device_get_userptr(mg_device_t mg_device)
{
	void *userptr = 0;
	VERIFY(mg_device) {
		userptr = mg_device->userptr;
	}

	return userptr;
}

int
mg_device_open(mg_device_t mg_device)
{
	VERIFY(mg_device) {
		if (mg_device->fd == -1) {
			mg_device->fd = open(mg_device->name,
					     (O_RDWR
					      | O_SYNC
					      | O_NONBLOCK));
		}
		if (-1 == mg_device->fd) {
			fprintf(stderr, "Cannot open '%s': %d, %s\n",
				mg_device->name, errno, strerror(errno));
		}
	}

	return mg_device->fd;
}

#ifdef TEST_MULTI_GEE_MG_DEVICE

#include <stdlib.h>
#include <stdio.h>

void
test_device(const char *name,
	    dev_t devno,
	    void *userptr)
{
	printf("%s(%s, {%d, %d}, %p)\n",
	       __func__,
	       name,
	       major(devno),
	       minor(devno),
	       userptr);
	fflush(0);

	mg_device_t dev;
	log_t log = lg_create("mg_device", "stderr");

	/* create */
	dev = mg_device_create(name, 3, log, userptr);

	XASSERT(mg_device_get_fd(dev) == -1) {
		/* empty */
	}
	XASSERT(mg_device_get_devno(dev) == devno) {
		/* empty */
	}

	/* device still valid */
	XASSERT(dev) {
		/* empty */
	}

	int fd = mg_device_open(dev);
	XASSERT(fd == mg_device_get_fd(dev)) {
		/* empty */
	}

	/* device still valid */
	XASSERT(dev) {
		/* empty */
	}

	printf("%s file descriptor = %d\n", name, fd);
	XASSERT(mg_device_get_devno(dev) == devno) {
		/* empty */
	}

	/* device still valid */
	XASSERT(dev) {
		/* empty */
	}

	printf("user pointer = %p\n", userptr);
	XASSERT(mg_device_get_userptr(dev) == userptr) {
		/* empty */
	}

	/* device still valid */
	XASSERT(dev) {
		/* empty */
	}

	/* destroy */
	dev = mg_device_destroy(dev);

	/* device still valid */
	XASSERT(dev == 0) {
		/* empty */
	}

	log = lg_destroy(log);
}

void
mg_device()
{
	test_device("/dev/no_such_device", makedev(-1, -1), 0);
	test_device("/bin/ls", makedev(-1, -1), (void *) -1);
	test_device("/dev/null", makedev(1, 3), (void *) 1);
	test_device("/dev/zero", makedev(1, 5), mg_device);
}

int
main()
{
	exit(cclass_assert_test(mg_device));
}

#endif /* TEST_MULTI_GEE_MG_DEVICE */
