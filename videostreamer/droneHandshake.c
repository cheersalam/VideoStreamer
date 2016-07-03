#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "droneHandshake.h"

typedef struct HANDSHAKE_DATA {
	char droneIpAdd[64];
	uint16_t port;
	int fd;
}HANDSHAKE_DATA;

void logErrorAndExit(const char *msg)
{
    perror(msg);
    exit(0);
}

int32_t startNetwork(const char *droneIpAdd, uint16_t port, int *droneCommandfd) {
	struct sockaddr_in serv_addr		= {0};
    struct hostent *server				= NULL;
	HANDSHAKE_DATA	*handshakeData = NULL;

	handshakeData = (HANDSHAKE_DATA *)malloc(sizeof(HANDSHAKE_DATA));
	if (NULL == handshakeData) {
		return NULL;
	}
	memset(handshakeData, 0, sizeof(HANDSHAKE_DATA));
	strncpy(handshakeData->droneIpAdd, droneCommandfd, 64);

	handshakeData->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (handshakeData->fd < 0) {
		printf("ERROR socket call failed\n");
		return NULL;
    }
    
    server = gethostbyname(handshakeData->droneIpAdd);
    if (server == NULL) {
		printf("ERROR, no such host\n");
		return NULL;
    }
    
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

    if (connect(*droneCommandfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		logErrorAndExit("ERROR connecting to Drone");
    }
    return 0;
}

void stopNetwork(int sockfd) {
	if (sockfd) {
		close(sockfd);
	}

}

int32_t startHandshake(int sockfd, const char *command, char *responseBuf, ssize_t responseBufLen) {
    ssize_t n = 0;
    if (command == NULL && responseBuf == NULL) {
        return -1;
    }

	printf("%s:%s:%d Sending command = %s\n", __FILE__, __func__, __LINE__, command);
    n = write(sockfd, command, strlen(command));
    if (n < 0) { 
		logErrorAndExit("ERROR writing to socket");
    }
    
    n = read(sockfd, &responseBuf[0], responseBufLen);
    if (n < 0) {
		logErrorAndExit("ERROR reading from socket");
    }
    printf("%s:%s:%d Response received = %s\n", __FILE__, __func__, __LINE__, responseBuf);
    return 0;
}
