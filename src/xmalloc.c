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
	char *file;			/* file name ptr or 0        */
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
static void list_insert(prefix *);
static void list_remove(prefix *);
static bool list_verify(void *);
static void render(prefix *, char *);

/**
 *  @brief    Memory New
 *
 *  @desc     Allocate a new block of memory from the heap.
 *
 *  @param    size       Size of object to allocate
 *  @param    classdesc  Class descriptor for object (or 0)
 *  @param    file       Filename where object was allocated
 *  @param    line       Line number where object was allocated
 *
 *  @returns  A long pointer to the memory object or 0
 */
void *
xnew(size_t size, classdesc *class, char *file, int line)
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

/**
 * @brief    Memory Free
 *
 * @desc     Free a block of memory that was previously allocated
 *           through xnew().
 *
 * @param    mem  Heap pointer to free or 0
 */
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

/**
 * @brief    Memory String Dup
 *
 * @desc     Helper function for the XSTRDUP() macro
 *
 * @param    s     String to duplicate (or 0)
 * @param    file  Filename where string is being duplicated
 * @param    line  Line number where string is being duplicated
 *
 * @returns  A pointer to the duplicated string or 0
 */
void *
xstrdup(char *s, char *file, int line)
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

/**
 * @brief    Memory Realloc
 *
 * @desc     Reallocate a block of memory
 *
 * @param    old   Heap object to reallocate or 0
 * @param    size  New size of the object
 * @param    file  Filename where realloc is taking place
 * @param    lIne  Line number where realloc is taking place
 *
 * @returns  A pointer to the reallocated memory or 0
 */
void *
xrealloc(void *old, size_t size, char *file, int line)
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

/**
 * @brief    Walk Heap
 *
 * @desc     Display a symbolic dump of the heap by walking the heap and
 *           displaying all objects in the heap.
 */
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

/**
 * @brief    Add Heap Object to Linked List)
 *
 * @desc     Add the given heap object into the doubly linked list
 *           of heap objects.
 *
 * @param    add  Prefix pointer to heap object
 */
static
void
list_insert(prefix *p)
{
	/* Add before current head of list */
	if (heap) {
		p->prev = heap->prev;
		(p->prev)->next = p;
		p->next = heap;
		(p->next)->prev = p;
	}

	/* Else first node */
	else {
		p->prev = p;
		p->next = p;
	}

	/* Make new item head of list */
	heap = p;
}

/**
 *  @brief    Remove Heap Object from Linked List)
 *
 *  @desc     Remove the given heap object from the doubly linked list
 *            of heap objects.
 *
 *  @param    p  Prefix pointer to heap object
 */
static
void
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

/**
 * @brief    Verify Heap Pointer)
 *
 * @desc     Verify that a pointer points into that heap to a valid
 *           object in the heap.
 *
 * @param    mem  Heap pointer to validate
 *
 * @returns  Heap pointer is valid (true) or not (false)
 */
static
bool
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

/**
 * @brief    Does Pointer Point into the Heap)
 *
 * @desc     Does the given memory pointer point anywhere into the heap.
 *
 * @param    mem  Heap pointer to check
 *
 * @returns  Pointer points into the heap (TRUE) or not (FALSE)
 */
bool
xtestptr(void *mem)
{
	return ((mem) && (!((long) mem & (ALIGNMENT - 1))));
}

/**
 * @brief    Render Description of Heap Object)
 *
 * @desc     Render a text description for the given heap object.
 *
 * @param    p       Prefix pointer to heap object
 * @param    buffer  Where to place text description
 *
 */
static
void
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

/**
 * @brief print error message corresponding to errno
 */
void
ferrno(FILE* f, const char *s)
{
	fprintf(f, "%s error %d, %s\n", s, errno, strerror(errno));

	fflush(f);
}

