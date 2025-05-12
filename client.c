#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ff_api.h"
#include <arpa/inet.h>
//#include <ff_loop.h>  // for ff_run

#define MAX 80
#define PORT 9002
#define SA struct linux_sockaddr

void func(int sockfd) {
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, MAX);
        printf("Enter the string : ");
        n = 0;

        // Read input
        while ((buff[n++] = getchar()) != '\n') {}

        // Send to server
        ff_write(sockfd, buff, MAX);

        // Read response
        memset(buff, 0, MAX);
        ff_read(sockfd, buff, MAX);
        printf("From Server : %s", buff);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}
void my_client(void *arg) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = ff_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(1);
    }
    printf("Socket successfully created..\n");
    memset(&servaddr, 0, sizeof(servaddr));

    // Server IP and port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
   // #unsigned short server_port = htons(PORT);
    //#memcpy(&servaddr.sa_data[0], &server_port, sizeof(server_port));
    char server_ip[16] = "172.20.8.199";
    //#memcpy(&servaddr.sa_data[2], &server_ip, sizeof(server_ip));
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=inet_addr(server_ip);
    // Connect to server
    if (ff_connect(sockfd, (struct linux_sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(1);
    }
    printf("Connected to the server..\n");

    // Start chat
    func(sockfd);

    // Close socket
    ff_close(sockfd);
}

int main(int argc, char **argv) {
 if (ff_init(argc, argv) < 0) {
        printf("F-Stack init failed.\n");
        return -1;
    }

    ff_run(my_client, NULL);
    return 0;
}

