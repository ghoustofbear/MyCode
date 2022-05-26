#include "putree.h"
#include "pu.h"
#include "hik_gu.h"
#include "hik_ehome_gu.h"
#include "hik_ehome_su.h"
#include "hik_record_gu.h"
#include "hik_su.h"
#include "msv_gu.h"
#include "msv_record_gu.h"
#include "gw_record_gu.h"
#include "gb28181_record_gu.h"
#include "helmet_gx_gu.h"
#include "helmet_su.h"
#include "tls_error.h"
#include "socket.h"
#include "streamlistener.h"
#include "triggerlistener.h"
#include "client.h"
#include "global_str_def.h"

#include "cfg_preset.h"
#include "cfg_device.h"


#include "tindy_gu.h"
#include "tindy_record_gu.h"

#include "dahua_gu.h"
#include "dahua_record_gu.h"


#include "helmet_gx_record_gu.h"
#include "huadian_gu.h"

#include "su.h"
#include "gwb_gu.h"
#include "cfg_alarm_log.h"
#include "climgr.h"
#include "cfg_sub_platform.h"
#include "gb28181_gu.h"
#include "gsessmgr.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif


Pu::Pu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int linktype)
{
    m_type     = type;
    m_addrcode = addrcode;
    m_name     = name;
    m_chnNum   = chnNum;
    m_user     = user;
    m_pswd     = pswd;
    m_host     = host;
    m_linktype = linktype;

    m_init     = false;
    m_first    = false;
    m_login    = false;

    Host h(host, m_port);
    m_ip = h.getIPStr();

	memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
    m_LastUpdateChnSt = 0;
    m_clienQueryRecord_cb = NULL;
}

Pu::~Pu()
{
    {
        AutoLock locker(_mtx);
        clear();
    }

    {
        AutoLock locker(m_realGu._mtx);
        for (list<RealGu *>::iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
        {
            delete *i;
        }

        m_realGu.clear();
    }

    {
        AutoLock locker(m_recordGu._mtx);
        for (list<RecordGu *>::iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
        {
            delete *i;
        }

        m_recordGu.clear();
    }

    {
        AutoLock locker(m_dualSu._mtx);
        for (list<Su *>::iterator i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
        {
            delete *i;
        }

        m_dualSu.clear();
    }
    {
        AutoLock locker(m_mapAlarm._mtx);
        m_mapAlarm.clear();
    }
}

bool
Pu::online()
{
    return m_login;
}

string
Pu::name()
{
    return m_name;
}

string
Pu::host()
{
    return m_host;
}

string
Pu::ip()
{
    return m_ip;
}

int
Pu::port()
{
    return m_port;
}

string
Pu::addrcode()
{
    return m_addrcode;
}

int
Pu::channels()
{
    nodedata_t * data = (nodedata_t *)putree->get_data(m_addrcode);
    if (!data)
    {
        return -1;
    }

    m_chnNum = data->subnum;
    return m_chnNum;
}

int
Pu::protocol()
{
    return m_protocol;
}

int                
Pu::linktype()
{
    return m_linktype;
}

int
Pu::type()
{
    return m_type;
}

string
Pu::userName()
{
    return m_user;
}
string
Pu::passWord()
{
    return m_pswd;
}

string
Pu::gucode(int channelNo)
{
    string child;
	nodedata_t * data = NULL; 
	int chidNum = putree->child_num(m_addrcode);
	for (int i = 0; i < chidNum;++i)
	{
		data = (nodedata_t *)putree->get_childata(m_addrcode, i, child);
		if (!data)
		{
			return "";
		}

		if (data->channel == channelNo)
			return child;
	}


    return "";
}

int
Pu::guchannelFromName(string name)
{
    for (int i = 0; i < channels(); ++i)
    {
        if (name == guname(i))
        {
            return i;
        }

    }
    return -1;
}

string
Pu::guname(int channelNo)
{
	string child;
	nodedata_t * data = NULL;
	int chidNum = putree->child_num(m_addrcode);
	for (int i = 0; i < chidNum; ++i)
	{
		data = (nodedata_t *)putree->get_childata(m_addrcode, i, child);
		if (!data)
		{
			return "";
		}

		if (data->channel == channelNo)
			return data->name;
	}


	return "";
}

string
Pu::guname(string gucode)
{
    nodedata_t * data = (nodedata_t *)putree->get_data(gucode);
    if (!data)
    {
        return "";
    }

    return data->name;
}

int
Pu::guchannel(string gucode)
{
    nodedata_t * data = (nodedata_t *)putree->get_data(gucode);
    if (!data)
    {
        return -1;
    }

    return data->channel;
}

int
Pu::start()
{
    if (protocol() == PROTO_TYPE_GW_B_UT||
        protocol() == PROTO_TYPE_GB28181)
    {
        //query sub resource from db
        string json;
        suberCamInfoCfg->query(json, addrcode());

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (reader.parse(json, rvalue, false) && rvalue.isArray())
        {
            for (uint32_t i = 0; i < rvalue.size(); i++)
            {
                nodedata_t *data = new nodedata_t(3, NULL, rvalue[i]["camera_name"].asString(), m_host, m_port, this->channels(),
                    0, 1, 0, 0, protocol());
                putree->insert_child(m_addrcode, rvalue[i]["camera_addrcode"].asString(), data);
            }
        }

        m_threadLogin.setEntry(threadEntry, this, T_PU_LOGIN);
        return m_threadLogin.start();
    }
    else
    {
        string json;
        string cond = string("parentCode=\'") + m_addrcode + string("\'");

        deviceCfg->query(json, cond);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (reader.parse(json, rvalue, false) && rvalue.isArray())
        {
            for (uint32_t i = 0; i < rvalue.size(); i++)
            {
                nodedata_t *data = new nodedata_t(3, NULL, rvalue[i]["name"].asString(), m_host, m_port, rvalue[i]["chnNo"].asInt(),
                    0, 0, rvalue[i]["recvCall"].asInt(), rvalue[i]["type"].asInt(), rvalue[i]["protocol"].asInt());
                putree->insert_child(m_addrcode, rvalue[i]["addrcode"].asString(), data);
            }
        }

        //if (m_chnNum > 0)
        {
            m_threadUpdateStatus.setEntry(threadEntry, this, T_UPDATE_CHNSTATUS);
            m_threadUpdateStatus.start();
        }

        m_threadLogin.setEntry(threadEntry, this, T_PU_LOGIN);
        return m_threadLogin.start();
    }
    return 0;
}

void
Pu::stop()
{
    {
        AutoLock locker(m_realGu._mtx);
        for (list<RealGu *>::const_iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
        {
            (*i)->stop();
        }
    }

    {
        AutoLock locker(m_recordGu._mtx);
        for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
        {
            (*i)->stop();
        }
    }

    {
        AutoLock locker(m_dualSu._mtx);
        for (list<Su *>::const_iterator i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
        {
            (*i)->stop();
        }
    }

    m_threadLogin.stop();
    m_threadUpdateStatus.stop();

    m_first = false;
}

bool
Pu::isSame(string host, int port)
{
    Host h(host, port);
    string ip = h.getIPStr();

    return (0 == m_ip.compare(ip)) && (m_port == port);
}

int
Pu::addRealGu(int channelNo, string subHost, int subPort, int streamType, int streamPotoType, StreamListener *listener)
{
    RealGu * gu = NULL;

    list<RealGu *>::const_iterator i;
    {
        AutoLock locker(m_realGu._mtx);

        for (i = m_realGu.begin(); i != m_realGu.end(); ++i)
        {
            if ((*i)->isSame(channelNo, subHost, subPort, streamType, streamPotoType))
            {
                gu = *i;
                break;
            }
        }
    }

    if (i == m_realGu.end())
    {
        switch (m_protocol)
        {
        case PROTO_TYPE_HIK:
            gu = new HikRealGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
        case PROTO_TYPE_DAHUA:
            gu = new DahuaRealGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
        case PROTO_TYPE_TIANDY:
            gu = new TindyRealGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
        case PROTO_TYPE_UT:
            gu = new MsvRealGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
        case PROTO_TYPE_HELMET:
            gu = new HelmetGXGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
		case PROTO_TYPE_EHOME:
			gu = new HikEhomeRealGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
			break;
        case PROTO_TYPE_HUADIAN:
            gu = new HuaDianGu(this, channelNo, subHost, subPort, streamType, streamPotoType);
            break;
        default:
            ERRE_MSV("unknow protocol type.");
            return -1;
        }
        gu->addStreamListener(listener);
        if (gu->start() < 0)
        {
            delete gu;
            return -1;
        }

        AutoLock locker(m_realGu._mtx);
        m_realGu.push_back(gu);
    }
	else
	{
        gu->addStreamListener(listener);

	}

    

    return 0;
}

int
Pu::addRealGu(string addrcode, int streamType, int streamPotoType, StreamListener *listener)
{
    RealGu * gu = NULL;
    
    list<RealGu *>::const_iterator i;
    {
        AutoLock locker(m_realGu._mtx);

        for (i = m_realGu.begin(); i != m_realGu.end(); ++i)
        {
            if ((*i)->isSame(addrcode, streamType, streamPotoType))
            {
                gu = *i;
                break;
            }
        }
    }

    if (i == m_realGu.end())
    {
        switch (m_protocol)
        {
        case PROTO_TYPE_HIK:
			gu = new HikRealGu(this, addrcode, streamType, streamPotoType);      
            break;

        case PROTO_TYPE_DAHUA:
            gu = new DahuaRealGu(this, addrcode, streamType, streamPotoType);
            break;

        case PROTO_TYPE_TIANDY:
            gu = new TindyRealGu(this, addrcode, streamType, streamPotoType);
            break;

        case PROTO_TYPE_UT:
            gu = new MsvRealGu(this, addrcode, streamType, listener->stream(), streamPotoType);
            break;

        case PROTO_TYPE_HELMET:
            gu = new HelmetGXGu(this, addrcode, streamType, streamPotoType);
            break;
		case PROTO_TYPE_EHOME:
			gu = new HikEhomeRealGu(this, addrcode, streamType, streamPotoType);
			break;
        case PROTO_TYPE_HUADIAN:
            gu = new HuaDianGu(this, addrcode, streamType, streamPotoType);
            break;

        case PROTO_TYPE_GW_A:
            /* gu = new GwAGu(this, addrcode, streamType); */
            break;
        case PROTO_TYPE_GW_B:
        case PROTO_TYPE_GW_B_HS:
        case PROTO_TYPE_GW_B_HIK:
        case PROTO_TYPE_GW_B_UT:
            gu = new GwbRealGu(this, addrcode, streamType, 0);
            break;
        case PROTO_TYPE_GB28181:
            gu = new Gb28181bRealGu(this, addrcode, streamType, 0);
            break;
        default:
            ERRE_MSV("unknow protocol type.");
            return -1;
        }

        gu->addStreamListener(listener);
        if (gu->start() < 0)
        {
            delete gu;
            return -1;
        }

        AutoLock locker(m_realGu._mtx);
        m_realGu.push_back(gu);
    }
	else
	{
        gu->addStreamListener(listener);

	}
    

    return 0;
}

void
Pu::delRealGu(int channelNo, string subHost, int subPort, int streamType, int streamPotoType, StreamListener *listener)
{
    AutoLock locker(m_realGu._mtx);

    list<RealGu *>::const_iterator i;
    for (i = m_realGu.begin(); i != m_realGu.end(); ++i)
    {
        if ((*i)->isSame(channelNo, subHost, subPort, streamType, streamPotoType))
        {
            (*i)->delStreamListener(listener);
            break;
        }
    }
    if (i == m_realGu.end())
    {
        LOGE_MSV("delRealGu f");
    }
}

void
Pu::delRealGu(string addrcode, int streamType, StreamListener *listener)
{
    AutoLock locker(m_realGu._mtx);

    list<RealGu *>::const_iterator i;
    for (i = m_realGu.begin(); i != m_realGu.end(); ++i)
    {
        if ((*i)->isSame(addrcode, streamType))
        {
            (*i)->delStreamListener(listener);
            break;
        }
    }
}

void
Pu::delRealGu(string addrcode, int streamType, int streamPotoType, StreamListener *listener)
{
    AutoLock locker(m_realGu._mtx);

    list<RealGu *>::const_iterator i;
    for (i = m_realGu.begin(); i != m_realGu.end(); ++i)
    {
        if ((*i)->isSame(addrcode, streamType, streamPotoType))
        {
            (*i)->delStreamListener(listener);
            break;
        }
    }
    if (i == m_realGu.end())
    {
        LOGE_MSV("delRealGu f");
    }
}

int
Pu::addRecordGu(string filename, string subHost, int subPort, int streamType, StreamListener *listener, int reserve, int mode)
{
    RecordGu * gu = NULL;
    switch (m_protocol)
    {
    case PROTO_TYPE_HIK:
        gu = new HikRecordGu(this, filename, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_DAHUA:
        gu = new DahuaRecordGu(this, filename, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_TIANDY:
        gu = new TindyRecordGu(this, filename, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_HELMET:
        gu = new HelmetGXRecordGu(this, filename, subHost, subPort, streamType, mode);
        break;
    default:
        ERRE_MSV("unknow protocol type.");
        return -1;
    }

    gu->addStreamListener(listener);
    if (gu->start(listener->m_offset) < 0)
    {
        delete gu;
        return -1;
    }

    {
        AutoLock locker(m_recordGu._mtx);
        m_recordGu.push_back(gu);
    }

    

    return 0;
}

int
Pu::addRecordGu(string filename, string addrcode, int streamType, StreamListener *listener, int reserve, int mode)
{
    RecordGu * gu = NULL;

    switch (m_protocol)
    {
    case PROTO_TYPE_HIK:
        gu = new HikRecordGu(this, filename, addrcode, streamType, mode);
        break;
    case PROTO_TYPE_DAHUA:
        gu = new DahuaRecordGu(this, filename, addrcode, streamType, mode);
        break;
    case PROTO_TYPE_TIANDY:
        gu = new TindyRecordGu(this, filename, addrcode, streamType, mode);
        break;
    case PROTO_TYPE_UT:
        gu = new MsvRecordGu(this, filename, addrcode, listener->stream(), streamType, mode);
        break;
    case PROTO_TYPE_HELMET:
        gu = new HelmetGXRecordGu(this, filename, addrcode, streamType, mode);
        break;
    case PROTO_TYPE_GW_B_HS:
    case PROTO_TYPE_GW_B:
    case PROTO_TYPE_GW_B_HIK:
    case PROTO_TYPE_GW_B_UT:
        gu = new GwRecordGu(this, filename, streamType);
        break;
    case PROTO_TYPE_GB28181:
        gu = new GB28181RecordGu(this, filename, addrcode, streamType);
        break;
    default:
        ERRE_MSV("unknow protocol type.");
        return -1;
    }

    gu->addStreamListener(listener);
    if (gu->start(listener->m_offset) < 0)
    {
        delete gu;
        return -1;
    }

    {
        AutoLock locker(m_recordGu._mtx);
        m_recordGu.push_back(gu);
    }

    

    return 0;
}

int
Pu::addRecordGu(string fileurl, int streamType, StreamListener *listener, int reserve, int mode)
{
    if (
        PROTO_TYPE_GW_B_HS  != m_protocol &&
        PROTO_TYPE_GW_B_HIK != m_protocol &&
        PROTO_TYPE_GW_A     != m_protocol &&
        PROTO_TYPE_GW_B     != m_protocol &&
        PROTO_TYPE_UT       != m_protocol &&
        PROTO_TYPE_GW_B_UT != m_protocol &&
        PROTO_TYPE_GB28181 != m_protocol)
    {
        char *p = (char *)strrchr(fileurl.c_str(), '/');
        if (!p)
        {
            ERRE_MSV("invalidate fileurl: %s", fileurl.c_str());
            return -1;
        }

        string filename = p + 1;

        ERRE_MSV("ip:%s, filename %s", m_ip.c_str(), filename.c_str());

		return addRecordGu(filename, "", streamType, listener);
    }

    {
        AutoLock locker(m_recordGu._mtx);

        list<RecordGu *>::const_iterator i;
        for (i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
        {
            if ((*i)->isSame(fileurl))
            {
                ERRE_MSV("Other users are playing back the file");
                return -1;
            }
        }
    }

    RecordGu * gu = NULL;

    switch (m_protocol)
    {
    case PROTO_TYPE_GW_B_HS:
    case PROTO_TYPE_GW_B:
    case PROTO_TYPE_GW_B_HIK:
    case PROTO_TYPE_GW_B_UT:
    case PROTO_TYPE_GB28181:
        gu = new GwRecordGu(this, fileurl, streamType);
        break;
    default:
        ERRE_MSV("unknow protocol type.");
        return -1;
    }

    gu->addStreamListener(listener);
    if (gu->start() < 0)
    {
        delete gu;
        return -1;
    }

    {
        AutoLock locker(m_recordGu._mtx);
        m_recordGu.push_back(gu);
    }

    

    return 0;
}

int
Pu::addRecordGu(int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, StreamListener *listener, int reserve, int mode)
{
    RecordGu * gu = NULL;

    switch (m_protocol)
    {
    case PROTO_TYPE_HIK:
        gu = new HikRecordGu(this, channelNo, type, start, end, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_DAHUA:
        gu = new DahuaRecordGu(this, channelNo, type, start, end, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_TIANDY:
        gu = new TindyRecordGu(this, channelNo, type, start, end, subHost, subPort, streamType, mode);
        break;
    case PROTO_TYPE_HELMET:
        gu = new HelmetGXRecordGu(this, channelNo, type, start, end, subHost, subPort, streamType, mode);
        break;
    default:
        ERRE_MSV("unknow protocol type.");
        return -1;
    }

    gu->addStreamListener(listener);
    if (gu->start() < 0)
    {
        delete gu;
        return -1;
    }

    {
        AutoLock locker(m_recordGu._mtx);
        m_recordGu.push_back(gu);
    }

    

    return 0;
}

int
Pu::addRecordGu(string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, StreamListener *listener, int reserve, int mode)
{
    RecordGu * gu = NULL;

    switch (m_protocol)
    {
    case PROTO_TYPE_HIK:
        gu = new HikRecordGu(this, addrcode, type, start, end, streamType, mode);
        break;
    case PROTO_TYPE_DAHUA:
        gu = new DahuaRecordGu(this, addrcode, type, start, end, streamType, mode);
        break;
    case PROTO_TYPE_TIANDY:
        gu = new TindyRecordGu(this, addrcode, type, start, end, streamType, mode);
        break;
    case PROTO_TYPE_UT:
        gu = new MsvRecordGu(this, addrcode, type, start, end, listener->stream(), streamType, mode);
        break;
    case PROTO_TYPE_HELMET:
        gu = new HelmetGXRecordGu(this, addrcode, type, start, end, streamType, mode);
        break;

    case PROTO_TYPE_GB28181:
        gu = new GB28181RecordGu(this, addrcode, type, start, end, streamType, mode);
        break;

    default:
        ERRE_MSV("unknow protocol type.");
        return -1;
    }

    ASSERT(gu);

    gu->addStreamListener(listener);
    if (gu->start() < 0)
    {
        delete gu;
        return -1;
    }

    {
        AutoLock locker(m_recordGu._mtx);
        m_recordGu.push_back(gu);
    }

    

    return 0;
}

void
Pu::delRecordGu(StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);

    list<RecordGu *>::iterator i = m_recordGu.begin();
    for (i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            break;
        }
    }

    if (i == m_recordGu.end())
    {
        return;
    }

    (*i)->delStreamListener(listener);
    (*i)->stop();

    delete *i;
    m_recordGu.erase(i);

    ERRE_MSV("delRecordGu ok this: %0x !!", this);
}

Su*
Pu::addDualTalkGu(int channelNo, string subHost, int subPort)
{
    Su * su = NULL;

    list<Su *>::const_iterator i;
    {
        AutoLock locker(m_dualSu._mtx);

        for (i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
        {
            if ((*i)->isSame(channelNo))
            {
                su = *i;
                ERRE_MSV("Other users are talking with the device");
                return NULL;
                //return su;
            }
        }
    }

    if (i == m_dualSu.end())
    {
        switch (m_protocol)
        {
        case PROTO_TYPE_HIK:
            su = new HikSu(this, channelNo);
            break;
		case PROTO_TYPE_EHOME:
			su = new HikEhomeSu(this, channelNo);
			break;
        case PROTO_TYPE_DAHUA:

            break;
        case PROTO_TYPE_TIANDY:

            break;
        case PROTO_TYPE_UT:

            break;
        case PROTO_TYPE_HELMET:
            su = new HelmetSu(this, channelNo);
            break;
        default:
            ERRE_MSV("unknow protocol type.");
            return NULL;
        }
    }

    ASSERT(su);
    if (su->start() < 0)
    {
        delete su;
        return NULL;
    }

    {
        AutoLock locker(m_dualSu._mtx);
        m_dualSu.push_back(su);
    }

    LOGD_MSV("\n\n\n\t\tadd su successs, channelNo:%d host:%s \n\n\n.", channelNo, m_host.c_str());
    return su;
}

Su*
Pu::addDualTalkGu(string addrcode)
{
    Su * su = NULL;

    list<Su *>::const_iterator i;
    {
        AutoLock locker(m_dualSu._mtx);

        for (i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
        {
            if ((*i)->isSame(addrcode))
            {
                su = *i;
                ERRE_MSV("Other users are talking with the device");
                return NULL;
            }
        }
    }

    if (i == m_dualSu.end())
    {
        switch (m_protocol)
        {
        case PROTO_TYPE_HIK:
            su = new HikSu(this, addrcode);
            break;
		case PROTO_TYPE_EHOME:
			su = new HikEhomeSu(this, addrcode);
			break;
        case PROTO_TYPE_DAHUA:

            break;
        case PROTO_TYPE_TIANDY:

            break;
        case PROTO_TYPE_UT:

            break;
        case PROTO_TYPE_HELMET:
            su = new HelmetSu(this, addrcode);
            break;
        default:
            ERRE_MSV("unknow protocol type.");
            return NULL;
        }
    }

    ASSERT(su);
    if (su->start() < 0)
    {
        delete su;
        return NULL;
    }

    {
        AutoLock locker(m_dualSu._mtx);
        m_dualSu.push_back(su);
    }

    return su;
}

void
Pu::delDualTalkGu(Su* su)
{
    AutoLock locker(m_dualSu._mtx);
    list<Su *>::iterator i = find(m_dualSu.begin(), m_dualSu.end(), su);
    if (i != m_dualSu.end())
    {
        (*i)->stop();
        delete *i;
        m_dualSu.erase(i);
    }
}

void
Pu::delDualTalkGu(int channelNo, string subHost, int subPort)
{
    AutoLock locker(m_dualSu._mtx);

    list<Su *>::const_iterator i;
    for (i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
    {
        if ((*i)->isSame(channelNo))
        {
            (*i)->stop();
            delete *i;
            m_dualSu.erase(i);
            break;
        }
    }
}



void
Pu::doAudioStream(int chn, void* data, int size)
{
    Su *su = getDualSu(chn);
    if (su)
    {
        su->doAudioStream(data, size);
    }
}

void
Pu::addTriggerListener(TriggerListener *listener)
{
    AutoLock locker(_mtx);
    list<TriggerListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
Pu::delTriggerListener(TriggerListener *listener)
{
    AutoLock locker(_mtx);
    list<TriggerListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

int
Pu::playRecord(StreamListener *listener, string scale, string range)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->play(scale, range);
        }
    }

    return -1;
}

int
Pu::pauseRecord(StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->pause();
        }
    }

    return -1;
}

int
Pu::continueRecord(StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->continuex();
        }
    }

    return -1;
}

int
Pu::startRepos(StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->startRepos();
        }
    }

    return -1;
}

int
Pu::setPosRecord(int pos, StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->setPos(pos);
        }
    }

    return -1;
}

int
Pu::setTimeRecord(datetime_s &time, StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->setTime(time);
        }
    }

    return -1;
}

int
Pu::insertReocrdLabel(string name, StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->insertRecordLabel(name);
        }
    }

    return -1;
}

int 
Pu::playFrame(StreamListener* listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->playFrame();
        }
    }

    return -1;
}

int Pu::playNormal(StreamListener *listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->playNormal();
        }
    }

    return -1;
}

int
Pu::playReverse(datetime_s &time, StreamListener* listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->playReverse(time);
        }
    }

    return -1;
}

int
Pu::playForward(datetime_s &time, StreamListener* listener)
{
    AutoLock locker(m_recordGu._mtx);
    for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
    {
        if ((*i)->isSame(listener))
        {
            return (*i)->playFoward(time);
        }
    }

    return -1;
}
void
Pu::notifyAlarm(string &json, void *data, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<TriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onAlarm(json, data, size);
    }
}

void
Pu::notifyYX(string &json, void *data, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<TriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onAIYX(json, data, size);
    }
}

void
Pu::notifyYC(string &json, void *data, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<TriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onAIYC(json, data, size);
    }
}

int
Pu::updateChannel()
{
    time_t tmLastDevStateTime = sys->currentTimeMillis();

     while (m_threadUpdateStatus.active())
     {
         uint64_t cursec = sys->currentTimeMillis();
         if (cursec - m_LastUpdateChnSt > 3000)
         {
             updateChnStatus();
             m_LastUpdateChnSt = cursec;
         }

         if (cursec - tmLastDevStateTime >= 5000)
         {
             uploadDeviceStatus();
             tmLastDevStateTime = cursec;
         }

         sys->sleep(500);
     }
     return 0;
}

void 
Pu::uploadDeviceStatus()
{
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"]    = "1.0";
    value["time"]       = sys->currentTime();
    value["deviceName"] = m_name;
    value["deviceHost"] = m_host;
    value["devicePort"] = m_port;

    SHL_Json::Value item;
    item["type"]        = TriggerListener::TR_DEV_STATUS;
    item["status"]      = (m_login) ? (1) : (0);
    item["name"]        = m_name;
    item["code"]        = m_addrcode;
    value["alarms"].append(item);

    string json = writer.write(value);
    notifyAlarm(json, NULL, 0);
}

void
Pu::writeDevLog(string info)
{
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["name"] = name();
    value["time"] = sys->currentTime();
    value["addrcode"] = addrcode();
    value["status"] = online();
    value["info"] = info;
    std::string json = writer.write(value);
    logOb->writeLog("tbl_device_log", json);
}

void
Pu::writeAlarmLog(const string &json)
{
    logOb->writeLog("tbl_alarm_log", json);
}


int Pu::loginLoop()
{
    while (m_threadLogin.active())
    {
        if (m_init)
        {
            if (!m_first)
            {
                if (0 == login())
                {
                    m_first = true;
                }
            }
        }
        sys->sleep(1000);
    }

    return 0;
}

int        
Pu::queryPreset(int channelNo, string &json)
{
    string addrcode = gucode(channelNo);
    return queryPreset(addrcode, json);
}

int
Pu::queryPreset(string addrcode, string &json)
{
    SHL_Json::Value rvalue;
    SHL_Json::Value va;
    SHL_Json::Reader reader;
    SHL_Json::FastWriter wr;
    string preset;
    string cond = string("addrcode=\'") + addrcode + string("\'");
    if (0 != presetCfg->query(preset, cond))
    {
        return -1;
    }

    if (!(reader.parse(preset, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    //for (uint32_t i = 0; i < rvalue.size(); i++)
    //{


    //	if (0 == groupMgr->group(rvalue[i]["groupId"].asString())->type())
    //	{
    //		AutoLock locker(m_mright._mtx);
    //		m_mright.push_back(rvalue[i]["groupId"].asString());
    //	}

    //	if (1 == groupMgr->group(rvalue[i]["groupId"].asString())->type())
    //	{
    //		AutoLock locker(m_oright._mtx);
    //		m_oright.push_back(rvalue[i]["groupId"].asString());
    //	}
    //}

    va["presetList"] = rvalue;
    json = wr.write(va);
    return 0;
}

int
Pu::setPreset(int channelNo, string &jin)
{
    string addrcode = gucode(channelNo);
    return setPreset(addrcode, jin);
}

int
Pu::setPreset(string addrcode, string &jin)
{
    string cond = string("addrcode=\'") + addrcode + string("\'");
    
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(jin, rvalue, false))
    {
        return -1;
    }

    if (rvalue["presetNum"].asInt() <= 0)
    {
        return 0;
    }

    SHL_Json::Value presetList = rvalue["presetList"];

    SHL_Json::FastWriter writer;
    string strPreset;

    for (uint32_t i = 0; i < presetList.size(); i++)
    {
        presetList[i]["addrcode"] = addrcode;
        strPreset = writer.write(presetList[i]);
        if (presetList[i]["name"].asString() == "")
        {
            presetCfg->del(strPreset);
        }
        else
        {
            presetCfg->modify(strPreset);
        }
        
    }

    return 0;
}

//RealGu*
//Pu::getRealGu(int channelNo)
//{
//    RealGu * ret = NULL;
//    AutoLock locker(m_realGu._mtx);
//    for (list<RealGu *>::const_iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
//    {
//        if ((*i)->isSame(channelNo, "", 0, 0))
//        {
//            ret = (*i);
//        }
//    }
//
//    return ret;
//}

//RealGu*
//Pu::getRealGu(string addrcode)
//{
//    RealGu * ret = NULL;
//    AutoLock locker(m_realGu._mtx);
//    for (list<RealGu *>::const_iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
//    {
//        if ((*i)->isSame(addrcode, 0, m_streamProtoType))
//        {
//            ret = (*i);
//        }
//    }
//
//    return ret;
//}

Su* 
Pu::getDualSu(int channelNo)
{
    Su * ret = NULL;
    AutoLock locker(m_dualSu._mtx);
    for (list<Su *>::const_iterator i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
    {
        if ((*i)->isSame(channelNo))
        {
            ret = (*i);
        }
    }

    return ret;
}

Su*
Pu::getDualSu(string addrcode)
{
    Su * ret = NULL;
    AutoLock locker(m_dualSu._mtx);
    for (list<Su *>::const_iterator i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
    {
        if ((*i)->isSame(addrcode))
        {
            ret = (*i);
        }
    }

    return ret;
}

void
Pu::update(const std::string &table, const std::string &json, int type)
{
    /* TODO: ui config channels, type = O_FLUSH <01-09-17, yourname> */
}

void 
Pu::closeAllRealGu()
{
    AutoLock locker(m_realGu._mtx);
    for (list<RealGu *>::const_iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
    {
        (*i)->notifyStatus(StreamListener::S_GU_CLOSED);
    }
}

void
Pu::onTimer()
{
    {
        AutoLock locker(m_realGu._mtx);
        for (list<RealGu *>::const_iterator i = m_realGu.begin(); i != m_realGu.end(); ++i)
        {
            (*i)->onTimer();
        }

        list<RealGu *>::iterator i;
        for (i = m_realGu.begin();  i != m_realGu.end();)
        {
            if (StreamListener::S_GU_CLOSED == (*i)->status())
            {
                (*i)->stop();
                delete (*i);

                //Thread *trd = new Thread;
                //tagThreadParamEx * param = new tagThreadParamEx;
                //if (NULL != param)
                //{
                //    param->pthis = (*i);
                //    param->trd = trd;
                //    trd->setEntry(threadEntryEx, param, 0);
                //    trd->start();
                //    trd->detach();
                //}

                list<RealGu *>::iterator ti = i++;
                m_realGu.erase(ti);
            }
            else if (StreamListener::S_GU_OFFLINE == (*i)->status())
            {
                //static uint64_t slastpts = 0;
                //uint64_t curpts = sys->currentTimeMillis();
                //
                //if (2000 < curpts - slastpts)
                //{
                //    (*i)->reset();
                //    printf("\n\n\treset gu gu\n\n");
                //    slastpts = curpts;
                //}
                i++;
            }
            else
            {
                i++;
            }
        }
    }

    {
        AutoLock locker(m_recordGu._mtx);
        for (list<RecordGu *>::const_iterator i = m_recordGu.begin(); i != m_recordGu.end(); ++i)
        {
            (*i)->onTimer();
        }

        list<RecordGu *>::iterator i;
        for (i = m_recordGu.begin();  i != m_recordGu.end();)
        {
            if (StreamListener::S_GU_CLOSED == (*i)->status())
            {
                (*i)->stop();
                delete (*i);
                list<RecordGu *>::iterator ti = i++;
                m_recordGu.erase(ti);
            }
            else
            {
                i++;
            }
        }
    }

    {
        AutoLock locker(m_dualSu._mtx);
        for (list<Su *>::const_iterator i = m_dualSu.begin(); i != m_dualSu.end(); ++i)
        {
            (*i)->onTimer();
        }

        list<Su *>::iterator i;
        for (i = m_dualSu.begin(); i != m_dualSu.end();)
        {
            if (StreamSpeaker::S_SU_OFFLINE == (*i)->status())
            {
                //(*i)->stop();
                //delete (*i);
                //list<Su *>::iterator ti = i++;
                //m_dualSu.erase(ti);
                i++;
            }
            else
            {
                i++;
            }
        }
    }
}

void *
Pu::threadEntry(ThreadInfo *info)
{
    Pu *pu = (Pu *)(info->data);

    switch(info->id)
    {
      case T_PU_LOGIN:
      {
        pu->loginLoop();
      }
        break;
      case T_UPDATE_CHNSTATUS:
      {
         pu->updateChannel();
      }
        break;
    }


    return NULL;
}
int Pu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
	SHL_Json::FastWriter writer;
	SHL_Json::Value      value;

	value["version"] = "1.0";
	value["time"] = sys->currentTime();
	value["deviceName"] = m_name;
	value["deviceHost"] = m_host;
	value["devicePort"] = m_port;
	value["addrcode"]   = m_addrcode;


	SHL_Json::Value item;
	item["type"]      = TriggerListener::TR_PTZ_CONTROL_ALARM;
	item["gucode"]    = gucode(channelNo);
	item["name"]      = guname(channelNo);
	item["cmd"]       = cmd;
	item["channelNo"] = channelNo;
	item["speed"]     = speed;
	item["preset"]    = preset;
	item["stop"]      = stop;
	value["alarms"].append(item);


	string json = writer.write(value);
	notifyAlarm(json, NULL, 0);
	return 0;
}

int
Pu::queryAlarm(int channelNo, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, int &cnt)
{
    char beginTime[128] = { 0 };
    char endTime[128] = { 0 };
    sprintf(beginTime, "%04d-%02d-%02d %02d:%02d:%02d", start.year, start.month, start.day, start.hour, start.minute, start.second);
    sprintf(endTime, "%04d-%02d-%02d %02d:%02d:%02d", end.year, end.month, end.day, end.hour, end.minute, end.second);

    char condition[512] = { 0 };
    sprintf(condition, "addrcode='%s' and channelNo='%d' and time>='%s' and time<='%s';", m_addrcode.c_str(), channelNo, beginTime, endTime);
    cnt = alarmLogCfg->count(condition);
    if (cnt < 0)
    {
        return -1;
    }

    char alarmtype[128] = { 0 };
    char temp[128] = { 0 };
    if (0x1ffff != type)
    {
        if (0x01 & type)
        {
            sprintf(alarmtype, "type='%d' ", TriggerListener::TR_DEV_VLOSS);
        }
        if (0x02 & type)
        {
            if (strcmp(alarmtype, "") != 0)
            {
                sprintf(temp, "or type='%d' ", TriggerListener::TR_DEV_MD);
            }
            else
            {
                sprintf(alarmtype, "type='%d' ", TriggerListener::TR_DEV_MD);
            }
            strcat(alarmtype, temp);
        }
        if (0x04 & type)
        {
            if (strcmp(alarmtype, "") != 0)
            {
                sprintf(alarmtype, "or type='%d' ", TriggerListener::TR_DEV_VCOVER);
            }
            else
            {
                sprintf(alarmtype, "type='%d' ", TriggerListener::TR_DEV_VCOVER);
            }
            strcat(alarmtype, temp);
        }
        if (0x10000 & type)
        {
            if (strcmp(alarmtype, "") != 0)
            {
                sprintf(alarmtype, "or type='%d' ", TriggerListener::TR_DEV_STATUS);
            }
            else
            {
                sprintf(alarmtype, "type='%d' ", TriggerListener::TR_DEV_STATUS);
            }
            strcat(alarmtype, temp);
        }
    }

    if (strcmp(alarmtype, "") != 0)
    {
        sprintf(condition, "addrcode='%s' and channelNo='%d' and time>='%s' and time<='%s' and %s limit '%d','%d';",
            m_addrcode.c_str(), channelNo, beginTime,
            endTime, alarmtype, fromIndex - 1, (toIndex - fromIndex + 1));
    }
    else
    {
        sprintf(condition, "addrcode='%s' and channelNo='%d' and time>='%s' and time<='%s' limit '%d','%d';",
            m_addrcode.c_str(), channelNo, beginTime,
            endTime, fromIndex - 1, (toIndex - fromIndex + 1));
    }
    
    int ret = alarmLogCfg->query(json, condition);
    return ret;
}

int
Pu::queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    return -1;
}

int Pu::getChnStatus(int channelNo, string &json)
{
    SHL_Json::Value value;

    value["version"]  = "v1.0";
    value["online"]   = validate(channelNo)?1:0;
    value["devonline"] = online()?1:0;

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);
    return 0;
}

int Pu::getChnStatus(string gucode, string &json)
{
	int channelNo = GetChanNoByCode(gucode);
    return getChnStatus(channelNo, json);
}

int 
Pu::CtrlPtzRight(string clientIp, string userName, string clientKey, uint32_t u32ClientOpt, uint32_t u32ChnNo, bool bStop, string& priorIp, string& priorUsername)
{
    AutoLock lock(m_mtxChnToPtzPrior);

    SHL_Json::Value  alarmValue;
    SHL_Json::Value item;
    SHL_Json::FastWriter writer;


    alarmValue["version"]    = "1.0";
    alarmValue["time"]       = sys->currentTime();
    alarmValue["devicePort"] = 9090;
    alarmValue["addrcode"]   = "";
    alarmValue["level"]      = 1;
    item["type"]             = TriggerListener::TR_PTZ_CONTROL;
    item["channelNo"]        = 0;
    item["gucode"]           = "";
    item["name"]             = "";

    time_t tmNow = time(NULL);
    map<uint32_t, PTZ_CONTRL_PRIOR>::iterator pos = m_mapChnToPtzPrior.find(u32ChnNo);
    if (pos == m_mapChnToPtzPrior.end())
    {
        PTZ_CONTRL_PRIOR ptzCtrlPri;
        ptzCtrlPri.strClientIp = clientIp;
        ptzCtrlPri.strClientKey = clientKey;
        ptzCtrlPri.strUserName = userName;
        ptzCtrlPri.lClientOpt = u32ClientOpt;
        ptzCtrlPri.lStartTime = long(time(NULL));

        m_mapChnToPtzPrior[u32ChnNo] = ptzCtrlPri;
    }
    else
    {
        TriggerListener* cli = NULL;
        PTZ_CONTRL_PRIOR &ptzCtrlPri = (*pos).second;
        long tmDiff = tmNow - ptzCtrlPri.lStartTime;

        if ((ptzCtrlPri.strClientKey == clientKey) ||
            ((ptzCtrlPri.strClientIp == clientIp) &&
            (ptzCtrlPri.lClientOpt == u32ClientOpt)))
        {
            ptzCtrlPri.lStartTime = time(NULL);

            return 0;
        }
        else if ((ptzCtrlPri.lClientOpt >=(long)(u32ClientOpt)) &&
            (tmDiff <= 15))
        {
            cli = (TriggerListener*)cliMgr->findClient(ptzCtrlPri.strClientKey);
            if (NULL == cli)
            {
                ptzCtrlPri.strClientIp = clientIp;
                ptzCtrlPri.strClientKey = clientKey;
                ptzCtrlPri.lClientOpt = u32ClientOpt;
                ptzCtrlPri.strUserName = userName;
                ptzCtrlPri.lStartTime = long(time(NULL));

                return 0;
            }

            //无法抢占，控制失败
            if (!bStop)
            {
                cli = (TriggerListener*)cliMgr->findClient(clientKey);
                alarmValue["deviceName"] = ptzCtrlPri.strUserName;
                alarmValue["deviceHost"] = ptzCtrlPri.strClientIp;
                item["status"]    = 1;
                alarmValue["alarms"].append(item);
                string aj = writer.write(alarmValue);
                cli->onAlarm(aj, NULL, 0);
            }

            return -1;
        }
        else
        {
            //抢占成功
            cli = (TriggerListener*)cliMgr->findClient(ptzCtrlPri.strClientKey);
            ptzCtrlPri.strClientIp = clientIp;
            ptzCtrlPri.strClientKey = clientKey;
            ptzCtrlPri.lClientOpt = u32ClientOpt;
            ptzCtrlPri.strUserName = userName;
            ptzCtrlPri.lStartTime = long(time(NULL));

            if (!bStop && cli)
            {
                priorIp = ptzCtrlPri.strClientIp;
                priorUsername = ptzCtrlPri.strUserName;

                alarmValue["deviceName"] = ptzCtrlPri.strUserName;
                alarmValue["deviceHost"] = ptzCtrlPri.strClientIp;
                item["status"]    = 0;
                alarmValue["alarms"].append(item);
                string aj = writer.write(alarmValue);
                cli->onAlarm(aj, NULL, 0);
            }

            return 0;
        }
    }

    return 0;
}

int
Pu::needProcess(string key, time_t tt)
{
    BOOL bRet = TRUE;
    AutoLock lock(m_mapAlarm._mtx);
    BOOL bFind = FALSE;
    map<std::string, time_t>::iterator i;
    for (i = m_mapAlarm.begin(); i != m_mapAlarm.end(); ++i)
    {
        if (key == i->first)
        {
            bFind = TRUE;
            time_t ttOld = i->second;
            if (tt - ttOld <= 5)
            {
                bRet = FALSE;
            }
            else
            {
                i->second = tt;
            }
        }
    }
    if (!bFind)
    {
        m_mapAlarm.insert(pair<std::string, time_t>(key, tt));
    }
    return bRet;
}

int Pu::GetChanNoByCode(string addrcode)
{
        if ("gb28181" == GetSipProtocol())
        {
            char *chanNo = (char*)(addrcode.c_str() + 17);
            int no = atoi(chanNo) - 1;
            return no;
        }
        else
        {

#if 0
        return atoi(addrcode.c_str() + 15) - 1;
#endif
        char chanNo[4] = { 0 };
        chanNo[0] = *(addrcode.c_str() + 10);
        chanNo[1] = *(addrcode.c_str() + 16);
        chanNo[2] = *(addrcode.c_str() + 17);
        int no = atoi(chanNo) - 1;
        return no;
    }
	/*else if (addrcode.size() == 20)
	{
		return atoi(addrcode.c_str() + 17) - 1;
	}*/
    return 0;
}

void               
Pu::setQueryRecordCB(QueryRecorCB cb, void *userdata)
{ 
    m_clienQueryRecord_cb = cb;
    m_clientUserdata = userdata; 
}