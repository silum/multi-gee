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

#include "xmalloc.h"

USE_XASSERT;

CLASS(multi_gee, multi_gee_t)
{
	sllist_t device;
};

multi_gee_t
mg_create()
{
	multi_gee_t multi_gee;
	NEWOBJ(multi_gee);

	multi_gee->device = 0;

	return multi_gee;
}

multi_gee_t
mg_destroy(multi_gee_t multi_gee)
{
	VERIFYZ(multi_gee) {
//		while (device) {

//			device =
//			while (multi_gee->device_list != multi_gee->device_list->next) {
//				
//			}
//		}
//		mg_device_destroy(
		FREE(multi_gee);
	}

	return 0;
}
