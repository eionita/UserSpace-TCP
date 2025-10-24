#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <assert.h>
#include "ff_config.h"
#include "ff_api.h"
#include "ff_epoll.h"

#define MAX_EVENTS 512
#define DEFAULT_PORT 9002
#define BUF_SIZE 512

static int epfd = -1;  //file descriptor del poller
static int listenfd = -1;
static int clientfd = -1;
static volatile int running = 1;

/* ciclo principale del server */
int loop_server(void *arg){
    struct epoll_event ev, events[8];
    int nevents = ff_epoll_wait(epfd, events, 8, 0);
    if (nevents < 0) {
        printf("ff_epoll_wait failed: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }
        int i;
    for (i = 0; i < nevents; ++i) {
        int fd = events[i].data.fd;
        printf("fd = %d, listenfd = %d\n ", fd, listenfd);
        /* Nuova connessione */
        if (fd == listenfd) {
                int nclientfd = ff_accept(listenfd, NULL, NULL);
                if (nclientfd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    printf("ff_accept failed: errno=%d (%s)\n", errno, strerror(errno));
                    break;
                                             }
                printf("server accept done");

                /* aggiungi alla lista epoll */
                printf("[Server] Client connected!\n");
                clientfd = nclientfd;
                ev.data.fd = nclientfd;
                ev.events = EPOLLIN | EPOLLHUP;
                if (ff_epoll_ctl(epfd, EPOLL_CTL_ADD, nclientfd, &ev) != 0) {   //registra il nuovo client in epoll per ricevere notifiche
                    printf("ff_epoll_ctl ADD failed: %d (%s)\n", errno, strerror(errno));
                    ff_close(nclientfd);
                } else {
                    printf("Accepted new connection (fd=%d)\n", nclientfd);
                }
         }


        /* Lettura e risposta*/
        if (events[i].events & EPOLLIN) {
            // Read from client  
            char buf[256];
            ssize_t readlen = ff_read(fd, buf, sizeof(buf));
            if (readlen > 0) {
                buf[readlen] = '\0';
                printf("Received: %s", buf);

                // Send "PONG" response  
                const char *response = "PONG\n";
                ff_write(fd, response, strlen(response));
                printf("Sent: PONG\n");
            } else {
                printf("Client disconnected\n");
                ff_epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                ff_close(fd);
            }
        }
    }

    return 0;
}

int main(int argc, char **argv){
if (ff_init(argc, argv) < 0) {
        fprintf(stderr, "ff_init failed\n");
        return 1;
    }

    /* crea socket di ascolto */
    listenfd = ff_socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        fprintf(stderr, "ff_socket failed\n");
        return 1;
    }

    /* imposta non-blocking */
    int on = 1;
    ff_ioctl(listenfd, FIONBIO, &on);

    /* prepara indirizzo di ascolto */
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = ff_bind(listenfd, (struct linux_sockaddr *)&addr, sizeof(addr));
    if (ret < 0){
        fprintf(stderr, "ff_bind failed: %d (%s)\n", errno, strerror(errno));
        return 1;
    }

    ret = ff_listen(listenfd, 5);
    if(ret < 0) {
        fprintf(stderr, "ff_listen failed: %d (%s)\n", errno, strerror(errno));
        return 1;
    }
    struct epoll_event ev;
    /* crea epoll e registra listenfd */
    assert((epfd = ff_epoll_create(0)) > 0);
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    ret = ff_epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    if(ret < 0){
        printf("ff_listen failed");
    }
    /* avvia loop F-Stack */
ff_run(loop_server, NULL);

    return 0;
}


