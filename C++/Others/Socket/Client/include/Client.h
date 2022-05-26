#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h> 
#include <vector>
const int receive_Max_Size=4096;
class Client
{

public:
    Client(/* args */);
    ~Client();
   
   
    bool Setup(std::string ip, int port,int protocolType);
    bool Send(std::string data);
    std::string receive(int size = receive_Max_Size);
    std::string read();
    void exit();
private:
    int m_socketfd;
    std::string m_address;
    int m_port;
    struct sockaddr_in m_server;
};



#endif