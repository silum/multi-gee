/* $Id$ */
/*
 * memory allocation functions
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
#ifndef DSM_XMALLOC_H
#define DSM_XMALLOC_H 1

#include <stdio.h>
#include <stdbool.h> /* bool */
#include <multi-gee/classdef.h>

__BEGIN_DECLS

#define xmalloc(size) \
     xnew(size, 0, __FILE__, __LINE__)

/**
 *  @brief memory new
 *
 *  allocate a new block of memory from the heap
 *
 *  @param size  size of object to allocate
 *  @param cd  class descriptor for object (or 0)
 *  @param file  filename where object was allocated
 *  @param line  line number where object was allocated
 *
 *  @return a pointer to the memory object or 0
 */
void *
xnew(size_t,
     classdesc *,
     const char *,
     int);

/**
 * @brief memory Free
 *
 * free a block of memory that was previously allocated through xnew()
 *
 * @param mem  heap pointer to free or 0
 *
 * @return 0
 */
void *xfree(void *);

/**
 * @brief memory realloc
 *
 * reallocate a block of memory
 *
 * @param old  heap object to reallocate or 0
 * @param size  new size of the object
 * @param file  filename where realloc is taking place
 * @param line  line number where realloc is taking place
 *
 * @return a pointer to the reallocated memory or 0
 */
void *xrealloc(void *, size_t, const char *, int);

/**
 * @brief memory string dup
 *
 * helper function for the xstrdup() macro
 *
 * @param s  string to duplicate (or 0)
 * @param file  filename where string is being duplicated
 * @param line  line number where string is being duplicated
 *
 * @return a pointer to the duplicated string or 0
 */
void *xstrdup(const char *,
	      const char *,
	      int);

/**
 * @brief walk heap
 *
 * display a symbolic dump of the heap by walking the heap and
 * displaying all objects in the heap.
 */
int
xwalkheap(void);

/**
 * @brief does pointer point into the heap)
 *
 * does the given memory pointer point anywhere into the heap
 *
 * @param mem  heap pointer to check
 *
 * @return true if pointer points into the heap, or false if not
 */
bool
xtestptr(void *);

/**
 * @brief prototype for user defined xassert report function
 *
 * @param file  name of file where assertion failed
 * @param line  line number where assertion failed
 */
void
report_xassert(const char *,
	       int);

__END_DECLS

#endif /* ndef DSM_XMALLOC_H */
