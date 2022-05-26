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

const int MAXPACKETSIZE= 1024*10*10;
const int  MAX_CLIENT =1000;
struct descript_socket{
	int socketfd     = -1;
	std::string ip      = "";
	int id         = -1; 
	std::string message;
	bool enable_message_runtime = false;
};
class Server
{

public:
    Server(/* args */);
    ~Server();
    bool SetUp(std::string ip,int port,int protocolType);
    bool Accepted();
    std::vector<descript_socket> getMessage();
    bool Send(std::string msg,int id);
    std::string  get_ip_addr(int id);
    bool is_online();
    void detach(int id);
    void closed();
    int get_last_closed_sockets();
    void clean(int id);
private:
    int m_sockfd, n;
	struct sockaddr_in m_serverAddress;
	struct sockaddr_in m_clientAddress;
    
     bool m_isonline;
     int m_last_closed;
     int m_clientNum;
	 std::mutex m_mutex;
     std::vector<descript_socket> newSocket;
     char msg[ MAXPACKETSIZE ];
	 std::vector<descript_socket> Message;//[CODA_MSG];
    void  AcceptTask(descript_socket  argv);
};
#endif


