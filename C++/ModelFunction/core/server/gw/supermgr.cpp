#include "supermgr.h"
#include "onuiupdate.h"
#include "tls_error.h"
#include "sipua.h"
#include "hs_superior.h"
#include "cfg_sup_platform.h"
#include "cfg_camera_belong_platform.h"
#include "json/json.h"
#include "gwb_superior.h"
#include "gwa_superior.h"
#include "hik_superior.h"
#include "ut_superior.h"
#include "gb28181_superior.h"

SuperMgr::SuperMgr()
{
    m_ui    = NULL;
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    superCfg->addObserver(this); 
}

SuperMgr::~SuperMgr()
{
    AutoLock locker(_mtx);
    for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
    {
        delete (*i);
    }
    clear();
}

int
SuperMgr::start()
{
    string json;
    superCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    int    result = 0;

    if (reader.parse(json, rvalue, false))
    {
        for (uint32_t i = 0; i < rvalue.size(); i++)
        {
            Superior * super =
                addSuperior(
                    rvalue[i]["addrcode"].asString(),
                    rvalue[i]["name"].asString(),
                    rvalue[i]["protocol"].asInt(),
                    rvalue[i]["username"].asString(),
                    rvalue[i]["password"].asString(),
                    rvalue[i]["host"].asString(),
                    rvalue[i]["sipPort"].asInt(),
                    rvalue[i]["rtspPort"].asInt(),
                    rvalue[i]["httpPort"].asInt(),
                    rvalue[i]["expires"].asInt(),
                    rvalue[i]["ifupIp"].asString(),
                    rvalue[i]["isregister"].asInt(),
                    rvalue[i]["ivauselocal"].asInt()
                    );

            if (super)
            {
                result += super->start();
            }
        }
    }

    return result;
}

void
SuperMgr::stop()
{
    {
        AutoLock locker(_mtx);
        for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
        {
            (*i)->stop();
        }
    }
}

Superior *
SuperMgr::addSuperior(string addrcode,
                      string name,
                      int    protocol,
                      string user,
                      string pswd,
                      string host,
                      int    sipPort,
                      int    rtspPort,
                      int    httpPort,
                      int    expires,
                      string ifupIp,
                      int    bregister,
                      int    ivauselocal
                      )
{

    Superior * super = NULL;

    AutoLock locker(_mtx);

    switch (protocol)
    {
    case SipUA::P_GWB_HS:
        super = new HSSuperior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    case SipUA::P_GWB:
        super = new GwbSuperior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    case SipUA::P_GWA:
        super = new GwaSuperior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    case SipUA::P_GWB_HIK:
        super = new HikSuperior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    case SipUA::P_GWB_UT:
        super = new UtSuperior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    case SipUA::P_GB28181:
        super = new GB28181Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp, bregister, ivauselocal);
        break;

    default:
        break;
    }

    if (super == NULL)
        return NULL;

    push_back(super);

    return super;
}

int
SuperMgr::delSuperior(string addrcode)
{
    list<Superior *>::iterator iter;

    AutoLock lock(_mtx);

    if (empty())
    {
        LOGW_MSV("Superior list is empty!");

        return -1;
    }

    Superior * super = NULL;

    for (iter = begin(); iter != end(); ++iter)
    {
        super = *iter;
        ASSERT(super);

        if (super->addrcode() == addrcode)
        {
            break;
        }
    }

    if (iter == end())
    {
        LOGW_MSV("Superior %s not exsit!", addrcode.c_str());

        return -1;
    }

    delete super;
    erase(iter);

    return 0;

}

Superior* 
SuperMgr::findSuperiorByGucode(string gucode)
{
    string json;
    string cond = "camera_addrcode=\"" + gucode + "\"";
    SHL_Json::Reader reader;
    SHL_Json::Value value;

    if (0 == cameraBelongPlatformCfg->query(json, cond))
    {
        reader.parse(json, value, false);
        string addcode = value[0]["platform_addrcode"].asString();

        return findSuperior(addcode);

    }
    else
    {
        return NULL;
    }

}

Superior *
SuperMgr::findSuperior(string addrcode)
{
    AutoLock locker(_mtx);

    for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
    {
        //前10位唯一确定站
        if (strncmp((*i)->addrcode().c_str(),addrcode.c_str(), 10) ==0 )
        {
            return *i;
        }
    }

    ERRE_MSV("superior %s is not exist.", addrcode.c_str());

    return NULL;
}

Superior *
SuperMgr::findSuperior(string host, int port)
{
    AutoLock locker(_mtx);

    for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
    {
        if ((*i)->isSame(host, port))
        {
            return *i;
        }
    }

    ERRE_MSV("superior host: %s, port: %d is not exist.", host.c_str(), port);

    return NULL;
}

void SuperMgr::push_alarm(string &json, void *data, uint32_t size)
{
	list<Superior *>::iterator iter;

	AutoLock lock(_mtx);
	for (iter = begin(); iter != end(); ++iter)
	{
		TriggerListener *sup = *iter;
		sup->onAIYX(json, data, size);
	}
}

void
SuperMgr::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return;
    }

    Superior * super = NULL;
    if (O_ADD == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        super = addSuperior(
                    rvalue["addrcode"].asString(),
                    rvalue["name"].asString(),
                    rvalue["protocol"].asInt(),
                    rvalue["user"].asString(),
                    rvalue["pswd"].asString(),
                    rvalue["host"].asString(),
                    rvalue["sipPort"].asInt(),
                    rvalue["rtspPort"].asInt(),
                    rvalue["httpPort"].asInt(),
                    rvalue["expires"].asInt(),
                    rvalue["ifupIp"].asString(),
                    rvalue["isregister"].asInt(),
                    rvalue["ivauselocal"].asInt()
                );

        if (super)
        {
            super->start();
        }
    }
    else if (O_MODIFY == type)
    {
        delSuperior(rvalue["id"].asString());

        super = addSuperior(
                    rvalue["addrcode"].asString(),
                    rvalue["name"].asString(),
                    rvalue["protocol"].asInt(),
                    rvalue["user"].asString(),
                    rvalue["pswd"].asString(),
                    rvalue["host"].asString(),
                    rvalue["sipPort"].asInt(),
                    rvalue["rtspPort"].asInt(),
                    rvalue["httpPort"].asInt(),
                    rvalue["expires"].asInt(),
                    rvalue["ifupIp"].asString(),
                    rvalue["isregister"].asInt(),
                    rvalue["ivauselocal"].asInt()
                );

        if (super)
        {
            super->start();
        }
    }
    else if (O_DELETE == type)
    {
        delSuperior(rvalue["id"].asString());
    }
}

void
SuperMgr::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
SuperMgr::updateUi()
{
    if (m_ui && size() > 0)
    {
        string json;
        {
            AutoLock locker(_mtx);
            for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
            {
                /* just for test */
                json += (*i)->addrcode();
                json += ":";
                json += (*i)->name();
                json += ":";
                json += std::to_string((*i)->protocol());
                json += ", ";
            }
        }

        m_ui->onUpdate(json, NULL, 0);
    }
}

void
SuperMgr::timerdriver()
{
    AutoLock locker(_mtx);
    for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->timerdriver();
    }
}

void
SuperMgr::setregisterstatus(string &addrcode, bool bregister)
{
    Superior *sup = findSuperior(addrcode);
    if (sup)
    {
        sup->setregisterstatus(bregister);
    }
}

void
SuperMgr::setuseivalocalcfg(string &addrcode, bool bUseivalocal)
{
    Superior *sup = findSuperior(addrcode);
    if (sup)
    {
        sup->setuseivalocalcfg(bUseivalocal);
    }
}

int 
SuperMgr::push(const string &type, const string &json)
{
    AutoLock locker(_mtx);
    for (list<Superior *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->push(type, json);
    }

    return 0;
}