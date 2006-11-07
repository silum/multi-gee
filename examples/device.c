/* $Id$
 * Copyright (C) 2006 Deneys S. Maartens <dsm@tlabs.ac.za>
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
  */
#include "device.h" /* class implemented */

USE_XASSERT

/**
 * @brief Device object structure
 */
CLASS(device, device_t)
{
	char *id;
	struct timeval tv;
	struct timeval diff;
	int seq;
};

device_t
dev_create(const char *id,
	   struct timeval tv,
	   struct timeval diff,
	   int seq)
{
	device_t device;
	NEWOBJ(device);

	STRDUP(device->id, id);
	device->tv = tv;
	device->diff = diff;
	device->seq = seq;

	return device;
}

device_t
dev_destroy(device_t device)
{
	VERIFYZ(device) {
		FREEOBJ(device->id);
		FREEOBJ(device);
	}
	return 0;
}

const char *
dev_id(device_t device)
{
	const char *id = 0;

	VERIFYZ(device) {
		id = device->id;
	}

	return id;
}

struct timeval
dev_tv(device_t device)
{
	struct timeval tv = {0, 0};

	VERIFYZ(device) {
		tv = device->tv;
	}

	return tv;
}

struct timeval
dev_diff(device_t device)
{
	struct timeval diff = {0, 0};

	VERIFYZ(device) {
		diff = device->diff;
	}

	return diff;
}

int
dev_seq(device_t device)
{
	int seq = -1;

	VERIFYZ(device) {
		seq = device->seq;
	}

	return seq;
}
