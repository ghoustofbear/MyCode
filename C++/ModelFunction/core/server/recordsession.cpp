#include "recordsession.h"
#include "putree.h"
#include "pu.h"
#include "socket.h"
#include "frame.h"
#include "bufferev.h"
#include "mempool.h"
#include "stk/stk_message_http.h"
#include "cfg_camera_plate.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

#define RECORD_BUFFER_MAX  (4*1024*1024)
#define RECORD_BUFFER_MIN  (512*1024)

RecordSession::RecordSession(Client *client)
{
    m_client = client;
    m_seq    = 0;
    m_buffStatus = S_BUFF_NONE;    
	m_pu         = NULL;
	m_lastSendpts = 0;
	m_rpause = false;
	m_rspeed = 0;
	m_videoframecnt = 0;
}

RecordSession::~RecordSession()
{
    stop();
}

int
RecordSession::start(string json, int mode)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("paser err: %s", json.c_str());
        return -1;
    }

    m_obtainType = rvalue["obtainType"].asInt();
    int streamType;
    if (m_obtainType == 0 || m_obtainType == 2)
    {
        streamType = 0;
    }
    else
    {
        streamType = 1;
    }

    int result   = -1;

    if (0 == m_obtainType || 1 == m_obtainType)
    {
        m_deviceHost = rvalue["deviceHost"].asString();
        m_devicePort = rvalue["devicePort"].asInt();
        m_offset = rvalue["offset"].asInt();
        int reverse = rvalue["reverse"].asInt();

        m_pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());

		if (!m_pu)
        {
            return -1;
        }

        if (!rvalue["filename"].isNull())
        {
            m_filename = rvalue["filename"].asString();

			if (Pu::PROTO_TYPE_UT == m_pu->protocol())
            {
                m_addrcode = putree->findCode(m_deviceHost, m_devicePort, m_channelNo);
				result = m_pu->addRecordGu(m_filename, m_addrcode, streamType, this, reverse, mode);
            }
			else if (Pu::PROTO_TYPE_HIK == m_pu->protocol() && m_pu->type() == Pu::DEV_TYPE_IPC)
            {
                //20200619 zhangshenyu add
                string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), 0);
                string plateJson;
                char cond[512] = { 0 };
                sprintf(cond, "plate_camaddr='%s'", addrcode.c_str());
                int ret = cameraPlateCfg->query(plateJson, cond);
                if (0 == ret)
                {
                    SHL_Json::Reader readerPlate;
                    SHL_Json::Value  rvaluePlate;
                    if (!readerPlate.parse(plateJson, rvaluePlate, false))
                    {
                        return -1;
                    }
                    if (rvaluePlate.size() != 1)
                    {
                        return -1;
                    }
                    addrcode = rvaluePlate[0]["nvr_camaddr"].asString();
					m_pu = putree->findPu(addrcode);
					m_pu = putree->findPu(m_pu->host(), m_pu->port());
					m_deviceHost = m_pu->host();
					m_devicePort = m_pu->port();
					result = m_pu->addRecordGu(m_filename, m_pu->host(), m_pu->port(), streamType, this, reverse, mode);
                }
            }
            else
            {
				result = m_pu->addRecordGu(m_filename, rvalue["subServHost"].asString(), rvalue["subServPort"].asInt(), streamType, this, reverse, mode);
            }
        }
        else if (!rvalue["start"].isNull())
        {
            string     sstart = rvalue["start"].asString();
            string     send   = rvalue["end"].asString();
            datetime_s start;
            datetime_s end;

            sys->strptime(sstart, start);
            sys->strptime(send,   end);

			if (Pu::PROTO_TYPE_UT == m_pu->protocol())
            {
                m_addrcode = putree->findCode(m_deviceHost, m_devicePort, m_channelNo);
				result = m_pu->addRecordGu(m_addrcode, rvalue["type"].asInt(), start, end, streamType, this, mode);
            }
			else if (Pu::PROTO_TYPE_HIK == m_pu->protocol() && m_pu->type() == Pu::DEV_TYPE_IPC)
            {
                //20200619 zhangshenyu add
                string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), 0);
                string plateJson;
                char cond[512] = { 0 };
                sprintf(cond, "plate_camaddr='%s'", addrcode.c_str());
                int ret = cameraPlateCfg->query(plateJson, cond);
                if (0 == ret)
                {
                    SHL_Json::Reader readerPlate;
                    SHL_Json::Value  rvaluePlate;
                    if (!readerPlate.parse(plateJson, rvaluePlate, false))
                    {
                        return -1;
                    }
                    if (rvaluePlate.size() != 1)
                    {
                        return -1;
                    }
                    addrcode = rvaluePlate[0]["nvr_camaddr"].asString();
					m_pu = putree->findPu(addrcode);
					m_pu = putree->findPu(m_pu->host(), m_pu->port());
					int chanNo = m_pu->guchannel(addrcode);
					m_deviceHost = m_pu->host();
					m_devicePort = m_pu->port();
                   
					result = m_pu->addRecordGu(
                                 chanNo,
                                 rvalue["type"].asInt(),
                                 start,
                                 end,
                                 rvalue["subServHost"].asString(),
                                 rvalue["subServPort"].asInt(),
                                 streamType,
                                 this,
                                 mode);
                }

            }
            else
            {
				result = m_pu->addRecordGu(
                             rvalue["channelNo"].asInt(),
                             rvalue["type"].asInt(),
                             start,
                             end,
                             rvalue["subServHost"].asString(),
                             rvalue["subServPort"].asInt(),
                             streamType,
							 this, 
							 mode);
            }
        }
        else
        {
            ERRE_MSV("invalidate parameters");
            return -1;
        }
    }
    else if (2 == m_obtainType || 3 == m_obtainType)
    {
        m_addrcode = rvalue["addrcode"].asString();

        m_pu = putree->findPu(m_addrcode);
		if (!m_pu)
        {
            return -1;
        }
		if (Pu::PROTO_TYPE_HIK == m_pu->protocol() && m_pu->type() == Pu::DEV_TYPE_IPC)
        {
        /** 202006019 zhangshenyu add  **/
			string addrcode = m_pu->addrcode();
            string plateJson;
            char cond[512] = { 0 };
            sprintf(cond, "plate_camaddr='%s'", addrcode.c_str());
            int ret = cameraPlateCfg->query(plateJson, cond);
            if (0 == ret)
            {
                SHL_Json::Reader readerPlate;
                SHL_Json::Value  rvaluePlate;
                if (!readerPlate.parse(plateJson, rvaluePlate, false))
                {
                    return -1;
                }
                if (rvaluePlate.size() != 1)
                {
                    return -1;
                }
                m_addrcode = rvaluePlate[0]["nvr_camaddr"].asString();
				m_pu = putree->findPu(m_addrcode);
            }
        }

        if (!rvalue["filename"].isNull())
        {
            m_filename = rvalue["filename"].asString();
			result = m_pu->addRecordGu(m_filename, m_addrcode, streamType, this, mode);
        }
        else if (!rvalue["start"].isNull())
        {
            string     sstart = rvalue["start"].asString();
            string     send   = rvalue["end"].asString();
            datetime_s start;
            datetime_s end;

            sys->strptime(sstart, start);
            sys->strptime(send,   end);

			result = m_pu->addRecordGu(m_addrcode, rvalue["type"].asInt(), start, end, streamType, this, mode);
        }
    }
    else
    { }

    if (0 == result)
    {
        m_time = sys->currentTime();
    }

    if (mode == 1)
    {
        //download
        m_rspeed = 4;        
    }

    m_sendThread.setEntry(threadProc, this, 0);
    m_sendThread.start();

    return result;
}


void
RecordSession::stop()
{
	m_sendThread.stop();
	if (m_pu)
	{
		m_pu->delRecordGu(this);
	}
	clearData();
}


int
RecordSession::stream()
{
    if (0 == m_obtainType || 2 == m_obtainType)
    {
        /* 0-private stream */
        return 0;
    }
    else if (1 == m_obtainType || 3 == m_obtainType)
    {
        /* 1-standard h.264 stream */
        return 1;
    }
    else
    {
        return -1;
    }
}

int
RecordSession::transport()
{
    return 0;
}

string
RecordSession::peerip()
{
    if (m_bev)
    {
        return m_bev->peerip();
    }

    return "";
}

int
RecordSession::peerport()
{
    if (m_bev)
    {
        return m_bev->peerport();
    }

    return 0;
}

string
RecordSession::peertime()
{
    return m_time;
}

int
RecordSession::streamtype()
{
    return 0;
}

void         
RecordSession::setPausestatus(bool status)
{
	m_rpause = status;
}

 int        
RecordSession::getspeed()
{
	 return m_rspeed;
}
 void     
RecordSession::setspeed(int speed)
{
	 m_rspeed = speed;
}

void
RecordSession::onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag)
{
    if (!(1 == m_obtainType || 3 == m_obtainType))
    {
        return;
    }

#ifdef MEMPOOL
    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));;
#else
    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
#endif
    if (!buf)
    {
        return;
    }

    ut_frame_s *hdr = (ut_frame_s *)buf;
    memcpy(hdr->buf, stream, size);

    hdr->tag          = tag;
    hdr->type         = type;
    hdr->encode       = encode;
    hdr->channel      = m_channelNo;
    hdr->encrypt      = 0;

    hdr->size         = size;
    hdr->pts          = pts;
    hdr->time         = pts;
    hdr->seq          = m_seq++;

    hdr->video.width  = w;
    hdr->video.height = h;
    hdr->video.fps    = fps;
    hdr->video.system = 'N';

    if (tag == TAG_POSI ||
        tag == TAG_RPOS)
    {
		onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
		//printf("standar 33");
		return;
    }
    else
    {
        AutoLock locker(m_wque._mtx);
        //if (m_videoframecnt >= 500)
        //{
        //    //³¬¹ý500Ö¡£¬¶ªµô
        //    return;
        //}


        m_wque.push(buf);
		m_videoframecnt++;
        if (m_videoframecnt >= getMaxframecnt())
		{
			m_buffStatus = S_BUFF_FULL;
            //printf("s full buf:%d\n\n\n", m_videoframecnt);
		}
    }
}

void
RecordSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    if (!(1 == m_obtainType || 3 == m_obtainType))
    {
        return;
    }

#ifdef MEMPOOL
    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));
#else
    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
#endif
    if (!buf)
    {
        return;
    }

    ut_frame_s *hdr = (ut_frame_s *)buf;
    memcpy(hdr->buf, stream, size);

    hdr->tag            = TAG_STD;
    hdr->type           = 'A';
    hdr->encode         = encode;
    hdr->channel        = m_channelNo;
    hdr->encrypt        = 0;

    hdr->size           = size;
    hdr->pts            = pts;
    hdr->time           = pts;
    hdr->seq            = m_seq++;

    hdr->audio.sps      = sps;
    hdr->audio.channels = channels;
    hdr->audio.bps      = bps;
    hdr->audio.packLen  = size;

	AutoLock locker(m_wque._mtx);
    m_wque.push(buf);  
}

void
RecordSession::onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type)
{    
    //if (!(0 == m_obtainType || 2 == m_obtainType))
    //{
    //    return;
    //}

#ifdef MEMPOOL
    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));
#else
    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
#endif
    if (!buf)
    {
        return;
    }

    ut_frame_s *hdr = (ut_frame_s *)buf;
    memcpy(hdr->buf, stream, size);

    hdr->tag = tag;
    hdr->type = type;
    hdr->encode = encode;
    hdr->channel = m_channelNo;
    hdr->encrypt = 0;

    hdr->size         = size;
    hdr->pts = sys->currentTimeMillis();
    hdr->time = hdr->pts;
    hdr->seq = m_seq++;

    hdr->video.width = 0;
    hdr->video.height = 0;
    hdr->video.fps = 0;
    hdr->video.system = 'N';

    if (tag == TAG_POSI ||
        tag == TAG_RPOS)
    {
		onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
		//printf("private 112");
		return;
    }
    else
    {
        AutoLock locker(m_wque._mtx);
        //if (m_videoframecnt >= 500)
        //{
        //    //³¬¹ý500Ö¡£¬¶ªµô
        //    return;
        //}

        m_wque.push(buf);
        m_videoframecnt++;
		if (m_videoframecnt >= getMaxframecnt())
		{
			m_buffStatus = S_BUFF_FULL;
            //printf("p full buf:%d\n\n\n", m_videoframecnt);
		}
    }


}



void
RecordSession::clearData()
{
	AutoLock locker(m_wque._mtx);
	while (m_wque.size())
	{
#ifdef MEMPOOL
		mempool->free(m_wque.front());
#else
		free(m_wque.front());
#endif
		m_wque.pop();
	}
	m_videoframecnt = 0;
	m_buffStatus = S_BUFF_EMPTY;
}



int
RecordSession::getframeinterval()
{
	int ret = 40;
	switch (m_rspeed)
	{
	case -4:
		ret = 640;
		break;
	case -3:
		ret = 320;
		break;
	case -2:
		ret = 160;
		break;
	case -1:
		ret = 80;
		break;
	case 0:
		ret = 40;
		break;
	case 1:
		ret = 20;
		break;
	case 2:
		ret = 10;
		break;
	case 3:
		ret = 5;
		break;
	case 4:
		ret = 2;
		break;
	default:
		ret = 40;
		break;
	}
	return ret;
}

/**
*
*
*
*/
int
RecordSession::getMaxframecnt()
{
	int ret = 75;
	switch (m_rspeed)
	{
	case -4:
		ret = 10;
		break;
	case -3:
		ret = 15;
		break;
	case -2:
		ret = 20;
		break;
	case -1:
		ret = 40;
		break;
	case 0:
		ret = 75;
		break;
	case 1:
		ret = 150;
		break;
	case 2:
		ret = 200;
		break;
	case 3:
		ret = 300;
		break;
	case 4:
		ret = 350;
		break;
	default:
		ret = 75;
		break;
	}
	return ret;
}

int
RecordSession::getMinframecnt()
{
	int ret = 25;
	switch (m_rspeed)
	{
	case -4:
		ret = 4;
		break;
	case -3:
		ret = 5;
		break;
	case -2:
		ret = 7;
		break;
	case -1:
		ret = 15;
		break;
	case 0:
		ret = 25;
		break;
	case 1:
		ret = 50;
		break;
	case 2:
		ret = 65;
		break;
	case 3:
		ret = 100;
		break;
	case 4:
		ret = 120;
		break;
	default:
		ret = 25;
		break;
	}
	return ret;
}

int
RecordSession::getframecntpresec()
{
	int ret = 25;
	switch (m_rspeed)
	{
	case -4:
		ret = 2;
		break;
	case -3:
		ret = 3;
		break;
	case -2:
		ret = 6;
		break;
	case -1:
		ret = 13;
		break;
	case 0:
		ret = 25;
		break;
	case 1:
		ret = 50;
		break;
	case 2:
		ret = 100;
		break;
	case 3:
		ret = 200;
		break;
	case 4:
		ret = 200;
		break;
	default:
		ret = 25;
		break;
	}
	return ret;
}

void 
RecordSession::sendRecordData()
{
	int32_t i = 0;
	int32_t framecnt = 0;
	int32_t coasttime = 0;
	int32_t fps = 0;
	m_lastSendpts = sys->currentTimeMillis();

	while(m_sendThread.active() && m_active)
	{
		if (m_buffStatus == S_BUFF_FULL)
		{
			m_pu->pauseRecord(this);
		}

		if (m_buffStatus == S_BUFF_EMPTY)
		{
			m_pu->continueRecord(this);
		}

		if (m_rpause)
		{
			sys->sleep(40);
			m_lastSendpts = sys->currentTimeMillis();
			continue;
		}

		{
			AutoLock locker(m_wque._mtx);
			framecnt = m_wque.size();
			fps = getframecntpresec();
			if (m_videoframecnt < fps)
			{			

				//È«²¿°lËÍ
				for (i = 0; i < framecnt; ++i)
				{
					uint8_t *buf = m_wque.front();
					ut_frame_s *hdr = (ut_frame_s *)buf;
					onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
#ifdef MEMPOOL
					mempool->free(m_wque.front());
#else
					free(m_wque.front());
#endif
					m_wque.pop();					
				}

				coasttime = m_videoframecnt * getframeinterval();

				m_videoframecnt = 0;

				m_buffStatus = S_BUFF_EMPTY;
			}
			else
			{
				i = fps;
				while (i > 0)
				{
					uint8_t *buf = m_wque.front();
					ut_frame_s *hdr = (ut_frame_s *)buf;
					onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
					if (hdr->type == 'P' || hdr->type == 'I')
					{
						m_videoframecnt--;
						i--;
					}
#ifdef MEMPOOL
					mempool->free(m_wque.front());
#else
					free(m_wque.front());
#endif
					m_wque.pop();					
				}
				coasttime = 1000;
				if (m_videoframecnt < getMinframecnt())
				{
					m_buffStatus = S_BUFF_EMPTY;
				}
			}
		}

		uint64_t cpts = sys->currentTimeMillis();
		
		int32_t tempval = int32_t(cpts - m_lastSendpts);		
		int32_t delta = coasttime - tempval;

		m_lastSendpts = cpts;

		if (delta-5 > 0)
		{
			sys->sleep(delta-5);
		}
	}
	//clear data
	clearData();
}

void
RecordSession::onRead(Bufferev *bev, void *data, uint32_t size)
{
	if (!m_pu)
	{
		return;
	}
	stk::HttpMsg httpMsg((const char *)data, size);
	string url = httpMsg.GetReqUrl();
	if ("/pause" == url)
	{
		//m_pu->pauseRecord(this);
		m_rpause = true;
		printf("sdk pause\n");
	}
	else if ("/continue" == url)
	{
		m_rpause = false;
		//m_pu->continueRecord(this);
		printf("sdk continue\n");
	}
	else if ("/startRepos" == url)
	{
		printf("sdk startRepos\n");
		m_pu->startRepos(this);		
		clearData();
		
	}
	else if ("/setPos" == url)
	{
		string body = httpMsg.GetBody();
		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		if (!reader.parse(body, rvalue, false))
		{
			return;
		}
		
		int pos = rvalue["pos"].asInt();
		printf("sdk setPos:%d\n", pos);
		m_pu->setPosRecord(pos, this);

		
	}
	else if ("/setTime" == url)
	{
		string body = httpMsg.GetBody();
		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;

		if (!reader.parse(body, rvalue, false))
		{
			return;
		}

		string time = rvalue["time"].asString();
		datetime_s dt;
		sys->strptime(time, dt);

		printf("sdk setTime:%s\n", time.c_str());
		m_pu->setTimeRecord(dt, this);
	}
	else if ("/fast" == url)
	{	
		if (m_rspeed < 4)
			m_rspeed++;
	}
	else if ("/slow" == url)
	{
		if (m_rspeed > -4)
			m_rspeed--;
	}
	else if ("/heartbeat" == url)
	{
		printf("Message heartbeat\n");
	}
    
}

void
RecordSession::onClose(Bufferev *bev)
{
    m_status = S_SOK;
    m_active = false;
}

void
RecordSession::onStatus(int code)
{
    m_status = code;
}

void*
RecordSession::threadProc(ThreadInfo *info)
{
	RecordSession *session = (RecordSession*)(info->data);

	switch (info->id)
	{
	case 0:
	{
		session->sendRecordData();
		break;
	}

	default:
		break;
	}

	return NULL;
}