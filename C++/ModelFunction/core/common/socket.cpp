#include "socket.h"
#include "utility.h"

Host::Host(uint32_t ip, int port)
{
    this->ip   = ip;
    this->port = port;
}

Host::Host(string str, int port)
{
    fromStr(str, port);
}

Host::Host()
{
    this->ip   = 0;
    this->port = 0;
}

Host::~Host()
{

}

string
Host::toStr()
{
    char buf[64] = { 0 };
    snprintf_s(buf, 64, "%d.%d.%d.%d:%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, (ip) & 0xff, port);

    string str = buf;

    return str;
}

int
Host::fromStr(const string str, int p)
{
    if ("" == str)
    {
        port = 0;
        ip   = 0;
        return -1;
    }

    char name[128] = { 0 };
    strncpy_s(name, str.c_str(), sizeof(name));
    port = p;

    char *pp = strstr(name, ":");
    if (pp)
    {
        port = atoi(pp + 1);
        pp[0] = 0;
    }

    ip = Socket::getIP(name);

    return 0;
}

string
Host::getIPStr()
{
    char buf[40] = { 0 };
    snprintf_s(buf, 40, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, (ip) & 0xff);
    string str = buf;

    return str;
}


Socket::Socket(): m_host(0, 0)
{
    m_sock         = -1;
    m_readTimeout  = 5000;
    m_writeTimeout = 5000;
}

Socket::~Socket()
{
    /* 析构函数，不能(要)调用虚函数!! */
    //close();
}


uint32_t
Socket::getIP(string hostname)
{
    struct hostent *he;

    he = gethostbyname(hostname.c_str());
    if (!he)
    {
        return 0;
    }

    char* lpAddr = he->h_addr_list[0];
    if (lpAddr)
    {
        struct in_addr  inAddr;
        memmove (&inAddr, lpAddr, 4);

        return ntohl(inAddr.s_addr);
    }

    return 0;
}

void
Socket::setReuse(bool yes)
{
    unsigned long op = yes ? 1 : 0;

    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&op, sizeof(op)) < 0)
    {
        printf("Unable to set REUSE");
    }
}
