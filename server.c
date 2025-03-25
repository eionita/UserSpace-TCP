#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 80
#define PORT 9002  // metti la porta necesaria
#define SA struct sockaddr

//chat between client and server
void func(int connfd)
{
    char buff[MAX];
    int n;

    // infinite loop for chat
    for (;;) {
        memset(buff, 0, MAX);

        // read the message from client and copy it in buffer
        read(connfd, buff, MAX);
        printf("Client message: %s\n", buff);
        printf("Server message: ");
        memset(buff, 0, MAX);
        n = 0;

        // server message
        while ((buff[n++] = getchar()) != '\n') {
            ;
        }

        //send message to client
        write(connfd, buff, MAX);

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //socket(prot ipv4, tcp, ?)
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    printf("Socket successfully created..\n");
    memset(&servaddr, 0,sizeof(servaddr));


    // assign IP, PORT of server
    servaddr.sin_family = AF_INET; //
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { //
        printf("socket bind failed...\n");
        exit(0);
    }
    printf("Socket successfully binded..\n");

    //server listen
    if (listen(sockfd, 5) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client
    connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    printf("server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    //close the socket
    close(sockfd);
    return 0;
}
