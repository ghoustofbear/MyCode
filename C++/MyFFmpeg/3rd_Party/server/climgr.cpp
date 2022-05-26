#include "climgr.h"
#include "priv_client.h"
#include "priv_server.h"
#include "session.h"
#include "tls_error.h"
#include "websock_client.h"
#include "websock_server.h"

CliMgr::CliMgr()
{
}

CliMgr::~CliMgr()
{
}

int
CliMgr::start()
{
    int result = -1;

    m_collecThread.setEntry(threadEntry, this, T_COLLECTOR);
    result += m_collecThread.start();

    m_timerThread.setEntry(threadEntry, this, T_TIMER);
    result += m_timerThread.start();

    return result;
}

void
CliMgr::stop()
{
    m_collecThread.stop();

    release();

    m_timerThread.stop();
}

Client *
CliMgr::addClient(Bufferev* bev, int type, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey)
{
    AutoLock locker(_mtx);

    Client * cli = NULL;
    switch (type)
    {
    case Client::T_PRIVATE:
        cli = new PrivClient(bev, name, userId, addrcode, mright, oright, userKey);
        break;

    case Client::T_WEBSOCK:
        cli = new WebSockClient(bev, name, userId, addrcode, mright, oright, userKey); 
        break;

    default:
        break;
    }

    if (!cli)
    {
        return NULL;
    }

    cli->start();
    push_back(cli);

    return cli;
}

Client *
CliMgr::findClient(string key)
{
    AutoLock locker(_mtx);

    if (empty())
    {
        return NULL;
    }

    for (list<Client *>::iterator i = begin(); i != end();  ++i)
    {
        if (key == (*i)->key())
        {
            return *i;
        }
    }

    return NULL;
}

int
CliMgr::startPreview(string &json, string key, Bufferev *bev, uint32_t &code)
{
    AutoLock locker(_mtx);
    list<Client *>::const_iterator i;
    for (i = begin(); i != end();  ++i)
    {
        if (key == (*i)->key())
        {
            break;
        }
    }

    if (i == end())
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        return -1;
    }

    Client *cli = *i;

    if (!cli->hasRight(Client::R_PREVIEW))
    {
        code = 404;
        ERRE_MSV("Permission denied");
        return -1;
    }

    Session *ss = cli->startPreview(json, bev);
    if (!ss)
    {
        code = 405;
        ERRE_MSV("create preview session faild");
        return -1;
    }
    code = 200;

    return 0;
}

int
CliMgr::startPlayback(string &json, string key, Bufferev *bev, uint32_t &code)
{
    AutoLock locker(_mtx);
    list<Client *>::const_iterator i;
    for (i = begin(); i != end(); ++i)
    {
        if (key == (*i)->key())
        {
            break;
        }
    }

    if (i == end())
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        return -1;
    }

    Client *cli = *i;

    if (!cli->hasRight(Client::R_PLAYBACK))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        return -2;
    }



    Session * rs = cli->startPlayback(json, bev);
    if (!rs)
    {
        code = 405;
        ERRE_MSV("create playback session faild");
        return -3;
    }
    code = 200;

    return 0;
}

int
CliMgr::startDownload(string &json, string key, Bufferev *bev, uint32_t &code)
{
    AutoLock locker(_mtx);
    list<Client *>::const_iterator i;
    for (i = begin(); i != end(); ++i)
    {
        if (key == (*i)->key())
        {
            break;
        }
    }

    if (i == end())
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        return -1;
    }

    Client *cli = *i;

    if (!cli->hasRight(Client::R_DOWNLOAD))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        return -2;
    }



    Session * rs = cli->startDownload(json, bev);
    if (!rs)
    {
        code = 405;
        ERRE_MSV("create download session faild");
        return -3;
    }
    code = 200;

    return 0;
}

int
CliMgr::startTalk(string &json, string key, Bufferev *bev, uint32_t &code)
{
    Client *cli = findClient(key);
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        return -1;
    }

    if (!cli->hasRight(Client::R_DUALTALK))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        return -1;
    }

    DSession * rs = cli->startDualTalk(json, bev);
    if (!rs)
    {
        code = 404;
		ERRE_MSV("create talk session faild");
        return -1;
    }

    //privSV->addBuffevListener(bev, rs);
    //rs->setbev(bev);

    code = 200;
    return 0;
}

void
CliMgr::release()
{
    AutoLock locker(_mtx);

    if (empty())
    {
        return;
    }

    for (list<Client *>::iterator i = begin(); i != end(); ++i)
    {
        ASSERT(*i);
        (*i)->stop();
        delete (*i);
    }

    erase(begin(), end());
}

void
CliMgr::releaseDead()
{
    AutoLock locker(_mtx);

    if (empty())
    {
        return;
    }

    for (list<Client *>::iterator i = begin(); i != end();)
    {
        Client * cli = *i;

        if (!cli->active())
        {
            cli->stop();

            delete cli;
            list<Client *>::iterator ti = i++;
            erase(ti);
        }
        else
        {
            i++;
        }
    }
}

int
CliMgr::collector()
{
    uint64_t cnt = 0;
    uint32_t len = 50;

    while (m_collecThread.active())
    {
        if (empty() && (0 == cnt % 5000))
        {
            LOGT_MSV("Client list is empty!");
        }
        else
        {
            releaseDead();
        }

        sys->sleep(len);
        cnt += len;
    }

    return 0;
}

void
CliMgr::onTimer()
{
    /* TODO:  <15-11-17, yourname> */
    uint64_t cnt = 0;
    uint32_t len = 25;

    while (m_timerThread.active())
    {
        if (size() > 0)
        {
            AutoLock locker(_mtx);
            for (list<Client *>::const_iterator i = begin(); i != end(); ++i)
            {
                (*i)->onTimer();
            }
        }

        sys->sleep(len);
        cnt += len + 2;
    }

}

void *
CliMgr::threadEntry(ThreadInfo *arg)
{
    CliMgr * mgr = (CliMgr *)arg->data;

    switch (arg->id)
    {
    case T_COLLECTOR:
        mgr->collector();
        break;

    case T_TIMER:
        mgr->onTimer();
        break;
    default:
        break;
    }

    return NULL;
}
