#include <string>


#include "../include/BaseSocket.h"

using namespace std;

class Client: public BaseSocket
{
    public:
        void Login(string ip, unsigned int port);
        void Refresh();
        void Send(string ip, unsigned int port);
        void Boardcast(string msg);
        void Block(string ip);
        void Unblock(string ip);
        void Logout();
        void Exit();
        void Received();
        void SendFile(string ip, string filePath);
};