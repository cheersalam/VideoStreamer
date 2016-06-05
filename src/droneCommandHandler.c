#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "droneCommandHandler.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int32_t startNetwork(char *droneIpAdd) {
    struct sockaddr_in  serv_addr;
    struct hostent      *server;
    int32_t             sockfd;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
        return -1;
    }
    
    server = gethostbyname(droneIpAdd);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        error("ERROR connecting");
    }
    return 0;
}

int32_t sendCommand(int32_t sockfd, char *command, char **response) {
    char buffer[256];
    int32_t n = 0;
    if (command == NULL && *response == NULL) {
        return -1;
    }
    bzero(buffer, 256);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) { 
         error("ERROR writing to socket");
    }
    
    n = read(sockfd,buffer,255);
    if (n < 0) {
         error("ERROR reading from socket");
    }
    return 0;
}
