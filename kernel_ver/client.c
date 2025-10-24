#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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
        write(sockfd, buff, MAX);

        // Read response
        memset(buff, 0, MAX);
        read(sockfd, buff, MAX);
        printf("From Server : %s", buff);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}
void my_client(const char *server_ip) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(1);
    }
    printf("Socket successfully created..\n");

    // Server IP and port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port=htons(PORT);
    if(inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0){
        printf("Invalid address/ Address not supported\n");
        exit(1);
    }
    // Connect to server
    if (connect(sockfd, (struct linux_sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(1);
    }
    printf("Connected to the server..\n");

    // Start chat
    func(sockfd);

    // Close socket
    close(sockfd);
}

int main(int argc, char **argv) {
    if(argc < 2){
        printf("Uso: %s <IP_DEL_SERVER>\n", argv[0]);
        return 1;
    }
    const char *server_ip = argv[1];
    my_client(server_ip);
    return 0;
}

