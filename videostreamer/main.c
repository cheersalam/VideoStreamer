#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "config.h"
#include "pthread.h"
#include "common.h"
#include "streamReceiver.h"
#include "droneHandshake.h"
#include "droneCommandHandler.h"


int32_t main() {
	int32_t err						= 0;
    void *handshakeHandle			= NULL;
    void *streamReceiverHandle		= NULL;
    void *commandHandle				= NULL;
	HANDSHAKE_DATA_T handshakeData	= {0};
	char *droneIp					= "192.168.42.1";
	uint32_t dronePort				= 44444;

	handshakeHandle = handshakeWithdrone(droneIp, dronePort, &handshakeData);
    if (NULL == handshakeHandle) 
    {
        printf("Handshake failed. Exit\n");
        return 0;
    }

    commandHandle = startDroneCommandHandler(droneIp, handshakeData.c2d_port);
    if (NULL == commandHandle) 
    {
        printf("CommandHandler failed. Exit\n");
        return 0;
    }

    streamReceiverHandle = startStreamReceiver(droneIp, D2C_PORT);
    if (NULL == streamReceiverHandle) 
    {
        printf("Start Receiver failed. Exit\n");
        return 0;
    }
   
    sleep(1);
    err = startVideoStreaming(commandHandle);
    if (err < 0) {
        printf("Command send failed. Exit\n");
    }


    while(1)
        sleep(1);
}
