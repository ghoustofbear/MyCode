#include "server.h"

Server::Server(int transport, string bindIp, int port)
{
    m_transport  = transport;
    m_bindIp     = bindIp;
    m_listenPort = port;
}

Server::~Server()
{}

int
Server::type()
{
    return m_type;
}

int
Server::status()
{
    return m_status;
}

int
Server::port()
{
    return m_listenPort;
}

void
Server::onTimer()
{

}
