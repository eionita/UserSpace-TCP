#include <stdio.h>
//#include <ff_loop.h>
//#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "ff_api.h"
//#include "ff_config.h
#include <unistd.h>

#define MAX 80
#define PORT 9002  // metti la porta necesaria
#define SA struct linux_sockaddr

//chat between client and server
void func(int connfd)
{
    char buff[MAX];
    int n;

    // infinite loop for chat
    for (;;) {
        memset(buff, 0, MAX);

        // read the message from client and copy it in buffer
        ff_read(connfd, buff, MAX);
        printf("Client message: %s\n", buff);
        printf("Server message: ");
        memset(buff, 0, MAX);
        n = 0;

        // server message
        while ((buff[n++] = getchar()) != '\n') {
            ;
        }

        //send message to client
ff_write(connfd, buff, MAX);

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


int main1(void *argv) {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create
    sockfd = ff_socket(AF_INET, SOCK_STREAM, 0); //socket(prot ipv4, tcp, ?)
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    printf("Socket successfully created..\n");
    memset(&servaddr, 0,sizeof(servaddr));


    // assign IP, PORT of server
    servaddr.sin_family = AF_INET; 
   // #unsigned short port = htons(PORT);
   // #memcpy(&servaddr.sa_data[0], &port, sizeof(port));
   // #nsigned int ip = htonl(INADDR_ANY);
   // #memcpy(&servaddr.sa_data[2], &ip, sizeof(ip));
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Binding
    if (ff_bind(sockfd, (struct linux_sockaddr *)&servaddr, sizeof(servaddr)) != 0) { //
        printf("socket bind failed...\n");
        exit(0);
    }
    printf("Socket successfully binded..\n");
//server listen
    if (ff_listen(sockfd, 5) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client
    connfd = ff_accept(sockfd, (struct linux_sockaddr *)&cli, (socklen_t*)&len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    printf("server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    //close the socket
    ff_close(sockfd);
}

int main(int argc, char **argv) {
  printf("starting server\n");
  if(ff_init( argc, argv) != 0) {
    printf("F-Stack init failed\n");
    return -1;
  }
  ff_run(main1, NULL);
  return 0;
}
