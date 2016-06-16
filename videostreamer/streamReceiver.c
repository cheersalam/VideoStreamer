#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


void * startReceiver(void *args) {
    int32_t nBytes      = 0;
    int32_t udpSocket   = 0;
    int32_t err         = 0;
    int32_t port        = 43210;
    char buffer[1024]   = {0};
     
    printf("Starting stream receiver\n");
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    
    /*Create UDP socket*/
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) {
        printf("ERROR opening socket\n");
        return;
    }

    /*Configure settings in address struct*/
    memset(&serverAddr, 0, sizeof(serverAddr));  
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(43210);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*Bind socket with address struct*/
    err = bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(err < 0) {
        printf("Bind failed\n");
        return err;
    }
    printf("Bind pass\n");

  /*Initialize size variable to be used later on*/
  addr_size = sizeof(serverStorage);

  while(1){
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

}


