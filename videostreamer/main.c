/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

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
#include "createPlaylist.h"

#define MAX_RTP_FRAME_LEN (5 * 1024 * 1024)

static void streamData(unsigned char *buffer, int32_t bufLen);
static void saveClip(unsigned char *buffer, int32_t bufLen, int64_t durationMsec);
static void rtpData(unsigned char *buffer, int32_t bufLen);
static void rtcpData(unsigned char *buffer, int32_t bufLen);
static void parseRTPHeader(unsigned char *buffer, int32_t bufLen);

static unsigned char rtpFrame[MAX_RTP_FRAME_LEN];
static uint32_t rtpFrameLen = 0;
static uint16_t rtpLastSeq = 0;
static uint32_t rtpLastTimestamp = 0;
static uint16_t rtpMarkerPos = 0x0080;
static int32_t dropPacket = 0;

typedef struct GLOBALARGS_T{
	int32_t enableHls;		// -h enable HLS streaming
	int32_t enableFileSave;  // -s save video files
	int32_t enableDisplay;   // -r save video files
	int32_t segmentDuration; // -t segment duration in sec
	int16_t port;  			// -p drone command listener port
	char *ipAddress; 		// -i ip address of drone
	char *outputFileName; 	// -f output file name
	char *outputFilePath;	// -d output file path
	char *playlistFileName;	// -u m3u8 file name
	char *playlistFilePath;	// -v output file path
}GLOBALARGS_T;

typedef struct RTP_HEADER_T{
	int32_t markerBit;
    int32_t csrcCount;
    int32_t payloadType;
    int32_t seqNum;
    int32_t timestamp;
    int32_t ssrc;
    int32_t csrc[16];
}RTP_HEADER_T;


static const char *optString = "i:p:f:d:";

static struct option longOptions[] = {
	{ "ip", required_argument, NULL, 'i' },
	{ "port", required_argument, NULL, 'p' },
	{ "outputFileName", required_argument, NULL, 'f' },
	{ "outputFilePath", required_argument, NULL, 'd' },
	{ "playlistFileName", required_argument, NULL, 'u' },
	{ "playlistFilePath", required_argument, NULL, 'v' },
	{ "enableHls", no_argument, NULL, 'h' },
	{ "enableFileSave", no_argument, NULL, 's' },
	{ "enableDisplay", no_argument, NULL, 'r' },
	{ "segmentDuration", required_argument, NULL, 't' },
	{ NULL, no_argument, NULL, 0 }
};


//only globals
static volatile int32_t startExit = 0;
void *droneHandle = NULL;
void *vcg = NULL;
void *display = NULL;
void *playlist = NULL;
struct sigaction sigact;
static GLOBALARGS_T globalArgs;


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
	printf("Drone IP address   		= %s\n", globalArgs->ipAddress);
	printf("Drone command port 		= %hu\n", globalArgs->port);
	printf("Enable HLS    			= %d\n", globalArgs->enableHls);
	printf("Enable FileSave    		= %d\n", globalArgs->enableFileSave);
	printf("Enable Display    		= %d\n", globalArgs->enableDisplay);
	printf("Video File duration		= %d\n", globalArgs->segmentDuration);
	printf("Video File prefix  		= %s\n", globalArgs->outputFileName);
	printf("Video File path    		= %s\n", globalArgs->outputFilePath);
	printf("Playlist file prefix	= %s\n", globalArgs->playlistFileName);
	printf("Playlist file path 		= %s\n", globalArgs->playlistFilePath);
}

void printUsage() {

}

int32_t main(int argc, char **argv) {
	int32_t err = 0;
	int32_t c = 0;
	void *handshakeHandle = NULL;
	HANDSHAKE_DATA_T handshakeData = { 0 };
	char *droneIp = "192.168.42.1";
	uint32_t dronePort = 44444;

	globalArgs.ipAddress = "192.168.42.1";
	globalArgs.port = 44444;
	globalArgs.outputFileName = "droneVideo";
	globalArgs.outputFilePath = "/var/www/html/parrot";
	globalArgs.playlistFileName = "parrot";
	globalArgs.playlistFilePath = "/var/www/html/parrot";
	globalArgs.enableFileSave = 1;
	globalArgs.enableHls = 1;
	globalArgs.enableDisplay = 1;
	globalArgs.segmentDuration = 1;

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
			printf("Setting Video File name prefix `%s'\n", optarg);
			globalArgs.outputFileName = optarg;
			break;

		case 'd':
			printf("Setting Video File `%s'\n", optarg);
			globalArgs.outputFilePath = optarg;
			break;

		case 'u':
			printf("Setting Playlist File name prefix `%s'\n", optarg);
			globalArgs.playlistFileName = optarg;
			break;

		case 'v':
			printf("Setting Playlist path `%s'\n", optarg);
			globalArgs.playlistFilePath = optarg;
			break;

		case 'h':
			printf("Enabling HLS Streaming \n");
			globalArgs.enableHls = 1;
			break;

		case 's':
			printf("Enabling File saving \n");
			globalArgs.enableFileSave = 1;
			break;

		case 'r':
			printf("Enabling Display \n");
			globalArgs.enableDisplay = 1;
			break;

		case 't':
			printf("Setting segment duration \n");
			globalArgs.segmentDuration = atoi(optarg);
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
	handshakeHandle = handshakeWithdrone(globalArgs.ipAddress, globalArgs.port, &handshakeData);
	if (NULL == handshakeHandle) {
		printf("Handshake failed. Exit\n");
		return 0;
	}

	droneHandle = initDroneComm(globalArgs.ipAddress, handshakeData.c2d_port, D2C_PORT, handshakeData.arstream2_server_stream_port, handshakeData.arstream2_server_control_port,
			&streamData, &rtpData, &rtcpData);
	if (NULL == droneHandle) {
		printf("initDroneComm failed. Exit\n");
		return 0;
	}

	vcg = initContainer(640, 368, VCG_CONTAINER_MPEGTS, VCG_CODEC_ID_NONE,
			VCG_CODEC_ID_H264, globalArgs.segmentDuration * 1000, &saveClip);
	if (vcg == NULL) {
		printf("initContainer failed \n");
		return 1;
	}

	if(globalArgs.enableDisplay) {
		display = initDisplay(640, 368, AV_PIX_FMT_YUV420P, 1920, 1080);
	//	display = initDisplay(640, 368, AV_PIX_FMT_YUV420P, 640, 368);
		if (display == NULL) {
			printf("failed in creating a display\n");
			return 1;
		}
	}

	err = startVideoStreaming(droneHandle);
	if (err < 0) {
		printf("Command send failed. Exit\n");
	}

	if(globalArgs.enableHls) {
		playlist = initPlayList(globalArgs.playlistFilePath, globalArgs.playlistFileName);
		if(playlist == NULL) {
			startExit = 1;
		}
	}

	while (!startExit) {
		sleep(1);
	}

	closeContainer(vcg);
	closeDisplay(display);
	finalizePlaylist(playlist);
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
			sendAck(droneHandle, buffer, bufLen);
			//printf("P_DATA_TYPE_ACK \n");
			break;

		case P_DATA_TYPE_DATA:
			sendAck(droneHandle, buffer, bufLen);
			//printf("P_DATA_TYPE_DATA \n");
			break;

		case P_DATA_TYPE_LOW_LATENCT_DATA:
			//printf("P_DATA_TYPE_LOW_LATENCT_DATA \n");
			readXBytestoint32(buffer, bufLen, 4, &pos, &frameNum);
			//printf("frameNum = %d size = %d frameSize = %d\n", frameNum, size, bufLen - pos);

			if(globalArgs.enableFileSave || globalArgs.enableHls) {
				err = writeFrame(vcg, &buffer[pos + 1], bufLen - pos, VCG_FRAME_VIDEO_COMPLETE, 33 * frameCount, 33 * frameCount);
			}
			if(globalArgs.enableDisplay) {
				err = displayH264Frame(display, &buffer[pos + 1], bufLen - pos);
			}
			if (err == -1) {
				//startExit = 1;
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

static void rtpData(unsigned char *buffer, int32_t bufLen) {
	int32_t pos = 0;
	int32_t err;
	uint16_t shortVal = 0;
	uint16_t seqNum = 0;
	RTP_HEADER_T *rtpHeader = NULL;;
	uint32_t timeStamp = 0;
	static int frameCount = 0;
	rtpHeader = (RTP_HEADER_T *)malloc(sizeof(struct RTP_HEADER_T));
	memset(rtpHeader, 0, sizeof(struct RTP_HEADER_T));

	err = readShortToShort(buffer, bufLen, &pos, &shortVal);
	printf("Short = %x pos = %d\n", shortVal, pos);

	err = readShortToShort(buffer, bufLen, &pos, &seqNum);
	printf("seqNum = %hu pos = %d\n", seqNum, pos);

	err = read4Bytetoint32(buffer, bufLen, &pos, &timeStamp);
	printf("timestamp = %u pos = %d\n", timeStamp, pos);

	if (!rtpLastSeq || (seqNum != rtpLastSeq + 1)) {
		printf("RTP packet lost. Dropping seqNum = %hu \n", seqNum);
		dropPacket = 1;
	}

	rtpLastSeq = seqNum;
	rtpLastTimestamp = timeStamp;
	if (shortVal & rtpMarkerPos) {
		printf("Marker bit set \n");
		if(!dropPacket) {
			memcpy(&rtpFrame[rtpFrameLen], &buffer[16], bufLen - 16);
			rtpFrameLen += bufLen - 16;
			printf("Display Frame of len = %d\n", rtpFrameLen);
			writeFrame(vcg, &rtpFrame[0], rtpFrameLen, VCG_FRAME_VIDEO_COMPLETE, 33 * frameCount, 33 * frameCount);
			//displayH264Frame(display, &rtpFrame[0], rtpFrameLen);
		}
		dropPacket = 0;
		frameCount++;
		memset(rtpFrame, 0, MAX_RTP_FRAME_LEN);
		rtpFrame[3] = 0x01;
		rtpFrameLen = 4;
	}
	else {
		memcpy(&rtpFrame[rtpFrameLen], &buffer[16], bufLen - 16);
		rtpFrameLen += bufLen - 16;
		printf("rtpFrameLen = %d\n", rtpFrameLen);
	}
}

static void rtcpData(unsigned char *buffer, int32_t bufLen) {
	int32_t pos = 0;

    printf("RTCP data received %d\n", bufLen);
}

static void saveClip(unsigned char *buffer, int32_t bufLen, int64_t durationMsec) {
	FILE *fp = NULL;
	char filename[64];
	char filenameWithPath[4096];
	static int32_t clipCount = 0;

	if (buffer && bufLen > 0) {
		sprintf(filename, "%s%d%s", globalArgs.outputFileName, clipCount++, ".ts");
		sprintf(filenameWithPath, "%s/%s", globalArgs.outputFilePath, filename);
		fp = fopen(filenameWithPath, "wb");
		if (NULL != fp) {
			fwrite(buffer, bufLen, 1, fp);
			fclose(fp);
			if(globalArgs.enableHls) {
				addFileToPlaylist(playlist, durationMsec, filename, "");
			}
		}else {
			printf("Cannot open file to write `%s` Make sure folder exists\n", filenameWithPath);
		}
	}
}

