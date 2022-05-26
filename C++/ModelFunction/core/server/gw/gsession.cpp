#include "gsession.h"
#include "sipcall.h"
#include "rtpsession.h"
#include "pu.h"
#include "putree.h"
#include "tls_error.h"
#include "json/json.h"
#include "hik_su.h"
#include "gw_su.h"
#include "gw_pu.h"
#include "gsessmgr.h"
#include "frame.h"
#include "gb28181_sipua.h"

GSession::GSession(string sessid)
{
    m_sessid          = sessid;
    m_rtpSession      = NULL;
    m_status          = S_SESS_NORMAL;
    m_streamtype      = 0;
    m_streamProtoType = 1;
    m_su              = NULL;
    m_pu              = NULL;

    m_ssrc = 0;
    m_playmode = 0;
    m_startTime = 0;
    m_endTime = 0;
}

GSession::GSession(string sessid, uint64_t startTime, uint64_t endTime, int playbackmode)
{
    m_sessid = sessid;
    m_rtpSession = NULL;
    m_status = S_SESS_NORMAL;
    m_streamtype = 0;
    m_streamProtoType = 1;
    m_ssrc = 0;
    m_su = NULL;
    m_pu = NULL;

    m_ssrc = 0;
    m_playmode = playbackmode;
    m_startTime = startTime;
    m_endTime = endTime;
}

GSession::~GSession()
{
    stop();
}

int
GSession::start(string json, int mode)
{
    m_pu = putree->findPu(m_addrcode);
    if (!m_pu)
    {
        return -1;
    }
    if (m_playmode == 0)
    {
		return m_pu->addRealGu(m_addrcode, m_streamtype, m_streamProtoType, this);
    }
    else
    {
        datetime_s stime;
        datetime_s etime;

        sys->strptime(m_startTime, stime);
        sys->strptime(m_endTime, etime);

        if (m_playmode == 1)
        {
			return m_pu->addRecordGu(m_addrcode, m_streamtype, stime, etime, m_streamProtoType, this);
        }
        else
        {
			return m_pu->addRecordGu(m_addrcode, m_streamtype, stime, etime, m_streamProtoType, this, 0, 1);
        }
    }
    
}

void
GSession::stop()
{ 
    if (m_pu)
    {
        if (m_playmode == 0)
        {
            m_pu->delRealGu(m_addrcode, m_streamtype, this);
        }
        else
        {
            m_pu->delRecordGu(this);
        }
        m_pu = NULL;
    }

    {
        AutoLock locker(_mtx);
        for (auto i = begin(); i != end(); i++)
        {
            delete i->second;
        }
        clear();
    }

    if (NULL != m_su)
    {
        delete m_su;
        m_su = NULL;
    }

    setstreamcallback(NULL, this);

    if (m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    } 
}

SipCall *
GSession::addSipCall(string &callid, string &fromn, string &fromh, int fromp, string &ton, string &toh, int top)
{
    AutoLock locker(_mtx);
    auto i = find(callid);
    if (i != end())
    {
        return i->second;
    }

    m_addrcode = ton;

    (*this)[callid] = new SipCall(callid, fromn, fromh, fromp, ton, toh, top);

    return (*this)[callid];
}

int
GSession::delSipCall(string &callid)
{
    AutoLock locker(_mtx);
    auto i = find(callid);
    if (i == end())
    {
        LOGW_MSV("callid: %s is not exsit.", callid.c_str());
        return -1;
    }

    if (m_rtpSession)
    {
        m_rtpSession->delDestination(i->second->destip(), i->second->destport(), i->second);
    }

    delete i->second;
    erase(i);

    return 0;
}

bool
GSession::hasCall(string &callid)
{
    AutoLock locker(_mtx);
    return (find(callid) != end()) ? true : false;
}

int
GSession::RTPSRCPort()
{
    if (!m_rtpSession)
    {
        m_rtpSession = new RTPSessionEx();
    }

    return m_rtpSession->srcport();
}

int
GSession::RTPStart(string &callid, string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        LOGE_MSV("parse json:%s fail.", json.c_str());
        return -1;
    }

    m_addrcode      = rvalue["ton"].asString();
    m_vcmode        = rvalue["vcmode"].asString();



    if (m_playmode == 0)
    {
        //preview
        if (("dualtalk" == m_vcmode) ||
            ("broadcast" == m_vcmode))
        {
            m_streamtype = 0;
            Pu *pu = putree->findPu(m_addrcode);
            if (!pu)
            {
                return -1;
            }

            if (Pu::PROTO_TYPE_HIK == pu->protocol())
            {
                m_su = new HikSu(pu, m_addrcode);
                m_su->start();
            }
            else if (Pu::PROTO_TYPE_GW_B == pu->protocol())
            {
                m_su = new GwSu(pu, m_addrcode);
                m_su->setgessionlistener(this);
                m_su->start();
            }
        }
        else if ("mainstream" == m_vcmode)
        {
            m_streamtype = 0;
        }
        else if ("substream" == m_vcmode)
        {
            m_streamtype = 1;
        }

        string fromn = rvalue["fromn"].asString();
        string fromh = rvalue["fromh"].asString();
        int    fromp = rvalue["fromp"].asInt();
        string ton = rvalue["ton"].asString();
        string toh = rvalue["toh"].asString();
        int    top = rvalue["top"].asInt();
        string destip = rvalue["destip"].asString();
        int    destport = rvalue["destport"].asInt();

        SipCall *call = addSipCall(callid, fromn, fromh, fromp, ton, toh, top);

        if (start("") < 0)
        {
            delSipCall(callid);

            LOGE_MSV(getError());
            return -1;
        };

        if (!m_rtpSession)
        {
            m_rtpSession = new RTPSessionEx();
        }

        if (!rvalue["ssrc"].isNull())
        {
            m_rtpSession->setSsrc(rvalue["ssrc"].asString());
        }

        call->setmedia(m_rtpSession->srcport(), destip, destport, m_streamtype);

        extern void sessdatacallback(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
        setstreamcallback((void*)sessdatacallback, this);

        return m_rtpSession->addDestination(destip, destport, call);
    }
    else
    {
        //playback

        m_streamtype = 0;
        string fromn = rvalue["fromn"].asString();
        string fromh = rvalue["fromh"].asString();
        int    fromp = rvalue["fromp"].asInt();
        string ton = rvalue["ton"].asString();
        string toh = rvalue["toh"].asString();
        int    top = rvalue["top"].asInt();
        string destip = rvalue["destip"].asString();
        int    destport = rvalue["destport"].asInt();

        SipCall *call = addSipCall(callid, fromn, fromh, fromp, ton, toh, top);

        if (start("") < 0)
        {
            delSipCall(callid);

            LOGE_MSV(getError());
            return -1;
        };

        if (!m_rtpSession)
        {
            m_rtpSession = new RTPSessionEx();
        }

        call->setmedia(m_rtpSession->srcport(), destip, destport, m_streamtype);

        extern void sessdatacallback(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
        setstreamcallback((void*)sessdatacallback, this);

        return m_rtpSession->addDestination(destip, destport, call);
    }


}

int
GSession::addDestination(string &callid, string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        LOGE_MSV("parse json:%s fail.", json.c_str());
        return -1;
    }

    m_addrcode      = rvalue["ton"].asString();
    string mode     = rvalue["vcmode"].asString();
    if ("mainstream" == mode)
    {
        m_streamtype = 0;
    }
    else if ("substream" == mode)
    {
        m_streamtype = 1;
    }
    else
    {
        m_streamtype = 0;
    }
    
    string fromn    = rvalue["fromn"].asString();
    string fromh    = rvalue["fromh"].asString();
    int    fromp    = rvalue["fromp"].asInt();
    string ton      = rvalue["ton"].asString();
    string toh      = rvalue["toh"].asString();
    int    top      = rvalue["top"].asInt();
    string destip   = rvalue["destip"].asString();
    int    destport = rvalue["destport"].asInt();

    SipCall *call   = addSipCall(callid, fromn, fromh, fromp, ton, toh, top);
    if (!m_rtpSession)
    {
        m_rtpSession = new RTPSessionEx();
    }
    if (!rvalue["ssrc"].isNull())
    {
        m_rtpSession->setSsrc(rvalue["ssrc"].asString());
    }
    call->setmedia(m_rtpSession->srcport(), destip, destport, m_streamtype);

    return m_rtpSession->addDestination(destip, destport, call);
}

int
GSession::RTPStop(string &callid)
{
    setstreamcallback(NULL, this);

    delSipCall(callid);

    if (0 == size())
    {
        if (m_pu)
        {
            if (m_playmode == 0)
            {
                m_pu->delRealGu(m_addrcode, m_streamtype, this);
            }
            else
            {
                m_pu->delRecordGu(this);
            }            
        }

        m_status = S_SESS_CLOSED;
    }

    return 0;
}

int
GSession::RTPPlay(string &callid, string scale, string range)
{
    if (m_pu)
    {
        m_pu->continueRecord(this);       
        m_pu->playRecord(this, scale, range);
    }

    return 0;
}

int
GSession::RTPPause(string &callid)
{   
    if (m_pu)
    {
        m_pu->pauseRecord(this);
    }

    return 0;
}

int
GSession::setSSRC(uint32_t ssrc)
{
    if (m_rtpSession)
    {
        m_rtpSession->setSsrc(to_string(ssrc));
    }
    
    return 0;
}

int
GSession::stream()
{
    return -1;
}

int
GSession::transport()
{
    return -1;
}

string
GSession::peerip()
{
    return "";
}

int
GSession::peerport()
{
    return -1;
}

string
GSession::peertime()
{
    return "";
}

int
GSession::streamtype()
{
    return -1;
}

void
sessdatacallback(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    GSession *sess = (GSession*)puser;
    if (NULL == sess)
    {
        return;
    }

    if (0 == type)     //H264
    {

    }
    else if (1 == type)//G711
    {
        if (NULL != sess->m_su)
        {
            sess->m_su->doAudioStream((void*)data, size, 20);//transmit audio data to device
        }	
    }
	else if (2 == type)//PS
	{
	}
}

void 
GSession::setstreamcallback(void *pfnstreamcallback, void *puser)
{
    if (NULL != m_rtpSession)
    {
        m_rtpSession->setstreamcallback((RTPSessionEx::pfnstreamcallback)pfnstreamcallback, puser);
    }
}

void
GSession::onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag)//transmit audio stream to superior
{
    if (("dualtalk" == m_vcmode) || 
        ("broadcast" == m_vcmode))
    {
        return;
    }

    if (tag == TAG_POSI)
    {
        int pos = *(int*)stream;

        if (pos == 100)
        {
            sipUA->push_mediaStatus(m_addrcode, "112", NULL);
        }

    }

    if (m_rtpSession)
    {
        if ("ps" == GetMediaStream())
            m_rtpSession->sendByPS(encode, type, stream, size, pts);
        else
            m_rtpSession->sendByFU(encode, type, stream, size, pts);
    }
}

void
GSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    if ("dualtalk" != m_vcmode)
    {
        return;
    }

    if (m_rtpSession)
    {
		if ("ps" == GetMediaStream())
			m_rtpSession->sendByPS(encode, 'A', stream, size, pts);
		else
			m_rtpSession->sendByFU(encode, 'A', stream, size, pts);
    }
}

void
GSession::onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type)
{

}

void
GSession::onStatus(int code)
{
    m_status = S_SESS_CLOSED;
}

void
GSession::timerdriver()
{
    AutoLock locker(_mtx);
    for (auto i = begin(); i != end(); ++i)
    {
        i->second->timerdriver();
    }

    for (auto i = begin(); i != end();)
    {
        if (SipCall::S_CLOSED == i->second->status())
        {
            if (m_rtpSession)
            {
                m_rtpSession->delDestination(i->second->destip(), i->second->destport(), i->second);
            }

            if (1 == size())
            {
                if (m_pu)
                {
                    m_pu->delRealGu(m_addrcode, m_streamtype, this);
                }
                
                m_status = S_SESS_CLOSED;
            }

            delete i->second;
            auto ti = i++;
            erase(ti);
        }
        else
        {
            i++;
        }
    }
}
