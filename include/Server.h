#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include <functional>

#include "../include/logger.h"
#include "../include/global.h"
#include "../include/common.h"

#define DEFAULT_PORT 8000
#define MAXLINE 4096

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

using namespace std;

class Server {
public:
    string ip;
    string hostname;
    int port;
    int server_socket;
    int head_socket;
    int selret;
    int sock_index;
    socklen_t caddr_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    fd_set master_list;
    fd_set watch_list;

    vector<struct info> clientList;
    map<string, vector<struct info> > blockList;
    map<string, vector<struct relayInfo> > relayList;

    Server(int port);

    void Author();

    void Ip();

    void Port();

    void List();

    void Statistic();

    void Blocked(string ip);

    void Relay(string fromClient, string toClient, char *msg);

    void Run();

    string ResponseList(int sockfd);

    string ResponseRelayMsg(int sockfd, string clientIp, int clientPort);

    string ResponseDone(int sockfd);

    int FindClient(string clientIp);

    bool Send(int sockfd, char* msg);

};
