#ifndef DSM_PGM_UTIL_H
#define DSM_PGM_UTIL_H 1

#include <stdbool.h>

__BEGIN_DECLS

bool
pgm_append(char *file,
	   int width,
	   int height,
	   int colors,
	   char *comment,
	   void *data);

__END_DECLS

#endif /* DSM_PGM_UTIL_H */
