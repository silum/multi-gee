/* $Id$ */

/*
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Revision:  $Rev$
 * Date:      $Date$
 */

#include "mg_frame.h"

#include "xmalloc.h"

USE_XASSERT;

CLASS(mg_frame, mg_frame_t)
{
	int device_id;
	char const **image;
	struct timespec timestamp;
	int sequence;
	mg_frame_t const * next;
};

