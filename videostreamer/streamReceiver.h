
void *startReceiver(void *args);

typedef struct RECEIVER_DATA_T {
    char        shortErrMsg[512];
    int32_t     running;
    uint16_t    port;
}RECEIVER_DATA_T;
