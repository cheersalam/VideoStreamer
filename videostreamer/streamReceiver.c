#include <stdio.h>

int32_t initReceiver(int32_t port) {
    pthread_t *thread
    int32_t nBytes = 0, i = 0;
    int32_t err    = 0;
    char buffer[1024] = {0};
     
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    
    /*Create UDP socket*/
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    /*Bind socket with address struct*/
    err = bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(!err) {
        printf("Bind failed\n");
        return err;
    }

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;

  while(1){
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


