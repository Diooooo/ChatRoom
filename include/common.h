//
// Created by bilin on 10/11/18.
//
#include <string>

#ifndef BILINSHI_COMMON_H
#define BILINSHI_COMMON_H

std::string GetIP();

std::string GetHostname();


int ConnectToHost(char *server_ip, int server_port);

void CommandFail(char *cmd);

std::string GetClientHostname(char *clientIp);

#endif //BILINSHI_COMMON_H
