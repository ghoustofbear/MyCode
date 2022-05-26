#include"Server.h"
Server::Server(/* args */)
{
     Server::m_last_closed=-1;
     Server::m_isonline=false;
     
}

Server::~Server()
{
    for(int i=0;i<newSocket.size();i++)
    {
        detach(i);
    }
    closed();
}
void  Server::AcceptTask(descript_socket argv)
{
    try
    {
        /* code */

        int n=-2;
        struct descript_socket desc = (struct descript_socket) argv;
        
        std::  cerr << "服务端open client[ id:"<< desc.id <<" ip:"<< desc.ip <<" socket:"<< desc.socketfd<<" send:"<< desc.enable_message_runtime <<" ]" << std::endl;
        while(1)
        {
            n = recv(desc.socketfd, msg, MAXPACKETSIZE, 0);
            if(n != -1) 
            {
                if(n==0)
                {
                    Server::m_isonline=true;
                    
                    std::cerr << "服务端close client[ id:"<< desc.id <<" ip:"<< desc.ip <<" socket:"<< desc.socketfd<<" ]" << std::endl;
                    Server::m_last_closed = desc.id;
                    close(desc.socketfd);

                    int id = desc.id;
                    auto new_end = std::remove_if(newSocket.begin(), newSocket.end(),
                                                        [id](descript_socket device)
                                                            { return device.id == id; });
                    newSocket.erase(new_end, newSocket.end());

                    if(m_clientNum>0) 
                    {
                        m_clientNum--;
                    }
                    break;
                }
                msg[n]=0;
                desc.message = std::string(msg);
                std::lock_guard<std::mutex> guard(m_mutex);
                Message.emplace_back( desc );
            }
            usleep(600);
        }
     
        std::cerr << "exit thread: " << std::this_thread::get_id() << std::endl;
       

        

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
       
    }
    
   
}

bool Server::SetUp(std::string ip,int port,int protocolType)
{
    try
    {
        
            /* code */
            //1.创建一个socket
        int m_sockfd=socket(AF_INET,(IPPROTO_TCP == protocolType)? SOCK_STREAM : SOCK_DGRAM,protocolType);

        if(m_sockfd<0)
        {
            std::cerr<<"服务端socket create fail"<<std::endl;
            return false;
        }
        else
        {
            std::cerr<<"服务端socket创建"<<std::endl;
        }
        //2.准备通讯地址（必须是服务器的）的IP
        memset(&m_serverAddress,0,sizeof(m_serverAddress));
        m_serverAddress.sin_family = AF_INET;
        m_serverAddress.sin_port = htons(port);//将一个无符号短整型的主机数值转换为网络字节顺序，即大尾顺序(big-endian)
        m_serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());//net_addr方法可以转化字符串，主要用来将一个十进制的数转化为二进制的数，用途多于ipv4的IP转化。
        //3.bind()绑定
        //参数一：0的返回值（socket_fd）
        //参数二：(struct sockaddr*)&addr 前面结构体，即地址
        //参数三: addr结构体的长度
        int bind_fd=bind(m_sockfd,(struct sockaddr*)&m_serverAddress,sizeof(m_serverAddress));
        if(bind_fd<0)
        {
            std::cerr<<"服务端bind绑定失败"<<std::endl;
            return false;
        }
        else
        {
            
            std::cerr<<"服务端bind绑定成功"<<std::endl;
        }
        
        //4.监听客户端listen()函数
    //参数二：进程上限，一般小于30
        int listen_fd=listen(m_sockfd,30);
        if(listen_fd<0)
        {
            std::cerr<<"服务端listen error"<<std::endl;
            return false;
        }
        m_isonline=true;
        m_clientNum=0;
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
   
}

bool Server::Accepted()
{
    try
    {
        //5.等待客户端的连接accept()，返回用于交互的socket描述符
        /* code */
        memset(&m_clientAddress,0,sizeof(m_clientAddress));
        socklen_t clientAddressLength=sizeof(m_clientAddress);
        descript_socket des_socket;
        des_socket.socketfd=accept(m_sockfd,(struct sockaddr*)&m_clientAddress,&clientAddressLength);
        des_socket.id=m_clientNum;
        des_socket.ip=inet_ntoa(m_clientAddress.sin_addr);
        newSocket.emplace_back(des_socket);
        std::cerr << "服务端accept client[ id:" << newSocket[m_clientNum].id<< 
                        " ip:" << newSocket[m_clientNum].ip << 
                    " handle:" << newSocket[m_clientNum].socketfd << " ]" << std::endl;
        if(newSocket[m_clientNum].socketfd<0)
        {
            std::cerr<<"服务端newSocket"<<"clientNum="<<m_clientNum<<"accept失败"<<std::endl;
        }
         std::thread t(&Server::AcceptTask,this,newSocket[m_clientNum]);
      
       
        
        m_isonline=true;
        m_clientNum++;
        return true;

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
    
}
std::vector<descript_socket> Server::getMessage()
{  
 
        /* code */
          std::lock_guard<std::mutex> guard(m_mutex);
            return Message; 

    


}
bool Server::Send(std::string msg,int id)
{
    try
    {
        /* code */
         int Sendfd=send(newSocket[id].socketfd,msg.c_str(),sizeof(msg),0);
            if(Sendfd<0)
            {
                std::cerr<<"服务端Sendfd<0，send失败"<<std::endl;
                return false;
            }
            return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
  
}
std::string Server::get_ip_addr(int id)
{
	return newSocket[id].ip;
}

bool Server::is_online() 
{
	return m_isonline;
}

void Server::detach(int id)
{
	close(newSocket[id].socketfd);
	newSocket[id].ip = "";
	newSocket[id].id = -1;
	newSocket[id].message = "";
} 

void Server::closed() 
{
   close(m_sockfd);
}
int Server::get_last_closed_sockets()
{
	return Server::m_last_closed;
}
void Server::clean(int id)
{
	
	memset(msg, 0, MAXPACKETSIZE);
}

