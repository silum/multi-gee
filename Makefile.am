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
    examples \
    multi-gee \
    tests

AM_CFLAGS = -std=gnu99 -Wall -Wextra -Werror
# -DDEBUG_SELECT

lib_LTLIBRARIES = multi-gee/libmulti-gee.la

TESTS = \
    multi-gee/mg_buffer \
    multi-gee/mg_device \
    multi-gee/mg_frame \
    multi-gee/sllist \
    tests/sllist

#bin_PROGRAMS = \
#    examples/check-rate

noinst_PROGRAMS = \
    $(TESTS) \
    examples/check-rate \
    examples/mg-tweak \
    examples/multi-gee \
    multi-gee/multi-gee

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
    examples/pgm_util.h \
    examples/process_util.h \
    linux/videodev2.h


examples_check_rate_CPPFLAGS = \
    -I$(top_srcdir)/multi-gee
examples_check_rate_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la \
    -lncurses
examples_check_rate_SOURCES = \
    examples/check-rate.c

examples_multi_gee_CPPFLAGS = \
    -I$(top_srcdir)/multi-gee
examples_multi_gee_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la
examples_multi_gee_SOURCES = \
    examples/multi-gee.c \
    examples/pgm_util.c \
    examples/process_util.c

examples_mg_tweak_CPPFLAGS = \
    -I$(top_srcdir)/multi-gee
examples_mg_tweak_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la \
    -lm
examples_mg_tweak_SOURCES = \
    examples/mg-tweak.c
multi_gee_libmulti_gee_la_CPPFLAGS = $(AM_CPPFLAGS)
multi_gee_libmulti_gee_la_LDFLAGS = \
    -version-info $(LIBVERSION)
multi_gee_libmulti_gee_la_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c

multi_gee_mg_buffer_CPPFLAGS = -DDEBUG_BUFFER
multi_gee_mg_buffer_SOURCES = \
    multi-gee/mg_buffer.c

multi_gee_mg_device_CPPFLAGS = -DDEBUG_DEVICE
multi_gee_mg_device_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c

multi_gee_mg_frame_CPPFLAGS = -DDEBUG_FRAME
multi_gee_mg_frame_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c

multi_gee_sllist_CPPFLAGS = -DDEBUG_SLLIST
multi_gee_sllist_SOURCES = \
    multi-gee/sllist.c

multi_gee_multi_gee_CPPFLAGS = -DDEBUG_MULTI_GEE
multi_gee_multi_gee_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c

tests_sllist_LDADD = \
    $(top_builddir)/multi-gee/libmulti-gee.la
tests_sllist_SOURCES = \
    multi-gee/sllist.c \
    tests/sllist.c

.PHONY: doc
doc: doc/doxy/doxygen.conf
	doxygen $< > /dev/null

maintainer-clean-local:
	-rm -rf doc/html/ doc/latex/

.PHONY: distrpm
distrpm: distcheck
	rpmbuild -ta $(distdir).tar.gz

EXTRA_DIST = \
    $(ac_aux_dir)/autogen.sh \
    $(ac_aux_dir)/bootstrap \
    $(ac_aux_dir)/clobber \
    BUILD \
    $(PACKAGE_NAME).spec

AUX_DIST = \
    $(ac_aux_dir)/compile \
    $(ac_aux_dir)/config.guess \
    $(ac_aux_dir)/config.sub \
    $(ac_aux_dir)/depcomp \
    $(ac_aux_dir)/install-sh \
    $(ac_aux_dir)/ltmain.sh \
    $(ac_aux_dir)/missing \
    $(ac_aux_dir)/mkinstalldirs

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
