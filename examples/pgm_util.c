/* $Id$
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
/**
 * @file
 */
#include <stdio.h>
#include "pgm_util.h"

bool
pgm_append(char *file,
	   int width,
	   int height,
	   int colors,
	   char *comment,
	   void *image)
{
	const char *type = "P5";
	FILE *f = fopen(file, "a");
	size_t written;

	if (!f)
		return false;

	fprintf(f, "%s\n%d %d", type, width, height);
	if (comment)
		fprintf(f, " # %s", comment);

	fprintf(f, "\n%d\n", colors);
	written = fwrite(image, width, height, f);
	fclose(f);

	if ((int)written < width)
		return false;

	return true;
}
