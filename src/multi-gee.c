/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "multi-gee.h"
#include "mg_device.h"
#include "sllist.h"
#include "string.h"

#include "xmalloc.h"

USE_XASSERT;

CLASS(multi_gee, multi_gee_t)
{
	bool busy;
	bool halt;

	sllist_t device_list;
	void (*callback)(sllist_t frame_list);
};

multi_gee_t
mg_create()
{
	multi_gee_t multi_gee;
	NEWOBJ(multi_gee);

	multi_gee->busy = false;
	multi_gee->halt = false;
	multi_gee->device_list = 0;
	multi_gee->callback = 0;

	return multi_gee;
}

multi_gee_t
mg_destroy(multi_gee_t multi_gee)
{
	VERIFYZ(multi_gee) {
		multi_gee->device_list = sll_empty(multi_gee->device_list);
		FREEOBJ(multi_gee);
	}

	return 0;
}

static
sllist_t
mg_capture_sync(sllist_t device_list)
{
}

int
mg_capture(multi_gee_t multi_gee,
	   int n)
{
	int ret = -3;

	VERIFY(multi_gee) {
		int count = 0;
		int sync_count = 0;

		bool done = false;
		while (!done) {
			/* assume we are done */
			done = true;

			/* find a reason to be done */
			if (!multi_gee->callback)
				ret = -3;
			else if (multi_gee->busy)
				ret = -2;
			else if (sync_count > 3)
				ret = -1;
			else if (n > 0 && count > n)
				ret = 0;
			else if (!multi_gee->device_list)
				ret = 1;
			else if (multi_gee->halt)
				ret = 2;
			else {
				/* ok... so we're not done */
				done = false;
				multi_gee->busy = true;

				sllist_t frame_list = mg_capture_sync(multi_gee->device_list);
				if (frame_list) {
					sync_count = 0;
					multi_gee->callback(frame_list);
				} else
					++sync_count;

				while (frame_list)
					frame_list =
						sll_remove_data(frame_list,
								sll_data(frame_list));
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

multi_gee_t
mg_register_callback(multi_gee_t multi_gee,
		     void (*callback)(const sllist_t))
{
	VERIFY(multi_gee) {
		if (callback)
			multi_gee->callback = callback;
	}
}


static
mg_device_t
find_device(sllist_t list,
	    mg_device_t seek)
{
	int major = mg_device_major(seek);
	int minor = mg_device_minor(seek);

	mg_device_t found = 0;
	while (list) {
		found = sll_data(list);
		if (mg_device_major(seek) == major
		    && mg_device_minor(seek) == minor)
			break;

		list = sll_next(list);
	}

	return found;
}

static
mg_device_t
find_device_id(sllist_t list,
	       int seek)
{
	mg_device_t found = 0;
	while (list) {
		found = sll_data(list);
		if (mg_device_fd(found) == seek)
			break;

		list = sll_next(list);
	}

	return found;
}

int
mg_register_device(multi_gee_t multi_gee,
		   char *file_name)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t new = mg_device_create(file_name);

		/* can device be registered? */
		if (mg_device_major(new) == -1
		    || mg_device_minor(new) == -1) {
			new = mg_device_destroy(new);
			ret = -1;
		} else {
			/* is device already registered? */
			mg_device_t item = find_device(multi_gee->device_list, new);
			if (item) {
				new = mg_device_destroy(new);
				ret = mg_device_fd(item);
			}
		}
		
		/* everything ok, add it to list */
		if (new) {
			multi_gee->device_list = sll_insert_data(multi_gee->device_list, new);

			ret = mg_device_fd(new);
		}
	}

	return ret;
}

int
mg_deregister_device(multi_gee_t multi_gee,
		     int id)
{
	int ret = -1;

	VERIFY(multi_gee) {
		mg_device_t device = find_device_id(multi_gee->device_list, id);
		if (device) {
			multi_gee->device_list = sll_remove_data(multi_gee->device_list, device);
			mg_device_destroy(device);
			ret = id;
		}
	}

	return ret;
}

#ifdef DEBUG_MG

#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include "debug_xassert.h"

void
multi_gee()
{

}

int
main()
{
	return debug_test(multi_gee);
}

#endif /* def DEBUG_MG */
