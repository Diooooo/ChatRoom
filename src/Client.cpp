#include <iostream>
#include <string>
#include <map>

#include "../include/Client.h"

using namespace std;

Client::Client(int portNumber) {
    port = portNumber;

    //get hostname and ip address
    ip = string(GetIP());
    struct info self;
    self.hostname = hostname.c_str();
    self.ip = ip.c_str();
    self.port = port;
    list.push_back(self);
}

void Client::Author() {
    char *cmd = "AUTHOR";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "bilinshi");
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Ip() {
    char *cmd = "IP";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("IP:%s\n", ip.c_str());
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Port() {
    char *cmd = "PORT";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("PORT:%d\n", port);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::List() {
    sort(list.begin(), list.end());
    int length = list.size();
    char *cmd = "LIST";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    for (int i = 0; i < length; i++) {
        cse4589_print_and_log(("%-5d%-35s%-20s%-8d\n", i + 1, list[i].hostname, list[i].ip, list[i].port));
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Login(string ip, int port) {

}

void Client::Refresh() {

}

void Client::Send(string ip, int port) {

}

void Client::Boardcast(string msg) {

}

void Client::Block(string ip) {

}

void Client::Unblock(string ip) {

}

void Client::Logout() {

}

void Client::Exit() {

}

void Client::Received() {

}

void Client::SendFile(string ip, string filePath) {

}

void Client::Run() {
    while (1) {
        cout <<
             char * cmd = (char *) malloc(sizeof(char) * MAXCMD);

        memset(cmd, '\0', MAXCMD);
        if (fgets(cmd, MAXCMD - 1, stdin) == NULL) {
            exit(-1);
        } else {
            int len = strlen(cmd);
            if (cmd[len - 1] == '\n') {
                cmd[len - 1] = '\0';
            }
            if (strcmp(cmd, "AUTHOR") == 0) {
                Author();
            } else if (strcmp(cmd, "IP") == 0) {
                Ip();
            } else if (strcmp(cmd, "PORT") == 0) {
                Port();
            } else if (strcmp(cmd, "LIST") == 0) {
                List();
            }
        }
    }
    int sockfd, n, rec_len;
    char recvline[4096], sendline[4096];
    char buf[MAXLINE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", "127.0.0.1");
        exit(0);
    }
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    while (1) {
        memset(buf, 0, sizeof(buf));
        memset(sendline, 0, sizeof(sendline));
        printf("Send massage to server: ");
        fgets(sendline, 4096, stdin);
        //send
        send(sockfd, sendline, strlen(sendline), 0);
        if (strcmp(sendline, "bye") == 0) {
            printf("Exit\n");
            break;
        }
        //receive
        printf("Waiting server send message now...\n");
        recv(sockfd, buf, MAXLINE, 0);
        printf("Received message: %s", buf);
    }
    close(sockfd);
    exit(0);
}
