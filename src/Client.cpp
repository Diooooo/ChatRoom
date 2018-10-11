#include <iostream>
#include <string>
#include <map>

#include "../include/Client.h"

using namespace std;

Client::Client(int portNumber)
{
    port = portNumber;
}

void Client::Author()
{

}

void Client::Ip()
{

}

void Client::Port()
{

}

void Client::List()
{

}

void Client::Login(string ip, int port)
{

}

void Client::Refresh()
{

}

void Client::Send(string ip, int port)
{

}

void Client::Boardcast(string msg)
{

}

void Client::Block(string ip)
{

}

void Client::Unblock(string ip)
{

}

void Client::Logout()
{

}

void Client::Exit()
{

}

void Client::Received()
{

}

void Client::SendFile(string ip, string filePath)
{
    
}

void Client::Run()
{
    int sockfd, n, rec_len;
    char recvline[4096], sendline[4096];
    char buf[MAXLINE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", "127.0.0.1");
        exit(0);
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        memset(sendline, 0, sizeof(sendline));
        printf("Send massage to server: ");
        fgets(sendline, 4096, stdin);
        //send
        send(sockfd, sendline, strlen(sendline), 0);
        if (strcmp(sendline, "bye") == 0)
        {
            printf("Exit\n");
            break;
        }
        //receive
        printf("Waiting server send message now...\n");
        recv(sockfd, buf, MAXLINE, 0);
        printf("Received message: %s", buf);
    }
    close(sockfd);
    exit(0);
}
