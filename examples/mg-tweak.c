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
/**
 * @file
 */
#include <cclass/xassert.h>
#include <cclass/xmalloc.h>
#include <libgen.h>
#include <math.h>
#include <multi-gee/multi-gee.h>
#include <multi-gee/tv_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/**
 * @brief Print struct timeval value
 *
 * struct timeval values always maintain the tv_usec member as positive.
 * The actual value is tv_sec + tv_usec / 1000000.  Thus -0.1, for
 * instance, is internally represented by {-1, 900000}.
 *
 * @param str  string to prefix to value -- may be ""
 * @param tv  timeval to print
 */
void
print_tv(char *str, struct timeval tv)
{
	if (tv.tv_sec == -1) {
		printf("%s%10s.%06ld", str, "-0", 1000000 - tv.tv_usec);
	} else if (tv.tv_sec < 0) {
		printf("%s%10ld.%06ld", str, tv.tv_sec - 1, 1000000 - tv.tv_usec);
	} else {
		printf("%s%10ld.%06ld", str, tv.tv_sec, tv.tv_usec);
	}
}

struct timeval
frame_time(float frames, float percent)
{
	struct timeval FRAME = {0, 40000};
	struct timeval tv;
	timerclear(&tv);

	int f = abs(frames);

	for (int i = 0; i < f; i++)
		timeradd(&tv, &FRAME, &tv);
	struct timeval frac;
	timerset(&frac, 0, FRAME.tv_usec * fabs(frames - truncf(frames)));
	timeradd(&tv, &frac, &tv);

	tv.tv_sec *= 1.0 + percent;
	tv.tv_usec *= 1.0 + percent;

	if (frames < 0)
		timerset(&tv, -tv.tv_sec, -tv.tv_usec);

	timernorm(&tv);
	return tv;
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
		printf("dev: %s\n", mg_device_get_name(mg_frame_get_device(frame)));
		struct timeval tv = mg_frame_get_timestamp(frame);

		print_tv(" tv: ", tv); printf("\n");

		timersub(&now, &tv, &diff);
		print_tv("  tv   now diff: ", diff); printf("\n");
		printf("  sequence: %d\n", mg_frame_get_sequence(frame));

	}
	printf("\n");

	return;

	static int dev_id = -1;
	if (count % 1 == 0) {
		static bool flag = true;
		if (flag) {
			dev_id = mg_register_device(mg, "/dev/video1", 0);
		} else {
			dev_id = mg_deregister_device(mg, dev_id);
		}
		flag = !flag;
	}
}

bool
register_device(multi_gee_t mg, unsigned int number)
{
	char dev[20];
	sprintf(dev, "/dev/video%d", number);

	int id = mg_register_device(mg, dev, 0);

	printf("dev id = %d\n", id);

	if (id < 0)
		return false;

	return true;
}

void
multi_gee(int buffers,
	  int count,
	  int devices,
	  int frames,
	  int sleeptime,
	  struct timeval in_sync,
	  struct timeval no_sync,
	  bool verbose,
	  int startdev)
{
	multi_gee_t mg = mg_create_special("stdout",
					   in_sync,
					   no_sync,
					   buffers);

	mg_register_callback(mg, process_images);

	for (int i = startdev; i < startdev + devices; i++)
		if (!register_device(mg, i))
			exit(EXIT_FAILURE);

	for (int i = 0; i < count; i++) {

		if (verbose)
			printf("sleep a while\n");
		usleep(sleeptime);

		struct timeval tv_start;
		gettimeofday(&tv_start, 0);

		int ret = mg_capture(mg, frames);

		struct timeval tv_end;
		gettimeofday(&tv_end, 0);
		struct timeval tv_diff;
		timersub(&tv_end, &tv_start, &tv_diff);

		if (verbose)
			printf("capture ret = %d\n", ret);

		print_tv(" **    start: ", tv_start); printf("\n");
		print_tv(" **      end: ", tv_end  ); printf("\n");
		print_tv(" **     diff: ", tv_diff ); printf("\n");

		struct timeval tv_sub = frame_time(frames, 0.0);
		timersub(&tv_diff, &tv_sub, &tv_diff);

		print_tv(" ** overhead: ", tv_diff ); printf("\n");

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
	printf("\nUsage : %s <-h> | <-?> | [options]\n", basename(progname));
	printf("\n"
	       " where:\n"
	       "   -h or -?       : print this message\n"
	       " options:\n"
	       "   -b <buffers>   : number of capture buffers (int >1)\n"
	       "   -c <count>     : number of capture repetitions (int)\n"
	       "   -d <devices>   : number of devices to use (int 1..6)\n"
	       "   -i <in_sync>   : max timestamp difference and still be in sync -- number of frames (float)\n"
	       "   -n <frames>    : number of frames to capture (int)\n"
	       "   -o <no_sync>   : min timestamp difference for fatal sync -- number of frames (float)\n"
	       "   -p <percent>   : percentage error to add to frame times (int)\n"
	       "   -s <sleeptime> : microseconds to sleep between captures (int)\n"
	       "   -v             : verbose output\n"
	       "   -x <sub>       : start capture frame offset -- number of frames (float)\n"
	      );
	exit(1);
}

double
arg_to_f(char *progname, char *arg)
{
	char *end;
	float f;
	f = strtof(arg, &end);

	char *ep = arg;
	while (*ep++) { /* empty */ }
	if (end != --ep) {
		printf("%s: could not convert `%s' to float\n",
		       basename(progname), arg);
		usage(arg);
	}

	return f;
}

long
arg_to_l(char *progname, char *arg)
{
	char *end;
	long l;
	l = strtol(arg, &end, 0);

	char *ep = arg;
	while (*ep++) { /* empty */ }
	if (end != --ep) {
		printf("%s: could not convert `%s' to int\n",
		       basename(progname), arg);
		usage(arg);
	}

	return l;
}

int
main(int argc, char *argv[])
{
	bool verbose = false;
	int buffers = 3;
	int count = 5;
	int frames = 5;
	int devices = 3;
	int sleeptime = 1000000;
	int percent = 5;
	int startdev = 0;
	struct timeval in_sync = frame_time(0.5, .05);
	struct timeval no_sync = frame_time(25, .05);
	struct timeval sub = frame_time(0, .05);

	while (true) {
		char c = getopt(argc, argv, "b:c:d:hi:n:o:p:s:S:vx:?");

		if (c == -1)
			break;

		switch (c) {
			case 'b':
				buffers = arg_to_l(argv[0], optarg);
				break;

			case 'c':
				count = arg_to_l(argv[0], optarg);
				break;

			case 'd':
				devices = arg_to_l(argv[0], optarg);
				break;

			case 'h':
				usage(argv[0]);
				break;

			case 'i':
				in_sync = frame_time(arg_to_f(argv[0], optarg), percent / 100.);
				break;

			case 'n':
				frames = arg_to_l(argv[0], optarg);
				break;

			case 'o':
				no_sync = frame_time(arg_to_f(argv[0], optarg), percent / 100.);
				break;

			case 'p':
				percent = arg_to_l(argv[0], optarg);
				break;

			case 's':
				sleeptime = arg_to_l(argv[0], optarg);
				break;

			case 'S':
				startdev = arg_to_l(argv[0], optarg);
				break;

			case 'v':
				verbose = !verbose;
				break;

			case 'x':
				sub = frame_time(arg_to_f(argv[0], optarg), percent / 100.);
				break;

			case '?':
				usage(argv[0]);
				break;

			default:
				printf ("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf ("\n");
	}

	if (buffers <= 1) {
		printf("%s: need at least 2 buffers\n", basename(argv[0]));
		usage(argv[0]);
	}
	if (devices < 1 || devices > 6) {
		printf("%s: only 1--6 devices supported\n", basename(argv[0]));
		usage(argv[0]);
	}

	if (verbose) {
		printf("  buffers: %d\n", buffers);
		printf("    count: %d\n", count);
		printf("   frames: %d\n", frames);
		printf("  devices: %d\n", devices);
		printf("sleeptime: %d\n", sleeptime);
		print_tv("  in_sync: ", in_sync); printf("\n");
		print_tv("  no_sync: ", no_sync); printf("\n");
		print_tv("      sub: ", sub); printf("\n");
		printf("\n");
	}

	multi_gee(buffers,
		  count,
		  devices,
		  frames,
		  sleeptime,
		  in_sync,
		  no_sync,
		  verbose,
		  startdev);
	return EXIT_SUCCESS;
}
