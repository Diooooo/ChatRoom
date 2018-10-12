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

void Server::Relay() {
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
                        string clientHostname = GetClientHostname(clientIp);
                        struct info newClient;
                        newClient.hostname = (char *) clientHostname.data();
                        newClient.ip = (char *) clientIp.data();
                        newClient.port = clientPort;
                        newClient.send = 0;
                        newClient.receive = 0;
                        newClient.status = LOGIN;

                        clientList.push_back(newClient);

                        //here complement the response
                        int length = clientList.size();
                        for (int i = 0; i < length; i++) {
                            if (clientList[i].status == LOGIN) {
                                char *msg = "List:";
                                strcat(msg, clientList[i].hostname);
                                strcat(msg, ",");
                                strcat(msg, clientList[i].ip);
                                strcat(msg, ",");
                                char portString[10];
                                itoa(clientList[i].port, portString, 10); //radix: decimal
                                strcat(msg, portString);
                                if (send(fdaccept, msg, strlen(msg), 0)) {
                                    cout << "Send online client: " << i + 1 << endl;
                                }
                                free(msg);
                            }
                        }
                        //then respond relay


                        //finally send Done
                        char *responseDone = "Done";
                        if (send(fdaccept, responseDone, strlen(responseDone), 0)) {
                            cout << "Response done" << endl;
                        }
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
                            if (strstr(buffer, "LOGIN")) {
                                //update client status
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *clientIp = inet_ntoa(client_addr.sin_addr);
                                int length = clientList.size();
                                for (int i = 0; i < length; i++) {
                                    if (strcmp(clientList[i].ip, clientIp) == 0) {
                                        clientList[i].status = LOGIN;
                                    }
                                }
                            } else if (strstr(buffer, "LOGOUT")) {
                                //update client status
                                getpeername(sock_index, (struct sockaddr *) &client_addr, &caddr_len);
                                char *clientIp = inet_ntoa(client_addr.sin_addr);
                                int length = clientList.size();
                                for (int i = 0; i < length; i++) {
                                    if (strcmp(clientList[i].ip, clientIp) == 0) {
                                        clientList[i].status = LOGOUT;
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
