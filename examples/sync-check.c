/* $Id: main.c,v 1.4 2004/05/11 12:19:53 dsm Exp $
 * Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
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
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

/* predefine yyparse() */
int yyparse(void);

/* required argument positions */
enum arguments {
	PROGNAME = 0,
	INFILE,
	NO_ARGS
};

/**
 * @brief print programme usage and exit
 *
 * @param progname  programme name
 */
void
usage(char *progname)
{
	fprintf(stderr, "Usage: %s inputfile\n\n", basename(progname));
	fprintf(stderr, "Parses inputfile and prints out configuration parameters.\n");
	fprintf(stderr, "Use an input file of `-' to read input from stdin.\n");
	exit(EXIT_FAILURE);
}

/**
 * @brief reopens filename on stdin
 *
 * A filename of '-' leaves stdin unchanged
 *
 * @param filename  file to open
 *
 * @return a file pointer, or stdin
 */
FILE*
open_stdin(const char* filename)
{
	FILE* file = 0;

	if (strcmp(filename, "-") == 0) {
		file = stdin;
	} else {
		file = freopen(filename, "r", stdin);
	}

	if (!file) {
		err(EXIT_FAILURE, "%s: %s\n", filename, strerror(errno));
	} else {
		setbuf(file, 0);
	}

	return file;
}

/**
 * @brief parse file
 */
int
main(int argc, char *argv[])
{
	int ret;
	if (argc < NO_ARGS)
		usage(argv[PROGNAME]);

	open_stdin(argv[INFILE]);

	while (!(ret = yyparse())) {
		/* empty */
	}

	return !ret;
}

/**
 * @brief render device time value
 *
 * @param device  object handle
 */
void
render_device(device_t device)
{
	int offset = rint(dev_diff(device).tv_usec / 4000.);
	printf("[");
	for (int i = 0; i < offset; i++) {
		printf(" ");
	}
	const char * id = dev_id(device);
	if (id) {
		size_t len = strlen(id);
		if (len > 0) {
			printf("%c", *(id + len - 1));
		}
	} else {
		printf("o");
	}
	for (int i = offset; i < 8; i++) {
		printf(" ");
	}
	printf("]");
	// printf("%s ", dev_id(device));
//	printf("%6d ", (int) );

}

/**
 * @brief render device time value
 *
 * @param device  object handle
 */
void
render_endl()
{
	static int i = 0;
	printf(" - %d\n", i++);
	fflush(0);
}

