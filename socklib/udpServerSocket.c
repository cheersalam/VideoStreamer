/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include "udpServerSocket.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>

#define MAX_RECV_BUF_LEN (1024 * 1024)

typedef struct UDP_SERVER_SOCKET_T {
    uint16_t port;
    int32_t fd;
    struct sockaddr_in serveraddr;
    pthread_t   threadId;
    int32_t isRunning;
    RECEIVER_CB callback;
}UDP_SERVER_SOCKET_T;

void *serverThread(void *data);

void *initUdpServerSocket(uint16_t port, char *hostname, RECEIVER_CB callback) {
    int32_t err                 = 0;
    int32_t optval              = 0;
    UDP_SERVER_SOCKET_T *udpSocketData = NULL;

    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    assert(hostname);

    udpSocketData = (UDP_SERVER_SOCKET_T*)malloc(sizeof(UDP_SERVER_SOCKET_T));
    if (NULL == udpSocketData) {
        printf("ERROR malloc error\n");
        return NULL;
    }
    memset((void *)udpSocketData, 0, sizeof(UDP_SERVER_SOCKET_T));
    udpSocketData->port = port;
    udpSocketData->callback = callback;

    udpSocketData->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocketData->fd < 0) {
        printf("ERROR opening socket\n");
        free(udpSocketData);
        return NULL;
    }

    optval = 1;//enable
    setsockopt(udpSocketData->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));

    memset((char *)&udpSocketData->serveraddr, 0, sizeof(udpSocketData->serveraddr));
    udpSocketData->serveraddr.sin_family = AF_INET;
    udpSocketData->serveraddr.sin_port = htons(port);
    udpSocketData->serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udpSocketData->fd, (struct sockaddr *)&udpSocketData->serveraddr, sizeof(udpSocketData->serveraddr)) < 0) {
        printf("ERROR on binding");
        free(udpSocketData);
        return NULL;
    }

    err = pthread_create(&udpSocketData->threadId, NULL, serverThread, (void *)udpSocketData);
    if (err) {
        printf("ERROR thread creation failed\n");
        close(udpSocketData->fd);
        free(udpSocketData);
    }
    return udpSocketData;
}

void *serverThread(void *args) {
    UDP_SERVER_SOCKET_T *udpSocketData              = args;
    int32_t nBytes                          = 0;
    uint32_t addrSize                       = 0;
    unsigned char buffer[MAX_RECV_BUF_LEN]  = {0};
    struct sockaddr_in clientaddr; /* client addr */
    

    printf("%s:%s:%d Waiting for incoming messages\n", __FILE__, __func__, __LINE__);
    addrSize = sizeof(clientaddr);
    udpSocketData->isRunning = 1;
    while (udpSocketData->isRunning) {
        nBytes = recvfrom(udpSocketData->fd, buffer, MAX_RECV_BUF_LEN, 0, (struct sockaddr *)&clientaddr, &addrSize);
        if (nBytes < 0) {
            printf("ERROR in recvfrom\n");
        }
        else if (nBytes == 0) {
        	printf("Connection closed\n");
        	udpSocketData->isRunning = 0;
        }
        else {
        	//printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        	udpSocketData->callback(buffer, nBytes);
        }
    }
    pthread_exit(NULL);
}

int32_t sendUdpDataToClient(void *handle, void *client, char *buffer, int32_t bufLen) {
    int32_t addrSize                = 0;
    int32_t nBytes                  = 0;
    UDP_SERVER_SOCKET_T *udpSocketData      = handle;
    struct sockaddr_in *clientaddr  = client;

    if (NULL == buffer) {
        return 0;
    }

    addrSize = sizeof(struct sockaddr_in);
    nBytes = sendto(udpSocketData->fd, buffer, bufLen, 0, (const struct sockaddr *)clientaddr, addrSize);
    if (nBytes < 0) {
        printf("ERROR in sendto\n");
    }
    return nBytes;
}

int32_t isUdpServerRunning(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    UDP_SERVER_SOCKET_T *udpSocketData = handle;
    return udpSocketData->isRunning;
}

int32_t closeUdpServer(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    UDP_SERVER_SOCKET_T *udpSocketData = handle;
    if (udpSocketData->isRunning) {
        udpSocketData->isRunning = 0;
        pthread_join(udpSocketData->threadId, NULL);
        close(udpSocketData->fd);
        free(udpSocketData);
    }
    return 1;
}

