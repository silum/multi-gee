# configure.in -- Process this file with autoconf to produce a configure script.
# $Id$
# Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

AC_PREREQ(2.57)
AC_INIT([multi-gee], [0.0], [dsm@tlabs.ac.za])

AC_REVISION($Rev$)

AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_AUX_DIR(config)
AC_SUBST(ac_aux_dir)

dnl AM_INIT_AUTOMAKE([1.8 dist-bzip2 subdir-objects])
# add foreign if this project is not GNU compliant
AM_INIT_AUTOMAKE([1.8 dist-bzip2 subdir-objects foreign])

# create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER([config.h])

AC_SUBST(VERSION)
dnl AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
    Makefile
])
AC_OUTPUT
