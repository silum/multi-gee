/* $Id$ */
/*
 *  OUTLINE:
 *
 *    This module REPLACES the memory management routines of
 *    the C run-time library.  As such, this new interface
 *    should be used exclusively.
 *
 *  IMPLEMENTATION:
 *
 *    A wrapper is provided around all memory objects that
 *    allows for run-time type checking, symbolic dumps of
 *    the heap and validation of heap pointers.
 *
 *  NOTES:
 *
 *    - YOU must code an xtestptr() that works properly for your
 *    environment.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "classdef.h"
#include "xmalloc.h"

USE_XASSERT;

/* Heap objects are aligned on sizeof(int) boundaries */
#define ALIGNMENT (sizeof(int))
#define DOALIGN(num) (((num)+ALIGNMENT-1)&~(ALIGNMENT-1))
compiler_assert(ISPOWER2(ALIGNMENT));

/* Prefix structure before every heap object */
typedef struct prefix_tag {
	struct prefix_tag *prev;	/* previous object in heap   */
	struct prefix_tag *next;	/* next object in heap       */
	struct postfix_tag *postfix;	/* ptr to postfix object     */
	const char *file;		/* file name ptr or 0        */
	long line;			/* line number or 0          */
	void *mem;			/* xnew() ptr of object      */
	classdesc *class;		/* class descriptor ptr or 0 */
} prefix;

/* Postfix structure after every heap object */
typedef struct postfix_tag {
	struct prefix_tag *prefix;
} postfix;

/* Verify alignment of prefix structure */
compiler_assert(!(sizeof(prefix) % ALIGNMENT));

/* Points to first object in linked list of heap objects */
static prefix *heap = 0;

/* Local prototypes */
/**
 * @brief add heap object to linked list)
 *
 * add the given heap object into the doubly linked list of heap
 * objects
 *
 * @param add  prefix pointer to heap object
 *
 * @return
 */
static void list_insert(prefix *);

/**
 *  @brief remove heap object from linked list)
 *
 *  remove the given heap object from the doubly linked list of heap
 *  objects
 *
 *  @param p  prefix pointer to heap object
 */
static void list_remove(prefix *);

/**
 * @brief verify heap pointer)
 *
 * verify that a pointer points into that heap to a valid object in the
 * heap
 *
 * @param mem  heap pointer to validate
 *
 * @return heap pointer is valid (true) or not (false)
 */
static bool list_verify(void *);

/**
 * @brief render description of heap object)
 *
 * render a text description for the given heap object
 *
 * @param p  prefix pointer to heap object
 * @param buffer  where to place text description
 */
static void
render(prefix *, char *);

void *
xnew(size_t size,
     classdesc *class,
     const char *file,
     int line)
{
	prefix *p;
	size = DOALIGN(size);
	p = (prefix *) malloc(sizeof(prefix) + size + sizeof(postfix));
	if (p) {
		list_insert(p);
		p->postfix = (postfix *) ((char *) (p + 1) + size);
		p->postfix->prefix = p;
		p->file = file;
		p->line = line;
		p->mem = p + 1;
		p->class = class;
		memset(p->mem, 0, size);
	} else {
		asserterror();	/* Report out of memory error */
	}

	return (p ? p + 1 : 0);
}

void *
xfree(void *mem)
{
	if (list_verify(mem)) {
		prefix *p = (prefix *) mem - 1;
		size_t size = (char *) (p->postfix + 1) - (char *) p;
		list_remove(p);
		memset(p, 0, size);
		free(p);
	}

	return 0;
}

void *
xrealloc(void *old,
	 size_t size,
	 const char *file,
	 int line)
{
	void *new = 0;

	/* Try to realloc */
	if (old) {
		if (list_verify(old)) {
			prefix *p = (prefix *) old - 1;
			prefix *new_p;

			/* Try to reallocate block */
			list_remove(p);
			memset(p->postfix, 0, sizeof(postfix));
			size = DOALIGN(size);
			new_p = (prefix *) realloc(p, sizeof(prefix) +
						   size + sizeof(postfix));

			/* Add new (or failed old) back in */
			p = (new_p ? new_p : p);
			list_insert(p);
			p->postfix = (postfix *) ((char *) (p + 1) + size);
			p->postfix->prefix = p;
			p->mem = p + 1;

			/* Finish */
			new = (new_p ? &new_p[1] : 0);
			if (!new) {
				/* Report out of memory error */
				asserterror();
			}
		}
	}

	/* Else try new allocation */
	else {
		new = xnew(size, 0, file, line);
	}

	/* Return address to object */
	return new;
}

void *
xstrdup(const char *s,
	const char *file,
	int line)
{
	void *ret = 0;

	if (s) {
		size_t size = (size_t) (strlen(s) + 1);
		ret = xnew(size, 0, file, line);
		if (ret) {
			memcpy(ret, s, size);
		}
	}
	return ret;
}

int
xwalkheap(void)
{
	int alloced = 0;

	if (heap) {
		prefix *p = heap;
		while (list_verify(&p[1])) {
			char buffer[100];
			render(p, buffer);

			++alloced;

			/* print out buffer */
			printf("xwalkheap: %s\n", buffer);
			p = p->next;
			if (p == heap) {
				break;
			}
		}
	}

	return alloced;
}

bool
xtestptr(void *mem)
{
	return ((mem) && (!((long) mem & (ALIGNMENT - 1))));
}

static void
render(prefix *p, char *buffer)
{
	if (p->mem == &p[1]) {
		sprintf(buffer, "%8p ", p);
		if (p->file) {
			sprintf(buffer + strlen(buffer), "%12s %4ld ",
				p->file, p->line);
		}
		if (p->class) {
			sprintf(buffer + strlen(buffer), "%s",
				p->class->name);
		}
	} else {
		strcpy(buffer, "(bad)");
	}
}

static void
list_insert(prefix *p)
{
	/* add before current head of list */
	if (heap) {
		p->prev = heap->prev;
		(p->prev)->next = p;
		p->next = heap;
		(p->next)->prev = p;
	}

	/* else first node */
	else {
		p->prev = p;
		p->next = p;
	}

	/* make new item head of list */
	heap = p;
}

static void
list_remove(prefix *p)
{
	/* Remove from doubly linked list */
	(p->prev)->next = p->next;
	(p->next)->prev = p->prev;

	/* Possibly correct head pointer */
	if (p == heap) {
		heap = ((p->next == p) ? 0 : p->next);
	}
}

static bool
list_verify(void *mem)
{
	bool ok = false;

	if (mem) {
		xassert(xtestptr(mem)) {
			prefix *p = (prefix *) mem - 1;
			xassert(p->mem == mem) {
				xassert(p->postfix->prefix == p) {
					ok = true;
				}
			}
		}
	}

	return (ok);
}

