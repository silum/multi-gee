## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id$
## Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

DIST_SUBDIRS = multi-gee

AM_CFLAGS = -std=gnu99 -Wall -Wextra -Werror

#noinst_PROGRAMS = multi-gee/capture

lib_LTLIBRARIES = multi-gee/multi-gee.la

TESTS = \
    multi-gee/mg_buffer \
    multi-gee/mg_device \
    multi-gee/mg_frame \
    multi-gee/sllist \
    multi-gee/test_sllist \
    multi-gee/tv_util

bin_PROGRAMS = \
    multi-gee/check-rate

noinst_PROGRAMS = \
    $(TESTS) \
    multi-gee/capture \
    multi-gee/dvideo \
    multi-gee/sync-stream \
    multi-gee/multi-gee

nobase_include_HEADERS = \
    multi-gee.h \
    multi-gee/classdef.h \
    multi-gee/debug_xassert.h \
    multi-gee/fg_util.h \
    multi-gee/log.h \
    multi-gee/mg_buffer.h \
    multi-gee/mg_device.h \
    multi-gee/mg_frame.h \
    multi-gee/sllist.h \
    multi-gee/tv_util.h \
    multi-gee/xmalloc.h

noinst_HEADERS = \
    linux/videodev2.h

multi_gee_multi_gee_la_LDFLAGS = -module
multi_gee_multi_gee_la_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c \
    multi-gee/tv_util.c \
    multi-gee/xmalloc.c

multi_gee_multi_gee_la_CPPFLAGS = $(AM_CPPFLAGS)

multi_gee_mg_buffer_CPPFLAGS = -DDEBUG_BUFFER
multi_gee_mg_buffer_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/mg_buffer.c \
    multi-gee/xmalloc.c

multi_gee_mg_device_CPPFLAGS = -DDEBUG_DEVICE
multi_gee_mg_device_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/xmalloc.c

multi_gee_mg_frame_CPPFLAGS = -DDEBUG_FRAME
multi_gee_mg_frame_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/xmalloc.c

multi_gee_sllist_CPPFLAGS = -DDEBUG_SLLIST
multi_gee_sllist_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/sllist.c \
    multi-gee/xmalloc.c

multi_gee_test_sllist_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/sllist.c \
    multi-gee/test_sllist.c \
    multi-gee/xmalloc.c

multi_gee_tv_util_CPPFLAGS = -DDEBUG_TV_UTIL
multi_gee_tv_util_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/tv_util.c \
    multi-gee/xmalloc.c

multi_gee_multi_gee_CPPFLAGS = -DDEBUG_MULTI_GEE
multi_gee_multi_gee_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c \
    multi-gee/tv_util.c \
    multi-gee/xmalloc.c

multi_gee_sync_stream_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/sllist.c \
    multi-gee/sync-stream.c \
    multi-gee/xmalloc.c

multi_gee_check_rate_LDADD = \
    -lcurses

multi_gee_check_rate_SOURCES = \
    multi-gee/check-rate.c \
    multi-gee/multi-gee.la

#multi_gee_mg_test_LDADD = -Lmulti-gee -lmulti-gee
# multi_gee_test_mg_SOURCES = \
#     multi-gee/debug_xassert.c \
#     multi-gee/mg_buffer.c \
#     multi-gee/mg_device.c \
#     multi-gee/mg_frame.c \
#     multi-gee/multi-gee.c \
#     multi-gee/sllist.c \
#     multi-gee/test_mg.c \
#     multi-gee/xmalloc.c

# multi_gee_test_multi_gee_SOURCES = \
#     multi-gee/test_multi-gee.c \
#     multi-gee/fg_util.c \
#     multi-gee/mg_buffer.c \
#     multi-gee/mg_device.c \
#     multi-gee/mg_frame.c \
#     multi-gee/multi-gee.c \
#     multi-gee/sllist.c \
#     multi-gee/tv_util.c \
#     multi-gee/xmalloc.c

# INCLUDES = -I $(top_srcdir)/include

# noinst_HEADERS = include/bar.h

#multi_gee_dvideo_SOURCES = multi-gee/dvideo.c

AUX_DIR = @ac_aux_dir@

EXTRA_DIST = \
    $(AUX_DIR)/bootstrap \
    $(AUX_DIR)/clobber

AUX_DIST = \
    $(AUX_DIR)/compile \
    $(AUX_DIR)/config.guess \
    $(AUX_DIR)/config.sub \
    $(AUX_DIR)/depcomp \
    $(AUX_DIR)/install-sh \
    $(AUX_DIR)/ltmain.sh \
    $(AUX_DIR)/missing \
    $(AUX_DIR)/mkinstalldirs

DISTCLEANFILES = \
    stamp-h \
    stamp-h[0-9]*

MAINTAINERCLEANFILES = \
    aclocal.m4 \
    config.h.in \
    configure \
    Makefile.in \
    $(AUX_DIST)

