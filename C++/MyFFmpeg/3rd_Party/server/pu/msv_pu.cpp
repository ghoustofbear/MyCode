#include "msv_pu.h"
#include "tls_error.h"
#include "json/json.h"
#include "putree.h"
#include "global_str_def.h"

void *
query(ThreadInfo *arg)
{
    MsvPu * pu = (MsvPu*)(arg->data);

    pu->queryDevice(pu->addrcode());

    return NULL;
}

void
logincb(int code, void* userdata)
{
    MsvPu *pu = (MsvPu *)userdata;

    if (CODE_LOGIN_SUCCESS == code)
    {
        LOGI_MSV("Device: %s login success!", pu->ip().c_str());

        pu->m_login = true;

        pu->writeDevLog(STR_MSV_ON_LINE);

        /* queryDevice */
        pu->m_thread.start();
        pu->m_thread.detach();
    }
    else
    {
        LOGE_MSV("Device: %s login fail, code: %d", pu->ip().c_str(), code);
    }
}

class C2SDKInitializer
{
public:
    C2SDKInitializer()
    {
        if (c2_init() < 0)
        {
            LOGF_MSV("%s", c2_get_error());
            return;
        }
        m_init = true;
    }

    ~C2SDKInitializer()
    {
        c2_uninit();
        m_init = false;
    }

    static bool inited()
    {
        return m_init;
    }

private:
    static bool m_init;
};
bool C2SDKInitializer::m_init = false;

MsvPu::MsvPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_UT;
    m_port     = port;
    m_handle   = NULL;
}

MsvPu::~MsvPu()
{

}

int
MsvPu::start()
{
    static C2SDKInitializer c2sdk;

    if (!c2sdk.inited())
    {
        LOGE_MSV("c2sdk init failed.");
        return -1;
    }

    if (Pu::start() < 0)
    {
        return -1;
    }

    LOGI_MSV("c2sdk version: %s", c2_version());
    m_init = true;

    return 0;
}

void
MsvPu::stop()
{
    m_init = false;
}

int
MsvPu::login()
{
    c2_login_s loginParam =
    {
        (char *)m_ip.c_str(),
        m_port,
        (char *)m_user.c_str(),
        (char *)m_pswd.c_str(),
        1,
        0,
        NULL
    };

    c2_ex_param_s exParam[2] =
    {
        {(char *)"key1", (char *)"value1"},
        {(char *)"key2", (char *)"value2"},
    };

    loginParam.exLen   = sizeof(exParam);
    loginParam.exParam = exParam;

    m_handle = c2_login(&loginParam, logincb, this);
    if (!m_handle)
    {
        ERRE_MSV("%s", c2_get_error());
        return -1;
    }
    
    m_thread.setEntry(query, this, 0);
    
    return 0;
}

void
MsvPu::logout()
{
    if (m_handle)
    {
        c2_logout(m_handle);
        m_handle = NULL;
    }

    m_login = false;
    writeDevLog(STR_MSV_OFF_LINE);

}

bool
MsvPu::validate(string gucode)
{
    return true;
}


int
MsvPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
    if (!m_login)
    {
        return 0;
    }

    c2_ptz_s ptzParam;
    memset(&ptzParam, 0, sizeof(c2_ptz_s));

    ptzParam.addrcode = (char *)addrcode.c_str();

    ptzParam.speed    = speed;
    ptzParam.stop     = stop;
    ptzParam.preset   = preset;

    if (c2_ptz_control(m_handle, cmd, &ptzParam) < 0)
    {
        ERRE_MSV("%s", c2_get_error());
        return -1;
    }

    return 0;
}

int
MsvPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    if (!m_login)
    {
        return 0;
    }

    c2_query_record_s query;
    memset(&query, 0, sizeof(query));

    query.addrcode  = (char *)addrcode.c_str();
    query.type      = type;
    query.fromIndex = fromIndex;
    query.toIndex   = toIndex;
    query.start     = start;
    query.end       = end;

    char buf[512*1024];
    int  size = sizeof(buf);

    int ret = c2_query_record_v2(m_handle, &query, buf, &size);

    if (m_clienQueryRecord_cb)
    {
		m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }

    if (ret < 0)
    {
        ERRE_MSV("%s", c2_get_error());

        return -1;
    }

    json = buf;

    return 0;
}

int
MsvPu::queryDevice(string addrcode)
{
    c2_query_devinfo_s info =
    {
        100,
        (char *)m_host.c_str(),
        m_port,
        (char *)addrcode.c_str(),
        0,
        NULL
    };

    char json[1024 * 20] = {0};
    int  size = sizeof(json);

    if (c2_query_device_v2(m_handle, &info, json, &size) < 0)
    {
        LOGE_MSV(c2_get_error());
        return -1;
    }

    LOGW_MSV("json: %s, size: %d", json, size);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        LOGE_MSV("parse json fail.");
        return -1;
    }


    SHL_Json::Value &devices = rvalue["devices"];

    if (addrcode == m_addrcode)
    {
        nodedata_t *data = (nodedata_t *)putree->get_data(m_addrcode);
        data->subnum = devices.size();
    }

    for (int i = 0; i < (int)devices.size() ; ++i)
    {
        string code    = devices[i]["code"].asString();
        string name    = devices[i]["name"].asString();
        string host    = devices[i]["host"].asString();
        int    port    = devices[i]["port"].asInt();
        int    channel = devices[i]["channel"].asInt();
        int    subnum  = devices[i]["subnum"].asInt();
        int    type    = devices[i]["type"].asInt();
        int    status  = devices[i]["status"].asInt();
        int    abt     = devices[i]["recvCall"].asInt();
		int    mantype = devices[i]["protocol"].asInt();
		nodedata_t *data = new nodedata_t(1, NULL, name, host, port, channel, subnum, status, abt, type, mantype);
        putree->insert_child(addrcode, code, data);        
    }

    for (int i = 0; i < (int)devices.size() ; ++i)
    {
        int subnum = devices[i]["subnum"].asInt();
        if (0 == subnum)
        {
            continue;
        }

        queryDevice(devices[i]["code"].asString());
    }

    return 0;
}

void
MsvPu::onTimer()
{
    /* TODO:  <06-01-18, yourname> */

    Pu::onTimer();
}

bool
MsvPu::validate(int channelNo)
{
    return true;
}

int
MsvPu::ptzCtrl(int cmd, int channelNo,   int speed, int preset, bool stop)
{
    return 0;
}

int
MsvPu::queryRecord(int channelNo,  int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    return 0;
}

int
MsvPu::syncTime(datetime_s &dt, const string &tzone)
{
    return 0;
}
