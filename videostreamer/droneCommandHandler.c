#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "droneCommandHandler.h"
#include "streamReceiver.h"
#include "udpClientSocket.h"
#include "utilities/parrot.h"
#include "utilities/utilities.h"

typedef struct DRONE_SOCKET_HANDLES {
	void *senderHandle;
	void *receiverHandle;
}DRONE_SOCKET_HANDLES;

static void commandResponse(char *buffer, int32_t bufLen);

void *initDroneComm(char *droneIp, uint16_t senderPort, uint16_t receiverPort) {
	DRONE_SOCKET_HANDLES *droneSocketHandles = NULL;
	
	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	droneSocketHandles = (DRONE_SOCKET_HANDLES *)malloc(sizeof(DRONE_SOCKET_HANDLES));
	if (NULL == droneSocketHandles) {
		return NULL;
	}
	
	droneSocketHandles->senderHandle = initUdpClientSocket(senderPort, droneIp, NULL);
	if (NULL == droneSocketHandles->senderHandle) {
		return NULL;
	}

	droneSocketHandles->receiverHandle = startStreamReceiver(droneIp, receiverPort, &streamData);
	if (NULL == droneSocketHandles->receiverHandle)
	{
		closeUdpClient(droneSocketHandles->senderHandle);
		printf("Start Receiver failed. Exit\n");
		return 0;
	}
	return droneSocketHandles;
}


static void streamData(char *buffer, int32_t bufLen) {
	int32_t err = 0;
	int32_t pos = 0;
	PARROT_DATA_TYPES dataType = 0;
	printf("stream data received len = %d\n", bufLen);
	if (NULL == buffer) {
		return;
	}
	if (readXBytestoint32(buffer, bufLen, 1, &pos, &dataType) == 0) {
		switch (dataType) {
		case P_DATA_TYPE_ACK:
			printf("P_DATA_TYPE_ACK \n");
			break;

		case P_DATA_TYPE_DATA:
			printf("P_DATA_TYPE_DATA \n");
			break;

		case P_DATA_TYPE_LOW_LATENCT_DATA:
			printf("P_DATA_TYPE_LOW_LATENCT_DATA \n");
			break;

		case P_DATA_TYPE_DATA_WITH_ACK:
			printf("P_DATA_TYPE_DATA_WITH_ACK \n");
			break;

		default:
			printf("datatype %d not handles \n", dataType);
		}
	}
}
//
//void *startDroneCommandHandler(char *droneIp, uint16_t dronePort) {
//	RECEIVER_CB callback = &commandResponse;
//	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
//	void *handle = initUdpClientSocket(dronePort, droneIp, callback);
//	if (NULL == handle) {
//		return NULL;
//	}
//	return handle;
//}
//
//static void commandResponse(char *buffer, int32_t bufLen) {
//	int32_t err = 0;
//	int32_t pos = 0;
//	PARROT_DATA_TYPES dataType = 0;
//    printf("Command Response len = %d\n", bufLen);
//    if (NULL == buffer) {
//        return;
//    }
//	if ( readXBytestoint32(buffer, bufLen, 1, &pos, &dataType) == 0 ) {
//		switch (dataType){
//		case P_DATA_TYPE_ACK:
//			printf("P_DATA_TYPE_ACK \n");
//			break;
//
//		case P_DATA_TYPE_DATA:
//			printf("P_DATA_TYPE_DATA \n");
//			break;
//
//		case P_DATA_TYPE_LOW_LATENCT_DATA:
//			printf("P_DATA_TYPE_LOW_LATENCT_DATA \n");
//			break;
//
//		case P_DATA_TYPE_DATA_WITH_ACK:
//			printf("P_DATA_TYPE_DATA_WITH_ACK \n");
//			break;
//
//		default:
//			printf("datatype %d not handles \n", dataType);
//		}
//	}
//
//}

int32_t startVideoStreaming(void *handle) {
	DRONE_SOCKET_HANDLES *droneSocketHandles = handle;
    char buffer[] = {0x04, 0x0b, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x00, 0x01};
    return sendClientUdpData(droneSocketHandles->senderHandle, buffer, 0x0c);
}




