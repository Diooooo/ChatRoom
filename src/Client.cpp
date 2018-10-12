#include <iostream>
#include <string>
#include <map>

#include "../include/Client.h"
#include "../include/common.h"

using namespace std;

Client::Client(int portNumber) {
    port = portNumber;
    clientfd = 0;
    status = LOGOUT;
    //get hostname and ip address
    hostname = GetHostname();
    ip = GetIP();
    struct info self;
    self.hostname = (char *) hostname.data();
    self.ip = (char *) ip.data();
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
        cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, list[i].hostname, list[i].ip, list[i].port);
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Login(string ip, int serverPort) {
    if (clientfd == 0) {
        clientfd = ConnectToHost((char *) ip.data(), serverPort);
    } else {
        char *msg = "LOGIN:NULL";
        if (send(clientfd, msg, strlen(msg), 0)) {
            cout << "Update Client Status--login" << endl;
        }
    }
    while (1) {
        char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
        memset(buffer, '\0', BUFFER_SIZE);

        if (recv(clientfd, buffer, BUFFER_SIZE, 0) >= 0) {
            if (strstr(buffer, "List")) {
                //update list
            } else if (strstr(buffer, "Msg")) {
                //call Receive()
            } else if (strstr(buffer, "Done")) {
                break;
            } else {
                perror("Unexpected message");
                break;
            }
        }
    }
    char *cmd = "LOGIN";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);


}

void Client::Refresh() {
    char *msg = "REFRESH:NULL";
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Refresh online clients" << endl;
    }
    while (1) {
        char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
        memset(buffer, '\0', BUFFER_SIZE);

        if (recv(clientfd, buffer, BUFFER_SIZE, 0) >= 0) {
            if (strstr(buffer, "List")) {
                //update list
            } else if (strstr(buffer, "Done")) {
                break;
            } else {
                perror("Unexpected message");
                break;
            }
        }
    }
    char *cmd = "REFRESH";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Send(string ip, int clientPort) {

}

void Client::Boardcast(string msg) {

}

void Client::Block(string ip) {

}

void Client::Unblock(string ip) {

}

void Client::Logout() {
    char *msg = "LOGOUT:NULL";
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Update Client Status--logout" << endl;
    }
    char *cmd = "LOGOUT";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Exit() {
    if (status == LOGIN) {
        close(clientfd);
    }
    char *cmd = "EXIT";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
    exit(0);
}

void Client::Received(string ip, string msg) {
    char *cmd = "RECEIVED";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("msg from:%s\n[msg]:%s\n", ip.c_str(), msg.c_str());
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::SendFile(string ip, string filePath) {

}

void Client::Run() {
    while (1) {
        string cmdLine;
        getline(cin, cmdLine);
        if (cmdLine.size() == 0) {
            exit(1);
        } else {
            const char *sep = " ";
            char *p;
            p = strtok((char *) cmdLine.data(), sep);
            char *cmd = p;
            vector<char *> params;
            //read other params
            while (p) {
                params.push_back(p);
                p = strtok(NULL, sep);
            }
            if (status == LOGOUT) {
                if (strcmp(cmd, "AUTHOR") == 0) {
                    Author();
                } else if (strcmp(cmd, "IP") == 0) {
                    Ip();
                } else if (strcmp(cmd, "PORT") == 0) {
                    Port();
                } else if (strcmp(cmd, "LOGIN") == 0) {
                    if (params.size() <= 2) {
                        cout << "Error Input" << endl;
                    }
                    Login(string(params[1]), atoi(params[2]))
                } else if (strcmp(cmd, "EXIT") == 0) {
                    Exit();
                } else {
                    perror("Unexpected command");
                }
            } else {
                if (strcmp(cmd, "LIST") == 0) {
                    List();
                } else if (strcmp(cmd, "REFRESH") == 0) {
                    Refresh();
                } else if (strcmp(cmd, "SEND") == 0) {

                } else if (strcmp(cmd, "BOARDCAST") == 0) {

                } else if (strcmp(cmd, "BLOCK") == 0) {

                } else if (strcmp(cmd, "UNBLOCK") == 0) {

                } else if (strcmp(cmd, "LOGOUT") == 0) {

                } else {
                    perror("Unexpected command");
                }
            }
        }
    }
}
