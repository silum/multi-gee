## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id$
## Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

DIST_SUBDIRS = src

bin_PROGRAMS = src/dvideo

# noinst_LTLIBRARIES = src/bar.la

# INCLUDES = -I $(top_srcdir)/include

# noinst_HEADERS = include/bar.h

src_dvideo_SOURCES = src/dvideo.c
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

