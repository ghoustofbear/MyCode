#include"Client.h"
Client::Client(/* args */)
{
    m_socketfd = -1;
	m_port = 0;
	m_address = "";
    m_sendBuf=new char[1024*1024*10];
    m_recvBuf=new char[1024*1024*10];
}

Client::~Client()
{
  
   delete[]m_sendBuf;
   delete[]m_recvBuf;
   close(m_socketfd);
}
bool Client::start()
{
       //1.创建socket
 bool ret =false;
  int m_socketfd=socket(AF_INET,SOCK_STREAM,0);
  // AF_INET 表示采用TCP/IP协议族
	// SOCK_STREAM 表示采用TCP协议
	// 0是通常的默认情况
	
  if(m_socketfd<0)
  {
      std::cerr<<"客户端socket创建失败，m_socketfd="<<m_socketfd<<std::endl;
  }
  else
  {
       std::cerr<<"客户端socket创建成功，m_socketfd="<<m_socketfd<<std::endl;
      
  }
  m_ClientAdress.sin_family=AF_INET;
  m_ClientAdress.sin_addr.s_addr=inet_addr("192.168.69.246");
  m_ClientAdress.sin_port=htons(9090);
  int ret1=connect(m_socketfd,(sockaddr*)&m_ClientAdress,sizeof(m_ClientAdress));
if(ret1<0)
{
     std::cerr<<"客户端connect创建失败，m_socketfd="<<m_socketfd<<std::endl;
}
else
{
    
    std::cerr<<"客户端connect创建成功，m_socketfd="<<m_socketfd<<std::endl;
}
memset(m_recvBuf,0,sizeof(m_recvBuf));
memset(m_sendBuf,0,sizeof(m_sendBuf));
  while(1)
  {
    std::string msg="这里是客户端，收到请回答";
      // recv(m_socketfd,m_recvBuf,100,0);
     
      // std::string msg="这里是客户端，收到请回答";
      // send(m_socketfd,msg.c_str(),strlen(msg.c_str())+1,0);
      // std::cout<<"msg="<<m_recvBuf<<std::endl;
      this->SendMsg(m_socketfd,msg,msg.size());
     std::string recvmessage;
      this->recvMsg(m_socketfd,recvmessage,recvmessage.size());
      std::cerr<<"recvmessage="<<recvmessage<<std::endl;
      ret=true;
  }
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
int Client::writeMsg(int fd, const char* msg, int size)
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
int Client::SendMsg(int fd, std::string sendmsg, int size)
{
    const char *msg=sendmsg.c_str();
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
int Client::readMsg(int fd,  char* msg, int size)
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

int Client::recvMsg(int fd,  std::string &recvmsg, int size)
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
    recvmsg = buf;
    return ret;

}

