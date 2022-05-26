#include "gsessmgr.h"
#include "gsession.h"
#include "thread.h"
#include "frame.h"
#include "json/json.h"
#include <fstream>
#include "tls_error.h"

string g_media_stream = "";
string g_sip_protocol = "";
string g_catalog_type = "";

const string &GetMediaStream()
{
    return g_media_stream;
}

const string &GetSipProtocol()
{
    return g_sip_protocol;
}

const string &GetCatalogType()
{
    return g_catalog_type;
}

GSessMgr::GSessMgr()
{
    m_timerThread = NULL;
    if (!m_timerThread)
    {
        m_timerThread = new Thread;
        m_timerThread->setEntry(threadProc, this, T_TIMER);
        m_timerThread->start();
    }
    InitMediaStream();
}

GSessMgr::~GSessMgr()
{
    if (m_timerThread)
    {
        m_timerThread->stop();
        delete m_timerThread;
        m_timerThread = NULL;
    }

    AutoLock locker(_mtx);
    for (auto i = begin(); i != end(); ++i)
    {
        i->second->stop();
        delete i->second;
    }
}

int
GSessMgr::addSession(string &sessid)
{
    auto i = find(sessid);

    if (i != end())
    {
        LOGI_MSV("sessid: %s is exist.", sessid.c_str());
        return 0;
    }

    (*this)[sessid] = new GSession(sessid);

    return 0;
}

int
GSessMgr::addSession(string &sessid, uint64_t startTime, uint64_t endTime, int playbackmode)
{
    auto i = find(sessid);

    if (i != end())
    {
        LOGI_MSV("sessid: %s is exist.", sessid.c_str());
        return 0;
    }

    (*this)[sessid] = new GSession(sessid, startTime, endTime, playbackmode);

    return 0;
}

int
GSessMgr::delSession(string &sessid)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return -1;
    }

    delete i->second;
    erase(i);

    return 0;
}

int
GSessMgr::addSession(string &sessid, string &callid, string &json)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);

    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return -1;
    }

    return (*this)[sessid]->addDestination(callid, json);
}

int
GSessMgr::RTPSRCPort(string &sessid)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        addSession(sessid);
    }

    return (*this)[sessid]->RTPSRCPort();
}


int
GSessMgr::RTPSRCPort(string &sessid, uint64_t startTime, uint64_t endTime, int playbackmode)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        addSession(sessid, startTime, endTime, playbackmode);
    }

    return (*this)[sessid]->RTPSRCPort();
}

void
GSessMgr::setstreamcallback(string &sessid, pfnstreamcallback fnstreamcb, void *puser)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return;
    }

    (*this)[sessid]->setstreamcallback((void*)fnstreamcb, puser);
}

int
GSessMgr::RTPStart(string &sessid, string &callid, string &json)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return -1;
    }

    return (*this)[sessid]->RTPStart(callid, json);
}

int
GSessMgr::RTPStop(string &callid)
{
    AutoLock locker(_mtx);

    auto i = begin();
    for (i = begin(); i != end(); ++i)
    {
        if (i->second->hasCall(callid))
        {
            break;
        }
    }

    if (i == end())
    {
        LOGI_MSV("callid: %s is not exist.", callid.c_str());
        return -1;
    }

    return i->second->RTPStop(callid);
}

int
GSessMgr::RTPPlay(string &callid, string scale, string range)
{
    AutoLock locker(_mtx);

    auto i = begin();
    for (i = begin(); i != end(); ++i)
    {
        if (i->second->hasCall(callid))
        {
            break;
        }
    }

    if (i == end())
    {
        LOGI_MSV("callid: %s is not exist.", callid.c_str());
        return -1;
    }

    return i->second->RTPPlay(callid, scale, range);
}

int
GSessMgr::RTPPause(string &callid)
{
    AutoLock locker(_mtx);

    auto i = begin();
    for (i = begin(); i != end(); ++i)
    {
        if (i->second->hasCall(callid))
        {
            break;
        }
    }

    if (i == end())
    {
        LOGI_MSV("callid: %s is not exist.", callid.c_str());
        return -1;
    }

    return i->second->RTPPause(callid);
}

int
GSessMgr::setSSRC(string &sessid, uint32_t ssrc)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return -1;
    }

    return (*this)[sessid]->setSSRC(ssrc);
}

void
GSessMgr::timerdriver()
{
    while (m_timerThread->active())
    {
        {
            AutoLock locker(_mtx);
            for (auto i = begin(); i != end(); ++i)
            {
                i->second->timerdriver();
            }

            for (auto it = begin(); it != end();)
            {
                if ((S_SESS_CLOSED == it->second->status()) && 
                    (0 == it->second->size()))
                {
                    delete it->second;
                    it = erase(it);
                }
                else
                {
                    it++;
                }
            }
        }

        sys->sleep(200);
    }
}


void * 
GSessMgr::threadProc(ThreadInfo *info)
{
    GSessMgr * gsessMgr = (GSessMgr *)(info->data);

    switch (info->id)
    {
    case T_TIMER:
        gsessMgr->timerdriver();
        break;

    default:
        break;
    }

    return NULL;
}

void
GSessMgr::doSendAudio(string sessid, const uint8_t * data, uint32_t size)
{
    AutoLock locker(_mtx);
    auto i = find(sessid);
    if (i == end())
    {
        LOGI_MSV("sessid: %s is not exist.", sessid.c_str());
        return;
    }

    return (*this)[sessid]->onAudioStream((void*)data, size, ENCODE_G711A, 0, 0, 0, 0);
}

void
GSessMgr::InitMediaStream()
{
    if (g_media_stream == "" && g_sip_protocol == "")
    {
        FILE *fp = fopen("lmt.json", "rb+");
        if (!fp)
        {
            g_media_stream = "h264";
            g_sip_protocol = "gw";
            g_catalog_type = "catalog_district";
            return;
        }
        int size = 10240;
        char *buf = new char[size];
        memset(buf, 0, size);
        fread(buf, 1, size, fp);
        fclose(fp);
        std::string json(buf);
        delete[]buf;

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            LOGE_MSV("parse json:%s fail.", json.c_str());
            g_media_stream = "h264";
            g_sip_protocol = "gw";
            g_catalog_type = "catalog_district";
            return;
        }
		if (rvalue["GB28181"].isNull())
		{
			LOGI_MSV("GB28181 is not error\n");
			g_media_stream = "h264";
			g_sip_protocol = "gw";
			g_catalog_type = "catalog_district";
			return;
		}
		else
		{
			if (rvalue["GB28181"]["MediaStream"].isNull())
			{
				LOGE_MSV("MediaStream is NULL:json = %s.", json.c_str());
				g_media_stream = "h264";
			}
			else
			{
				g_media_stream = rvalue["GB28181"]["MediaStream"].asString();

				if (g_media_stream == "")
				{
					g_media_stream = "h264";
				}
			}

			if (rvalue["GB28181"]["SipProtocol"].isNull())
			{
				LOGE_MSV("SipProtocol is NULL:json = %s.", json.c_str());
				g_sip_protocol = "gw";
			}
			else
			{
				g_sip_protocol = rvalue["GB28181"]["SipProtocol"].asString();

				if (g_sip_protocol == "")
				{
					g_sip_protocol = "gw";
				}
			}


			if (rvalue["GB28181"]["CatalogType"].isNull())
			{
				LOGE_MSV("CatalogType is NULL:json = %s.", json.c_str());
				g_catalog_type = "catalog_district";
			}
			else
			{
				g_catalog_type = rvalue["GB28181"]["CatalogType"].asString();

				if (g_catalog_type == "")
				{
					g_catalog_type = "catalog_district";
				}
			}
		}
    }
}
