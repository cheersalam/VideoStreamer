#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "config.h"
#include "pthread.h"
#include "socklibCommon.h"
#include "droneHandshake.h"
#include "droneCommandHandler.h"
#include "utilities/parrot.h"
#include "utilities/utilities.h"

static void streamData(unsigned char *buffer, int32_t bufLen);

//only global
void *droneHandle = NULL;
void *vcg = NULL;
static int count = 0;

int32_t main() {
    int32_t err                     = 0;
    void *handshakeHandle           = NULL;
    HANDSHAKE_DATA_T handshakeData  = {0};
    char *droneIp                   = "192.168.42.1";
    uint32_t dronePort              = 44444;

    handshakeHandle = handshakeWithdrone(droneIp, dronePort, &handshakeData);
    if (NULL == handshakeHandle) 
    {
        printf("Handshake failed. Exit\n");
        return 0;
    }

    droneHandle = initDroneComm(droneIp, handshakeData.c2d_port, D2C_PORT, &streamData);
    if (NULL == droneHandle)
    {
        printf("initDroneComm failed. Exit\n");
        return 0;
    }

#if 0
    vcg = initContainer(640, 480, VCG_CONTAINER_MPEGTS, VCG_CODEC_ID_NONE, VCG_CODEC_ID_H264, 60);
    if(vcg == NULL) {
        printf("initContainer failed \n");
        return 1;
    }
#endif
    sleep(1);
    err = startVideoStreaming(droneHandle);
    if (err < 0) {
        printf("Command send failed. Exit\n");
    }

    while(count < 200)
        sleep(1);
    //closeContainer(vcg, "out.ts");
}

static void streamData(unsigned char *buffer, int32_t bufLen) {
    int32_t pos = 0;
    int32_t err = 0;
    int32_t datatype = 0;
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
            //printf("P_DATA_TYPE_ACK \n");
            break;

        case P_DATA_TYPE_DATA:
            //printf("P_DATA_TYPE_DATA \n");
            break;

        case P_DATA_TYPE_LOW_LATENCT_DATA:
            //printf("P_DATA_TYPE_LOW_LATENCT_DATA \n");
            readXBytestoint32(buffer, bufLen, 4, &pos, &frameNum);
            printf("frameNum = %d size = %d frameSize = %d\n", frameNum, size, bufLen - pos);
     //       err = writeFrame(vcg, &buffer[pos + 1], bufLen - pos, VCG_FRAME_VIDEO_COMPLETE, 33 * frameCount, 33 * frameCount);
            sendAck(droneHandle, buffer, bufLen);
            frameCount++;
            count++;
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


