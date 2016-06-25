#include "pthread.h"


typedef struct RECEIVER_CONFIG_T {
    uint16_t    port;
}RECEIVER_CONFIG_T;

typedef struct RECEIVER_DATA_T {
    char        shortErrMsg[512];
    int32_t     running;
    uint16_t    port;
    pthread_t   threadId;
}RECEIVER_DATA_T;

void *startReceiver(RECEIVER_CONFIG_T *config);
int32_t isRunning(void *handle);
