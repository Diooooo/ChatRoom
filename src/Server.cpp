#include <iostream>
#include <string>
#include <map>

#include "../include/Server.h"

using namespace std;

Server::Server(int portNumber) {
    port = portNumber;
    hostname = GetHostname();
    ip = GetIP();
    caddr_len = sizeof(client_addr);
}

void Server::Author() {
    char *cmd = "AUTHOR";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "bilinshi");
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Ip() {
    char *cmd = "IP";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("IP:%s\n", ip.c_str());
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Port() {
    char *cmd = "PORT";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("PORT:%d\n", port);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::List() {
    sort(clientList.begin(), clientList.end());
    int length = clientList.size();
    char *cmd = "LIST";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    for (int i = 0; i < length; i++) {
        if (clientList[i].status == LOGIN) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, clientList[i].hostname, clientList[i].ip,
                                  clientList[i].port);
        }
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Statistic() {
    sort(clientList.begin(), clientList.end());
    char *cmd = "STATISTIC";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    int length = clientList.size();
    for (int i = 0; i < length; i++) {
        char *clientStatus;
        if (clientList[i].status == LOGIN) {
            clientStatus = "logged-in";
        } else {
            clientStatus = "logged-out";
        }
        cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i + 1, clientList[i].hostname, clientList[i].send,
                              clientList[i].receive, clientStatus);
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Blocked(string ip) {
    char *cmd = "BLOCKED";
    //judge if ip valid

    map<string, vector<struct info> >::iterator iter;
    iter = blockList.find(ip);
    if (iter != blockList.end()) {
        vector<info> blockClients = iter->second;
        int length = blockClients.size();
        sort(blockClients.begin(), blockClients.end());
        cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
        for (int i = 0; i < length; i++) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, blockClients[i].hostname, blockClients[i].ip,
                                  blockClients[i].port);
        }
        cse4589_print_and_log("[%s:END]\n", cmd);
    } else {
        CommandFail(cmd);
    }
}

void Server::Relay(string fromClient, string toClient, char *msg) {
    map<string, vector<struct relayInfo> >::iterator iter;
    iter = relayList.find(toClient);
    if (iter != relayList.end()) {
        struct relayInfo relay;
        relay.ip = (char *) fromClient.data();
        relay.msg = msg;
        iter->second.push_back(relay);
    } else {
        vector<relayInfo> relayMessages;
        relayList.insert(pair<string, vector<relayInfo> >(toClient, relayMessages));
    }
}

bool Server::Send(int sockfd, char *msg) {
    unsigned long total = 0;
    unsigned long bytes_remaining = strlen(msg);
    ssize_t n = 0;
    cout << "send " << strlen(msg) << " bytes: " << msg << endl;

    while (total < strlen(msg)) {
        n = send(sockfd, msg + total, bytes_remaining, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytes_remaining -= n;
    }

    return n != -1;
}


string Server::ResponseList(int sockfd) {
    int length = clientList.size();
    char msg[BUFFER_SIZE];
    memset(msg, '\0', sizeof(msg));
    strcpy(msg, "List\n");
    for (int i = 0; i < length; i++) {
        if (clientList[i].status == LOGIN) {
            strcat(msg, "List");
            strcat(msg, ",");
            strcat(msg, clientList[i].hostname);
            strcat(msg, ",");
            strcat(msg, clientList[i].ip);
            strcat(msg, ",");
            char portString[10];
            sprintf(portString, "%d", clientList[i].port);

            strcat(msg, portString);
            strcat(msg, "\n");
//            if (send(sockfd, msg, strlen(msg), 0)) {
//                cout << "Send online client: " << i + 1 << endl;
//            }
        }
    }
    strcat(msg, "ListEnd\n");
    return string(msg);
}


string Server::ResponseRelayMsg(int sockfd, string clientIp, int clientPort) {
    map<string, vector<struct relayInfo> >::iterator iter;
    iter = relayList.find(clientIp);
    cout << "S1" << endl;
    if (iter != relayList.end()) {
        int clientIndex = FindClient(string(clientIp), clientPort);
        cout << "clientindex:" << clientIndex << endl;

        if (clientIndex != -1) {
            vector<relayInfo> relayMessage = iter->second;
            int length = relayMessage.size();
            char msg[BUFFER_SIZE];
            memset(msg, '\0', sizeof(msg));
            strcpy(msg, "Msg\n");
            for (int i = 0; i < length; i++) {
                strcat(msg, "Msg");
                strcat(msg, ",");
                strcat(msg, relayMessage[i].ip);
                strcat(msg, ",");
                strcat(msg, relayMessage[i].msg);
                strcat(msg, "\n");
//                if (send(sockfd, msg, strlen(msg), 0)) {
//                    cout << "Send online client: " << i + 1 << endl;
//                }
                clientList[clientIndex].receive++;
            }
            strcat(msg, "MsgEnd\n");
            return string(msg);
        }
    }
    return "";

}

string Server::ResponseDone(int sockfd) {
    char *responseDone = "Done\n";
    return string(responseDone);
}


int Server::FindClient(string clientIp, int clientPort) {
    for (int i = 0; i < clientList.size(); i++) {
        if ((strcmp(clientList[i].ip, (char *) clientIp.data()) == 0)
            && (clientList[i].port == clientPort)) {

            return i;
        }
    }
    return -1;
}

void Server::Run() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        perror("Create socket failed!");
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr((char *) ip.data());
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed!");
        exit(0);
    }

    if (listen(server_socket, 10) < 0) {
        printf("Listen socket error!");
    }

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    FD_SET(server_socket, &master_list);

    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    while (1) {
        memcpy(&watch_list, &master_list, sizeof(master_list));
        bzero(&client_addr, sizeof(client_addr));
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
                            exit(-1);
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
                            if (strcmp(cmd, "AUTHOR") == 0) {
                                Author();
                            } else if (strcmp(cmd, "IP") == 0) {
                                Ip();
                            } else if (strcmp(cmd, "PORT") == 0) {
                                Port();
                            } else if (strcmp(cmd, "LIST") == 0) {
                                List();
                            } else if (strcmp(cmd, "STATISTIC") == 0) {
                                Statistic();
                            } else if (strcmp(cmd, "BLOCKED") == 0) {
                                if (params.size() <= 1) {
                                    cout << "Error Input" << endl;
                                }
                                Blocked(string(params[1]));
                            } else {
                                perror("Unexpected command");
                            }

                        }
                    } else if (sock_index == server_socket) { //new client login
                        int fdaccept = accept(server_socket, (struct sockaddr *) &client_addr, &caddr_len);
                        if (fdaccept < 0)
                            perror("Accept failed.");


                        char *clientIp = inet_ntoa(client_addr.sin_addr);
                        int clientPort = (int) ntohs(client_addr.sin_port);
                        cout << "IP" << clientIp << endl;
                        string clientHostname = GetClientHostname(clientIp);
//                        getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                        cout << clientHostname << endl;
                        struct info newClient;
                        newClient.hostname = (char *) clientHostname.data();
                        newClient.ip = clientIp;
                        newClient.port = clientPort;
                        newClient.send = 0;
                        newClient.receive = 0;
                        newClient.status = LOGIN;
                        newClient.socketfd = fdaccept;

                        clientList.push_back(newClient);
                        cout << 3 << endl;
                        //here complement the response
                        string listMsg = ResponseList(fdaccept);
                        //then respond relay
                        string relayMsg = ResponseRelayMsg(fdaccept, string(clientIp), clientPort);
                        //finally send Done
                        string responseDone = ResponseDone(fdaccept);

                        string message;

                        message = string(listMsg);
                        message += string(relayMsg);
                        message += string(responseDone);

                        Send(fdaccept, (char *) message.data());
//                        char message[BUFFER_SIZE];
//                        strcpy(message, listMsg);
//                        if (relayMsg != NULL) {
//                            strcat(message, relayMsg);
//                        }
//                        strcat(message, responseDone);
//                        cout << "generate message: " << message << endl;
//                        Send(fdaccept, message);
                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if (fdaccept > head_socket)
                            head_socket = fdaccept;
                    }
                        /* Read from existing clients */
                    else {
                        /* Initialize buffer to receive response */
                        char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if (recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0) {
                            //clear data of sock_index


                            close(sock_index);
                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                        } else {
                            //Process incoming data from existing clients here ...
//                            const char *sep = ":,";
//                            char *p;
//                            p = strtok(buffer, sep);
//                            char *sign = p;
                            vector<char *> params = Split(buffer, ":,");
                            //read other params
//                            while (p) {
//                                params.push_back(p);
//                                p = strtok(NULL, sep);
//                            }
                            char *sign = params[0];
                            if (strcmp(sign, "LOGIN") == 0) {
                                //update client status
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *clientIp = inet_ntoa(client_addr.sin_addr);

                                int loginPort = (int)ntohs(client_addr.sin_port);
                                int clientIndex = FindClient(string(clientIp), loginPort);

                                if (clientIndex != -1) {
                                    clientList[clientIndex].status = LOGIN;
                                }
                                ResponseList(sock_index);
                                ResponseRelayMsg(sock_index, string(clientIp), loginPort);

                                ResponseDone(sock_index);

                            } else if (strcmp(sign, "LOGOUT") == 0) {
                                //update client status
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *clientIp = inet_ntoa(client_addr.sin_addr);

                                int logoutPort = (int)ntohs(client_addr.sin_port);
                                int clientIndex = FindClient(string(clientIp), logoutPort);

                                if (clientIndex != -1) {
                                    clientList[clientIndex].status = LOGOUT;
                                }

                            } else if (strcmp(sign, "REFRESH") == 0) {
                                string listMsg = ResponseList(sock_index);
                                Send(sock_index, (char *) listMsg.data());
                            } else if (strcmp(sign, "SEND") == 0) {
                                if (params.size() <= 3) {
                                    perror("Unexpected params");
                                }
                                char *toClient = params[1];
                                int clientPort = atoi(params[2]);
                                char *message = params[3];

                                int toClientIndex = FindClient(toClient, clientPort);

                                // block issue

                                if (toClientIndex != -1) {
                                    getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                    char *fromClient = inet_ntoa(client_addr.sin_addr);

                                    int fromClientPort = (int)ntohs(client_addr.sin_port);
                                    int fromClientIndex = FindClient(string(fromClient), fromClientPort);

                                    if (clientList[toClientIndex].status == LOGIN) {
                                        char msg[255];

                                        strcpy(msg, "Send:");
                                        strcat(msg, fromClient);
                                        strcat(msg, ",");
                                        strcat(msg, message);
                                        cout << msg << endl;
                                        if (send(clientList[toClientIndex].socketfd, msg, strlen(msg), 0)) {
                                            cout << "Send online client: " << toClientIndex + 1 << endl;
                                        }

                                        clientList[fromClientIndex].send++;
                                    } else {
                                        Relay(string(fromClient), string(toClient), message);
                                    }
                                    clientList[fromClientIndex].send++;
                                }

                            } else if (strcmp(sign, "BOARDCAST") == 0) {
                                if (params.size() <= 1) {
                                    perror("Unexpected params");
                                }
                                char *message = params[1];
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *fromClient = inet_ntoa(client_addr.sin_addr);

                                int fromClientPort = (int)ntohs(client_addr.sin_port);
                                int fromClientIndex = FindClient(string(fromClient), fromClientPort);
                                for (int i = 0; i < clientList.size(); i++) {
                                    cout << "i:" << i << endl;
                                    if (clientList[i].status == LOGIN && i != fromClientIndex) {

                                        char msg[255];

                                        strcpy(msg, "Send:");
                                        strcat(msg, fromClient);
                                        strcat(msg, ",");
                                        strcat(msg, message);
                                        cout << msg << endl;
                                        if (send(clientList[i].socketfd, msg, strlen(msg), 0)) {
                                            cout << "Send online client: " << i + 1 << endl;
                                        }

                                        clientList[fromClientIndex].send++;
                                    }
                                }
                            } else if (strcmp(sign, "BLOCK") == 0) {
                                if (params.size() <= 1) {
                                    perror("Unexpected params");
                                }
                                char *blockIp = params[1];
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *fromClient = inet_ntoa(client_addr.sin_addr);
                                map<string, vector<struct info> >::iterator iter;
                                iter = blockList.find(fromClient);
                                if (iter != blockList.end()) {
                                    vector<info> blockClients = iter->second;
                                    struct info blockInfo;
                                    blockInfo.ip = blockIp;
                                    blockClients.push_back(blockInfo);
                                } else {
                                    vector<info> blockClients;
                                    struct info blockInfo;
                                    blockInfo.ip = blockIp;
                                    blockClients.push_back(blockInfo);
                                    blockList.insert(pair<string, vector<info> >(fromClient, blockClients));
                                }
                            } else if (strcmp(sign, "UNBLOCK") == 0) {
                                if (params.size() <= 1) {
                                    perror("Unexpected params");
                                }
                                char *unblockIp = params[1];
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *fromClient = inet_ntoa(client_addr.sin_addr);
                                map<string, vector<struct info> >::iterator iter;
                                iter = blockList.find(fromClient);
                                if (iter != blockList.end()) {
                                    vector<info> blockClients = iter->second;
                                    for (vector<info>::iterator it = blockClients.begin();
                                         it != blockClients.end(); it++) {
                                        if (strcmp(it->ip, unblockIp) == 0) {
                                            blockClients.erase(it);
                                            break;
                                        }
                                    }
                                }

                            }
                        }
                        free(buffer);
                    }

                }

            }


        }

    }

}



