#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256

#define BUFFER_SIZE 65535

#include <string>

enum clientStatus {
    LOGIN,
    LOGOUT,
    OFFLINE
};

struct info {
    std::string hostname;
    std::string ip;
    int port;
    enum clientStatus status;
    int send;
    int receive;
    int socketfd;

    bool operator<(const info &info1) const {

        return port < info1.port;

    }


    bool operator>(const info &info1) const {

        return port > info1.port;

    }
};


struct relayInfo {
    std::string ip;
    std::string msg;
};
#endif
