#include <stdio.h>

#include "process_util.h"
#include "pgm_util.h"

void
process(multi_gee_t multi_gee, mg_frame_t frame)
{
	// silence unused parameter compiler warning
	(void) multi_gee;

	mg_device_t device = mg_frame_device(frame);
	const char *name = mg_device_name(device);
	struct timeval tv = mg_frame_timestamp(frame);

	char comment[255];
	snprintf(comment,
		 sizeof(comment),
		 "device: %s, timestamp: %ld,%06ld",
		 name,
		 tv.tv_sec,
		 tv.tv_usec);

	pgm_append("file.pgm",
		   768, 576, 255,
		   comment,
		   mg_frame_image(frame));
}

