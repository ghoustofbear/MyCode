#include<Server.h>
#include<iostream>
Server::Server(/* args */)
{
    m_socketfd=-1;
    m_sendBuf=new char[1024*1024*10];
    m_recvBuf=new char[1024*1024*10];
    memset(m_recvBuf,0,sizeof(m_recvBuf));
    memset(m_sendBuf,0,sizeof(m_sendBuf));
}

Server::~Server()
{
    delete[] m_recvBuf;
    delete[]m_sendBuf;
    close(m_socketfd);

}
bool Server::start()
{
    bool ret=false;
    //1.创建socket

  int m_socketfd=socket(AF_INET,SOCK_STREAM,0);
  // AF_INET 表示采用TCP/IP协议族
	// SOCK_STREAM 表示采用TCP协议
	// 0是通常的默认情况
	
  if(m_socketfd<0)
  {
      std::cerr<<"服务端socket创建失败，m_socketfd="<<m_socketfd<<std::endl;
  }
  else
  {
       std::cerr<<"服务端socket创建成功，m_socketfd="<<m_socketfd<<std::endl;
      
  }
  //2.准备通讯地址（必须是服务器的）的IP
  memset(&m_serverAddress,0,sizeof(m_serverAddress));
  m_serverAddress.sin_family=AF_INET;
  m_serverAddress.sin_addr.s_addr=inet_addr("0.0.0.0"); ;//net_addr方法可以转化字符串，主要用来将一个十进制的数转化为二进制的数，用途多于ipv4的IP转化。INADDR_ANY; //inet_addr("0.0.0.0"); 
  m_serverAddress.sin_port=htons(8888);
//3.bind()绑定
//参数一：0的返回值（socket_fd）
//参数二：(struct sockaddr*)&addr 前面结构体，即地址
//参数三: addr结构体的长度
    auto m=bind(m_socketfd,(struct sockaddr*)&m_serverAddress,sizeof(sockaddr));
    if(m<0)
    {
        std::cerr<<"服务端bind绑定失败"<<std::endl;
    }
    else
    {
    /* code */
    std::cerr<<"服务端bind绑定成功"<<std::endl;
    }
    auto k=listen(m_socketfd,100);
    if(k<0)
    {
    std::cerr<<"服务端bind绑定失败"<<std::endl;
    }
    else
    {

    std::cerr<<"服务端bind绑定成功"<<std::endl;
    }
    memset(&m_clientAddress,0,sizeof(m_clientAddress));
    int length=sizeof(m_clientAddress);
    int acceptfd=accept(m_socketfd,(sockaddr*)&m_clientAddress,(socklen_t*)&length);
    std::cerr<<"acceptfd="<<acceptfd<<std::endl;
//sprintf(m_sendBuf,"%s", "这里是服务端"); // 将客户端的IP地址保存下来
    std::string msg="这里是服务端，收到请回答";
    while(1)
    {
        this->serverSend(msg,acceptfd);
        //auto m= send(acceptfd,msg.c_str(),strlen(msg.c_str())+1,0);
        //std::cerr<<"sendfd="<<m<<std::endl;
        //auto n= recv(acceptfd,m_recvBuf,100,0);
    // std::string s=m_recvBuf;
        //std::cerr<<"recvfd="<<n<<std::endl;
        //std::cerr<<"服务端m_recvBuf="<<m_recvBuf<<std::endl;
        std::string recvmsg;
        this->serverRecv(recvmsg,acceptfd);
         ret=true;
    }
   return ret;
}
bool Server::serverSend(std::string &msg, int acceptfd)
{
    bool ret=false;
   int m= send(acceptfd,msg.c_str(),strlen(msg.c_str())+1,0);
   if(m<0)
   {
       std::cerr<<"服务端send失败"<<std::endl;
   }
   else
   {
       std::cerr<<"服务端send发送长度为"<<m<<std::endl;
        ret=true;
   } 
   return ret;
}
bool Server::serverRecv(std::string &msg,int acceptfd)
{
    bool ret=false;

	std::vector<char> recvBuff;
    int m=recv(acceptfd,m_recvBuf,strlen(m_recvBuf)+1,0);
    if(m<0)
   {
       std::cerr<<"服务端recv失败"<<std::endl;
   }
   else
   {
       std::cerr<<"服务端recv接受长度为"<<m<<std::endl;
       int vectorsize=recvBuff.size();
       
        msg=std::string(m_recvBuf);
       std::cerr<<"服务端recv的msg="<<msg<<std::endl;
       ret=true;
   }
 return ret;
}