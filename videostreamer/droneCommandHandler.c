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
#include "utilities/parrot.h"
#include "utilities/utilities.h"

typedef struct DRONE_SOCKET_HANDLES {
    void *senderHandle;
    void *receiverHandle;
    void *rtpHandle;
    void *rtcpHandle;
}DRONE_SOCKET_HANDLES;

void *initDroneComm(char *droneIp, uint16_t dronePort, uint16_t receiverPort, uint16_t rtpPort, uint16_t rtcpPort, RECEIVER_CB streamCallback, RECEIVER_CB rtpCallback, RECEIVER_CB rtcpCallback) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = NULL;
    
    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    droneSocketHandles = (DRONE_SOCKET_HANDLES *)malloc(sizeof(DRONE_SOCKET_HANDLES));
    if (NULL == droneSocketHandles) {
        return NULL;
    }
    
    droneSocketHandles->senderHandle = initUdpClientSocket(dronePort, droneIp, NULL);
    if (NULL == droneSocketHandles->senderHandle) {
        return NULL;
    }

    droneSocketHandles->receiverHandle = initUdpServerSocket(receiverPort, droneIp, streamCallback);
    if (NULL == droneSocketHandles->receiverHandle)
    {
        closeUdpClient(droneSocketHandles->senderHandle);
        printf("Start Receiver failed. Exit\n");
        return 0;
    }
   
/*    if (rtpPort) {
        droneSocketHandles->rtpHandle = initUdpServerSocket(rtpPort, droneIp, rtpCallback);
        if (NULL == droneSocketHandles->rtpHandle) {
            printf("RTP socket failed\n");
        }
    }

    if (rtcpPort) {
        droneSocketHandles->rtcpHandle = initUdpServerSocket(rtcpPort, droneIp, rtcpCallback);
        if (NULL == droneSocketHandles->rtcpHandle) {
            printf("RTCP socket failed\n");
        }
    }*/

    return droneSocketHandles;
}

int32_t startVideoStreaming(void *handle) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = handle;
    unsigned char buffer[] = {0x04, 0x0b, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x00, 0x01};
    return sendClientUdpData(droneSocketHandles->senderHandle, buffer, 0x0c);
}

void sendAck(void *handle, unsigned char *buffer, int32_t bufLen) {
    DRONE_SOCKET_HANDLES *droneSocketHandles = handle;
    unsigned char ackBuffer[8] = {0};

    memcpy(ackBuffer, buffer, 8);
    ackBuffer[0] = '1'; //ack ID
    ackBuffer[1] = 128 + ackBuffer[1];
    ackBuffer[3] = '8'; //ack ID

   if(droneSocketHandles) {
        sendClientUdpData(droneSocketHandles->senderHandle, ackBuffer, 0x08);
   }
}

