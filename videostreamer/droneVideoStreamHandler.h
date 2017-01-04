/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#ifndef DRONE_VIDEO_STREAM_HANDLER_H
#define DRONE_VIDEO_STREAM_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "socklibCommon.h"


void *initDroneVideoStreams(char *droneIp, uint16_t rtpPort, uint16_t rtcpPort, RECEIVER_CB rtpCallback, RECEIVER_CB rtcpCallback);
void closeVideoStreams(void *handle);

#endif //DRONE_VIDEO_STREAM_HANDLER_H
