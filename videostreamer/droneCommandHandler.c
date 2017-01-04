/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "droneCommandHandler.h"
#include "udpClientSocket.h"
#include "udpServerSocket.h"

typedef struct DRONE_SOCKET_HANDLES {
    void *senderHandle;
    void *receiverHandle;
}DRONE_SOCKET_HANDLES;

void *initDroneComm(char *droneIp, uint16_t commandPort, uint16_t receiverPort, RECEIVER_CB receiverCallback) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = NULL;
    
    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    droneSocketHandles = (DRONE_SOCKET_HANDLES *)malloc(sizeof(DRONE_SOCKET_HANDLES));
    if (NULL == droneSocketHandles) {
        return NULL;
    }
    
    droneSocketHandles->senderHandle = initUdpClientSocket(commandPort, droneIp, NULL);
    if (NULL == droneSocketHandles->senderHandle) {
        free(droneSocketHandles);
        printf("Drone command channel socket connection failed. Exit\n");
        return NULL;
    }

    droneSocketHandles->receiverHandle = initUdpServerSocket(receiverPort, droneIp, receiverCallback);
    if (NULL == droneSocketHandles->receiverHandle)
    {
        closeUdpClient(droneSocketHandles->senderHandle);
        free(droneSocketHandles);
        printf("Drone receiver channel socket connection failed. Exit\n");
        return NULL;
    }
    return droneSocketHandles;
}

int32_t startVideoStreaming(void *handle) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = handle;
    unsigned char buffer[] = {0x04, 0x0b, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x00, 0x01};

    if( NULL != handle)
        return sendClientUdpData(droneSocketHandles->senderHandle, buffer, 0x0c);
    return -1;
}

void sendAck(void *handle, unsigned char *buffer, int32_t bufLen) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = handle;
    unsigned char ackBuffer[8] = {0};

    if( NULL == handle)
        return;

    memcpy(ackBuffer, buffer, 8);
    ackBuffer[0] = '1'; //ack ID
    ackBuffer[1] = 128 + ackBuffer[1];
    ackBuffer[3] = '8'; //ack ID

   if(droneSocketHandles) {
        sendClientUdpData(droneSocketHandles->senderHandle, ackBuffer, 0x08);
   }
}

void closeDroneComm(void *handle) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = handle;

    if( NULL == handle)
        return;

    closeUdpClient(droneSocketHandles->senderHandle);
    closeUdpClient(droneSocketHandles->receiverHandle);
    free(droneSocketHandles);
}
