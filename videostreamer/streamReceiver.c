#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "assert.h"
#include "streamReceiver.h"


void *startReceiverThread(void *data);
    
void *startReceiver(RECEIVER_CONFIG_T *config) {
    int32_t err                     = 0; 
    RECEIVER_DATA_T *receiverData   = NULL; 

    assert(config);

    receiverData = (RECEIVER_DATA_T *)malloc(sizeof(RECEIVER_DATA_T));
    if (!receiverData) {
        return NULL;
    }
    memset(receiverData, 0, sizeof(RECEIVER_DATA_T));
    receiverData->port = config->port;
    err = pthread_create(&receiverData->threadId, NULL, startReceiverThread, (void *)receiverData);
    if(err) {
        printf("Thread creation failed\n");
    }
    return receiverData;
}

void *startReceiverThread(void *args) {
    int32_t nBytes              = 0;
    int32_t udpSocket           = 0;
    int32_t err                 = 0;
    char buffer[1024]           = {0};
    RECEIVER_DATA_T *receiverData   = args; 
     
    printf("Starting stream receiver\n");
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    
    /*Create UDP socket*/
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) {
        printf("ERROR opening socket\n");
        strncpy(receiverData->shortErrMsg, "ERROR opening socket", 511);
        pthread_exit(NULL);
    }

    /*Configure settings in address struct*/
    memset(&serverAddr, 0, sizeof(serverAddr));  
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(receiverData->port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*Bind socket with address struct*/
    err = bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(err < 0) {
        printf("ERROR Bind failed\n");
        strncpy(receiverData->shortErrMsg, "ERROR Bind failed", 511);
        pthread_exit(NULL);
    }
    printf("Bind pass\n");

    /*Initialize size variable to be used later on*/
    addr_size = sizeof(serverStorage);

    receiverData->running = 1;
    while(receiverData->running){
        printf("Waiting for messages\n");
        /* Try to receive any incoming UDP datagram. Address and port of 
        requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);

        /*Convert message received to uppercase*/
        // for(i=0;i<nBytes-1;i++)
        //   buffer[i] = toupper(buffer[i]);

        printf("Recieved buffer %d\n", nBytes);
        /*Send uppercase message back to client, using serverStorage as the address*/
        //sendto(udpSocket,buffer,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
    }
    pthread_exit(NULL);
}

int32_t isRunning(void *handle) {
    assert(handle);
    RECEIVER_DATA_T *data = handle; 
    
    return data->running;
}

