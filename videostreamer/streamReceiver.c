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

void *startStreamReceiver(char *droneIp, uint16_t dronePort, RECEIVER_CB *callback) {
	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	void *handle = initUdpServerSocket((dronePort, droneIp, callback);
	if (NULL == handle) {
		return NULL;
	}
	return handle;
}



