#include <string>
#include "../include/BaseSocket.h"

using namespace std;

class Server: public BaseSocket
{
    public:
        void Statistic();
        void Blocked(string ip);
        void Relay();
};