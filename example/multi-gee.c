#include <multi-gee.h>
#include <stdio.h>

#include "process_util.h"


// user defined callback function
void process_images(multi_gee_t multi_gee,
		    sllist_t frame_list)
{
	for (sllist_t f = frame_list; f; f = sll_next(f))
	{
		mg_frame_t frame = sll_data(f);

		// process image here
		process(multi_gee, frame);

	}
	printf(".");
}

int
main()
{
	// create capture object handle
	multi_gee_t mg = mg_create("stderr");

	// register callback function
	mg_register_callback(mg, process_images);

	// register a device, or two
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video0"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video1"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video2"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video3"));

	// capture 10 synced sets of images
	int ret = mg_capture(mg, 10);
	printf("\n");

	// handle return value
	switch (ret) {
		case RET_UNDEF    : printf("should not happen\n"); break;
		case RET_CALLBACK : printf("no callback registered\n"); break;
		case RET_SYNC     : printf("sync lost\n"); break;
		case RET_BUSY     : printf("multiple call to capture\n"); break;
		case RET_DEVICE   : printf("no devices registered\n"); break;
		case RET_HALT     : printf("capture_halt called\n"); break;
		default           : printf("captured %d frames\n", ret); break;
	}

	// we're done, destroy handle
	mg_destroy(mg);
}
