#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256

#define BUFFER_SIZE 65535
enum clientStatus {
    LOGIN,
    LOGOUT,
    OFFLINE
};

struct info {
    char *hostname;
    char *ip;
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
    char *ip;
    char *msg;
};
#endif
