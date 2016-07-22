#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "assert.h"
#include "streamReceiver.h"
#include "udpServerSocket.h"
#include "utilities/parrot.h"
#include "utilities/utilities.h"

static void streamData(char *buffer, int32_t bufLen);

void *startStreamReceiver(char *droneIp, uint16_t dronePort) {
	RECEIVER_CB callback = &streamData;
	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	void *handle = initUdpServerSocket(dronePort, droneIp, callback);
	if (NULL == handle) {
		return NULL;
	}
	return handle;
}

static void streamData(char *buffer, int32_t bufLen) {
	int32_t err = 0;
	int32_t pos = 0;
	PARROT_DATA_TYPES dataType = 0;
	printf("stream data received len = %d\n", bufLen);
    if (NULL == buffer) {
        return;
    }
	if ( readXBytestoint32(buffer, bufLen, 1, &pos, &dataType) == 0 ) {
		switch (dataType){
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

