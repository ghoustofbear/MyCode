#include "gb28181_record_gu.h"
#include "gb28181_pu.h"
#include "gsessmgr.h"
#include "system.h"
#include "gumgr.h"
#include "frame.h"
#include "sipua.h"
#include "json/json.h"
#include "tls_error.h"
#include "rtpsession.h"


#define  GB28181_PLAYBACK_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Playback\r\n" \
    "u=%s:%d\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=%d %d\r\n" \
    "m=video %d RTP/AVP 96 97 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=recvonly\r\n" \
    "y=%s\r\n\r\n"


#define GB28181_RTSP_CONTINUE \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Range: npt=now-\r\n\r\n"

#define GB28181_RTSP_SCALE \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Scale: %s\r\n\r\n"

#define GB28181_RTSP_SETTIME \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Range: npt=%d-\r\n\r\n"

#define GB28181_RTSP_PAUSE \
    "PAUSE RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "PauseTime: now\r\n\r\n"

GB28181RecordGu::GB28181RecordGu(Pu *pu, string fileurl, string addrcode, int st)
    : RecordGu(pu, fileurl, addrcode, st,0)
{
    m_strCallId = "";

    GB28181Pu * pGb28181Pu = (GB28181Pu *)m_pu;
    m_keepLive = NULL;
    m_strScale = "";
    m_strRange = "";

    m_uploadStream = NULL;
    m_streamState = 0;
    m_cseq = 1;
    m_speed = 0;
}

GB28181RecordGu::GB28181RecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
    : RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    GB28181Pu * pGb28181Pu = (GB28181Pu *)m_pu;
    m_keepLive = NULL;
    m_uploadStream = NULL;
    m_strScale = "";
    m_strRange = "";

    m_strCallId = "";
    m_streamState = 0;
    m_cseq = 1;
    m_speed = 0;
}

GB28181RecordGu::~GB28181RecordGu()
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
		AutoLock lock(m_gb28181RecordList._mtx);
        while (m_gb28181RecordList.size())
		{
            stRecordGb28181 *stream = (*m_gb28181RecordList.begin());
			delete [] stream->data;
			delete stream;

            m_gb28181RecordList.pop_front();
		}
	}
}

int
GB28181RecordGu::start(int offset)
{
    do 
    {
        GB28181Pu * pGb28181BPu = (GB28181Pu *)m_pu;
        if (NULL == pGb28181BPu)
        {
            break;
        }
        
        bool bRet = start_by_time();
        if (!bRet)
        {
            break;
        }

        if (NULL == m_keepLive)
        {
            m_keepLive = new Thread();
            m_keepLive->setEntry(threadEntry, this, GB28181_RECORD_HEART);
            m_keepLive->start();
        }

		if (NULL == m_uploadStream)
		{
			m_uploadStream = new Thread();
			m_uploadStream->setEntry(threadEntry, this, GB28181_RECORD_UPLOAD);
			m_uploadStream->start();
		}

        return 0;
    } while (0);	

    return -1;
}

bool               
GB28181RecordGu::start_by_time()
{
    stop_by_time();

    /**
    * 1,send invite request
    * 2,new rtp session
    * 3,transport stream
    *
    */
    GB28181Pu * pPu = (GB28181Pu *)m_pu;

    string resp;

    char sdp[1024] = { 0 };

    m_strSessId = m_addrcode + (":") + std::to_string(m_streamType) + ":playback";;

    int srcport = gsessMgr->RTPSRCPort(m_strSessId);
    if (srcport < 0)
    {
        return false;
    }
    uint32_t start = sys->datetime2int(m_start);
    uint32_t end = sys->datetime2int(m_end);
    LOGD_COM("GB28181RecordGu::start(), sess: %s, srcport: %d", m_strSessId.c_str(), srcport);
    snprintf(sdp, sizeof(sdp), GB28181_PLAYBACK_VIDEO_SDP_FORMAT, 
        m_addrcode.c_str(), 
        pPu->m_sipua->ip().c_str(), 
        systm->addrcode().c_str(),
        1,
        pPu->m_sipua->ip().c_str(),
        start,
        end,
        srcport, 
        GenerateSSRC(systm->addrcode()).c_str());

    int port = pPu->m_sipua->port();
    int sport = pPu->sport();
    int ret = pPu->m_sipua->invite(systm->name(), pPu->m_sipua->ip(), pPu->m_sipua->port(), m_addrcode, pPu->ip(), pPu->sport(), sdp, resp);
    if (ret < 0)
    {
        return false;
    }


    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        return false;
    }
    m_strCallId = rvalue["callid"].asString();
    m_iConId = atoi(rvalue["cid"].asString().c_str());
    m_iDialId = atoi(rvalue["did"].asString().c_str());

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               value;

    value["fromn"] = systm->addrcode();
    value["fromh"] = pPu->m_sipua->ip();
    value["fromp"] = srcport;
    value["ton"] = m_addrcode;
    value["toh"] = pPu->ip();
    value["top"] = pPu->port();
    value["destip"] = pPu->ip();
    value["destport"] = rvalue["videoport"].asInt();
    value["vcmode"] = ((0 == m_streamType) ? "mainstream" : "substream");
    //value["ssrc"] = rvalue["ssrc"];

    string json = SHL_Json::writeString(writer, value);

    ret = gsessMgr->addSession(m_strSessId, m_strCallId, json);
    if (ret < 0)
    {
        return false;
    }

    gsessMgr->setstreamcallback(m_strSessId, stream_cb, this);

    recordGuMgr->addGu(this);

    return true;
}

void               
GB28181RecordGu::stop_by_time()
{
    if ("" == m_strCallId)
    {
        return;
    }

    GB28181Pu * pPu = (GB28181Pu *)m_pu;
    pPu->m_sipua->bye(m_iConId, m_iDialId);

    gsessMgr->RTPStop(m_strCallId);

    recordGuMgr->delGu(this);
}

void 
GB28181RecordGu::pushRecord(const uint8_t * data, uint32_t size, uint8_t type)
{
	AutoLock lock(m_gb28181RecordList._mtx);

	do 
	{
        stRecordGb28181 *stream = new stRecordGb28181;
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

		m_gb28181RecordList.push_back(stream);
	} while (0);
}

void
GB28181RecordGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    GB28181RecordGu *gu = (GB28181RecordGu *)puser;
	if (gu)
	{
		gu->pushRecord(data, size, type);
	}
}

void
GB28181RecordGu::stop()
{
        gsessMgr->setstreamcallback(m_strCallId, NULL, this);
        stop_by_time();
}

int        
GB28181RecordGu::play(string scale, string range)
{
    return 0;
}

int
GB28181RecordGu::pause()
{
    streamCtrl(PLAYBACK_PAUSE, 0);

    m_pause = true;

    return 0;
}

int
GB28181RecordGu::continuex()
{
    streamCtrl(PLAYBACK_CONTINUE, 0);

    m_pause = false;

    return 0;
}

int
GB28181RecordGu::startRepos()
{
    return -1;
}

int
GB28181RecordGu::setPos(int pos)
{
    return -1;
}

int
GB28181RecordGu::setTime(datetime_s &t)
{
    return -1;
}

void * 
GB28181RecordGu::threadEntry(ThreadInfo *arg)
{
    GB28181RecordGu *pthis = (GB28181RecordGu*)(arg->data);
    if (pthis)
    {
		switch (arg->id)
		{
		    case GB28181_RECORD_HEART:
		     {
				pthis->sendHeartbeat();
				break;
		     }

			case GB28181_RECORD_UPLOAD:
			{
				pthis->uploadRecordStream();
			    break;
			}
		}
        
    }
    return 0L;
}

void
GB28181RecordGu::sendHeartbeat()
{
    uint64_t tmPrevTime = sys->currentTimeMillis();
    uint64_t tmNowTime = sys->currentTimeMillis();

    while (m_keepLive->active())
    {
        tmNowTime = sys->currentTimeMillis();

        if (tmNowTime - tmPrevTime >= 3 * 1000)
        {
            tmPrevTime = tmNowTime;

			int pos = 0;
			notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
			notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
        }
        
        sys->sleep(500);
    }
}

void 
GB28181RecordGu::uploadRecordStream()
{
	int sz = 0;
	stRecordGb28181 *recordData;

	while (m_uploadStream->active())
	{
		{
			AutoLock lock(m_gb28181RecordList._mtx);
            sz = m_gb28181RecordList.size();
		}

        if (0 == sz || m_pause)
		{
			sys->sleep(10);
			continue;
		}

		{
            AutoLock lock(m_gb28181RecordList._mtx);
            recordData = (*m_gb28181RecordList.begin());
            m_gb28181RecordList.pop_front();
		}

		if (0 == recordData->type)
		{
			this->notifyVideoStream((void*)recordData->data, recordData->size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis());
		}
		else if (1 == recordData->type)
		{
			this->notifyAudioStream((void*)recordData->data, recordData->size, ENCODE_G711A, 8000, 1, 16, 0);
		}
        else if (2 == recordData->type)
        {
            this->notifyVideoStream((void*)recordData->data, recordData->size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis(), TAG_HIK);
        }

		this->traffic(recordData->size);
		this->m_lastpts = sys->currentTimeMillis();

		delete[] recordData->data;
        delete recordData;
        
		sys->sleep(10);
	}
}

void GB28181RecordGu::streamCtrl(int cmd, int param)
{
    int ret = -1;
    string resp = "";
    char strrtsp[128] = { 0 };


    if (cmd == m_streamState && (cmd == PLAYBACK_PAUSE || cmd == PLAYBACK_CONTINUE))
    {
        return;
    }

    if (PLAYBACK_PAUSE == cmd)
    {
        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_PAUSE, m_cseq++);


    }
    else if (PLAYBACK_CONTINUE == cmd)
    {
        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_CONTINUE, m_cseq++);


    }
    else if (PLAYBACK_FAST == cmd)
    {
        char s8Scale[10] = { 0 };
        if (m_speed > 0)
        {
            snprintf(s8Scale, 10, "%d", m_speed);
        }
        else if (0 == m_speed)
        {
            snprintf(s8Scale, 10, "%d", 1);
        }
        else if (-1 == m_speed)
        {
            strcpy(s8Scale, "0.5");
        }
        else if (-2 == m_speed)
        {
            strcpy(s8Scale, "0.25");
        }
        else if (-3 == m_speed)
        {
            strcpy(s8Scale, "0.125");
        }
        else if (-4 == m_speed)
        {
            strcpy(s8Scale, "0.0625");
        }

        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_SCALE, m_cseq++, s8Scale);

    }
    else if (PLAYBACK_SLOW == cmd)
    {
        char s8Scale[10] = { 0 };

        if (m_speed > 0)
        {
            snprintf(s8Scale, 10, "%d", m_speed);
        }
        else if (0 == m_speed)
        {
            snprintf(s8Scale, 10, "%d", 1);
        }
        else if (-1 == m_speed)
        {
            strcpy(s8Scale, "0.5");
        }
        else if (-2 == m_speed)
        {
            strcpy(s8Scale, "0.25");
        }
        else if (-3 == m_speed)
        {
            strcpy(s8Scale, "0.125");
        }
        else if (-4 == m_speed)
        {
            strcpy(s8Scale, "0.0625");
        }

        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_SCALE, m_cseq++, s8Scale);

    }
    else if (PLAYBACK_SETTIME == cmd)
    {

    }
    m_streamState = cmd;
    LOGD_COM("\n%s", strrtsp);
    GB28181Pu * pPu = (GB28181Pu *)m_pu;
    ret = pPu->m_sipua->info(systm->name(), pPu->m_sipua->ip(), pPu->m_sipua->port(), m_addrcode, pPu->ip(), pPu->sport()
        , m_iDialId, strrtsp, resp);
    LOGD_COM("\n%s", resp.c_str());
}
