#include <iostream>
#include <string>
#include <map>

#include "../include/Client.h"
#include "../include/common.h"

using namespace std;

Client::Client(int portNumber) {
    port = portNumber;
    clientfd = 0;
    status = OFFLINE;
    //get hostname and ip address
    hostname = GetHostname();
    cout << hostname << endl;

    ip = GetIP();
    bzero(&client_addr, sizeof(client_addr));
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
    if (status == OFFLINE) {
        ConnectToHost((char *) ip.data(), serverPort);
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
            const char *sep = ":,";
            char *p;
            p = strtok(buffer, sep);
            char *sign = p;
            vector<char *> params;
            //read other params
            while (p) {
                params.push_back(p);
                p = strtok(NULL, sep);
            }
            if (strcmp(sign, "List") == 0) {
                if (params.size() >= 4) {
                    struct info onlineClient;
                    onlineClient.hostname = params[1];
                    onlineClient.ip = params[2];
                    onlineClient.port = atoi(params[3]);
                    list.push_back(onlineClient);
                }

            } else if (strcmp(sign, "Msg") == 0) {
                //call Receive()
                if (params.size() >= 3) {
                    Received(string(params[1]), string(params[2]));
                }
            } else if (strcmp(sign, "done") == 0) {
                break;
            } else {
                perror("Unexpected message");
                break;
            }
        }
    }
    status = LOGIN;
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
                list.clear();

                const char *sep = ":,";
                char *p;
                p = strtok(buffer, sep);
                char *sign = p;
                vector<char *> params;
                //read other params
                while (p) {
                    params.push_back(p);
                    p = strtok(NULL, sep);
                }
                if (params.size() >= 4) {
                    struct info onlineClient;
                    onlineClient.hostname = params[1];
                    onlineClient.ip = params[2];
                    onlineClient.port = atoi(params[3]);
                    list.push_back(onlineClient);
                }
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

void Client::Send(string ip, int clientPort, char *message) {
    char msg[255];
    strcpy(msg, "SEND:");
    strcat(msg, (char *) ip.data());
    strcat(msg, ",");
    char portString[10];
    sprintf(portString, "%d", clientPort);
    strcat(msg, portString);
    strcat(msg, ",");
    strcat(msg, message);
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Send to " << ip << endl;
    }
    char *cmd = "SEND";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Boardcast(string message) {
    char *msg = "BOARDCAST:";
    strcat(msg, (char *) message.data());
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Boardcast to all" << endl;
    }
    char *cmd = "BOARDCAST";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Block(string ip) {
    char *msg = "BLOCK:";
    strcat(msg, (char *) ip.data());
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Block ip: " << ip << endl;
    }
    char *cmd = "BLOCK";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Unblock(string ip) {
    char *msg = "UNBLOCK:";
    strcat(msg, (char *) ip.data());
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Unblock ip: " << ip << endl;
    }
    char *cmd = "UNBLOCK";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Logout() {
    char *msg = "LOGOUT:NULL";
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Update Client Status--logout" << endl;
    }
    status = LOGOUT;
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


int Client::ConnectToHost(char *server_ip, int server_port) {
    struct sockaddr_in remote_server_addr;

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if (connect(clientfd, (struct sockaddr *) &remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return clientfd;
}

void Client::Run() {
    int head_socket, selret, sock_index = 0;
    fd_set master_list, watch_list;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0)
        perror("Create socket failed!");

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr((char *) ip.data());
    client_addr.sin_port = htons(port);
    if (bind(clientfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        printf("Bind failed!");
        exit(0);
    }

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    FD_SET(clientfd, &master_list);

    FD_SET(STDIN, &master_list);

    head_socket = clientfd;
    while (1) {
        memcpy(&watch_list, &master_list, sizeof(master_list));

        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if (selret < 0)
            perror("Select error!");

        if (selret > 0) {
            for (sock_index = 0; sock_index <= head_socket; sock_index++) {

                if (FD_ISSET(sock_index, &watch_list)) {
                    if (sock_index == 0) {
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
                            switch (status) {
                                case OFFLINE:
                                case LOGIN:
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
                                        Login(string(params[1]), atoi(params[2]));

                                    } else if (strcmp(cmd, "EXIT") == 0) {
                                        Exit();
                                    } else {
                                        perror("Unexpected command");
                                    }
                                    break;
                                case LOGOUT:
                                    if (strcmp(cmd, "AUTHOR") == 0) {
                                        Author();
                                    } else if (strcmp(cmd, "IP") == 0) {
                                        Ip();
                                    } else if (strcmp(cmd, "PORT") == 0) {
                                        Port();
                                    } else if (strcmp(cmd, "EXIT") == 0) {
                                        Exit();
                                    } else if (strcmp(cmd, "LIST") == 0) {
                                        List();
                                    } else if (strcmp(cmd, "REFRESH") == 0) {
                                        Refresh();
                                    } else if (strcmp(cmd, "SEND") == 0) {
                                        if (params.size() <= 3) {
                                            perror("Error Input");
                                        }
                                        Send(string(params[1]), atoi(params[2]), params[3]);
                                    } else if (strcmp(cmd, "BOARDCAST") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }
                                        Boardcast(string(params[1]));
                                    } else if (strcmp(cmd, "BLOCK") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }
                                        Block(string(params[1]));
                                    } else if (strcmp(cmd, "UNBLOCK") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }
                                        Unblock(string(params[1]));
                                    } else if (strcmp(cmd, "LOGOUT") == 0) {
                                        Logout();
                                    } else {
                                        perror("Unexpected command");
                                    }
                                    break;
                            }
                        }
                    } else if (sock_index == clientfd) { //new client login
                        //receive
                        char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if (recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0) {
                            perror("Impossible");
                        } else {
                            const char *sep = ":,";
                            char *p;
                            p = strtok(buffer, sep);
                            char *sign = p;
                            vector<char *> params;
                            //read other params
                            while (p) {
                                params.push_back(p);
                                p = strtok(NULL, sep);
                            }

                            if (strcmp(sign, "Send") == 0) {
                                if (params.size() >= 3) {
                                    Received(string(params[1]), string(params[2]));
                                }
                            }
                        }
                    }
                }

            }


        }

    }
}

