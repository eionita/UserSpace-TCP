#include <arpa/inet.h> // inet_addr()
#include <errno.h>
#include <netdb.h> //?? lo posso togliere?
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 9002
#define SA struct sockaddr
void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        memset(buff, 0, MAX);
        printf("Enter the string : ");
        n = 0;

        //write message
        while ((buff[n++] = getchar()) != '\n') {
            ;
        }
        write(sockfd, buff, MAX);
        memset(buff, 0, MAX);
        read(sockfd, buff, MAX);
        printf("From Server : %s", buff);
        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    printf("Socket successfully created..\n");
    memset(&servaddr, 0,sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("172.20.8.199");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        printf("connection with the server failed: [%d] %s\n", errno, strerror(errno));
        exit(0);
    }
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}

