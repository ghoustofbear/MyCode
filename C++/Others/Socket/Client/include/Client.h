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
#include<mutex>
#include<thread>
const int receive_Max_Size=4096;
class Client
{

public:
    Client(/* args */);
    ~Client();
    bool start();
    int recvMsg(int fd,   std::string &recvmsg, int size);
    int SendMsg(int fd, std::string sendmsg, int size);
private:
    void test(std::string testmsg);
    int createClient(std::string ip,u_int32_t port);
    int writeMsg(int fd, const char* msg, int size);
    int readMsg(int fd,   char* msg, int size);
    std::mutex m_mux;
};



#endif