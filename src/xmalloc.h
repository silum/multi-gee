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

#include <sys/cdefs.h>
#include "classdef.h"

__BEGIN_DECLS

#define xmalloc(size) \
     xnew(size, 0, __FILE__, __LINE__)

void *xnew(size_t, classdesc *, char *, int);
void *xfree(void *);
void *xrealloc(void *, size_t, char *, int);
void *xstrdup(char *, char *, int);
int  xwalkheap(void);
bool xtestptr(void *);

void report_xassert(char *, int);

__END_DECLS

#endif /* ndef DSM_XMALLOC_H */
