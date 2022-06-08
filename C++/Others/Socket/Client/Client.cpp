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
  m_ClientAdress.sin_port=htons(8888);
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
      recv(m_socketfd,m_recvBuf,100,0);
     
      std::string msg="这里是客户端，收到请回答";
      send(m_socketfd,msg.c_str(),strlen(msg.c_str())+1,0);
      std::cout<<"msg="<<m_recvBuf<<std::endl;
      ret=true;
  }
  return ret;
}
