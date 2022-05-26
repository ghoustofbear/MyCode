#include "dog.h"
#include "sys.h"
#include "socket.h"

#ifdef _WIN32
static Socket *g_dogSocket = NULL;
#else
#include <sys/un.h>
static int g_dogSocketFd = -1;
#endif

void initDog()
{
#ifdef _WIN32
    if (!g_dogSocket)
    {
        Host rh("127.0.0.1", 9089);
        g_dogSocket = sys->createSocket();
        g_dogSocket->open(rh, IPPROTO_UDP);
    }
#else
    if (-1 == g_dogSocketFd)
    {
        g_dogSocketFd = socket(AF_LOCAL,SOCK_DGRAM,0);
    }
#endif
}

void keepAlive(const char* keyWords)
{
#ifdef _WIN32
    if (g_dogSocket)
    {
        g_dogSocket->writeTo(keyWords, strlen(keyWords), NULL, 0);
    }
#else
    static int conStatus = -1;
    if (conStatus == -1)
    {
        struct sockaddr_un addr = {};
        addr.sun_family = AF_LOCAL;
        strcpy(addr.sun_path,C2WATCHDOG_SOCK_FILE);
        conStatus = connect(g_dogSocketFd,(struct sockaddr*)&addr,sizeof(addr));
    }

    if (-1 != g_dogSocketFd && conStatus == 0)
    {
        write(g_dogSocketFd, keyWords, strlen(keyWords)+1);
    }
#endif
}
