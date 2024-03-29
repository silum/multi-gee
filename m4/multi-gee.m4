## $Id$
## Copyright (C) 2006  Deneys S. Maartens  <dsm@tlabs.ac.za>
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

dnl AM_LIB_MULTI_GEE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for MULTI_GEE, and define MULTI_GEE_CPPFLAGS and MULTI_GEE_LIBS

AC_DEFUN([AM_LIB_MULTI_GEE], [
  AC_ARG_WITH([multi-gee],
    [AC_HELP_STRING([--with-multi-gee@<:@=PATH@:>@],
      [prefix where multi-gee is installed [default=auto]])])

  AC_ARG_WITH([multi-gee-include],
    [AC_HELP_STRING([--with-multi-gee-include=PATH],
      [multi-gee include directory])])

  AC_ARG_WITH([multi-gee-lib],
    [AC_HELP_STRING([--with-multi-gee-lib=PATH],
      [multi-gee library directory])])

  if test "x$with_multi_gee_include" = "xno"; then
    with_multi_gee="no"
  fi

  if test "x$with_multi_gee_lib" = "xno"; then
    with_multi_gee="no"
  fi

  if test "x$with_multi_gee" != "xno"; then
    if test "x$with_multi_gee" = "xyes"; then
      with_multi_gee=
    fi

    # Prefix does not take precedence over include and lib.
    if test "x$with_multi_gee" != "x" ; then
      if test "x$with_multi_gee_include" = "x"; then
        with_multi_gee_include="$with_multi_gee/include"
      fi

      if test "x$with_multi_gee_lib" = "x"; then
        with_multi_gee_lib="$with_multi_gee/lib"
      fi
    fi

    # Set include directory.
    if test "x$with_multi_gee_include" != "x" ; then
      MULTI_GEE_CPPFLAGS="-I$with_multi_gee_include"
    fi

    # Set library directory.
    if test "x$with_multi_gee_lib" != "x" ; then
      MULTI_GEE_LIBS="-L$with_multi_gee_lib -lmulti-gee"
    else
      MULTI_GEE_LIBS="-lmulti-gee"
    fi

    # Check for library.
    ac_save_LIBS="$LIBS"
    LIBS="$MULTI_GEE_LIBS $LIBS"
    AC_CHECK_LIB(
      [multi-gee],
      [mg_create],
      [],
      [with_multi_gee="no"]
    )
    LIBS="$ac_save_LIBS"

    # Check for headers.
    ac_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$MULTI_GEE_CPPFLAGS $CPPFLAGS"
    AC_CHECK_HEADERS(
      [multi-gee/multi-gee.h],
      [],
      [with_multi_gee="no"]
    )
    CPPFLAGS="$ac_save_CPPFLAGS"
  fi

  if test "x$with_multi_gee" != "xno"; then
    ifelse([$1], [], [:], [$1])
  else
    MULTI_GEE_CPPFLAGS=""
    MULTI_GEE_LIBS=""
    ifelse([$2], [], [:], [$2])
  fi

  AC_SUBST(MULTI_GEE_CPPFLAGS)
  AC_SUBST(MULTI_GEE_LIBS)
])

dnl -fin-
dnl vim:sts=2:sw=2:
