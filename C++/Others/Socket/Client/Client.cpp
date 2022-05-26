#include"Client.h"
Client::Client(/* args */)
{
    m_socketfd = -1;
	m_port = 0;
	m_address = "";
}

Client::~Client()
{
    exit();
}
bool Client::Setup(std::string ip, int port,int protocolType)
{
    if(m_socketfd==-1)
    {
        m_socketfd=socket(AF_INET,(IPPROTO_TCP == protocolType)? SOCK_STREAM : SOCK_DGRAM,protocolType);
        if(m_socketfd<0)
        {
            std::cerr<<"客户端socket 创建失败"<<std::endl;
        }
        if((signed)inet_addr(ip.c_str()) == -1)
        {
            struct hostent *hostent;
            struct in_addr **addr_list;
            if((hostent = gethostbyname(ip.c_str() ) ) == NULL)
            {
                herror("客户端gethostbyname");
                std::cerr<<"客户端Failed to resolve hostname\n";
                return false;
            }
            addr_list = (struct in_addr **) hostent->h_addr_list;
            for(int i = 0; addr_list[i] != NULL; i++)
            {
                m_server.sin_addr = *addr_list[i];
                break;
            }
        }
        else
        {
            m_server.sin_addr.s_addr = inet_addr( ip.c_str() );
        }
        m_server.sin_family = AF_INET;
        m_server.sin_port = htons( port );
        if (connect(m_socketfd , (struct sockaddr *)&m_server , sizeof(m_server)) < 0)
        {
            perror("客户端connect failed. Error");
            return false;
        }
        return true;
    }
}
bool Client::Send(std::string data)
{
	if(m_socketfd != -1) 
	{
		if( send(m_socketfd , data.c_str() , strlen( data.c_str() ) , 0) < 0)
		{
			std::cerr << "客户端Send failed : " << data << std::endl;
			return false;
		}
	}
	else
		return false;
	return true;
}

std::string Client::receive(int size)
{
  	char buffer[size];
	memset(&buffer[0], 0, sizeof(buffer));

  	std::string reply;
	if( recv(m_socketfd , buffer , size, 0) < 0)
  	{
	    	std::cerr << "客户端receive failed!" << std::endl;
		return nullptr;
  	}
	buffer[size-1]='\0';
  	reply = buffer;
  	return reply;
}

std::string Client::read()
{
  	char buffer[1] = {};
  	std::string reply;
  	while (buffer[0] != '\n') {
    		if( recv(m_socketfd , buffer , sizeof(buffer) , 0) < 0)
    		{
      			std::cerr << "客户端receive failed!" <<std:: endl;
			return nullptr;
    		}
		reply += buffer[0];
	}
	return reply;
}

void Client::exit()
{
    close( m_socketfd );
}
