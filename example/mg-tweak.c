/* $Id: multi-gee.c 57 2005-03-14 12:56:20Z dsm $
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
#include <libgen.h>
#include <multi-gee.h>
#include <multi-gee/tv_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <xassert.h>
#include <xmalloc.h>

void
print_tv(char *str, struct timeval tv)
{
	printf("%s %10ld.%06ld", str, tv.tv_sec, tv.tv_usec);
}

static void
process_images(multi_gee_t mg, sllist_t frame_list)
{
	static struct timeval then = {0, 0};
	static int count = 0;

	struct timeval now;
	gettimeofday(&now, 0);
	print_tv("now: ", now); printf("\n");

	struct timeval diff;
	timersub(&now, &then, &diff);
	print_tv("  then now diff: ", diff); printf("\n");
	then = now;

	printf("  count   : %d\n", count++);

	for (sllist_t f = frame_list; f; f = sll_next(f)) {
		mg_frame_t frame = sll_data(f);
		printf("dev: %s\n", mg_device_name(mg_frame_device(frame)));
		struct timeval tv = mg_frame_timestamp(frame);

		print_tv(" tv: ", tv); printf("\n");

		timersub(&now, &tv, &diff);
		print_tv("  tv   now diff: ", diff); printf("\n");
		printf("  sequence: %d\n", mg_frame_sequence(frame));

	}
	printf("\n");

	return;

	static int dev_id = -1;
	if (count % 1 == 0) {
		static bool flag = true;
		if (flag) {
			dev_id = mg_register_device(mg, "/dev/video1");
		} else {
			dev_id = mg_deregister_device(mg, dev_id);
		}
		flag = !flag;
	}
}

void
multi_gee(struct timeval tv_no_sync,
	  struct timeval tv_in_sync,
	  struct timeval tv_sub,
	  int num_bufs)
{
	multi_gee_t mg = mg_create_special("stderr",
					   tv_no_sync,
					   tv_in_sync,
					   tv_sub,
					   num_bufs);

	mg_register_callback(mg, process_images);

	printf("dev id = %d\n", mg_register_device(mg, "/dev/video0"));
	printf("dev id = %d\n", mg_register_device(mg, "/dev/video1"));
	// printf("dev id = %d\n", mg_register_device(mg, "/dev/video2"));
	// printf("dev id = %d\n", mg_register_device(mg, "/dev/video3"));

	for (int i = 0; i < 5; i++) {

		printf("sleep a while\n");
		sleep(1);

		struct timeval tv_start;
		gettimeofday(&tv_start, 0);
		int ret = mg_capture(mg, 5);
		struct timeval tv_end;
		gettimeofday(&tv_end, 0);
		struct timeval tv_diff;
		timersub(&tv_end, &tv_start, &tv_diff);
		printf("capture ret = %d\n", ret);

		if (ret == 5) {
			print_tv(" **    start: ", tv_start); printf("\n");
			print_tv(" **      end: ", tv_end  ); printf("\n");
			print_tv(" **     diff: ", tv_diff ); printf("\n");

			struct timeval tv_sub = {0, 200000};
			timersub(&tv_diff, &tv_sub, &tv_diff);

			print_tv(" ** overhead: ", tv_diff ); printf("\n");
		}


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

	}

	mg_destroy(mg);
}

void
usage(char *progname)
{
	printf("Usage : %s <in_sync> <no_sync> <sub> <bufs>\n", basename(progname));
	printf("\n");
	printf(" where\n");
	printf("  in_sync: max timestamp difference and still be in sync -- number of frames\n");
	printf("  no_sync: min timestamp difference for fatal sync -- number of frames\n");
	printf("      sub: start capture frame offset -- number of frames\n");
	printf("     bufs: number of capture buffers (>1)\n");
	exit(1);
}

void
timernorm(struct timeval *tv)
{
	while (tv->tv_usec < 0) {
		tv->tv_sec--;
		tv->tv_usec += 1000000;
	}

	while (tv->tv_usec > 1000000) {
		tv->tv_sec++;
		tv->tv_usec -= 1000000;
	}
}

struct timeval
frame_time(float frames, float percent)
{
	struct timeval FRAME = {0, 40000};
	struct timeval tv;
	timerclear(&tv);

	int f = frames;
	// printf("frames = %f\n", frames);
	// printf("     f = %d\n", f);

	for (int i = 0; i < f; i++)
		timeradd(&tv, &FRAME, &tv);
	struct timeval frac;
	timerset(&frac, 0, FRAME.tv_usec * (frames - f));
	// print_tv("  frac = ", frac);
	// printf("\n");
	timeradd(&tv, &frac, &tv);

	tv.tv_sec *= 1.0 + percent;
	tv.tv_usec *= 1.0 + percent;

	timernorm(&tv);
	return tv;
}

double
arg_to_f(char *argv[], int i)
{
	char *end;
	float f;
	f = strtof(argv[i], &end);

	char *ep = argv[i];
	while (*ep++) { /* empty */ }
	if (end != --ep) {
		printf("%s: could not convert %s to float\n",
		       basename(argv[0]), argv[i]);
		usage(argv[0]);
	}

	return f;
}

double
arg_to_l(char *argv[], int i)
{
	char *end;
	long l;
	l = strtol(argv[i], &end, 0);

	char *ep = argv[i];
	while (*ep++) { /* empty */ }
	if (end != --ep) {
		printf("%s: could not convert %s to int\n",
		       basename(argv[0]), argv[i]);
		usage(argv[0]);
	}

	return l;
}

int
main(int argc, char *argv[])
{
	if (argc != 5) {
		usage(argv[0]);
	}

	struct timeval tv_in_sync = frame_time(arg_to_f(argv, 1), .05);
	struct timeval tv_no_sync = frame_time(arg_to_f(argv, 2), .05);
	struct timeval tv_sub = frame_time(arg_to_f(argv, 3), .05);
	unsigned int num_bufs = arg_to_l(argv, 4);
	if (num_bufs <= 1) usage(argv[0]);

	for (int i = 0; i < 5; i++) {
		print_tv("in_sync: ", tv_in_sync);
		printf("\n");
		print_tv("no_sync: ", tv_no_sync);
		printf("\n");
		print_tv("    sub: ", tv_sub);
		printf("\n");
		printf("number of buffers %d", num_bufs);
		printf("\n");
		multi_gee(tv_in_sync,
			  tv_no_sync,
			  tv_sub,
			  num_bufs);
	}
	return EXIT_SUCCESS;
}
