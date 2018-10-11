#include <iostream>
#include <string>
#include <map>

#include "../include/Server.h"

using namespace std;

Server::Server(int portNumber)
{
    port = portNumber;
}

void Server::Author()
{
}

void Server::Ip()
{
}

void Server::Port()
{
}

void Server::List()
{
}

void Server::Statistic()
{
}

void Server::Blocked(string ip)
{
}

void Server::Relay()
{
}

void Server::Run()
{
    int socket_fd, connect_fd;
    struct sockaddr_in servaddr;
    char buff[4096];
    int n;
    char sendline[4096];
    //初始化Socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    //初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(port);      //设置的端口为DEFAULT_PORT

    //将本地地址绑定到所创建的套接字上
    if (bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    //开始监听是否有客户端连接
    if (listen(socket_fd, 10) == -1)
    {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    if ((connect_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL)) < 0)
    {
        printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
    }

    while (1)
    {
        memset(buff, 0, sizeof(buff));
        printf("Waiting client send message now...\n");
        //接受客户端传过来的数据
        n = recv(connect_fd, buff, MAXLINE, 0);
        if (strcmp(buff, "bye") == 0)
        {
            printf("Exit\n");
            break;
        }
        buff[n] = '\0';
        printf("recv msg from client: %s", buff);
        printf("send message to client: ");
        fgets(sendline, 4096, stdin);
        send(connect_fd, sendline, strlen(sendline), 0);
    }
    close(connect_fd);
    close(socket_fd);
}