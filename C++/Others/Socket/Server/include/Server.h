#ifndef SERVER_H
#define SERVER_H
#include<mutex>
#include<memory>
#include<vector>
#include <iostream>
#include <vector>
extern "C" 
{

#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
}
#include <thread>
#include <algorithm>
#include <cctype>

class Server
{
private:
    /* data */
    int m_socketfd;
    char* m_sendBuf;
    char* m_recvBuf;
    struct sockaddr_in m_serverAddress;
    struct sockaddr_in m_clientAddress;
public:
    Server(/* args */);
    ~Server();
    bool start();
    bool serverSend(std::string &msg,int acceptfd);
    bool serverRecv(std::string &msg,int acceptfd);
};



#endif


