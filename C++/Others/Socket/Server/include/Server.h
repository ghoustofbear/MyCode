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
// 长度编码方案C++ 解决粘包：
//  关于数据包的包头大小可以根据自己的实际需求进行设定，这里没有啥特殊需求，因此规定包头的固定大小为4个字节，用于存储当前数据块的总字节数。


// 发送端：
// 数据的发送分为 4 步：

// 根据待发送的数据长度 N 动态申请一块固定大小的内存：N+4（4 是包头占用的字节数）
// 将待发送数据的总长度写入申请的内存的前四个字节中，此处需要将其转换为网络字节序（大端）
// 将待发送的数据拷贝到包头后边的地址空间中，将完整的数据包发送出去（字符串没有字节序问题）
// 释放申请的堆内存。

class Server
{
private:
    /* data */
    

    int writeMsg(int fd, const char* msg, int size);
    int readMsg(int fd,   char* msg, int size);
    int createServer(std::string ip,u_int32_t port);
    int acceptSocket(int socketfd);
public:
    Server(/* args */);
    ~Server();
    bool start();
    
    int recvMsg(int fd,   std::string &recvmsg, int size);
    int SendMsg(int fd,  std::string sendmsg, int size);
};



#endif


