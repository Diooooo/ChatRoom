#include <iostream>
#include <string>
#include <map>

using namespace std;

class BaseSocket
{
    public:
        string ip;
        unsigned int port;
        map<string, unsigned int> list;

        BaseSocket(unsigned int port);
        ~BaseSocket();


        void Author();
        void Ip();
        void Port();
        void List();
};