## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id$
## Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

DIST_SUBDIRS = src

AM_CFLAGS = -I$(top_srcdir)/src -std=gnu99

TESTS = \
    src/mg_device \
    src/mg_frame \
    src/multi_gee \
    src/sllist \
    src/test_mg \
    src/test_sllist

noinst_PROGRAMS = \
    $(TESTS) \
    src/capture


#src/dvideo
#noinst_PROGRAMS = src/capture

#lib_LTLIBRARIES = src/libmulti-gee.la

src_mg_device_CFLAGS = -DDEBUG
src_mg_device_SOURCES = \
    src/debug_xassert.c \
    src/mg_device.c \
    src/xmalloc.c

src_mg_frame_CFLAGS = -DDEBUG
src_mg_frame_SOURCES = \
    src/debug_xassert.c \
    src/mg_frame.c \
    src/xmalloc.c

src_multi_gee_CFLAGS = -DDEBUG_MG
src_multi_gee_SOURCES = \
    src/debug_xassert.c \
    src/mg_device.c \
    src/mg_frame.c \
    src/multi-gee.c \
    src/sllist.c \
    src/xmalloc.c

src_sllist_CFLAGS = -DDEBUG
src_sllist_SOURCES = \
    src/debug_xassert.c \
    src/sllist.c \
    src/xmalloc.c

#src_mg_test_LDADD = -Lsrc -lmulti-gee
src_test_mg_SOURCES = \
    src/debug_xassert.c \
    src/mg_device.c \
    src/mg_frame.c \
    src/multi-gee.c \
    src/sllist.c \
    src/test_mg.c \
    src/xmalloc.c

src_test_sllist_SOURCES = \
    src/debug_xassert.c \
    src/sllist.c \
    src/test_sllist.c \
    src/xmalloc.c

#src_libmulti_gee_la_SOURCES = \
#    src/mutli-gee.c

# INCLUDES = -I $(top_srcdir)/include

# noinst_HEADERS = include/bar.h

#src_dvideo_SOURCES = src/dvideo.c
# foo_LDADD = src/bar.la

# src_bar_la_SOURCES = src/bar.cc
# src_bar_la_LDFLAGS = -module

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

