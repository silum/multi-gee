# configure.in -- Process this file with autoconf to produce a configure script.
# $Id$
# Copyright (C) 2004 Deneys S. Maartens <dsm@tlabs.ac.za>

AC_PREREQ(2.57)
AC_INIT([multi-gee], [0.91], [dsm@tlabs.ac.za])
LIBVERSION="0.0.0"
AC_SUBST(LIBVERSION)

AC_REVISION($Rev$)

AC_CONFIG_SRCDIR(multi-gee/multi-gee.h)
AC_CONFIG_AUX_DIR(config)
AC_SUBST(ac_aux_dir)

AM_INIT_AUTOMAKE([1.8 subdir-objects foreign])

# create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

AC_SUBST(LIBTOOL_DEPS)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/cdefs.h stdbool.h],,
    AC_MSG_ERROR([required header file missing]))

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
    doxygen.conf
    example/Makefile
    Makefile
    multi-gee/Makefile
])
AC_OUTPUT
