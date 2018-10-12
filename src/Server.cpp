#include <iostream>
#include <string>
#include <map>

#include "../include/Server.h"

using namespace std;

Server::Server(int portNumber)
{
    port = portNumber;
    hostname = GetHostname();
    ip = GetIP();

}

void Server::Author()
{
    char *cmd = "AUTHOR";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "bilinshi");
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Ip()
{
    char *cmd = "IP";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("IP:%s\n", ip.c_str());
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::Port()
{
    char *cmd = "PORT";
    cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
    cse4589_print_and_log("PORT:%d\n", port);
    cse4589_print_and_log("[%s:END]\n", cmd);
}

void Server::List()
{
}

void Server::Statistic()
{
}

void Server::Blocked(string ip)
{
}

void Server::Relay()
{
}

void Server::Run()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        printf("Create socket failed!");
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr((char*)ip.data());
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed!");
        exit(0);
    }

    if (listen(server_socket, 10) < 0) {
        printf ("Listen socket error!");
    }

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    FD_SET(server_socket, &master_list);

    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    while (1){
        memcpy(&watch_list, &master_list, sizeof(master_list));

        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if (selret < 0)
            printf("Select error!");

        if (selret > 0) {
            //
            for (sock_index = 0; sock_index <= head_socket; sock_index++){

                if (FD_ISSET(sock_index, &watch_list)){
                    if(sock_index == 0){
                        string cmdLine;
                        getline(cin, cmdLine);
                        if (cmdLine.size()==0) {
                            exit(-1);
                        }
                        else {
                            char* cmd = strtok((char*)cmdLine.data(), " ");
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
                    else if(sock_index == server_socket){
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
                        if(fdaccept < 0)
                            perror("Accept failed.");

                        printf("\nRemote Host connected!\n");

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_socket)
                            head_socket = fdaccept;
                    }
                    /* Read from existing clients */
                    else{
                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                            close(sock_index);
                            printf("Remote Host terminated connection!\n");

                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                        }
                        else {
                            //Process incoming data from existing clients here ...

                            printf("\nClient sent me: %s\n", buffer);
                            printf("ECHOing it back to the remote host ... ");
                            if(send(sock_index, buffer, strlen(buffer), 0) == strlen(buffer))
                                printf("Done!\n");
                            fflush(stdout);
                        }

                        free(buffer);
                    }

                }


            }

        }

    }



}
