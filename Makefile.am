## $Id$
## Copyright (C) 2004, 2005, 2006 Deneys S. Maartens <dsm@tlabs.ac.za>
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

## Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = \
    -std=gnu99 \
    -Wall \
    -Werror \
    -Wextra
AM_CPPFLAGS = \
    $(CCLASS_CPPFLAGS)

DIST_SUBDIRS = \
    examples \
    m4 \
    multi-gee

aclocaldir = \
    $(datadir)/aclocal
dist_aclocal_DATA = \
    m4/multi-gee.m4

dist_noinst_DATA = \
    examples/sync-grammar.y \
    examples/sync-scanner.l

nobase_include_HEADERS = \
    multi-gee/fg_util.h \
    multi-gee/log.h \
    multi-gee/mg_buffer.h \
    multi-gee/mg_device.h \
    multi-gee/mg_frame.h \
    multi-gee/multi-gee.h \
    multi-gee/sllist.h \
    multi-gee/tv_util.h

noinst_HEADERS = \
    examples/device.h \
    examples/pgm_util.h \
    examples/process_util.h \
    linux/videodev2.h

lib_LTLIBRARIES = \
    multi-gee/libmulti-gee.la

check_PROGRAMS = \
    $(TESTS)

noinst_PROGRAMS = \
    examples/mg-tweak \
    examples/multi-gee \
    examples/sllist \
    examples/sync-check \
    multi-gee/multi-gee \
    multi-gee/multi-gee-select

TESTS = \
    multi-gee/mg_buffer \
    multi-gee/mg_device \
    multi-gee/mg_frame \
    multi-gee/sllist

examples_sllist_LDADD = \
    multi-gee/libmulti-gee.la
examples_sllist_SOURCES = \
    examples/sllist.c

examples_sync_check_CFLAGS = \
    $(AM_CFLAGS) \
    $(GLIB_CFLAGS)
examples_sync_check_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(GLIB_CPPFLAGS)
examples_sync_check_LDADD = \
    multi-gee/libmulti-gee.la
examples_sync_check_LDFLAGS = \
    $(GLIB_LIBS) \
    -lm
examples_sync_check_SOURCES = \
    examples/device.c \
    examples/lex.yy.c \
    examples/sync-check.c \
    examples/y.tab.c

examples_multi_gee_LDADD = \
    multi-gee/libmulti-gee.la
examples_multi_gee_SOURCES = \
    examples/multi-gee.c \
    examples/pgm_util.c \
    examples/process_util.c

examples_mg_tweak_LDADD = \
    multi-gee/libmulti-gee.la \
    -lm
examples_mg_tweak_SOURCES = \
    examples/mg-tweak.c

multi_gee_libmulti_gee_la_LDFLAGS = \
    -version-info $(LIBVERSION)
multi_gee_libmulti_gee_la_LIBADD = \
    $(CCLASS_LIBS)
multi_gee_libmulti_gee_la_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c

multi_gee_mg_buffer_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DTEST_MULTI_GEE_MG_BUFFER
multi_gee_mg_buffer_LDADD = \
    $(CCLASS_LIBS)
multi_gee_mg_buffer_SOURCES = \
    multi-gee/mg_buffer.c

multi_gee_mg_device_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DTEST_MULTI_GEE_MG_DEVICE
multi_gee_mg_device_LDADD = \
    $(CCLASS_LIBS)
multi_gee_mg_device_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c

multi_gee_mg_frame_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DTEST_MULTI_GEE_MG_FRAME
multi_gee_mg_frame_LDADD = \
    $(CCLASS_LIBS)
multi_gee_mg_frame_SOURCES = \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c

multi_gee_sllist_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DTEST_SLLIST
multi_gee_sllist_LDADD = \
    $(CCLASS_LIBS) \
    -lcheck
multi_gee_sllist_SOURCES = \
    multi-gee/sllist.c \
    multi-gee/verbose-argp.c \
    multi-gee/test-runner.c

multi_gee_multi_gee_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DTEST_MULTI_GEE_MULTI_GEE
multi_gee_multi_gee_LDADD = \
    $(CCLASS_LIBS)
multi_gee_multi_gee_SOURCES = \
    multi-gee/fg_util.c \
    multi-gee/log.c \
    multi-gee/mg_buffer.c \
    multi-gee/mg_device.c \
    multi-gee/mg_frame.c \
    multi-gee/multi-gee.c \
    multi-gee/sllist.c

multi_gee_multi_gee_select_CPPFLAGS = \
    $(multi_gee_multi_gee_CPPFLAGS) \
    -DDEBUG_SELECT
multi_gee_multi_gee_select_LDADD = \
    $(multi_gee_multi_gee_LDADD)
multi_gee_multi_gee_select_SOURCES = \
    $(multi_gee_multi_gee_SOURCES)

examples/y.tab.h examples/y.tab.c: $(srcdir)/examples/sync-grammar.y
	$(YACC) -d $(srcdir)/examples/sync-grammar.y
	mv -f y.tab.h y.tab.c examples/

examples/lex.yy.c: $(srcdir)/examples/sync-scanner.l examples/y.tab.h
	$(LEX) $(srcdir)/examples/sync-scanner.l
	mv -f lex.yy.c examples/

.PHONY: doc
doc: doc/doxy/doxygen.conf
	doxygen $< > /dev/null

maintainer-clean-local:
	-rm -rf doc/html/ doc/latex/

.PHONY: dist-rpm
dist-rpm: distcheck
	rpmbuild -ta $(distdir).tar.gz

AUX_DIST = \
    $(ac_aux_dir)/compile \
    $(ac_aux_dir)/config.guess \
    $(ac_aux_dir)/config.sub \
    $(ac_aux_dir)/depcomp \
    $(ac_aux_dir)/install-sh \
    $(ac_aux_dir)/ltmain.sh \
    $(ac_aux_dir)/missing \
    $(ac_aux_dir)/mkinstalldirs

CLEANFILES = \
    examples/sync-scanner.c \
    examples/sync-grammar.c \
    examples/sync-grammar.h \
    examples/lex.yy.c \
    examples/y.tab.c \
    examples/y.tab.h

DISTCLEANFILES = \
    stamp-h \
    stamp-h[0-9]*

EXTRA_DIST = \
    $(ac_aux_dir)/autogen.sh \
    $(ac_aux_dir)/clobber \
    BUILD \
    $(PACKAGE_NAME).spec

MAINTAINERCLEANFILES = \
    aclocal.m4 \
    config.h.in \
    configure \
    Makefile.in \
    $(AUX_DIST)

## -fin-
