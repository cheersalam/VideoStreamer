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

    sleep(1);
    err = startVideoStreaming(droneHandle);
    if (err < 0) {
        printf("Command send failed. Exit\n");
    }

    while(1)
        sleep(1);
}

static void streamData(unsigned char *buffer, int32_t bufLen) {
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
            if (1) { //condition met
                sendAck(droneHandle, buffer, bufLen);
            }
            break;

        default:
            printf("datatype %d not handles \n", dataType);
        }
    }
}


