/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "xmalloc.h"
#include "mg_device.h"

USE_XASSERT;

CLASS(mg_device, mg_device_t)
{

};

mg_device_t
mg_device_create()
{
	mg_device_t mg_device;
	NEWOBJ(mg_device);

	return mg_device;
}

mg_device_t
mg_device_destroy(mg_device_t mg_device)
{
	VERIFYZ(mg_device) {
		FREE(mg_device);
	}

	return 0;
}
