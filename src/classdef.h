/* $Id$ */
/*
 * C private class methodology
 *
 * Based on the source presented in
 *
 * Writing Bug-Free C Code
 * A Programming Style That Automatically Detects Bugs in C Code
 * by Jerry Jongerius / January 1995
 *
 * http://www.duckware.com/bugfreec/
 * <last visited: Tue Sep 28 08:20:13 SAST 2004>
 */
#ifndef DSM_CLASSDEF_H
#define DSM_CLASSDEF_H 1

#include <sys/types.h> /* size_t */
#include <stdbool.h>   /* bool */

__BEGIN_DECLS

/* Assert during compiling (not run-time) */
#define compiler_assert(exp) extern char _compiler_assert[(exp)?1:-1]

/* Absolute value macro */
#define ABS(x) (((x)>0)?(x):-(x))

/* Is a number a power of two */
#define ISPOWER2(x) (!((x)&((x)-1)))

/* Number of static elements in an array */
#define NUMSTATICELS(array) (sizeof(array)/sizeof(*array))

/* Loop Macros */
#define LOOP(arg) { int _max=arg; int loop; \
    for (loop=0; loop<_max; ++loop)
#define ULOOP(arg) { unsigned int _max=arg; unsigned int loop; \
    for (loop=0; loop<_max; ++loop)
#define ENDLOOP }

/* xassert support */
#define USE_XASSERT static char SRCFILE[]=__FILE__;  \
  static bool _do_xassert(int line) {                \
    report_xassert(SRCFILE, line);                   \
    xassert(line) {}                                 \
    return(false);                                   \
    }
#define asserterror() _do_xassert(__LINE__)
#define xassert(exp) if (!(exp)) {asserterror();} else

/* What is a class descriptor */
typedef struct classdesc_tag {
	char *name;
} classdesc;

/**
 * @brief declare a new handle
 *
 * @desc NEWHANDLE() declarations are almost always placed in an
 * include file that gets included into all source files. NEWHANDLE() is
 * usually not used in source files.
 *
 * @param handle_t  new handle to declare
 *
 * @eg NEWHANDLE(list_t);
 */
#define NEWHANDLE(handle) typedef struct tag_##handle *handle

/* Class descriptor name from object name */
#define _CD(obj) obj##_classdesc

/**
 * @brief the class macro
 *
 * @desc The CLASS() macro is used only by source files that implement an
 * object.  The CLASS() macro is never used in include files.
 *
 * @param handle  the object handle, to be used in the VERIFY* type
 * macros
 * @param handle_t  the object handle type, used to declare an object
 *
 * @eg  CLASS(list, list_t)
 */
#define CLASS(obj,handle) \
    static classdesc _CD(obj)={#obj}; \
    struct tag_##handle

/* Object verification macros */
/**
 * @brief verify an object
 *
 * @desc verify that the object (variable name) matches the object type, as
 * declared to the heap manager
 *
 * @eg VERIFY(obj);
 * @eg VERIFY(obj) {
 *     // only executed if verfication holds
 * }
 */
#define VERIFY(obj) xassert(_VERIFY(obj))
/**
 * @brief verify an object, allows NULL too
 *
 * @desc verify that the object (variable name) matches the object type, as
 * declared to the heap manager, or that the object is NULL
 *
 * @eg VERIFY(obj);
 * @eg VERIFY(obj) {
 *     // only executed if verfication holds
 * }
 */
#define VERIFYZ(obj) if (!(obj)) {} else VERIFY(obj)

/* WARNING: _VERIFY needs be tailored to your environment */
#define _S4 (sizeof(classdesc*))
#define _S8 (sizeof(classdesc*)+sizeof(void *))
#define _VERIFY(obj) \
    ( xtestptr(obj) && \
      (((void *)obj) == *(void **)((char *)obj-_S8)) \
      && ((&_CD(obj)) == *(classdesc **)((char *)obj-_S4)) )

/* NEWOBJ() and FREEOBJ() interface macros */
/**
 * @brief allocate memory for an object
 *
 * @param obj object to allocate
 *
 * @eg obj_t obj; NEWOBJ(obj);
 */
#define NEWOBJ(obj) \
  (obj = xnew(sizeof(*obj),&_CD(obj),SRCFILE,__LINE__))
/**
 * @brief free memory allocated memory for an object
 *
 * @param obj object to free
 *
 * @eg FREEOBJ(obj);
 */
#define FREEOBJ(obj) (obj = xfree(obj))

/* String interface macros */
/**
 * @brief allocates memory for a string of size - 1 bytes
 *
 * 
 *
 */
#define NEWSTRING(dst, size) \
  (dst = xnew((size_t)(size),NULL,SRCFILE,__LINE__))
/*
 * duplicate a string
 */
#define STRDUP(dst, src) \
  (dst = xstrdup(src,SRCFILE,__LINE__))

/* Array interface macros */
/*
 * allocate memory to contain N (size) array elements
 */
#define NEWARRAY(array, size) \
  (array = xnew((size_t)(sizeof(*(array))*(size)), \
  NULL,SRCFILE,__LINE__))
/*
 * resize an array so contain N (size) array elements
 */
#define RESIZEARRAY(array, size) \
  (array = xrealloc((array), \
  (size_t)(sizeof(*(array))*(size)),SRCFILE,__LINE__))

__END_DECLS

#endif /* ndef DSM_CLASSDEF_H */
