#include "gw_record_gu.h"
#include "gwb_pu.h"
#include "gsessmgr.h"
#include "system.h"
#include "gumgr.h"
#include "frame.h"
#include "sipua.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

GwRecordGu::GwRecordGu(Pu *pu, string fileurl, int st)
    : RecordGu(pu, fileurl, st)
{
    char sSession[1024] = { 0 };
    snprintf(sSession, 1024, "%p_%s", this, fileurl.c_str());

    m_sessid = sSession;

    GwPu * pGwBPu = (GwPu *)m_pu;
    m_rtspClient = new RtspClient(pGwBPu->host().c_str(), pGwBPu->rport());
    m_keepLive = NULL;
    m_strScale = "";
    m_strRange = "";

	m_uploadStream = NULL;
}

GwRecordGu::~GwRecordGu()
{
    if (m_keepLive)
    {
        m_keepLive->stop();
        delete m_keepLive;
        m_keepLive = NULL;
    }

	if (m_uploadStream)
	{
		m_uploadStream->stop();
		delete m_uploadStream;
		m_uploadStream = NULL;
	}

	{
		AutoLock lock(m_gwRecordList._mtx);
		while (m_gwRecordList.size())
		{
			stRecordGw *stream = (*m_gwRecordList.begin());
			delete [] stream->data;
			delete stream;

			m_gwRecordList.pop_front();
		}
	}

    if (NULL != m_rtspClient)
    {
        delete m_rtspClient;
        m_rtspClient = NULL;
    }
}

int
GwRecordGu::start(int offset)
{
    do 
    {
        GwPu * pGwBPu = (GwPu *)m_pu;
        if (NULL == pGwBPu)
        {
            break;
        }

        bool bRet = m_rtspClient->start(m_fileurl.c_str(), this);
        if (!bRet)
        {
            break;
        }

        bRet = m_rtspClient->doOption();
        if (!bRet)
        {
            break;
        }

        bRet = m_rtspClient->doDescribe();
        if (!bRet)
        {
            break;
        }

        int32_t s32BasePort = gsessMgr->RTPSRCPort(m_sessid);
        int32_t s32DestPort = 0;

        bRet = m_rtspClient->doSetup("RTP/AVP", "unicast", s32BasePort, s32DestPort);
        if (!bRet)
        {
            break;
        }

        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        value["fromn"]    = systm->addrcode();
        value["fromh"]    = pGwBPu->m_sipua->ip();
        value["fromp"]    = s32BasePort;
        value["ton"]      = m_addrcode;
        value["toh"]      = pGwBPu->ip();
        value["top"]      = pGwBPu->port();
        value["destip"]   = pGwBPu->ip();
        value["destport"] = s32DestPort;
        value["vcmode"]   = 1;

        string json = SHL_Json::writeString(writer, value);

        int iRet = gsessMgr->addSession(m_sessid, m_sessid, json);
        if (iRet < 0)
        {
            return -1;
        }

        gsessMgr->setstreamcallback(m_sessid, stream_cb, this);

        recordGuMgr->addGu(this);

        if ("" == m_strScale)
        {
            //return -1;
            m_strScale = "1";
			m_strRange = "clock=0-";
        }
        m_rtspClient->doPlay(m_strScale.c_str(),m_strRange.c_str());

        if (NULL == m_keepLive)
        {
            m_keepLive = new Thread();
			m_keepLive->setEntry(threadEntry, this, GW_RECORD_HEART);
            m_keepLive->start();
        }

		if (NULL == m_uploadStream)
		{
			m_uploadStream = new Thread();
			m_uploadStream->setEntry(threadEntry, this, GW_RECORD_UPLOAD);
			m_uploadStream->start();
		}

        return 0;
    } while (0);	

    return -1;
}

void 
GwRecordGu::pushRecord(const uint8_t * data, uint32_t size, uint8_t type)
{
	AutoLock lock(m_gwRecordList._mtx);

	do 
	{
		stRecordGw *stream = new stRecordGw;
		if (!stream)
		{
			break;
		}

		stream->data = new uint8_t[size];
		if (!stream->data)
		{
			delete stream;
			break;
		}

		memcpy(stream->data, data, size);
		stream->size = size;
		stream->type = type;

		m_gwRecordList.push_back(stream);
	} while (0);
}

void
GwRecordGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    GwRecordGu *gu = (GwRecordGu *)puser;
	if (gu)
	{
		gu->pushRecord(data, size, type);
	}
}

void
GwRecordGu::stop()
{
    do
    {
        gsessMgr->setstreamcallback(m_sessid, NULL, this);

        gsessMgr->RTPStop(m_sessid);
        recordGuMgr->delGu(this);

        bool bRet = m_rtspClient->doTeardown();
        if (!bRet)
        {
            break;
        }

        m_rtspClient->stop();
    } while (0);
}

int        
GwRecordGu::play(string scale, string range)
{
    bool bRet = m_rtspClient->doPlay(scale.c_str(), range.c_str());
    if (!bRet)
    {
        return -1;
    }
    m_strScale = scale;
    m_strRange = range;

    return 0;
}

int
GwRecordGu::pause()
{
    bool bRet = m_rtspClient->doPause();
    if (!bRet)
    {
        return -1;
    }

    return 0;
}

int
GwRecordGu::continuex()
{
    if ("" == m_strScale)
    {
        //return -1;
        m_strScale = "1";
    }

    bool bRet = m_rtspClient->doPlay(m_strScale.c_str(), m_strRange.c_str());
    if (!bRet)
    {
        return -1;
    }

    return 0;
}

int
GwRecordGu::startRepos()
{
    return -1;
}

int
GwRecordGu::setPos(int pos)
{
    return -1;
}

int
GwRecordGu::setTime(datetime_s &t)
{
    return -1;
}

void * 
GwRecordGu::threadEntry(ThreadInfo *arg)
{
    GwRecordGu *pthis = (GwRecordGu*)(arg->data);
    if (pthis)
    {
		switch (arg->id)
		{
		    case GW_RECORD_HEART:
		     {
				pthis->sendHeartbeat();
				break;
		     }

			case GW_RECORD_UPLOAD:
			{
				pthis->uploadRecordStream();
			    break;
			}
		}
        
    }
    return 0L;
}

void
GwRecordGu::sendHeartbeat()
{
    uint64_t tmPrevTime = sys->currentTimeMillis();
    uint64_t tmNowTime = sys->currentTimeMillis();

    while (m_keepLive->active())
    {
        tmNowTime = sys->currentTimeMillis();
        if (m_rtspClient)
        {
            if (tmNowTime - tmPrevTime >= 3 * 1000)
            {
                m_rtspClient->doGetParameter();
                tmPrevTime = tmNowTime;

				int pos = 0;
				notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
				notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
            }
        }
        
        sys->sleep(500);
    }
}

void 
GwRecordGu::uploadRecordStream()
{
	int sz = 0;
	stRecordGw *recordData;

	while (m_uploadStream->active())
	{
		{
			AutoLock lock(m_gwRecordList._mtx);
			sz = m_gwRecordList.size();
		}

		if (0 == sz)
		{
			sys->sleep(10);
			continue;
		}

		{
			AutoLock lock(m_gwRecordList._mtx);
			recordData = (*m_gwRecordList.begin());
			m_gwRecordList.pop_front();
		}

		if (0 == recordData->type)
		{
			this->notifyVideoStream((void*)recordData->data, recordData->size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis());
		}
		else if (1 == recordData->type)
		{
			this->notifyAudioStream((void*)recordData->data, recordData->size, ENCODE_G711A, 8000, 1, 16, 0);
		}

		this->traffic(recordData->size);
		this->m_lastpts = sys->currentTimeMillis();

		delete[] recordData->data;
		delete recordData;

		sys->sleep(10);
	}
}