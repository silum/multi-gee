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
	if (!f)
		return false;

	fprintf(f, "%s\n%d %d", type, width, height);
	if (comment)
		fprintf(f, " # %s", comment);

	fprintf(f, "\n%d\n", colors);
	fwrite(image, width, height, f);
	fclose(f);

	return true;
}
