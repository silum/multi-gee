## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id$
## Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation,
## Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

DIST_SUBDIRS = \
    example \
    multi-gee

AM_CFLAGS = -std=gnu99 -Wall -Wextra -Werror

lib_LTLIBRARIES = multi-gee/libmulti-gee.la

TESTS = \
    multi-gee/mg_buffer \
    multi-gee/mg_device \
    multi-gee/mg_frame \
    multi-gee/sllist \
    multi-gee/test_sllist \
    multi-gee/tv_util

#bin_PROGRAMS = \
#    example/check-rate

noinst_PROGRAMS = \
    $(TESTS) \
    multi-gee/multi-gee \
    example/multi-gee

include_HEADERS = \
    multi-gee/multi-gee.h

nobase_include_HEADERS = \
    multi-gee/fg_util.h \
    multi-gee/log.h \
    multi-gee/mg_buffer.h \
    multi-gee/mg_device.h \
    multi-gee/mg_frame.h \
    multi-gee/sllist.h \
    multi-gee/tv_util.h

noinst_HEADERS = \
    example/pgm_util.h \
    example/process_util.h \
    linux/videodev2.h

.PHONY: doc
doc:
	doxygen doc/doxy/doxygen.conf

maintainer-clean-local:
	-rm -rf doc/html/ doc/latex/

multi_gee_libmulti_gee_la_CPPFLAGS = $(AM_CPPFLAGS)
multi_gee_libmulti_gee_la_LDFLAGS = \
    -version-info @LIBVERSION@ \
    -lcclass
multi_gee_libmulti_gee_la_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c \
    multi-gee/tv_util.c

multi_gee_mg_buffer_CPPFLAGS = -DDEBUG_BUFFER
multi_gee_mg_buffer_LDFLAGS = -lcclass
multi_gee_mg_buffer_SOURCES = \
    multi-gee/mg_buffer.c

multi_gee_mg_device_CPPFLAGS = -DDEBUG_DEVICE
multi_gee_mg_device_LDFLAGS = -lcclass
multi_gee_mg_device_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c

multi_gee_mg_frame_CPPFLAGS = -DDEBUG_FRAME
multi_gee_mg_frame_LDFLAGS = -lcclass
multi_gee_mg_frame_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c

multi_gee_sllist_CPPFLAGS = -DDEBUG_SLLIST
multi_gee_sllist_LDFLAGS = -lcclass
multi_gee_sllist_SOURCES = \
    multi-gee/sllist.c

multi_gee_test_sllist_LDFLAGS = -lcclass
multi_gee_test_sllist_SOURCES = \
    multi-gee/sllist.c \
    multi-gee/test_sllist.c

multi_gee_tv_util_CPPFLAGS = -DDEBUG_TV_UTIL
multi_gee_tv_util_LDFLAGS = -lcclass
multi_gee_tv_util_SOURCES = \
    multi-gee/tv_util.c

multi_gee_multi_gee_CPPFLAGS = -DDEBUG_MULTI_GEE
multi_gee_multi_gee_LDFLAGS = -lcclass
multi_gee_multi_gee_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c \
    multi-gee/tv_util.c

example_check_rate_CPPFLAGS = -I$(top_srcdir)/multi-gee
example_check_rate_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la \
    -lncurses
example_check_rate_SOURCES = \
    example/check-rate.c \
    multi-gee/libmulti-gee.la

example_multi_gee_CPPFLAGS = -I$(top_srcdir)/multi-gee
example_multi_gee_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la
example_multi_gee_SOURCES = \
    example/multi-gee.c \
    example/pgm_util.c \
    example/process_util.c \
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

## -fin-
