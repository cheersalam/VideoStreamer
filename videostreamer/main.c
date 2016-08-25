#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <getopt.h>
#include "config.h"
#include "pthread.h"
#include "socklibCommon.h"
#include "droneHandshake.h"
#include "droneCommandHandler.h"
#include "utilities/parrot.h"
#include "utilities/utilities.h"
#include "VideoContainerGenerator.h"
#include "ffmpegDecoder.h"

static void streamData(unsigned char *buffer, int32_t bufLen);
static void saveClip(unsigned char *buffer, int32_t bufLen);

//only globals
static volatile int32_t startExit = 0;
void *droneHandle = NULL;
void *vcg = NULL;
void *display = NULL;
struct sigaction sigact;


typedef struct GLOBALARGS_T{
	char *ipAddress; 		// -i ip address of drone
	int16_t port;  			// -p drone command listener port
	char *outputFileName; 	// -f output file name
	char *path;				// -d output file path
}GLOBALARGS_T;


static const char *optString = "i:p:f:d:";

static struct option longOptions[] = {
	{ "ip", required_argument, NULL, 'i' },
	{ "port", required_argument, NULL, 'p' },
	{ "filemane", required_argument, NULL, 'f' },
	{ "path", required_argument, NULL, 'd' },
	{ NULL, no_argument, NULL, 0 }
};

static void signalHandler(int sig) {
	if (sig == SIGINT) {
		printf("\nBye Bye... \n");
		startExit = 1;
	}
}

void initSignals(void) {
	sigact.sa_handler = signalHandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, (struct sigaction *) NULL);
}

void printSettings(GLOBALARGS_T *globalArgs) {
	printf("Drone IP address    = %s\n", globalArgs->ipAddress);
	printf("Drone command port  = %hu\n", globalArgs->port);
	printf("File prefix         = %s\n", globalArgs->outputFileName);
	printf("File path           = %s\n", globalArgs->path);
}

void printUsage() {

}

int32_t main(int argc, char **argv) {
	int32_t err = 0;
	int32_t c = 0;
	void *handshakeHandle = NULL;
	HANDSHAKE_DATA_T handshakeData = { 0 };
	static GLOBALARGS_T globalArgs;
	char *droneIp = "192.168.42.1";
	uint32_t dronePort = 44444;

	globalArgs.ipAddress = "192.168.42.1";
	globalArgs.port = 44444;
	globalArgs.outputFileName = "droneVideo";
	globalArgs.path = "./";

	while (1) {
		int optionIndex = 0;
		c = getopt_long(argc, argv, optString, longOptions, &optionIndex);

		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (longOptions[optionIndex].flag != 0)
				break;
			printf("option %s", longOptions[optionIndex].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 'i':
			printf("Setting Drone ip addrress = %s\n", optarg);
			globalArgs.ipAddress = optarg;
			break;

		case 'p':
			printf("Setting Drone port = %s\n", optarg);
			globalArgs.port = atoi(optarg);
			puts("option -b\n");
			break;

		case 'f':
			printf("Setting file name prefix `%s'\n", optarg);
			globalArgs.outputFileName = optarg;
			break;

		case 'd':
			printf("Setting path `%s'\n", optarg);
			globalArgs.path = optarg;
			break;

		case ':':
			fprintf(stderr, "%s: option '-%c' requires an argument\n",
			                argv[0], optopt);
			break;
		case '?':
			fprintf(stderr, "%s: option '-%c' is invalid: ignored\n",
			                argv[0], optopt);
			break;
		default:
			abort();
		}
	}
	printSettings(&globalArgs);
	initSignals();
	handshakeHandle = handshakeWithdrone(droneIp, dronePort, &handshakeData);
	if (NULL == handshakeHandle) {
		printf("Handshake failed. Exit\n");
		return 0;
	}

	droneHandle = initDroneComm(droneIp, handshakeData.c2d_port, D2C_PORT,
			&streamData);
	if (NULL == droneHandle) {
		printf("initDroneComm failed. Exit\n");
		return 0;
	}

	vcg = initContainer(640, 368, VCG_CONTAINER_MPEGTS, VCG_CODEC_ID_NONE,
			VCG_CODEC_ID_H264, 1000, &saveClip);
	if (vcg == NULL) {
		printf("initContainer failed \n");
		return 1;
	}

	display = initDisplay(640, 368, AV_PIX_FMT_YUV420P, 640, 368);
	if (display == NULL) {
		printf("failed in creating a display\n");
		return 1;
	}

	sleep(1);
	err = startVideoStreaming(droneHandle);
	if (err < 0) {
		printf("Command send failed. Exit\n");
	}

	while (!startExit) {
		sleep(1);
	}

	closeContainer(vcg);
	closeDisplay(display);
	//closeContainer(vcg, "out.ts");
}

static void streamData(unsigned char *buffer, int32_t bufLen) {
	int32_t pos = 0;
	int32_t err = 0;
	uint32_t bufferId = 0;
	uint32_t seqNum = 0;
	uint32_t size = 0;
	uint32_t frameNum = 0;
	PARROT_DATA_TYPES dataType = 0;
	static int frameCount = 0;
	//printf("stream data received len = %d\n", bufLen);
	if (NULL == buffer) {
		return;
	}

	readXBytestoint32(buffer, bufLen, 1, &pos, &dataType);
	readXBytestoint32(buffer, bufLen, 1, &pos, &bufferId);
	readXBytestoint32(buffer, bufLen, 1, &pos, &seqNum);
	readXBytestoint32(buffer, bufLen, 4, &pos, &size);

	if (dataType) {
		switch (dataType) {
		case P_DATA_TYPE_ACK:
			//printf("P_DATA_TYPE_ACK \n");
			break;

		case P_DATA_TYPE_DATA:
			//printf("P_DATA_TYPE_DATA \n");
			break;

		case P_DATA_TYPE_LOW_LATENCT_DATA:
			//printf("P_DATA_TYPE_LOW_LATENCT_DATA \n");
			readXBytestoint32(buffer, bufLen, 4, &pos, &frameNum);
			//printf("frameNum = %d size = %d frameSize = %d\n", frameNum, size, bufLen - pos);
			//err = writeFrame(vcg, &buffer[pos + 1], bufLen - pos, VCG_FRAME_VIDEO_COMPLETE, 33 * frameCount, 33 * frameCount);
			err = displayH264Frame(display, &buffer[pos + 1], bufLen - pos);
			if (err == -1) {
				startExit = 1;
				printf("Cannot display frame...\n");
			}
			sendAck(droneHandle, buffer, bufLen);
			frameCount++;
			break;

		case P_DATA_TYPE_DATA_WITH_ACK:
			//printf("P_DATA_TYPE_DATA_WITH_ACK \n");
			if (1) { //condition met
				sendAck(droneHandle, buffer, bufLen);
			}
			break;

		default:
			printf("datatype %d not handles \n", dataType);
		}
	}
}

static void saveClip(unsigned char *buffer, int32_t bufLen) {
	FILE *fp = NULL;
	char filename[64];
	static int32_t clipCount = 1;

	if (buffer && bufLen > 0) {
		sprintf(filename, "%s%d%s", "clip", clipCount++, ".ts");
		fp = fopen(filename, "wb");
		fwrite(buffer, bufLen, 1, fp);
		fclose(fp);
	}
}

