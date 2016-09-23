/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include "tcpClientSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_RECV_BUF_LEN (1024 * 1024)

typedef struct TCP_SOCKET_T {
    uint16_t port;
    int32_t fd;
    struct sockaddr_in serveraddr;
    pthread_t   threadId;
    int32_t isRunning;
    RECEIVER_CB callback;
}TCP_SOCKET_T;

static void *clientThread(void *data);

void *initTcpClientSocket(uint16_t port, char *hostname, RECEIVER_CB callback) {
    int32_t err = 0;
    TCP_SOCKET_T *tcpSocketData = NULL;
    struct hostent *server;

    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__ );
    assert(hostname);
    tcpSocketData = (TCP_SOCKET_T*)malloc(sizeof(TCP_SOCKET_T));
    if (NULL == tcpSocketData) {
        printf("ERROR malloc error\n");
        return NULL;
    }
    memset(tcpSocketData, 0, sizeof(TCP_SOCKET_T));
    tcpSocketData->port = port;
    tcpSocketData->callback = callback;

    tcpSocketData->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocketData->fd < 0) {
        printf("ERROR opening socket\n");
        free(tcpSocketData);
        return NULL;
    }

    server = gethostbyname(hostname);
    if (NULL == server) {
        printf("ERROR, no such host as %s\n", hostname);
        free(tcpSocketData);
        return NULL;
    }

    memset(&tcpSocketData->serveraddr, 0, sizeof(tcpSocketData->serveraddr));
    tcpSocketData->serveraddr.sin_family = AF_INET;
    tcpSocketData->serveraddr.sin_port = htons(port);
    tcpSocketData->serveraddr.sin_addr.s_addr = inet_addr(hostname);
    //memcpy((char *)&tcpSocketData->serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    if (connect(tcpSocketData->fd, (struct sockaddr *)&tcpSocketData->serveraddr, sizeof(tcpSocketData->serveraddr)) < 0) {
        printf("Connect failed for %s\n", hostname);
        free(tcpSocketData);
        return NULL;
    }

    if(tcpSocketData->callback) {
    	err = pthread_create(&tcpSocketData->threadId, NULL, clientThread, (void *)tcpSocketData);
    	if (err) {
    		printf("ERROR thread creation failed\n");
    		close(tcpSocketData->fd);
    		free(tcpSocketData);
    		return NULL;
    	}
    }
    return tcpSocketData;
}

void *clientThread(void *args) {
    TCP_SOCKET_T *tcpSocketData = args;
    int32_t nBytes = 0;
    unsigned char buffer[MAX_RECV_BUF_LEN];

    printf("%s:%s:%d TCP clientThread started\n", __FILE__, __func__, __LINE__ );
    tcpSocketData->isRunning = 1;
    while (tcpSocketData->isRunning) {
        nBytes = recv(tcpSocketData->fd, buffer, MAX_RECV_BUF_LEN, 0);
        if (nBytes < 0) {
            printf("ERROR in read\n");
        }
        else if (nBytes == 0) {
            printf("%s:%s:%d Socket closed\n", __FILE__, __func__, __LINE__);
            tcpSocketData->isRunning = 0;
        }
        else {
        	tcpSocketData->callback(buffer, nBytes);
        	printf("%s:%s:%d TCP clientThread Bytes Received = %d\n", __FILE__, __func__, __LINE__, nBytes );
        }
    }
    pthread_exit(NULL);
}

int32_t sendDataToTcpServer(void *handle, char *buffer, int32_t bufLen) {
    int32_t nBytes;
    TCP_SOCKET_T *tcpSocketData = handle;

    if (NULL == buffer) {
        return -1;
    }

    nBytes = write(tcpSocketData->fd, buffer, bufLen);
    if (nBytes < 0) {
        printf("ERROR in sendto\n");
        return -1;
    }
    return nBytes;
}

int32_t sendSyncDataToTcpServer(void *handle, char *inBuf, int32_t inBufLen, char *outBuf, int32_t *outBufLen ) {
    int32_t nBytes;
    TCP_SOCKET_T *tcpSocketData = handle;

    assert(inBuf && outBuf);

    nBytes = write(tcpSocketData->fd, inBuf, inBufLen);
    if (nBytes < 0) {
        printf("ERROR in sendto\n");
        return -1;
    }
    
    nBytes = recv(tcpSocketData->fd, outBuf, MAX_RECV_BUF_LEN, 0);
    if (nBytes < 0) {
        printf("ERROR in read\n");
        return -1;
    }
    printf("Bytes Received = %s\n", outBuf);
    *outBufLen = nBytes;
    return nBytes;
}

int32_t isTcpClientRunning(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    TCP_SOCKET_T *tcpSocketData = handle;
    return tcpSocketData->isRunning;
}

int32_t closeTcpClient(void *handle) {
    if (NULL == handle) {
        return 0;
    }
    TCP_SOCKET_T *tcpSocketData = handle;
    if (tcpSocketData->isRunning) {
        tcpSocketData->isRunning = 0;
        pthread_join(tcpSocketData->threadId, NULL);
        close(tcpSocketData->fd);
        free(tcpSocketData);
    }
    return 1;
}

