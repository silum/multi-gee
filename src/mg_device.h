/* $Id$ */
/*
 * multi-gee Frame Internals
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_MG_DEVICE_H
#define DSM_MG_DEVICE_H 1

#include "classdef.h"

#include "multi-gee.h"

__BEGIN_DECLS

NEWHANDLE(mg_device_t); /* multi-gee device object handle */

/* create object */
mg_device_t /* new handle */
mg_device_create();

/* destroy object */
mg_device_t
mg_device_destroy(mg_device_t /* object to destroy */);

__END_DECLS

#endif /* ndef DSM_MULTI_GEE_H */
