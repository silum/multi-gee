/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>		/* getopt_long() */

#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>		/* for videodev2.h */
#include "linux/videodev2.h"

#include <stdint.h>
#include <stdbool.h>

#define DEVICE "/dev/video0"


#define KERNEL_VERSION_A(v) (((v) >> 16) & 0xff)
#define KERNEL_VERSION_B(v) (((v) >>  8) & 0xff)
#define KERNEL_VERSION_C(v) (((v)      ) & 0xff)


#define TW 20
#define PRINTF(fmt,name,...) printf("%*s " fmt "\n", TW, name, ##__VA_ARGS__)

static void
errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	exit(EXIT_FAILURE);
}


void
print_capability(uint32_t capability)
{
	switch (capability) {
		case 0:
			/* no-op */
			break;
		case V4L2_CAP_VIDEO_CAPTURE:
			PRINTF("0x%08x: %s", "", capability, "is a video capture device");
			break;
		case V4L2_CAP_VIDEO_OUTPUT:
			PRINTF("0x%08x: %s", "", capability, "is a video output device");
			break;
		case V4L2_CAP_VIDEO_OVERLAY:
			PRINTF("0x%08x: %s", "", capability, "can do video overlay");
			break;
		case V4L2_CAP_VBI_CAPTURE:
			PRINTF("0x%08x: %s", "", capability, "is a VBI capture device");
			break;
		case V4L2_CAP_VBI_OUTPUT:
			PRINTF("0x%08x: %s", "", capability, "is a VBI output device");
			break;
		case V4L2_CAP_RDS_CAPTURE:
			PRINTF("0x%08x: %s", "", capability, "RDS data capture");
			break;
		case V4L2_CAP_TUNER:
			PRINTF("0x%08x: %s", "", capability, "has a tuner");
			break;
		case V4L2_CAP_AUDIO:
			PRINTF("0x%08x: %s", "", capability, "has audio support");
			break;
//		case V4L2_CAP_RADIO:
//			PRINTF("0x%08x: %s", "", capability, "is a radio device");
//			break;
		case V4L2_CAP_READWRITE:
			PRINTF("0x%08x: %s", "", capability, "read/write systemcalls");
			break;
		case V4L2_CAP_ASYNCIO:
			PRINTF("0x%08x: %s", "", capability, "async I/O");
			break;
		case V4L2_CAP_STREAMING:
			PRINTF("0x%08x: %s", "", capability, "streaming I/O ioctls");
			break;
		default:
			PRINTF("0x%08x: %s", "", capability, "<undefined>");
	}
}


void
print_input_type(uint32_t type)
{
	switch (type) {
		case V4L2_INPUT_TYPE_TUNER:
			PRINTF("%d: %s", "", type, "this input uses a tuner");
			break;
		case V4L2_INPUT_TYPE_CAMERA:
			PRINTF("%d: %s", "", type, "analog baseband input");
			break;
		default:
			PRINTF("%d: %s", "", type, "<undefined>");
	}
}


void
print_v4l2_std_id(v4l2_std_id std_id)
{
	switch (std_id) {
		case 0:
			/* no-op */
			break;
		case V4L2_STD_PAL_B:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_B");
			break;
		case V4L2_STD_PAL_B1:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_B1");
			break;
		case V4L2_STD_PAL_G:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_G");
			break;
		case V4L2_STD_PAL_H:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_H");
			break;
		case V4L2_STD_PAL_I:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_I");
			break;
		case V4L2_STD_PAL_D:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_D");
			break;
		case V4L2_STD_PAL_D1:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_D1");
			break;
		case V4L2_STD_PAL_K:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_K");
			break;
		case V4L2_STD_PAL_M:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_M");
			break;
		case V4L2_STD_PAL_N:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_N");
			break;
		case V4L2_STD_PAL_Nc:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_Nc");
			break;
		case V4L2_STD_PAL_60:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_PAL_60");
			break;
		case V4L2_STD_NTSC_M:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_NTSC_M");
			break;
		case V4L2_STD_NTSC_M_JP:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_NTSC_M_JP");
			break;
		case V4L2_STD_SECAM_B:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_B");
			break;
		case V4L2_STD_SECAM_D:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_D");
			break;
		case V4L2_STD_SECAM_G:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_G");
			break;
		case V4L2_STD_SECAM_H:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_H");
			break;
		case V4L2_STD_SECAM_K:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_K");
			break;
		case V4L2_STD_SECAM_K1:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_K1");
			break;
		case V4L2_STD_SECAM_L:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_SECAM_L");
			break;
		case V4L2_STD_ATSC_8_VSB:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_ATSC_8_VSB");
			break;
		case V4L2_STD_ATSC_16_VSB:
			PRINTF("0x%016llx: %s", "", std_id, "V4L2_STD_ATSC_16_VSB");
			break;
		default:
			PRINTF("0x%016llx: %s", "", std_id, "<undefined>");
	}
}


void
print_v4l2_input_status(uint32_t status)
{
	switch (status) {
		case 0:
			/* no-op */
			break;
		case V4L2_IN_ST_NO_POWER:  /* fall */
		case V4L2_IN_ST_NO_SIGNAL: /* fall */
		case V4L2_IN_ST_NO_COLOR:
			PRINTF("0x%08x: %s", "", status, "Attached device is off.");
			break;

		case V4L2_IN_ST_NO_H_LOCK:
			PRINTF("0x%08x: %s", "", status, "No horizontal sync lock.");
			break;
		case V4L2_IN_ST_COLOR_KILL:
			PRINTF("0x%08x: %s", "", status, "The color killer is a circuit that shuts off the color decoding when it cannot find the color burst. This flag indicates if the color killer is enabled, while V4L2_IN_ST_NO_COLOR is set when no color is detected.");
			break;
		case V4L2_IN_ST_NO_SYNC:
			PRINTF("0x%08x: %s", "", status, "No synchronization lock.");
			break;
		case V4L2_IN_ST_NO_EQU:
			PRINTF("0x%08x: %s", "", status, "No equalizer lock.");
			break;
		case V4L2_IN_ST_NO_CARRIER:
			PRINTF("0x%08x: %s", "", status, "Carrier recovery failed.");
			break;
		case V4L2_IN_ST_MACROVISION:
			PRINTF("0x%08x: %s", "", status, "Macrovision is an analog copy protection system mangling the video signal to confuse video recorders. When this flag is set Macrovision protection has been detected.");
			break;
		case V4L2_IN_ST_NO_ACCESS:
			PRINTF("0x%08x: %s", "", status, "Conditional access denied.");
			break;
		case V4L2_IN_ST_VTR:
			PRINTF("0x%08x: %s", "", status, "VTR time constant. [?]");
			break;

		default:
			PRINTF("0x%08x: %s", "", status, "<undefined>");
	}
}


void
print_v4l2_std_id_class(v4l2_std_id std_id)
{
	if (std_id & V4L2_STD_PAL_BG)
		PRINTF("0x%016llx: %s", "", V4L2_STD_PAL_BG, "V4L2_STD_PAL_BG");
	if (std_id & V4L2_STD_PAL_DK)
		PRINTF("0x%016llx: %s", "", V4L2_STD_PAL_DK, "V4L2_STD_PAL_DK");
	if (std_id & V4L2_STD_PAL)
		PRINTF("0x%016llx: %s", "", V4L2_STD_PAL,    "V4L2_STD_PAL");
	if (std_id & V4L2_STD_NTSC)
		PRINTF("0x%016llx: %s", "", V4L2_STD_NTSC,   "V4L2_STD_NTSC");
	if (std_id & V4L2_STD_SECAM)
		PRINTF("0x%016llx: %s", "", V4L2_STD_SECAM,  "V4L2_STD_SECAM");
	if (std_id & V4L2_STD_525_60)
		PRINTF("0x%016llx: %s", "", V4L2_STD_525_60, "V4L2_STD_525_60");
	if (std_id & V4L2_STD_625_50)
		PRINTF("0x%016llx: %s", "", V4L2_STD_625_50, "V4L2_STD_625_50");
	if (std_id & V4L2_STD_ALL)
		PRINTF("0x%016llx: %s", "", V4L2_STD_ALL,    "V4L2_STD_ALL");
}


void
print_v4l2_capability(struct v4l2_capability *capability)
{
	if (capability) {
		PRINTF("%s", "v4l2_capability: ", "");
		PRINTF("%s", "driver",   capability->driver);
		PRINTF("%s", "card",     capability->card);
		PRINTF("%s", "bus_info", capability->bus_info);
		PRINTF("%d.%d.%d", "version",
		       KERNEL_VERSION_A(capability->version),
		       KERNEL_VERSION_B(capability->version),
		       KERNEL_VERSION_C(capability->version));
		PRINTF("0x%08x", "capabilities", capability->capabilities);

		for (unsigned int i = 0; i < sizeof(capability->capabilities) * 8; ++i)
			print_capability(capability->capabilities & (1 << i));

		for (unsigned int i = 0; i < 4; ++i)
			PRINTF("0x%08x", "reserved[]", capability->reserved[i]);
	}
}


void
print_v4l2_input(struct v4l2_input *input)
{
	if (input) {
		PRINTF("%s", "v4l2_input: ", "");
		PRINTF("%d", "index", input->index);
		PRINTF("%s", "name", input->name);
		PRINTF("%d", "type", input->type);

		print_input_type(input->type);

		PRINTF("%d", "audioset", input->audioset);
		PRINTF("%d", "tuner", input->tuner);

		PRINTF("0x%016x", "std", (unsigned int) input->std);
		for (unsigned int i= 0; i < sizeof(input->std) * 8; ++i)
			print_v4l2_std_id(input->std & (1ull << i));
		PRINTF("%s", "", "class:");
		print_v4l2_std_id_class(input->std);


		PRINTF("0x%016x", "status", input->status);
		for (unsigned int i = 0; i < sizeof(input->status) * 8; ++i)
			print_v4l2_input_status(input->status & (1 << i));

		for (int i = 0; i < 4; ++i)
			PRINTF("0x%08x", "reserved[]", input->reserved[i]);
	}
}


bool
print_input(int fd, int i)
{
	struct v4l2_input input;
	memset(&input, 0, sizeof(input));
	input.index = i;

	/* query input */
	if (ioctl(fd, VIDIOC_ENUMINPUT, &input) < 0) {
		if (errno == EINVAL)
			return false;
		errno_exit("VIDIOC_ENUMINPUT");
	}

	print_v4l2_input(&input);
	printf("\n");

	return true;
}


int
main(void)
{
	/* open device */
	int fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
	if (fd < 0)
		errno_exit("open device");

	/* query capabilities */
	struct v4l2_capability capability;
	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0)
		errno_exit("VIDIOC_QUERYCAP");

	print_v4l2_capability(&capability);
	printf("\n");

	/* query input id */
	int index;
	if (ioctl(fd, VIDIOC_G_INPUT, &index) < 0)
		errno_exit("VIDIOC_G_INPUT");
	PRINTF("%d", "current input", index);
	printf("\n");

	for (int i = 0; ; ++i)
		if (!print_input(fd, i))
			break;

	v4l2_std_id std_id;
	if (ioctl(fd, VIDIOC_QUERYSTD, &std_id) < 0) {
		if (errno != EINVAL)
			errno_exit("VIDIOC_QUERYSTD");
		else
			printf("VIDIOC_QUERYSTD ioctl not supported\n");
	} else {
		PRINTF("%016llx", "detected input", std_id);
		for (unsigned int i= 0; i < sizeof(std_id) * 8; ++i)
			print_v4l2_std_id(std_id & (1ull << i));
	}

	if (ioctl(fd, VIDIOC_G_STD, &std_id) < 0) {
		if (errno != EINVAL)
			errno_exit("VIDIOC_G_STD");
		else
			printf("VIDIOC_G_STD ioctl not supported\n");
	} else {
		PRINTF("%016llx", "current input std", std_id);
		for (unsigned int i= 0; i < sizeof(std_id) * 8; ++i)
			print_v4l2_std_id(std_id & (1ull << i));
		printf("\n");
		print_v4l2_std_id_class(std_id);
	}

	/* close device */
	if (close(fd) < 0)
		errno_exit("close device");

	return EXIT_SUCCESS;
}

