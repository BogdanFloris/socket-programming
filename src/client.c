/*
** client.c -- a stream socket client demo
*/
#include "get_in_addr.h"

#include "unistd.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "3049"
#define MAX_SIZE 100

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAX_SIZE];
    struct addrinfo hints, *serv_info, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostnamen\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &serv_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = serv_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: socket");
            continue;
        }

        break;
    }

    freeaddrinfo(serv_info);

    if (p == NULL) {
        fprintf(stderr, "client failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)&p->ai_addr), s,
              sizeof s);
    printf("client: connecting to %s\n", s);

    if ((numbytes = recv(sockfd, buf, MAX_SIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("client: received %s\n", buf);

    close(sockfd);
    return 0;
}
