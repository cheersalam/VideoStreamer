#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "droneCommandHandler.h"
#include "udpClientSocket.h"


void *startDroneCommandHandler(char *droneIp, uint16_t dronePort) {
	RECEIVER_CB callback = &commandResponse;
	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	void *handle = initUdpClientSocket(dronePort, droneIp, callback);
	if (NULL == handle) {
		return NULL;
	}
	return handle;
}

static void commandResponse(char *buffer, int32_t bufLen) {
	printf("stream data received len = %d\n", bufLen);
}

int32_t startVideoStreaming(void *handle) {
    char buffer[] = {0x04, 0x0b, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x00, 0x01};
    return sendClientUdpData(handle, buffer, 0x0c);
}




