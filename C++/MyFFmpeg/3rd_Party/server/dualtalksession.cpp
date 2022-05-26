#include "dualtalksession.h"
#include "putree.h"
#include "pu.h"
#include "gu.h"
#include "su.h"
#include "socket.h"
#include "bufferev.h"
#include "frame.h"
#include "mempool.h"
#include "stk/stk_message_http.h"
#include "priv_server.h"
#include "cfg_device.h"
#include "cfg_camera_related.h"
#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

DualtalkSession::DualtalkSession(Client *client, Bufferev *bev)
{
    m_client = client;
    m_bConnect = false;

    m_tmpAudioBuf = new char[1024*1024];
    memset(m_tmpAudioBuf, 0, 1024 * 1024);
    m_tmpAudioBufUsed = 0;
    m_last_send = sys->currentTimeMillis();



	m_bev = bev;
	m_bev->addBuffevListener(this);

}

DualtalkSession::~DualtalkSession()
{
    DualtalkSession::stop();
    printf("~DualtalkSession  DualtalkSession\n");

    delete[] m_tmpAudioBuf;

	m_bev->delBuffevListener(this);
}

int
DualtalkSession::addSu(string host, int port, int chn)
{
    LOGD_MSV("add talk");
    Pu *pu = putree->findPu(host, port);
    if (pu)
    {
		//判断pu下chn有无对讲属性勾选，有则继续，无则查找摄像机共享关联表
		string json;
		char condition[100];
		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		string addrcode = pu->gucode(chn);
		sprintf(condition, "addrcode = %s", addrcode.c_str());
		deviceCfg->query(json,condition);
		if (reader.parse(json, rvalue, false) && rvalue.isArray() && 1 == rvalue.size())
		{
			int abt = rvalue[0]["recvCall"].asInt();
			if (!(abt & 0x8))//若对讲属性未勾选,则查询摄像机共享关联表
			{
				sprintf(condition, "camera_addrcode = %s or camera_related = %s", addrcode.c_str(),addrcode.c_str());
				cameraRelatedCfg->query(json, condition);
				if (reader.parse(json, rvalue, false) && rvalue.isArray() && 1 == rvalue.size())
				{
					string camera_addrcode = rvalue[0]["camera_addrcode"].asString();
					string camera_related  = rvalue[0]["camera_related"].asString();
					if (addrcode == camera_addrcode)//赋值为共享摄像机的地址码
					{
						addrcode = camera_related;
					}
					else
					{
						addrcode = camera_addrcode;
					}
					sprintf(condition, "addrcode = %s", addrcode.c_str());
					deviceCfg->query(json, condition);
					if (reader.parse(json, rvalue, false) && rvalue.isArray() && 1 == rvalue.size())
					{
						string parentcode = rvalue[0]["parentCode"].asString();
						int abt = rvalue[0]["recvCall"].asInt();
						if (abt & 0x8)//共享摄像机勾选对讲属性
						{
							pu = putree->findPu(parentcode);
							chn = rvalue[0]["chnNo"].asInt();
						}
						else
						{
							return -1;
						}
					}
				}
			}
		}

		///////////////////////////////////////////////////////////////
        Su *su = pu->addDualTalkGu(chn, "", 0);
        if (su)
        {
            addStreamSpeaker(su);
            m_sulist[pu] = su;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}


int
DualtalkSession::addSu(string addrcode)
{
    LOGD_MSV("add talk, addrcode:%s", addrcode.c_str());
    Pu *pu = putree->findPu(addrcode);
    if (pu)
    {
        Su *su = pu->addDualTalkGu(addrcode);
        if (su)
        {
            addStreamSpeaker(su);
            m_sulist[pu] = su;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int
DualtalkSession::start(string json)
{
    //containt channes;
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("paser err: %s", json.c_str());
        return -1;
    }

    int m_channelNo = rvalue["channelNo"].asInt();
    int m_obtainType = rvalue["obtainType"].asInt();
    int m_streamType = rvalue["streamType"].asInt();
    int m_transport = rvalue["transport"].asInt();

    int result = -1;

	m_alive.setEntry(threadEntry, this, 0);

    if (0 == m_obtainType || 1 == m_obtainType)
    {
        string m_deviceHost = rvalue["deviceHost"].asString();
        int m_devicePort = rvalue["devicePort"].asInt();
        string m_subServHost = rvalue["subServHost"].asString();
        int m_subServPort = rvalue["subServPort"].asInt();

		m_alive.start();
        return addSu(m_deviceHost, m_devicePort, m_channelNo);

    }
    else if (2 == m_obtainType || 3 == m_obtainType)
    {

        string addrcode = rvalue["addrcode"].asString();
		m_alive.start();
		return addSu(addrcode);
    }
    else
    {
        ERRE_MSV("not support");
        return -1;
    }
}

void
DualtalkSession::stop()
{
    AutoLock lock(m_sulist._mtx);
    map<Pu*, Su*>::const_iterator i;
    for (i = m_sulist.begin(); i != m_sulist.end(); ++i)
    {
        i->first->delDualTalkGu(i->second);
        delStreamSpeaker(i->second);
    }
    m_sulist.clear(); 
    m_bConnect = false;
	m_alive.stop();
}


void*
DualtalkSession::threadEntry(ThreadInfo *arg)//?a??èy???í??μ??êìa
{
	DualtalkSession* ds = (DualtalkSession*)arg->data;
	while (ds->m_alive.active())
	{
		uint64_t curr = sys->currentTimeMillis();
		//m_last_send = curr;

		if (curr - ds->m_last_send >= 1000)
		{
			LOGD_MSV("talk heartbeat");

			stk::HttpMsg msg;
			msg.SetMethod("POST");
			msg.SetReqUrl("/heartbeat");
			msg.SetVersion("HTTP/1.1");
			msg.SetHost("localhost");
			msg.SetConnection("Keep-Alive");

			string strMsg;
			msg.to_str(strMsg);
			ds->onResponsePacket(ds->m_bev, (uint8_t*)strMsg.c_str(), strMsg.size());
			ds->m_last_send = curr;
		}
	}
	return NULL;

}

void
DualtalkSession::onWrite(Bufferev *bev)
{    

    if (!m_bConnect)
    {
        return;
    }

    uint64_t curr = sys->currentTimeMillis();
    //m_last_send = curr;

    if (curr - m_last_send >= 1000)
    {
        LOGD_MSV("talk heartbeat");

        stk::HttpMsg msg;
        msg.SetMethod("POST");
        msg.SetReqUrl("/heartbeat");
        msg.SetVersion("HTTP/1.1");
        msg.SetHost("localhost");
        msg.SetConnection("Keep-Alive");

        string strMsg;
        msg.to_str(strMsg);

        onResponsePacket(m_bev, (uint8_t*)strMsg.c_str(), strMsg.size());
        m_last_send = curr;
    }

}

void
DualtalkSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    char buf[1024] = { 0 };
    ut_frame_s *hdr = (ut_frame_s *)buf;
    memcpy(hdr->buf, stream, size);

    hdr->tag = TAG_STD;

    hdr->type = 'A';
    hdr->encode = encode;
    hdr->channel = channels;
    hdr->encrypt = 0;

    hdr->size = size;
    hdr->pts = pts;
    hdr->time = pts;
    hdr->seq = 0;

    hdr->audio.sps = sps;
    hdr->audio.channels = channels;
    hdr->audio.bps = bps;
    hdr->audio.packLen = size;

    if (m_bev)
    {
        m_bev->push((uint8_t*)buf, (sizeof(ut_frame_s) + size));
    }
}

void
DualtalkSession::onRead(Bufferev *bev, void *data, uint32_t size)
{
    //m_bev = bev;


    ASSERT((int)(size) <= 1024*1024 - m_tmpAudioBufUsed);

    memcpy(m_tmpAudioBuf+m_tmpAudioBufUsed, data, size);
    m_tmpAudioBufUsed += size;

    //printf("onRead:asize:%d  used:%d\n", size, m_tmpAudioBufUsed);

    ut_frame_s *hdr = NULL;
    char *src = m_tmpAudioBuf;  

    for (int i = 0, tl = m_tmpAudioBufUsed; i < tl;)
    {
        hdr = (ut_frame_s*)(src + i);
        if (hdr->tag == TAG_STD && hdr->type == 'A')
        {
#if 1
            int fl = sizeof(ut_frame_s) + hdr->size;

            if (fl <= m_tmpAudioBufUsed)
            {
                notifyAudioStream(hdr, fl);
                i += fl;
                m_tmpAudioBufUsed -= fl;
                //printf("   notifyAudio i:%d tl:%d used:%d audiolen:%d\n", i, tl, m_tmpAudioBufUsed, fl);
            }
            else
            {
                memcpy(m_tmpAudioBuf, src+i, m_tmpAudioBufUsed);
                //m_tmpAudioBufUsed = size - i;
                //printf("        movemem start:%d len:%d\n", i, m_tmpAudioBufUsed);
                break;
            }
#else         
            int fl = sizeof(ut_frame_s) + hdr->size;
            if (fl <= m_tmpAudioBufUsed - i)
            {
                notifyAudioStream(hdr, fl);
                
                memcpy(hdr, hdr + fl, m_tmpAudioBufUsed - i - fl);
                m_tmpAudioBufUsed -= fl;
                //i = i + fl;

                printf("recf i:%d used:%d audio:%d\n", i, m_tmpAudioBufUsed, hdr->size);
            }
            else
            {
                break;
            }
#endif   
        }
        else
        {
            i++;
            //printf("aabb\n");
        }

    }

    m_bConnect = true;
}

void
DualtalkSession::onClose(Bufferev *bev)
{
    printf("DualtalkSession::onClose\n");
    m_active = false;  
    m_status = SS_CLOSED;
}


