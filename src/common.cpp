//
// Created by bilin on 10/11/18.
//
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/common.h"

#define DNS_ADDR "8.8.8.8"
#define DNS_PORT 11451
#define MAXHOST 255

using namespace std;

char *GetIP() {
    char *ip;
    int fd;
    struct sockaddr_in servaddr;
    struct sockaddr_in localaddr;
    socklen_t len;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("Failed to create socket");
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    char *servIP = DNS_ADDR;
    inet_pton(AF_INET, servIP, &servaddr.sin_addr);
    if (connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
    }
    socklen_t l = INET_ADDRSTRLEN;

    getsockname(fd, (struct sockaddr*)&localaddr, &l);
    ip = inet_ntoa(localaddr.sin_addr);
    close(fd);
    return ip;
};

char *GetHostname() {
    char hostname[MAXHOST];
    gethostname(hostname, sizeof(hostname));
    return hostname;
};