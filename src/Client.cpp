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
        cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, (char *) list[i].hostname.data(),
                              (char *) list[i].ip.data(),
                              list[i].port);
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Login(string ip, int serverPort) {
    if (status == OFFLINE) {
        clientfd = ConnectToHost((char *) ip.data(), serverPort);
    } else {
        char *msg = "LOGIN:NULL";
        if (send(clientfd, msg, strlen(msg), 0)) {
            cout << "Update Client Status--login" << endl;
            list.clear();
        }
    }

    char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    memset(buffer, '\0', BUFFER_SIZE);

    if (recv(clientfd, buffer, BUFFER_SIZE, 0) >= 0) {
        cout << "Buffer:" << buffer << endl;

        vector<char *> params = Split(buffer, "\n");
        for (int i = 0; i < params.size(); i++) {
            if (strcmp(params[i], "List") == 0 || strcmp(params[i], "ListEnd") == 0 || strcmp(params[i], "Msg") == 0 ||
                strcmp(params[i], "MsgEnd") == 0 || strcmp(params[i], "Done") == 0) {
                continue;
            } else {
                if (strstr(params[i], "List")) {
                    vector<char *> listParams = Split(params[i], ",");

                    struct info onlineClient;
                    onlineClient.hostname = string(listParams[1]);
                    onlineClient.ip = string(listParams[2]);
                    onlineClient.port = atoi(listParams[3]);
                    list.push_back(onlineClient);

                }
                if (strstr(params[i], "Msg")) {
                    vector<char *> msgParams = Split(params[i], ",");
                    Received(string(msgParams[1]), string(msgParams[2]));
                }
            }
        }
//        if (strcmp(sign, "List") == 0) {
//
//            cout << "receive list" << endl;
//
//            if (params.size() >= 2) {
//                for (int i = 1; i < params.size() - 1; i++) {
//                    char *p1 = strtok(params[i], ",");
//                    char *p2 = strtok(NULL, ",");
//                    char *p3 = strtok(NULL, ",");
//                    struct info onlineClient;
//                    onlineClient.hostname = p1;
//                    onlineClient.ip = p2;
//                    onlineClient.port = atoi(p3);
//                    list.push_back(onlineClient);
//                }
//            }
//
//        } else if (strcmp(sign, "Msg") == 0) {
//
//            cout << "receive message" << endl;
//
//            //call Receive()
//            if (params.size() >= 2) {
//                for (int i = 1; i < params.size() - 1; i++) {
//                    char *p1 = strtok(params[i], ",");
//                    char *p2 = strtok(NULL, ",");
//                    Received(string(p1), string(p2));
//                }
//            }
//
//        } else if (strcmp(sign, "Done") == 0) {
//
//            break;
//        } else {
//            perror("Unexpected message");
//            break;
//        }
        fflush(stdout);
    }
//    fflush(stdout);


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
    char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    memset(buffer, '\0', BUFFER_SIZE);

    if (recv(clientfd, buffer, BUFFER_SIZE, 0) >= 0) {
        cout << "Buffer:" << buffer << endl;
        list.clear();
        vector<char *> params = Split(buffer, "\n");
        for (int i = 0; i < params.size(); i++) {
            if (strcmp(params[i], "List") == 0 || strcmp(params[i], "ListEnd") == 0) {
                continue;
            } else {
                if (strstr(params[i], "List")) {
                    vector<char *> listParams = Split(params[i], ",");

                    struct info onlineClient;
                    onlineClient.hostname = string(listParams[1]);
                    onlineClient.ip = string(listParams[2]);
                    onlineClient.port = atoi(listParams[3]);
                    list.push_back(onlineClient);
                }

            }
        }
        fflush(stdout);
    }
    char *cmd = "REFRESH";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Send(string ip, char *message) {
    char msg[BUFFER_SIZE];
    strcpy(msg, "SEND:");
    strcat(msg, (char *) ip.data());
    strcat(msg, ",");
    strcat(msg, message);
    SendData(clientfd, msg);
//    if (send(clientfd, msg, strlen(msg), 0)) {
//        cout << "Send to " << ip << endl;
//    }
    char *cmd = "SEND";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Broadcast(string message) {
    char msg[BUFFER_SIZE];
    strcpy(msg, "BROADCAST:");
    strcat(msg, (char *) message.data());
    cout << "MSG : " << msg << endl;
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Boardcast to all" << endl;
    }
    char *cmd = "BROADCAST";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Block(string ip) {
    char msg[256];
    strcpy(msg, "BLOCK:");
    strcat(msg, (char *) ip.data());
    if (send(clientfd, msg, strlen(msg), 0)) {
        cout << "Block ip: " << ip << endl;
    }
    char *cmd = "BLOCK";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Client::Unblock(string ip) {
    char msg[256];
    strcpy(msg, "UNBLOCK:");
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
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdsocket < 0)
        perror("Failed to create socket");

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr.s_addr = inet_addr((char *) ip.data());

    if (bind(fdsocket, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        perror("Bind failed!");
    }

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if (connect(fdsocket, (struct sockaddr *) &remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}

void Client::Run() {
    int head_socket, selret, sock_index = 0;
    fd_set master_list, watch_list;

//    clientfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (clientfd < 0)
//        perror("Create socket failed!");
//
//
//    cout << "Clientfd:" << clientfd << endl;
//
//    client_addr.sin_family = AF_INET;
//    client_addr.sin_addr.s_addr = inet_addr((char *) ip.data());
//    client_addr.sin_port = htons(port);
//    if (bind(clientfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
//        printf("Bind failed!");
//        exit(0);
//    }

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);


    //FD_SET(clientfd, &master_list);

    FD_SET(STDIN, &master_list);

    head_socket = 0;

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

                                case LOGOUT:

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
                                        if (ValidIp(string(params[1])) && ValidPort(string(params[2]))) {
                                            Login(string(params[1]), atoi(params[2]));

                                            cout << "status" << status << endl;
                                            FD_SET(clientfd, &master_list);
                                            head_socket = clientfd;
                                        } else {
                                            CommandFail(cmd);
                                        }

                                    } else if (strcmp(cmd, "EXIT") == 0) {
                                        Exit();
                                    } else {
                                        perror("Unexpected command");
                                    }
                                    break;

                                case LOGIN:

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
                                        if (params.size() <= 2) {
                                            perror("Error Input");
                                        }
                                        bool beSend = ValidIp(string(params[1]));
                                        if (beSend) {
                                            beSend = false;
                                            for (int i = 0; i < list.size(); i++) {
                                                if (strcmp(params[1], (char *) list[i].ip.data()) == 0) {
                                                    beSend = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (beSend) {
                                            string message;
                                            for (int index = 2; index < params.size(); index++) {
                                                message += string(params[index]);
                                                if (index != params.size() - 1) {
                                                    message += " ";
                                                }
                                            }

                                            Send(string(params[1]), (char *) message.data());
                                        } else {
                                            CommandFail(cmd);
                                        }
                                    } else if (strcmp(cmd, "BROADCAST") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }
                                        Broadcast(string(params[1]));
                                    } else if (strcmp(cmd, "BLOCK") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }
                                        bool doBlock = ValidIp(string(params[1]));
                                        if (doBlock) {
                                            doBlock = false;
                                            for (int i = 0; i < list.size(); i++) {
                                                if (strcmp(params[1], (char *) list[i].ip.data()) == 0) {
                                                    doBlock = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (doBlock) {
                                            for (int i = 0; i < blockList.size(); i++) {
                                                if (strcmp(params[1], (char *) blockList[i].ip.data()) == 0) {
                                                    doBlock = false;
                                                    break;
                                                }
                                            }
                                        }
                                        if (doBlock) {
                                            struct info blockClient;
                                            blockClient.ip = string(params[1]);
                                            blockList.push_back(blockClient);
                                            Block(string(params[1]));
                                        } else {
                                            CommandFail(cmd);
                                        }
                                    } else if (strcmp(cmd, "UNBLOCK") == 0) {
                                        if (params.size() <= 1) {
                                            perror("Error Input");
                                        }

                                        bool doUnBlock = ValidIp(string(params[1]));
                                        if (doUnBlock) {
                                            doUnBlock = false;
                                            for (int i = 0; i < list.size(); i++) {
                                                if (strcmp(params[1], (char *) list[i].ip.data()) == 0) {
                                                    doUnBlock = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (doUnBlock) {
                                            doUnBlock = false;
                                            for (int i = 0; i < blockList.size(); i++) {
                                                if (strcmp(params[1], (char *) blockList[i].ip.data()) == 0) {
                                                    blockList.erase(blockList.begin() + i);
                                                    doUnBlock = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (doUnBlock) {

                                            Unblock(string(params[1]));
                                        } else {
                                            CommandFail(cmd);
                                        }
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
//                            perror("Impossible");
                            cout << "Goodbye Server" << endl;
                            close(clientfd);
                            exit(0);
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
                                    vector<char *> paramMsg = Split(buffer, ",");
                                    string message;
                                    for (int index = 1; index < paramMsg.size(); index++) {
                                        message += string(params[index]);
                                        if (index != params.size() - 1) {
                                            message += ",";
                                        }
                                    }
                                    Received(string(params[1]), message);
                                }
                            }
                        }
                    }
                }

            }


        }

    }
}

