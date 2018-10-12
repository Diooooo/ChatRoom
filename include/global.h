#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256
enum clientStatus {
    LOGIN,
    LOGOUT
};

struct info {
    char *hostname;
    char *ip;
    int port;
    enum clientStatus status;
    int send;
    int receive;

    bool operator<(const info &info1) const {

        return port < info1.port;

    }


    bool operator>(const info &info1) const {

        return port > info1.port;

    }
};


struct blockInfo {
    char *ip;
    char *msg;
};
#endif
