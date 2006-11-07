/* $Id$
 * Copyright (C) 2006 Deneys S. Maartens <dsm@tlabs.ac.za>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file
 * @brief sync check visualiser
 */

%{
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "examples/device.h"

/* function prototypes */

/**
 * @brief lexical token scanner
 */
int yylex(void);

/**
 * @brief print error message and exit
 *
 * @param s  error message string
 */
void yyerror(const char *s);

/**
 * @brief compare two device structures
 *
 * @param a  first pointer to device
 * @param b  second pointer to device
 *
 * @return a negative integer if the first value comes before the
 * second, 0 if they are equal, or a positive integer if the first value
 * comes after the second
 */
gint
dev_compare(gconstpointer a,
	    gconstpointer b);

/**
 * @brief prototype of render_device
 *
 * @param device  object handle
 */
void
render_device(device_t device);

/**
 * @brief prototype of render_endl
 */
void
render_endl(void);

%}

%union {
	int n;
	char *s;
	struct timeval tv;
	device_t dev;
	struct GSList *gsl_t;
}

/* identifiers */
%token CAPTURED COUNT DEV DIFF END END_OF_FILE FRAMES ID MULTI_GEE NOW
%token OVERHEAD SEQUENCE STARS START STARTUP THEN TV
/* integer */
%token INTEGER
/* strings */
%token IDENTIFIER DATE_STR TIME_STR

/* start symbol */
%start run

/* types */
%type <n> INTEGER
%type <s> IDENTIFIER DATE_STR TIME_STR
%type <tv> time_spec
%type <dev> dev_block
%type <gsl_t> dev_block_list

%%

run :
          sync_block_list
          end_block
{
	YYACCEPT;
}
        | start_block
          sync_block_list
          end_block
{
	YYACCEPT;
}
        | END_OF_FILE
{
	YYABORT;
}
        ;

dev_block :
          DEV ':' IDENTIFIER
          TV ':' time_spec
          TV NOW DIFF ':' time_spec
          SEQUENCE ':' INTEGER
{
	device_t dev = dev_create($3, $6, $11, $14);
	$$ = dev;
}
        ;

dev_block_list :
          dev_block
{
	$$ = (void *) g_slist_append(0, $1);
}
        | dev_block_list dev_block
{
	$$ = (void *) g_slist_insert_sorted((void *) $1,
					    $2,
					    dev_compare);
}
        ;

dev_id :
          DEV ID '=' INTEGER
        ;

dev_id_list :
          dev_id
        | dev_id_list dev_id
        ;

end_block :
          STARS START ':' time_spec
          STARS END ':' time_spec
          STARS DIFF ':' time_spec
          STARS OVERHEAD ':' time_spec
          CAPTURED INTEGER FRAMES
        ;

now_block :
          NOW ':' time_spec
          THEN NOW DIFF ':' time_spec
          COUNT ':' INTEGER
        ;

start_block :
          MULTI_GEE ':' DATE_STR TIME_STR ':' STARTUP
          dev_id_list
        ;

sync_block :
          now_block
          dev_block_list
{
	GSList *list = (void *) $2;
	for (;
	     list;
	     list = list->next) {
		device_t dev = list->data;
		render_device(dev);
		dev = dev_destroy(dev);
	}
	render_endl();
	g_slist_free((void *) $2);
}
        ;

sync_block_list :
          sync_block
        | sync_block_list sync_block
        ;

time_spec :
          INTEGER '.' INTEGER
{
	struct timeval tv;
	tv.tv_sec = $1;
	tv.tv_usec = $3;
	$$ = tv;
}
        ;

%%

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/* used in error reporting */
/*  declared in scanner.l */
extern int column;
extern int line;

/*
 * prints error message
 */
void
yyerror(char const *s)
{
	fflush(stdout);
#if 0
	/*
	 * Alternate error reporting printf.  This relies on the input
	 * to the scanner to be echoed by the scanner.  A caret is
	 * printed which points to the specific column on the line which
	 * generated an error.
	 */
	printf("\n%*s\n%*s\n", column, "^", column, s);

	printf("line %d column %d : %s\n", line, column, s);
#endif /* 0 */
	errx(EXIT_FAILURE, "line %d column %d: %s\n", line, column, s);
}

gint
dev_compare(gconstpointer a,
	    gconstpointer b)
{
	device_t first = (void *) a;
	device_t second = (void *) b;

	return strcmp(dev_id(first), dev_id(second));
}

// vim:set noet:
