#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256

struct info {
    char *hostname;
    char *ip;
    int port;

    bool operator<(const info &info1) const

    {

        return port < info1.port;

    }


    bool operator>(const info &info1) const

    {

        return port > info1.port;

    }
};

#endif
