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

#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256
#define MAXCMD 500
using namespace std;

class Client
{
  public:
    string ip;
    int port;
    string hostname;
    vector<info> list;
    int clientfd;
    enum clientStatus status;

    Client(int portNumber);

    void Author();
    void Ip();
    void Port();
    void List();
    void Login(string ip, int serverPort);
    void Refresh();
    void Send(string ip, int clientPort);
    void Boardcast(string msg);
    void Block(string ip);
    void Unblock(string ip);
    void Logout();
    void Exit();
    void Received(string ip, string msg);
    void SendFile(string ip, string filePath);
    void Run();
};