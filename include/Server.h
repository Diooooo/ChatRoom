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

class Server
{
  public:
    string ip;
    string hostname;
    int port;
    int server_socket, head_socket, selret, sock_index;
    int fdaccept = 0;
    socklen_t caddr_len;
    struct sockaddr_in server_addr, client_addr;
    fd_set master_list, watch_list;

    map<string, int> list;

    Server(int port);

    void Author();
    void Ip();
    void Port();
    void List();
    void Statistic();
    void Blocked(string ip);
    void Relay();
    void Run();
};
