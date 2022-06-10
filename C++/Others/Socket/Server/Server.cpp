#include<Server.h>
#include<iostream>
Server::Server(/* args */)
{



}

Server::~Server()
{

 

    
}
//1.创建socket
//返回socketfd
int  Server::createServer(std::string ip,u_int32_t port)
{
    int  socketfd=socket(AF_INET,SOCK_STREAM,0);
  // AF_INET 表示采用TCP/IP协议族
	// SOCK_STREAM 表示采用TCP协议
	// 0是通常的默认情况
	
  if(socketfd<0)
  {
      std::cerr<<"服务端socket创建失败，socketfd="<<socketfd<<std::endl;
      return -1;
      
  }
  else
  {
       std::cerr<<"服务端socket创建成功，m_socketfd="<<socketfd<<std::endl;
      
  }
    //2.准备通讯地址（必须是服务器的）的IP
  struct sockaddr_in ServerAddress;
  memset(&ServerAddress,0,sizeof(ServerAddress));
  ServerAddress.sin_family=AF_INET;
  //m_serverAddress.sin_addr.s_addr=inet_addr("0.0.0.0"); //net_addr方法可以转化字符串，主要用来将一个十进制的数转化为二进制的数，用途多于ipv4的IP转化。INADDR_ANY; //inet_addr("0.0.0.0"); 
  ServerAddress.sin_addr.s_addr=inet_addr(ip.c_str()); //net_addr方法可以转化字符串，主要用来将一个十进制的数转化为二进制的数，用途多于ipv4的IP转化。INADDR_ANY; //inet_addr("0.0.0.0"); 
  ServerAddress.sin_port=htons(port);
//3.bind()绑定
//参数一：0的返回值（socket_fd）
//参数二：(struct sockaddr*)&addr 前面结构体，即地址
//参数三: addr结构体的长度
    auto m=bind(socketfd,(struct sockaddr*)&ServerAddress,sizeof(sockaddr));
    if(m<0)
    {
        std::cerr<<"服务端bind绑定失败 bindfd="<<m<<std::endl;
        close(socketfd);
        close(m);
        return -1;
    }
    else
    {
    /* code */
    std::cerr<<"服务端bind绑定成功"<<std::endl;
    }
    auto k=listen(socketfd,100);
    if(k<0)
    {
    std::cerr<<"服务端listen绑定失败 listenfd="<<k<<std::endl;
      close(socketfd);
      close(m);
      close(k);
     return -1;
    }
    else
    {

    std::cerr<<"服务端listen绑定成功"<<std::endl;
    }
  return socketfd;
}
int Server::acceptSocket(int socketfd)
{
     struct sockaddr_in clientAddress;
    memset(&clientAddress,0,sizeof(clientAddress));
    int length=sizeof(clientAddress);
    int  acceptedfd=accept(socketfd,(sockaddr*)&clientAddress,(socklen_t*)&length);
    if(acceptedfd<0)
    {
        std::cerr<<"服务端接受失败,acceptfd="<<acceptedfd<<std::endl;
        close(acceptedfd);
        close(socketfd);
        return -1;
    }
    else
    {
        return acceptedfd;
    }
    
}
bool Server::start()
{
    bool ret=false;
  
    int socketfd=createServer("0.0.0.0",9090);
    int acceptfd=acceptSocket(socketfd);
    
   
        

        std::string msg="这里是服务端，收到请回答";
      int t= this->SendMsg(acceptfd,msg,msg.size());
      std::cerr<<"t="<<t<<std::endl;     
      std::string recvmessage;
      this->recvMsg(acceptfd,recvmessage,recvmessage.size());
      std::cerr<<"recvmessage="<<recvmessage<<std::endl;
      ret=true;
    
   return ret;
}


/*
函数描述: 发送指定的字节数
函数参数:
    - fd: 通信的文件描述符(套接字)
    - msg: 待发送的原始数据
    - size: 待发送的原始数据的总字节数
函数返回值: 函数调用成功返回发送的字节数, 发送失败返回-1
*/
int Server::writeMsg(int fd, const char* msg, int size)
{
    const char* buffer=msg;
    u_int64_t count=size;
    while(count>0)
    {
        int length=send(fd,buffer,count,0);
        if(length<0)
        {
            close(fd);
            std::cerr<<"服务端send发送失败"<<std::endl;
            return -1;
        }
        else 
        {
            /* code */
         
        buffer+=length;
        count-=length;
        }
     
        
    }
     return size;
}
/*
函数描述: 发送带有数据头的数据包
函数参数:
    - cfd: 通信的文件描述符(套接字)
    - msg: 待发送的原始数据
    - len: 待发送的原始数据的总字节数
函数返回值: 函数调用成功返回发送的字节数, 发送失败返回-1
*/
int Server::SendMsg(int fd, std::string sendmsg, int size)
{
    const char* msg=sendmsg.c_str();
    if(fd<=0||msg==NULL||size<=0)
    {
        return -1;
    }
     // 申请内存空间: 数据长度 + 包头8字节(存储数据长度)
   char* data = (char*)malloc(size+8);
   int bigLen = htonl(size);
   memcpy(data, &bigLen, 8);
   memcpy(data+8, msg, size);
   // 发送数据
   int ret = writeMsg(fd, data, size+8);
   // 释放内存
   free(data);
   return ret;

}
/*
函数描述: 接收指定的字节数
函数参数:
    - fd: 通信的文件描述符(套接字)
    - buf: 存储待接收数据的内存的起始地址
    - size: 指定要接收的字节数
函数返回值: 函数调用成功返回发送的字节数, 发送失败返回-1
*/
int Server::readMsg(int fd,   char* msg, int size)
{
     char* recvbuff = msg;
    int count = size;
    while (count > 0)
    {
        int len = recv(fd, recvbuff, count, 0);
        if (len == -1)
        {
            return -1;
        }
        else if (len == 0)
        {
            return size - count;
        }
        recvbuff += len;
        count -= len;
    }
    return size;

}
/*
函数描述: 接收带数据头的数据包
函数参数:
    - cfd: 通信的文件描述符(套接字)
    - msg: 一级指针的地址，函数内部会给这个指针分配内存，用于存储待接收的数据，这块内存需要使用者释放
函数返回值: 函数调用成功返回接收的字节数, 发送失败返回-1
*/

int Server::recvMsg(int fd,  std::string &s, int size)
{
// 接收数据
    // 1. 读数据头
    int len = 0;
    readMsg(fd, (char*)&len, 8);
    len = ntohl(len);
    printf("数据块大小: %d\n", len);

    // 根据读出的长度分配内存，+1 -> 这个字节存储\0
     char *buf = (char*)malloc(len+1);
    int ret = readMsg(fd, buf, len);
    if(ret != len)
    {
        close(fd);
        free(buf);
        return -1;
    }
    buf[len] = '\0';
    s = buf;
    return ret;
    

}
