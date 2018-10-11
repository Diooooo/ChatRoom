#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
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
    int port;
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