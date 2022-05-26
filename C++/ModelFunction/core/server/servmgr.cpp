#include "servmgr.h"
#include "version.h"
#include "icommon.h"
#include "sys.h"
#include "tmulti_queue.h"
#include "mempool.h"
#include "utility.h"
#include "md5.h"
#include "json/json.h"

#include "gumgr.h"
#include "onuiupdate.h"
#include "gu.h"
#include "system.h"
#include "preset.h"
#include "platform.h"
#include "groupmgr.h"
#include "usermgr.h"
#include "gumgr.h"
#include "sumgr.h"
#include "priv_server.h"
#include "hs_sipua.h"
#include "rtspserver.h"
#include "httpserver.h"
#include "gw_sipua.h"
#include "putree.h"
#include "climgr.h"
#include "cfg_server.h"
#include "log.h"
#include "websock_server.h"
#include "supermgr.h"
#include "gsessmgr.h"
#include "superior.h"
#include "ntp.h"
#include "openapi_server.h"
#include "pu.h"
#include "gb28181_sipua.h"
#include "gsessmgr.h"

System     * systm    = NULL;
Preset	   * pset     = NULL;
Platform   * platform = NULL;
GroupMgr   * groupMgr = NULL;
UserMgr    * userMgr  = NULL;

NtpTime    * ntpTime  = NULL;

PrivServer * privSV   = NULL;

SipUA      * sipUA    = NULL;
RTSPServer * rtspSV   = NULL; 
HttpServer * httpSV   = NULL;
WebSocketServer * websockSV = NULL;
OpenAPIServer   * openApiSVR = NULL;
PuTree     * putree   = NULL;
CliMgr     * cliMgr   = NULL;
SuperMgr   * superMgr = NULL;
GSessMgr   * gsessMgr = NULL;

tmulti_queue<string> * mutiQu = NULL;

LogMan* logOb = NULL;

ServMgr *
ServMgr::instance()
{
    static ServMgr mgr;

    return &mgr;
}

char *
ServMgr::version()
{
    static char ver[100] = {0};
    snprintf_s(ver, sizeof(ver), "%s-%s", VERSION_TAG, VERSION_SHA1);
    return ver;
}

char *
ServMgr::buildPTS()
{
    return (char *)VERSION_BUILD_PTS;
}

int
ServMgr::startAll()
{
    if (m_start)
    {
        LOGI_MSV("All have started!");
        return -1;
    }

    systm = new System;
    systm->init();

	pset = new Preset();
	pset->init();

    ntpTime = new NtpTime;
    ntpTime->init();

    platform = new Platform;
    platform->init();

    mutiQu   = new tmulti_queue<string>;
    groupMgr = new GroupMgr;
    userMgr  = new UserMgr;
    putree   = new PuTree;
    cliMgr   = new CliMgr;

    groupMgr->init();
    userMgr ->init();

    logOb = new LogMan;
    logOb->start();

    putree->start();

    cliMgr->start();

	sys->sleep(1000);

	instance()->start();

    m_start = true;

    return 0;
}

void
ServMgr::stopAll()
{
    //instance()->stop();

    if (mutiQu)
    {
        delete mutiQu;
        mutiQu = NULL;
    }


    if (cliMgr)
    {
        cliMgr->stop();
        delete cliMgr;
        cliMgr = NULL;
    }

	instance()->stop();

	if (putree)
	{
		putree->stop();
		delete putree;
		putree = NULL;
	}

	if (logOb)
	{
		logOb->stop();
		delete logOb;
		logOb = NULL;
	}

    if (userMgr)
    {
        delete userMgr;
        userMgr = NULL;
    }

    if (groupMgr)
    {
        delete groupMgr;
        groupMgr = NULL;
    }

    log4x::ilog4x::instance()->stop();
	//stop();
	if (ntpTime)
	{
		delete ntpTime;
		ntpTime = NULL;
	}

	if (systm)
	{
		delete systm;
		systm = NULL;
	}

	if (pset)
	{
		delete pset;
		pset = NULL;
	}

    m_start = false;
}

ServMgr::ServMgr()
{
    m_start       = false;
    m_uiThread    = NULL;
    m_timerThread = NULL;
    m_ui          = NULL;
    m_uimtx       = new Mutex;

    m_islopt      = false;

    servCfg->addObserver(this);

    //shl
    mempool = new Mempool(MAX_MEMPOOL_SIZE*3);
    if (icommon::instance()->start() < 0)
    {
        LOGF_MSV("All have started!");
        abort();
    }

#if _DEBUG
    DEBUG_LEVEL_MSV(LOG_LEVEL_DEBUG);
#else
    DEBUG_LEVEL_MSV(LOG_LEVEL_INFO);
#endif

}

ServMgr::~ServMgr()
{
    icommon::instance()->stop();
}

void
ServMgr::setMessageCallback(pfnMsgcallback pfnMsgCb, void *pUser)
{
    if (putree)
    {
        putree->setMessageCallback(pfnMsgCb, pUser);
    }
}

void
ServMgr::setregisterstatus(string &addrcode, bool bregister)
{
    if (superMgr)
    {
        superMgr->setregisterstatus(addrcode, bregister);
    }
}

void
ServMgr::setuseivalocalcfg(string &addrcode, bool bUseivalocal)
{
    if (superMgr)
    {
        superMgr->setuseivalocalcfg(addrcode, bUseivalocal);
    }
}

int
ServMgr::start()
{
    if (!gsessMgr)
    {
        gsessMgr = new GSessMgr;
    }

    string json;
    servCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    for (uint32_t i = 0; i < rvalue.size(); i++)
    {
        bool   enable = rvalue[i]["enable"].asBool();
        int    type   = rvalue[i]["type"].asInt();
        string ip     = rvalue[i]["bindIp"].asString();
        int    port   = rvalue[i]["listenPort"].asInt();

        if (!enable)
        {
            continue;
        }

        Server * serv = NULL;
        if (0 == type)
        {
            privSV = new PrivServer("", port);
            serv   = privSV;
        }
        else if (1 == type)
        {
            if("gb28181" == GetSipProtocol())
            {
                sipUA = new GB28181_SipUA(IPPROTO_UDP, "", platform->sport());
            }
            else
            {
                sipUA = new GWSipUA(IPPROTO_UDP, "", platform->sport());
            }
            sipUA->start();
            
            rtspSV = new RTSPServer(0, "", platform->rport());
            rtspSV->start();

            httpSV = new HttpServer(0, "", platform->hport());
            httpSV->start();

            push_back(sipUA);
            push_back(rtspSV);
            push_back(httpSV);
            continue;
        }
        else if (2 == type)
        {
#if 0
            int sipPort  = 5070;
            /* int sipPort  = platform->sport(); */
            sipUA = new HSSipUA(IPPROTO_UDP, "", sipPort);
            sipUA->start();

            rtspSV = new RTSPServer(0, "", platform->rport());
            rtspSV->start();

            push_back(sipUA);
            push_back(rtspSV);
#endif
            continue;
        }
        else if (3 == type)
        {
            websockSV = new WebSocketServer("", port);
            serv = websockSV;
        }
		else if (4 == type)
		{
			openApiSVR = new OpenAPIServer("", port);
			serv = openApiSVR;
        }
        else if (6 == type)
        {
            sipUA = new GB28181_SipUA(IPPROTO_UDP, "", platform->sport());
            sipUA->start();

            rtspSV = new RTSPServer(0, "", platform->rport());
            rtspSV->start();

            httpSV = new HttpServer(0, "", platform->hport());
            httpSV->start();

            push_back(sipUA);
            push_back(rtspSV);
            push_back(httpSV);
            continue;
        }
        else
        {
            continue;
        }

        serv->start();
        push_back(serv);
    }

    if (!m_uiThread)
    {
        m_uiThread = new Thread;
        m_uiThread->setEntry(threadProc, this, T_UPDATE_UI);
        m_uiThread->start();
    }

    if (!m_timerThread)
    {
        m_timerThread = new Thread;
        m_timerThread->setEntry(threadProc, this, T_TIMER);
        m_timerThread->start();
    }

    if (!superMgr)
    {
        superMgr = new SuperMgr;
        superMgr->start();
    }

    return 0;
}

void
ServMgr::stop()
{
	if (gsessMgr)
	{
		delete gsessMgr;
		gsessMgr = NULL;
	}

    if (m_uiThread)
    {
        m_uiThread->stop();
        delete m_uiThread;
        m_uiThread = NULL;
    }

    if (m_timerThread)
    {
        m_timerThread->stop();
        delete m_timerThread;
        m_timerThread = NULL;
    }

    {
        /* AutoLock locker(_mtx); */
        for (list<Server *>::const_iterator i = begin(); i != end(); ++i)
        {
            delete *i;
        }
        clear();
    }

    if (m_uimtx)
    {
        delete m_uimtx;
        m_uimtx = NULL;
    }

	if (superMgr)
	{
		delete superMgr;
		superMgr = NULL;
	}
}

void
ServMgr::setUi(OnUiUpdate *ui)
{
    AutoLock locker(*m_uimtx);
    m_ui = ui;
}

void
ServMgr::setPuUi(OnUiUpdate *ui)
{
    if (putree)
    {
        //AutoLock locker(*m_uimtx);
        putree->setUi(ui);
    }
}

void
ServMgr::setPuUi(const string &addrcode, OnUiUpdate *ui)
{

	if (putree)
	{
		AutoLock locker(*m_uimtx);
		putree->setUi(addrcode, ui);
	}
	//for (list<Pu *>::const_iterator i = begin(); i != end(); ++i)
	//{
	//	if (addrcode == (*i)->addrcode())
	//	{
	//		(*i)->setUi(ui);
	//		break;
	//	}
	//}


}

void
ServMgr::setGuUi(OnUiUpdate *ui, int type)
{
    if ((0 == type) && realGuMgr)
    {
        AutoLock locker(*m_uimtx);
        realGuMgr->setUi(ui);
    }

    if ((1 == type) && recordGuMgr)
    {
        AutoLock locker(*m_uimtx);
        recordGuMgr->setUi(ui);
    }
}

void
ServMgr::setGuUi(const string &addrcode, OnUiUpdate *ui, int type)
{
    if ((0 == type) && realGuMgr)
    {
        AutoLock locker(*m_uimtx);
        realGuMgr->setUi(addrcode, ui);
    }

    if ((1 == type) && recordGuMgr)
    {
        AutoLock locker(*m_uimtx);
        recordGuMgr->setUi(addrcode, ui);
    }
}

void
ServMgr::setUserUi(OnUiUpdate *ui)
{
    if (userMgr)
    {
        AutoLock locker(*m_uimtx);
        userMgr->setUi(ui);
    }
}

void
ServMgr::setUserUi(const string &id, OnUiUpdate *ui)
{
    if (userMgr)
    {
        userMgr->setUi(id, ui);
    }
}

void ServMgr::setSystemStatusUi(OnUiUpdate *ui)
{
    if (systm)
    {
        AutoLock locker(*m_uimtx);
        systm->setUi(ui);
    }
}

void
ServMgr::setSuperUi(OnUiUpdate *ui)
{
#if 0
    if (sipUA && superMgr)
    {
        AutoLock locker(*m_uimtx);
        superMgr->setUi(ui);
    }
#endif
}

int
ServMgr::authent(const string &user, const string &pswd, uint64_t &mright, uint64_t &oright)
{
    bool    relogin  = true;
    string  crypto   = "";
    string  userId   = "";
    string  addrcode = "";
    char    md5[33]  = {0};

    MD5((uint8_t *)pswd.c_str(), (uint8_t *)md5, sizeof(md5));

    return userMgr->authent(user, md5, mright, oright, relogin, userId, addrcode);
}

int
ServMgr::writeLog(const string &json, const string& table)
{
    logOb->writeLog(table, json);
    return 0;
}

void
ServMgr::setlopt(const string &user, bool islopt)
{
    strncpy_s(m_luser, user.c_str(), sizeof(m_luser));
    m_islopt = islopt;
}

void
ServMgr::update(const string &table, const std::string &json, int type)
{

}

void
ServMgr::onTimer()
{
    uint64_t cnt = 0;
    uint32_t len = 25;

    while (m_timerThread->active())
    {
        if (size() > 0)
        {
            string json;
            for (list<Server *>::const_iterator i = begin(); i != end(); ++i)
            {
                (*i)->onTimer();
            }
        }

        if (putree)
        {
#ifndef TIMERITSELF
            putree->onTimer();
#endif
        }

        if (logOb && cnt > 1000)
        {
            cnt = 0;
            logOb->onTimer();
        }

        if (superMgr)
        {
            superMgr->timerdriver();
        }

        sys->sleep(len);
        cnt += len+2;
    }
}

int
ServMgr::updateUi()
{
    uint64_t cnt = 0;
    uint32_t len = 25;

    while (m_uiThread->active())
    {
        /* update ui every 3 second */
        if (0 == cnt % (systm->recycle() * 1000))
        {
            if (m_ui && size() > 0)
            {
                AutoLock locker(*m_uimtx);

                string json;
                for (list<Server *>::const_iterator i = begin(); i != end(); ++i)
                {
                    (*i)->status();
                    (*i)->type();
                }
                m_ui->onUpdate(json, NULL, 0);
            }

            if (putree)
            {
                putree->updateUi();
            }

            if (userMgr)
            {
                userMgr->updateUi();
            }

            if (systm)
            {
                systm->updateUi();
            }

#if 0
            if (superMgr)
            {
                superMgr->updateUi();
            }
#endif
        }

        sys->sleep(len);
        cnt += len;
    }

    return 0;
}

void *
ServMgr::threadProc(ThreadInfo *info)
{
    ServMgr * svmgr = (ServMgr *)(info->data);

    switch (info->id)
    {
    case T_TIMER:
        svmgr->onTimer();
        break;

    case T_UPDATE_UI:
        svmgr->updateUi();
        break;

    default:
        break;
    }

    return NULL;
}

int
ServMgr::QuerySubPlatResouce(string addrcode, int fromIndex, int toIndex, string &json)
{
    Pu *pu = putree->findPu(addrcode);
    if (pu)
    {
        return pu->queryResource(addrcode, fromIndex, toIndex, json);
    }
    return -1;
}