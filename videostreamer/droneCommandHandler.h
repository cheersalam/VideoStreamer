#include <pthread.h>
#include <stdint.h>

typedef struct COMMAND_DATA_T {
    char        shortErrMsg[512];
    int32_t     running;
    uint16_t    port;
	int32_t		fd;
    pthread_t   threadId;
}COMMAND_DATA_T;

