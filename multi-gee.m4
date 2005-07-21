dnl $Id$
dnl Copyright (C) 2005 Deneys S. Maartens <dsm@tlabs.ac.za>

dnl AM_LIB_MULTI_GEE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for MULTI_GEE, and define MULTI_GEE_LIBS
dnl
AC_DEFUN([AM_LIB_MULTI_GEE],
[AC_ARG_WITH([multi-gee],
    [AC_HELP_STRING([--with-multi-gee=DIR],
        [multi-gee base directory, or:])],
    [multi_gee="$withval"])

AC_ARG_WITH([multi-gee-include],
    [AC_HELP_STRING([--with-multi-gee-include=DIR],
        [multi-gee headers directory])],
    [multi_gee_include="$withval"])

# Disable multi-gee support.
if test "x$with_multi_gee_include" = "xno"; then
    with_multi_gee="no"
fi

AC_ARG_WITH([multi-gee-lib],
    [AC_HELP_STRING([--with-multi-gee-lib=DIR],
        [multi-gee library directory])],
    [multi_gee_lib="$withval"])

# Disable multi-gee support.
if test "x$with_multi_gee_lib" = "xno"; then
    with_multi_gee="no"
fi

AC_SUBST(MULTI_GEE_LIBS)
if test "x$with_multi_gee" != "xno"; then
    if test "x$multi_gee" = "xyes"; then
        multi_gee=
    fi

    # Base directory takes precendence over lib and include.
    if test "x$multi_gee" != "x" ; then
        if test "x$multi_gee_include" = "x"; then
            multi_gee_include=$multi_gee/include
        fi

        if test "x$multi_gee_lib" = "x"; then
            multi_gee_lib=$multi_gee/lib
        fi
    fi

    # set include directory
    if test "x$multi_gee_include" != "x" ; then
        if test -d "$multi_gee_include"; then
            CPPFLAGS="$CPPFLAGS -I$multi_gee_include"
        fi
    fi

    # set library directory
    if test "x$multi_gee_lib" != "x" ; then
        if test -d "$multi_gee_lib"; then
            LDFLAGS="$LDFLAGS -L$multi_gee_lib"
        fi
    fi

    # Check for library.
    AC_CHECK_LIB([multi-gee], [mg_create])
    if test "$ac_cv_lib_multi_gee_mg_greate" != "yes" ; then
        with_multi_gee="no"
    fi

    # Check for headers.
    AC_CHECK_HEADERS([multi-gee.h], [],
        with_multi_gee="no"
    )

    if test "x$with_multi_gee" != "xno"; then
        MULTI_GEE_LIBS="-lmulti_gee"
        ifelse([$1], [], :, [$1])
    else
        MULTI_GEE_LIBS=
        ifelse([$2], [], :, [$2])
    fi
fi])

dnl -fin-
