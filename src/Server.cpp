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
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, (char *) clientList[i].hostname.data(),
                                  (char *) clientList[i].ip.data(),
                                  clientList[i].port);
        }
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Statistics() {
    sort(clientList.begin(), clientList.end());
    char *cmd = "STATISTICS";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    int length = clientList.size();
    for (int i = 0; i < length; i++) {
        char *clientStatus;
        if (clientList[i].status == LOGIN) {
            clientStatus = "logged-in";
        } else {
            clientStatus = "logged-out";
        }
        cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i + 1, (char *) clientList[i].hostname.data(),
                              clientList[i].send,
                              clientList[i].receive, clientStatus);
    }
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Blocked(string ip) {
    char *cmd = "BLOCKED";
    //judge if ip valid
    if (ValidIp(ip)) {
        map<string, vector<struct info> >::iterator iter;
        iter = blockList.find(ip);
        if (iter != blockList.end()) {
            vector<info> blockClients = iter->second;
            int length = blockClients.size();
            sort(blockClients.begin(), blockClients.end());
            cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
            for (int i = 0; i < length; i++) {
                cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, (char *) blockClients[i].hostname.data(),
                                      (char *) blockClients[i].ip.data(),
                                      blockClients[i].port);
            }
            cse4589_print_and_log("[%s:END]\n", cmd);
        } else {
            CommandFail(cmd);
        }
    } else {
        CommandFail(cmd);
    }
}

void Server::Relay(string fromClient, string toClient, char *msg) {
    struct relayInfo relay;
    relay.ip = fromClient;
    relay.msg = string(msg);
    map<string, vector<struct relayInfo> >::iterator iter;
    iter = relayList.find(toClient);
    if (iter != relayList.end()) {
        iter->second.push_back(relay);
    } else {
        vector<relayInfo> relayMessages;
        relayMessages.push_back(relay);
        relayList.insert(pair<string, vector<relayInfo> >(toClient, relayMessages));
    }
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
            strcat(msg, (char *) clientList[i].hostname.data());
            strcat(msg, ",");
            strcat(msg, (char *) clientList[i].ip.data());
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
    cout << "ResponseRelayMsg()" << endl;
    if (iter != relayList.end()) {
        int clientIndex = FindClient(string(clientIp));

        if (clientIndex != -1) {
            vector<relayInfo> relayMessage = iter->second;
            int length = relayMessage.size();

            string msg("Msg\n");
            for (int i = 0; i < length; i++) {
                msg += "Msg,";
                msg += relayMessage[i].ip;
                msg += ",";
                msg += relayMessage[i].msg;
                msg += "\n";
//                if (send(sockfd, msg, strlen(msg), 0)) {
//                    cout << "Send online client: " << i + 1 << endl;
//                }
                clientList[clientIndex].receive++;
            }
            msg += "MsgEnd\n";
            return msg;
        }
    }
    return "";

}

string Server::ResponseDone(int sockfd) {
    char *responseDone = "Done\n";
    return string(responseDone);
}


int Server::FindClient(string clientIp) {
    for (int i = 0; i < clientList.size(); i++) {
        if (clientList[i].ip == clientIp) {

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
    struct info newClient;
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
                            } else if (strcmp(cmd, "STATISTICS") == 0) {
                                Statistics();
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
//                        struct info newClient;
                        newClient.hostname = clientHostname;
                        newClient.ip = string(clientIp);
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

                        SendData(fdaccept, (char *) message.data());
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
                            getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                            cout << "Getting exit client info..." << endl;
                            char *clientIp = inet_ntoa(client_addr.sin_addr);
//                            int clientPort = (int) ntohs(client_addr.sin_port);
                            int clientIndex = FindClient(string(clientIp));
                            cout << "Got exit client info!" << endl;

                            clientList.erase(clientList.begin() + clientIndex);
                            cout << "Clear client data from clientList" << endl;

                            map<string, vector<struct relayInfo> >::iterator keyRelay = relayList.find(
                                    string(clientIp));
                            if (keyRelay != relayList.end()) {
                                relayList.erase(keyRelay);
                            }
                            cout << "Clear client data from relayList" << endl;

                            map<string, vector<struct info> >::iterator keyBlock = blockList.find(string(clientIp));
                            if (keyBlock != blockList.end()) {
                                blockList.erase(keyBlock);
                            }
                            cout << "Clear client data from blockList" << endl;

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

                                int loginPort = (int) ntohs(client_addr.sin_port);
                                int clientIndex = FindClient(string(clientIp));

                                if (clientIndex != -1) {
                                    clientList[clientIndex].status = LOGIN;
                                }
                                //here complement the response
                                string listMsg = ResponseList(sock_index);
                                //then respond relay
                                string relayMsg = ResponseRelayMsg(sock_index, string(clientIp), loginPort);
                                //finally send Done
                                string responseDone = ResponseDone(sock_index);

                                string message;

                                message = string(listMsg);
                                message += string(relayMsg);
                                message += string(responseDone);

                                SendData(sock_index, (char *) message.data());

                            } else if (strcmp(sign, "LOGOUT") == 0) {
                                //update client status
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *clientIp = inet_ntoa(client_addr.sin_addr);

//                                int logoutPort = (int) ntohs(client_addr.sin_port);
                                int clientIndex = FindClient(string(clientIp));

                                if (clientIndex != -1) {
                                    clientList[clientIndex].status = LOGOUT;
                                }

                            } else if (strcmp(sign, "REFRESH") == 0) {
                                string listMsg = ResponseList(sock_index);
                                SendData(sock_index, (char *) listMsg.data());
                            } else if (strcmp(sign, "SEND") == 0) {
                                if (params.size() <= 2) {
                                    perror("Unexpected params");
                                }
                                char *toClient = params[1];
//                                char *message = params[2];
                                vector<char *> paramMsg = Split(buffer, ",");
                                string message;
                                for (int index = 1; index < paramMsg.size(); index++) {
                                    message += string(paramMsg[index]);
                                    if (index != paramMsg.size() - 1) {
                                        message += ",";
                                    }
                                }

                                int toClientIndex = FindClient(string(toClient));

                                if (toClientIndex != -1) {
                                    getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                    char *fromClient = inet_ntoa(client_addr.sin_addr);
//                                    int fromClientPort = (int) ntohs(client_addr.sin_port);
                                    int fromClientIndex = FindClient(string(fromClient));

                                    bool beBlocked = false;
                                    // block issue
                                    map<string, vector<struct info> >::iterator keyBlock = blockList.find(
                                            string(toClient));
                                    if (keyBlock != blockList.end()) {
                                        vector<info> blockClients = keyBlock->second;
                                        for (int i = 0; i < blockClients.size(); i++) {
                                            if (strcmp((char *) blockClients[i].ip.data(), fromClient) == 0) {
                                                beBlocked = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (!beBlocked) {
                                        if (clientList[toClientIndex].status == LOGIN) {
                                            char msg[BUFFER_SIZE];

                                            strcpy(msg, "Send:");
                                            strcat(msg, fromClient);
                                            strcat(msg, ",");
                                            strcat(msg, (char *) message.data());
                                            cout << msg << endl;
//                                            if (send(clientList[toClientIndex].socketfd, msg, strlen(msg), 0)) {
//                                                cout << "Send online client: " << toClientIndex + 1 << endl;
//                                            }
                                            if (SendData(clientList[toClientIndex].socketfd, msg)) {
                                                cout << "Send online client: " << toClientIndex + 1 << endl;
                                            }

                                            clientList[toClientIndex].receive++;
                                        } else {
                                            char *cmd = "RELAY";
                                            cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
                                            cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", fromClient,
                                                                  toClient, (char *) message.data());
                                            cse4589_print_and_log("[%s:END]\n", cmd);
                                            Relay(string(fromClient), string(toClient), (char *) message.data());
                                        }
                                    }
                                    clientList[fromClientIndex].send++;
                                }

                            } else if (strcmp(sign, "BROADCAST") == 0) {
                                if (params.size() <= 1) {
                                    perror("Unexpected params");
                                }
//                                char *message = params[1];

                                vector<char *> paramMsg = Split(buffer, ":");
                                string message = string(paramMsg[1]);

                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *fromClient = inet_ntoa(client_addr.sin_addr);

//                                int fromClientPort = (int) ntohs(client_addr.sin_port);
                                int fromClientIndex = FindClient(string(fromClient));
                                for (int i = 0; i < clientList.size(); i++) {
                                    if (i != fromClientIndex) {

                                        bool beBlocked = false;
                                        // block issue
                                        map<string, vector<struct info> >::iterator keyBlock = blockList.find(
                                                clientList[i].ip);
                                        if (keyBlock != blockList.end()) {
                                            vector<info> blockClients = keyBlock->second;
                                            for (int j = 0; j < blockClients.size(); j++) {
                                                if (strcmp((char *) blockClients[j].ip.data(), fromClient) == 0) {
                                                    beBlocked = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (!beBlocked) {
                                            if (clientList[i].status == LOGIN) {

                                                char msg[BUFFER_SIZE];

                                                strcpy(msg, "Send:");
                                                strcat(msg, fromClient);
                                                strcat(msg, ",");
                                                strcat(msg, (char *) message.data());
                                                cout << msg << endl;
//                                                if (send(clientList[i].socketfd, msg, strlen(msg), 0)) {
//                                                    cout << "Send online client: " << i + 1 << endl;
//                                                }
                                                if (SendData(clientList[i].socketfd, msg)) {
                                                    cout << "Send online client: " << i + 1 << endl;
                                                }

                                            } else {
                                                char *cmd = "RELAY";
                                                cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
                                                cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", fromClient,
                                                                      (char *) string("255.255.255.255").data(),
                                                                      (char *) message.data());
                                                cse4589_print_and_log("[%s:END]\n", cmd);
                                                Relay(string(fromClient), string("255.255.255.255"),
                                                      (char *) message.data());
                                            }
                                        }
//                                        clientList[fromClientIndex].send++;
                                    }
                                }
                                clientList[fromClientIndex].send++;

                            } else if (strcmp(sign, "BLOCK") == 0) {
                                if (params.size() <= 1) {
                                    perror("Unexpected params");
                                }
                                string blockIp = string(params[1]);
                                int blockIndex = FindClient(blockIp);
                                struct info blockInfo;
                                blockInfo.hostname = clientList[blockIndex].hostname;
                                blockInfo.ip = blockIp;
                                blockInfo.port = clientList[blockIndex].port;

                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *fromClient = inet_ntoa(client_addr.sin_addr);
                                map<string, vector<struct info> >::iterator iter;
                                iter = blockList.find(fromClient);


                                if (iter != blockList.end()) {
                                    cout << "block for some times" << endl;
                                    iter->second.push_back(blockInfo);
                                } else {
                                    cout << "first block" << endl;
                                    vector<info> blockClients;
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
                                    for (vector<info>::iterator it = iter->second.begin();
                                         it != iter->second.end(); it++) {
                                        if (strcmp((char *) it->ip.data(), unblockIp) == 0) {
                                            iter->second.erase(it);
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



