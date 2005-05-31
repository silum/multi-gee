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
#include <multi-gee.h>
#include <stdio.h>
#include <unistd.h>

#include "process_util.h"

/* user defined callback function */
void process_images(multi_gee_t multi_gee,
		    sllist_t frame_list)
{
	for (sllist_t f = frame_list; f; f = sll_next(f))
	{
		mg_frame_t frame = sll_data(f);

		/* process image here: user defined stuff goes here */
		process(multi_gee, frame);

	}
	printf(".");
}

int
main()
{
	/* nuke old file */
	unlink(PGM_FILE);

	for(int i = 0; i < 5; i++)
	{
		/* create capture object handle */
		multi_gee_t mg = mg_create("stderr");

		/* register callback function */
		mg_register_callback(mg, process_images);

		/* register a device, or two */
		printf("dev id = %d\n", mg_register_device(mg, "/dev/video0"));
		printf("dev id = %d\n", mg_register_device(mg, "/dev/video1"));
		printf("dev id = %d\n", mg_register_device(mg, "/dev/video2"));
		printf("dev id = %d\n", mg_register_device(mg, "/dev/video3"));

		/* capture 10 synced sets of images */
		int ret = mg_capture(mg, 5);
		printf("\n");

		/* handle return value */
		switch (ret) {
			case RET_UNDEF    : printf("should not happen\n"); break;
			case RET_CALLBACK : printf("no callback registered\n"); break;
			case RET_SYNC     : printf("sync lost\n"); break;
			case RET_BUSY     : printf("multiple call to capture\n"); break;
			case RET_DEVICE   : printf("no devices registered\n"); break;
			case RET_HALT     : printf("capture_halt called\n"); break;
			default           : printf("captured %d frames\n", ret); break;
		}

		/* we're done, destroy handle */
		mg_destroy(mg);

		/* give time for hardware to recover -- might not be necessary */
		usleep(100000);
	}
}
