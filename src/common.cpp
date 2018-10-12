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
#include "../include/logger.h"

#define DNS_ADDR "8.8.8.8"
#define DNS_PORT 53
#define MAXHOST 255

using namespace std;

string GetIP() {
    char *ip;
    int fd;
    struct sockaddr_in servaddr;
    struct sockaddr_in localaddr;
    socklen_t len;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
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

    getsockname(fd, (struct sockaddr *) &localaddr, &l);
    ip = inet_ntoa(localaddr.sin_addr);
    close(fd);
    return string(ip);
};

string GetHostname() {
    char hostname[MAXHOST];
    int hostreturn = gethostname(hostname, sizeof(hostname));
    if (hostreturn == -1) {
        perror("get host name error");
    }
    return string(hostname);
};

int ConnectToHost(char *server_ip, int server_port) {
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdsocket < 0)
        perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if (connect(fdsocket, (struct sockaddr *) &remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}

void CommandFail(char *cmd) {
    cse4589_print_and_log("[%s:ERROR]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

string GetClientHostname(char *clientIp) {
    struct hostent *he;
    struct in_addr ipv4addr;
    inet_pton(AF_INET, clientIp, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
    return string(he->h_name);

}

