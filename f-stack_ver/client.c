#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

#include "ff_config.h"
#include "ff_api.h"
#include "ff_epoll.h"

#define MAX_EVENTS 512
#define DEFAULT_PORT 9002
#define BUF_SIZE 512

int epfd;
int sockfd;
int connected = 0;
int ping_count = 0;
int max_pings = 10;
int running;

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
    printf("\n[Client] Stopping...\n");
    if (sockfd >= 0) ff_close(sockfd);
    if (epfd >= 0) ff_close(epfd);
    exit(0);
}

/* Loop principale per ricezione messaggi */
int loop(void *arg) {
    struct epoll_event events[MAX_EVENTS];
    int nevents= ff_epoll_wait(epfd, events, MAX_EVENTS, 0);
   /* printf("nevents = %d", nevents);*/
    if (nevents < 0) return 0;
for (int i = 0; i < nevents; i++) {
            if (events[i].events & EPOLLOUT && !connected) {
            connected = 1;
            printf("Connected to server\n");

            // Send first PING  
            const char *msg = "PING\n";
            ff_write(sockfd, msg, strlen(msg));
            printf("Sent PING #%d\n", ++ping_count);

            // Switch to monitoring reads  
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sockfd;
            ff_epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);

        } else if (events[i].events & EPOLLIN) {
            char buf[256];
            ssize_t readlen = ff_read(sockfd, buf, sizeof(buf));
            if (readlen > 0) {
                buf[readlen] = '\0';
                printf("Received: %s", buf);

                // Send another PING if we haven't reached max  
                if (ping_count < max_pings) {
                    sleep(1);  // Wait 1 second between pings  
                    const char *msg = "PING\n";
                    ff_write(sockfd, msg, strlen(msg));
                    printf("Sent PING #%d\n", ++ping_count);
                } else {
                    printf("Completed %d ping-pongs. Closing.\n", max_pings);
                    ff_close(sockfd);
                    return -1;  // Exit loop  
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    ff_init(argc, argv);

    epfd = ff_epoll_create(0);
    sockfd = ff_socket(AF_INET, SOCK_STREAM, 0);

    int on = 1;
    ff_ioctl(sockfd, FIONBIO, &on);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    // inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);  

    ff_connect(sockfd, (struct linux_sockaddr *)&addr, sizeof(addr));

    // Monitor for connection completion (EPOLLOUT)  
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = sockfd;
    if (ff_epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == 0) {
        printf("client connection done");
    } else {
       printf("connection failed %d: %s", errno, strerror(errno));
    }
    ff_run(loop, NULL);
    return 0;
}
                          
