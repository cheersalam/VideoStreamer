/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include "udpClientSocket.h"
#include <assert.h>
#include <pthread.h>

#define MAX_RECV_BUF_LEN (1024 * 1024)

typedef struct UDP_CLIENT_SOCKET_T {
    uint16_t port;
    int32_t fd;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    pthread_t   threadId;
    int32_t isRunning;
    RECEIVER_CB callback;
}UDP_CLIENT_SOCKET_T;

static void *clientThread(void *data);

void *initUdpClientSocket(uint16_t port, char *hostname, RECEIVER_CB callback) {
    int32_t err = 0;
    UDP_CLIENT_SOCKET_T *udpSocketData = NULL;

    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    assert(hostname);
    udpSocketData = (UDP_CLIENT_SOCKET_T*)malloc(sizeof(UDP_CLIENT_SOCKET_T));
    if (NULL == udpSocketData) {
        printf("ERROR malloc error\n");
        return NULL;
    }
    memset(udpSocketData, 0, sizeof(UDP_CLIENT_SOCKET_T));
    udpSocketData->port = port;
    udpSocketData->callback = callback;

    udpSocketData->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocketData->fd < 0) {
        printf("ERROR opening socket\n");
        free(udpSocketData);
        return NULL;
    }

    udpSocketData->server = gethostbyname(hostname);
    if (NULL == udpSocketData->server) {
        printf("ERROR, no such host as %s\n", hostname);
        free(udpSocketData);
        return NULL;
    }

    memset(&udpSocketData->serveraddr, 0, sizeof(udpSocketData->serveraddr));
    udpSocketData->serveraddr.sin_family = AF_INET;
    udpSocketData->serveraddr.sin_port = htons(port);
    memcpy((char *)&udpSocketData->serveraddr.sin_addr.s_addr, (char *)udpSocketData->server->h_addr, udpSocketData->server->h_length);

    err = pthread_create(&udpSocketData->threadId, NULL, clientThread, (void *)udpSocketData);
    if (err) {
        printf("ERROR thread creation failed\n");
        close(udpSocketData->fd);
        free(udpSocketData);
        return NULL;
    }
    return udpSocketData;
}

static void *clientThread(void *args) {
    UDP_CLIENT_SOCKET_T *udpSocketData = args;
    int32_t nBytes = 0;
    uint32_t addrSize = 0;
    unsigned char buffer[MAX_RECV_BUF_LEN];

    addrSize = sizeof(udpSocketData->serveraddr);
    udpSocketData->isRunning = 1;
    printf("UDP client socket Waiting for new messages from %s on port %d\n", udpSocketData->server->h_addr, udpSocketData->port);
    while (udpSocketData->isRunning) {
        nBytes = recvfrom(udpSocketData->fd, buffer, MAX_RECV_BUF_LEN, 0, (struct sockaddr *)&udpSocketData->serveraddr, &addrSize);
        if (nBytes < 0) {
            printf("ERROR in recvfrom\n");
        }
        printf("UDP Client socket Bytes Received = %d\n", nBytes);

        if (udpSocketData->callback) {
            printf("UDP client socket receiver callback not set, dropping packets\n");
            udpSocketData->callback(buffer, nBytes);
        }
    }
    printf("Exiting thread UDP Client\n ");
    pthread_exit(NULL);
}

int32_t sendClientUdpData(void *handle, unsigned char *buffer, int32_t bufLen) {
    int32_t addrSize = 0;
    int32_t nBytes;
    UDP_CLIENT_SOCKET_T *udpSocketData = handle;

    //printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (NULL == buffer) {
        return 0;
    }

    addrSize = sizeof(udpSocketData->serveraddr);
    nBytes = sendto(udpSocketData->fd, buffer, bufLen, 0, (struct sockaddr *)&udpSocketData->serveraddr, addrSize);
    if (nBytes < 0) {
        printf("ERROR in sendto\n");
        return 0;
    }
    //printf("Command sent \n");
    return nBytes;
}

int32_t isUdpClientRunning(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    UDP_CLIENT_SOCKET_T *udpSocketData = handle;
    return udpSocketData->isRunning;
}

int32_t closeUdpClient(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    UDP_CLIENT_SOCKET_T *udpSocketData = handle;
    if (udpSocketData->isRunning) {
        udpSocketData->isRunning = 0;
        pthread_join(udpSocketData->threadId, NULL);
        close(udpSocketData->fd);
        free(udpSocketData);
    }
    return 1;
}

