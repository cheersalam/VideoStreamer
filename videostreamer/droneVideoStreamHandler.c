/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "droneVideoStreamHandler.h"
#include "udpClientSocket.h"
#include "udpServerSocket.h"

typedef struct DRONE_VIDEO_STREAM_HANDLE {
    void *rtpHandle;
    void *rtcpHandle;
}DRONE_VIDEO_STREAM_HANDLE;

void *initDroneVideoStreams(char *droneIp, uint16_t rtpPort, uint16_t rtcpPort, RECEIVER_CB rtpCallback, RECEIVER_CB rtcpCallback) {
    DRONE_VIDEO_STREAM_HANDLE *droneVideoStreamHandle = NULL;
    
    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    droneVideoStreamHandle = (DRONE_VIDEO_STREAM_HANDLE *)malloc(sizeof(DRONE_VIDEO_STREAM_HANDLE));
    if (NULL == droneVideoStreamHandle) {
        return NULL;
    }
        
    if (rtpPort) {
        droneVideoStreamHandle->rtpHandle = initUdpServerSocket(rtpPort, droneIp, rtpCallback);
        if (NULL == droneVideoStreamHandle->rtpHandle) {
            free(droneVideoStreamHandle);
            printf("RTP socket failed\n");
        }
    }

    if (rtcpPort) {
        droneVideoStreamHandle->rtcpHandle = initUdpServerSocket(rtcpPort, droneIp, rtcpCallback);
        if (NULL == droneVideoStreamHandle->rtcpHandle) {
            closeUdpClient(droneVideoStreamHandle->rtpHandle);
            free(droneVideoStreamHandle);
            printf("RTCP socket failed\n");
        }
    }

    return droneVideoStreamHandle;
}

void closeVideoStreams(void *handle) {
    DRONE_VIDEO_STREAM_HANDLE *droneVideoStreamHandle = handle;

    if( NULL == handle)
        return;

    closeUdpClient(droneVideoStreamHandle->rtpHandle);
    closeUdpClient(droneVideoStreamHandle->rtcpHandle);
    free(droneVideoStreamHandle);
}

