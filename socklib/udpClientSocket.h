/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#ifndef UDP_CLIENT_SOCKET_H
#define UDP_CLIENT_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "socklibCommon.h"


int32_t closeUdpClient(void *handle);
int32_t isUdpClientRunning(void *handle);
int32_t sendClientUdpData(void *handle, unsigned char *buffer, int32_t bufLen);
void *initUdpClientSocket(uint16_t port, char *hostname, RECEIVER_CB callback);

#endif //UDP_CLIENT_SOCKET_H
