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
#include "mg_buffer.h"

__BEGIN_DECLS

NEWHANDLE(mg_device_t); /* multi-gee device object handle */

/* create object */
mg_device_t /* new handle */
mg_device_create(char *file_name);

/* destroy object */
mg_device_t
mg_device_destroy(mg_device_t /* object to destroy */);

char * /* file name */
mg_device_file_name(mg_device_t);

int /* 1 - device already open,
       0 - action successful,
       -1 - failed to open device */
mg_device_open(mg_device_t);

int /* file descriptor,
       or -1 if device not open */
mg_device_fd(mg_device_t);

int /* device major number,
       or -1 if not a device */
mg_device_major(mg_device_t);

int /* device minor number,
       or -1 if not a device */
mg_device_minor(mg_device_t);

mg_buffer_t
mg_device_buffer(mg_device_t);

__END_DECLS

#endif /* ndef DSM_MG_DEVICE_H */
