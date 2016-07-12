#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef void (*RECEIVER_CB)(char *buffer, int32_t bufLen);

int32_t closeUdpServer(void *handle);
int32_t isUdpServerRunning(void *handle);
int32_t sendServerUdpData(void *handle, char *buffer, int32_t bufLen);
void *initUdpServerSocket(uint16_t port, char *hostname, RECEIVER_CB callback);

