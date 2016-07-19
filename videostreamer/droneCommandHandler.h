#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


#if 0
typedef struct COMMAND_DATA_T {
    char        shortErrMsg[512];
    int32_t     running;
    uint16_t    port;
	int32_t		fd;
    pthread_t   threadId;
    struct sockaddr_in serverAddr;
}COMMAND_DATA_T;
#endif

void *startDroneCommandHandler(char *droneIp, uint16_t dronePort);
int32_t startVideoStreaming(void *handle);
