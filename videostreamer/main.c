#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "droneHandshake.h"
#include "config.h"
//#include "jsmn.h"
#include "pthread.h"
#include "streamReceiver.h"
#include "common.h"


int32_t parseHandshakeResponse(char *jsonStr, HANDSHAKE_DATA_T *handshakeData);

//static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
//	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
//			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
//		return 0;
//	}
//	return -1;
//}

int32_t main() {
	int32_t err = 0;
    void *handshakeHandle = NULL;
    void *streamReceiverHandle = NULL;
	HANDSHAKE_DATA_T handshakeData = {0};
	char *droneIp = "127.0.0.1";//"192.168.42.1";
	uint32_t dronePort = 5000;//44444;

	handshakeHandle = handshakeWithdrone(droneIp, dronePort, &handshakeData);

    if (NULL == handshakeHandle) 
    {
        printf("Handshake failed. Exit\n");
        return 0;
    }

    streamReceiverHandle = startStreamReceiver(droneIp, handshakeData.c2d_port);
    if (NULL == streamReceiverHandle) 
    {
        printf("Start Receiver failed. Exit\n");
        return 0;
    }



    while(1)
        sleep(1);
}

//int32_t oldmain(){
//    int droneCommandfd              = 0;;
//	int32_t err                     = 0;
//	char response[1024]             = {0};
//    void *receiverHandle            = NULL;
//    void *handshakeHandle           = NULL;
//    void *commandHandle           = NULL;
//    RECEIVER_CONFIG_T config        = {0};
//    HANDSHAKE_DATA_T handshakeData  = {0};
//	char buffer[] = {0x04, 0x0b, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x00, 0x01};
//
//    err = startNetwork(DRONE_IP_ADD, DRONE_COMM_PORT, &droneCommandfd);
//    if (err) {
//        printf("startNetwork Failed\n");
//        return 0;
//    }
//
//	err = startHandshake(droneCommandfd, DRONE_HANDSHAKE_REQ, response, 1024 - 1);
//    if(err) {
//        printf("send command failed. Exiting\n");
//        return 0;
//    }	 
//   
//    parseHandshakeResponse(response, &handshakeData);
//
//    config.port = 43210;
//    receiverHandle = startReceiver(&config);
//    if (NULL == receiverHandle) {
//        printf("Starting receiver failed. Exiting...\n");
//        return 0;
//    }
//
//	commandHandle = startDroneCommandHandler(54321);
//	if(NULL == commandHandle) {
//		return 0;
//	}
//
//	sendCommand(commandHandle, buffer, 0x0c);
//    while(1) {
//        sleep(1);
//        printf("Receiver thread status = %d\n", isRunning(receiverHandle));
//    }
//}
//
//int32_t parseHandshakeResponse(char *jsonStr, HANDSHAKE_DATA_T *handshakeData) {
//	int32_t     i = 0;
//	int32_t     r = 0;
//    char        value[256];
//	jsmn_parser p;
//	jsmntok_t   t[128]; /* We expect no more than 128 tokens */
//	jsmn_init(&p);
//
//    assert(jsonStr);
//
//	r = jsmn_parse(&p, jsonStr, strlen(jsonStr), t, sizeof(t)/sizeof(t[0]));
//	if (r < 0) {
//		printf("Failed to parse JSON: %d\n", r);
//		return r;
//	}
//
//    /* Loop over all keys of the root object */
//	for (i = 1; i < r; i++) {
//        memset(value, 0, 256);
//        memcpy(value, jsonStr + t[i+1].start, t[i+1].end - t[i+1].start);
//		if (jsoneq(jsonStr, &t[i], "status") == 0) {
//			printf("- Status: %.*s\n", t[i+1].end - t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->status = (int32_t)strtol(value, NULL, 0);
//			i++;
//		} else if (jsoneq(jsonStr, &t[i], "c2d_port") == 0) {
//			printf("- c2d_port: %.*s\n", t[i+1].end - t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->c2d_port = (int32_t)strtol(value, NULL, 0);
//			i++;
//		} else if (jsoneq(jsonStr, &t[i], "arstream_fragment_size") == 0) {
//			printf("- arstream_fragment_size: %.*s\n", t[i+1].end-t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->arstream_fragment_size = (int32_t)strtol(value, NULL, 0);
//			i++;
//        } else if (jsoneq(jsonStr, &t[i], "arstream_fragment_maximum_number") == 0) {
//			printf("- arstream_fragment_size: %.*s\n", t[i+1].end-t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->arstream_fragment_maximum_number = (int32_t)strtol(value, NULL, 0);
//			i++;
//        } else if (jsoneq(jsonStr, &t[i], "arstream_max_ack_interval") == 0) {
//			printf("- arstream_max_ack_interval: %.*s\n", t[i+1].end-t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->arstream_max_ack_interval = (int32_t)strtol(value, NULL, 0);
//			i++;
//        } else if (jsoneq(jsonStr, &t[i], "c2d_update_port") == 0) {
//			printf("- c2d_update_port: %.*s\n", t[i+1].end-t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->c2d_update_port = (int32_t)strtol(value, NULL, 0);
//			i++;
//        } else if (jsoneq(jsonStr, &t[i], "c2d_user_port") == 0) {
//			printf("- c2d_user_port: %.*s\n", t[i+1].end-t[i+1].start, jsonStr + t[i+1].start);
//            handshakeData->c2d_user_port = (int32_t)strtol(value, NULL, 0);
//			i++;
//		} else {
//			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
//					jsonStr + t[i].start);
//		}
//	}
//    return 0;
//}

