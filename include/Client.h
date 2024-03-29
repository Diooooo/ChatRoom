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


#define STDIN 0
using namespace std;

class Client {
public:
    string ip;
    int port;
    string hostname;
    vector<info> list;
    vector<info> blockList;
    int clientfd;
    enum clientStatus status;
    struct sockaddr_in client_addr;

    Client(int portNumber);

    void Author();

    void Ip();

    void Port();

    void List();

    void Login(string ip, int serverPort);

    void Refresh();

    void Send(string ip, char *message);

    void Broadcast(string message);

    void Block(string ip);

    void Unblock(string ip);

    void Logout();

    void Exit();

    void Received(string ip, string msg);

    void SendFile(string ip, string filePath);

    void Run();
    
    int ConnectToHost(char *server_ip, int server_port);
};
