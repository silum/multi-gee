## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id$
## Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

DIST_SUBDIRS = multi-gee src

AM_CFLAGS = -std=gnu99 -Wall -Wextra -Werror

lib_LTLIBRARIES = multi-gee/libmulti-gee.la

TESTS = \
    multi-gee/mg_buffer \
    multi-gee/mg_device \
    multi-gee/mg_frame \
    multi-gee/sllist \
    multi-gee/test_sllist \
    multi-gee/tv_util

bin_PROGRAMS = \
    src/check-rate

noinst_PROGRAMS = \
    $(TESTS) \
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

.PHONY: doc
doc:
	doxygen doxygen.conf

maintainer-clean-local:
	-rm -rf doc/html/ doc/latex/

multi_gee_libmulti_gee_la_CPPFLAGS = $(AM_CPPFLAGS)
multi_gee_libmulti_gee_la_LDFLAGS = -version-info @LIBVERSION@
multi_gee_libmulti_gee_la_SOURCES = \
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

multi_gee_mg_buffer_CPPFLAGS = -DDEBUG_BUFFER
multi_gee_mg_buffer_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/mg_buffer.c \
    multi-gee/xmalloc.c

multi_gee_mg_device_CPPFLAGS = -DDEBUG_DEVICE
multi_gee_mg_device_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/xmalloc.c

multi_gee_mg_frame_CPPFLAGS = -DDEBUG_FRAME
multi_gee_mg_frame_SOURCES = \
    multi-gee/debug_xassert.c \
    multi-gee/log.c \
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

src_check_rate_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la \
    -lncurses
src_check_rate_SOURCES = \
    src/check-rate.c \
    multi-gee/libmulti-gee.la

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

