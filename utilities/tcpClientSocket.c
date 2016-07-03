#include "udpCLientSocket.h"
#include <pthread.h>

#define MAX_RECV_BUF_LEN (1024 * 1024)

typedef struct TCP_SOCKET_T {
	uint16_t port;
	int32_t fd;
	struct sockaddr_in serveraddr;
	pthread_t   threadId;
	int32_t isRunning;
}TCP_SOCKET_T;

void *clientThread(void *data);

void *initTcpClientSocket(uint16_t port, char *hostname) {
	int32_t err = 0;
	TCP_SOCKET_T *tcpSocketData = NULL;
	struct hostent *server;

	assert(hostname);
	tcpSocketData = (TCP_SOCKET_T*)malloc(sizeof(TCP_SOCKET_T));
	if (NULL == tcpSocketData) {
		printf("ERROR malloc error\n");
		return NULL;
	}
	memset(tcpSocketData, 0, sizeof(TCP_SOCKET_T));
	tcpSocketData->port = port;

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

	memset(tcpSocketData->serveraddr, 0, sizeof(tcpSocketData->serveraddr));
	tcpSocketData->serveraddr->sin_family = AF_INET;
	tcpSocketData->serveraddr->sin_port = htons(port);
	memcpy((char *)server->h_addr, (char *)&tcpSocketData->serveraddr.sin_addr.s_addr, server->h_length);

	if (connect(tcpSocketData->fd, &tcpSocketData->serveraddr, sizeof(tcpSocketData->serveraddr)) < 0) {
		printf("ERROR connecting");
		free(tcpSocketData);
		return NULL;
	}

	err = pthread_create(&tcpSocketData->threadId, NULL, clientThread, (void *)tcpSocketData);
	if (err) {
		printf("ERROR thread creation failed\n");
		close(tcpSocketData->fd);
		free(tcpSocketData);
	}
	return tcpSocketData;
}

void *clientThread(void *args) {
	TCP_SOCKET_T *udpSocketData = args;
	int32_t nBytes = 0;
	int32_t addrSize = 0;
	unsigned char buffer[MAX_RECV_BUF_LEN];

	addrSize = sizeof(udpSocketData->serveraddr);
	udpSocketData->isRunning = 1;
	while (udpSocketData->isRunning) {
		nBytes = read(udpSocketData->fd, buffer, MAX_RECV_BUF_LEN);
		if (nBytes < 0) {
			printf("ERROR in read\n");
		}
		printf("Bytes Received = %s\n", nBytes);
	}
	pthread_exit(NULL);
}

int32_t sendDataToTcpServer(void *handle, char *buffer, int32_t bufLen) {
	int32_t addrSize = 0;
	int32_t nBytes;
	TCP_SOCKET_T *udpSocketData = handle;

	if (NULL == buffer) {
		return 0;
	}

	addrSize = sizeof(udpSocketData->serveraddr);
	nBytes = write(udpSocketData->fd, buffer, bufLen);
	if (nBytes < 0) {
		printf("ERROR in sendto\n");
	}
	return nBytes;
}

int32_t isTcpClientRunning(void *handle) {
	if (NULL == handle) {
		return 0;
	}
	TCP_SOCKET_T *udpSocketData = handle;
	return udpSocketData->isRunning;
}

int32_t closeTcpClient(void *handle) {
	if (NULL == handle) {
		return 0;
	}
	TCP_SOCKET_T *udpSocketData = handle;
	if (udpSocketData->isRunning) {
		udpSocketData->isRunning = 0;
		pthread_join(udpSocketData->threadId, NULL);
		close(udpSocketData->fd);
		free(udpSocketData);
	}
	return 1;
}

