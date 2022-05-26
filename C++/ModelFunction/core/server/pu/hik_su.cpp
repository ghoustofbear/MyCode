#include "hik_su.h"
#include "hik_pu.h"

#include "tls_error.h"
#include "session.h"
#include "frame.h"

#include "encoder_g711u.h"
#include "encoder_g711a.h";

HikSu::HikSu(Pu * pu, int channelNo)
    : Su(pu, channelNo)
{
    m_dualHandle   = -1;
	m_audio_encode = NULL;
    m_workspace    = new unsigned char[SEND_SIZE];
	m_buff         = new unsigned char[BUFFER_SIZE];
	m_size         = 0;
}

HikSu::HikSu(Pu *pu, string addrcode)
    : Su(pu, addrcode)
{
    m_dualHandle   = -1;
	m_audio_encode = NULL;
    m_workspace    = new unsigned char[SEND_SIZE];
	m_buff         = new unsigned char[BUFFER_SIZE];
	m_size         = 0;
}

HikSu::~HikSu()
{
    stop();
    printf("~HikSu\n");
    if (m_workspace)
    {
        delete[] m_workspace;
		m_workspace = NULL;
    }
	if (m_buff)
	{
		delete[] m_buff;
		m_buff = NULL;
		m_size = 0;
	}

    if (m_audio_encode)
    {
        delete m_audio_encode;
        m_audio_encode = NULL;
    }
}

int
HikSu::start()
{
    do
    {
        HikPu* hpu = (HikPu*)m_pu;

        if (!hpu->online())
        {
            ERRE_MSV("dualtalk device ip: %s is not on-line.", m_pu->ip().c_str());
            return -1;
        }


        int s32Ret = -1;
        uint32_t dwVoiceChan = 0;
        HIK_DVR_CFG dvrCfg = hpu->getDvrCfg();
        long lUserID = hpu->getUserID();
        if (m_channelNo < dvrCfg.byStartChan + dvrCfg.byChanNum)
        {
            dwVoiceChan = m_channelNo + 1;
        }
        else
        {
            NET_DVR_IPPARACFG_V40 ipCfg;
            DWORD dwReturn;
            BOOL bRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_IPPARACFG_V40, 0, &ipCfg, sizeof(NET_DVR_IPPARACFG_V40), &dwReturn);
            if (!bRet)
            {
                LOGE_MSV("NET_DVR_GetDVRConfig failed no:%d\n", NET_DVR_GetLastError());
                break;
            }

            if ((unsigned long)m_channelNo + 1 < ipCfg.dwStartDChan)
            {
                LOGE_MSV("channel number is invilide:%d\n", m_channelNo);
                break;
            }
            else
            {
                dwVoiceChan = (m_channelNo - ipCfg.dwStartDChan + 1) + 3;  //digital 语音通道从3开始
            }
        }


        if (m_dualHandle == -1)
        {
            NET_DVR_COMPRESSION_AUDIO dca;
            memset(&dca, 0, sizeof(dca));
            NET_DVR_GetCurrentAudioCompress(lUserID, &dca);//to get current audio dualtalk compress param
			BYTE AudioEncType = dca.byAudioEncType;
			if (1 == AudioEncType)//G711U
			{
				m_audio_encode = new G711UEncoder();
			}
			else if (2 == AudioEncType)//G711A
			{
				m_audio_encode = new G711AEncoder();
			}
            m_dualHandle = NET_DVR_StartVoiceCom_MR_V30(lUserID, dwVoiceChan, NULL, this);
            if (-1 == m_dualHandle)
            {
				int ErrorCode = NET_DVR_GetLastError();
				LOGI_MSV("NET_DVR_StartVoiceCom_MR_V30 failed no:%d\n", ErrorCode);
                break;
            }
            m_dualtalkCount = 1;
        }
        else
        {
            m_dualtalkCount++;
        }

        m_vState = 1;
        sys->currentTimeMillis();

		//dualtalkSuMgr->addSu(this);

        return 0;

    } while (0);

    return -1;
}

void
HikSu::stop()
{
    if (-1 != m_dualHandle)
    {
        if (--m_dualtalkCount == 0)
        {
            NET_DVR_StopVoiceCom(m_dualHandle);
            m_vState = 0;
        }
    }
	//dualtalkSuMgr->delSu(this);
    return;
}

int
HikSu::reset()
{
    return 0;
}

void
HikSu::doVideoStream(void *stream, uint32_t size)
{

}

void
HikSu::doAudioStream(void *stream, uint32_t size, int flag)
{
    if (-1 == m_dualHandle)
    {
        return;
    }
    
    m_lastpts = sys->currentTimeMillis();

    if (0 == flag)//contain 40 bytes header
    {
        ut_frame_s* hdr = (ut_frame_s*)stream;
		stream = hdr->buf;
		size = hdr->size;
		flag = 1;
    }
	if (1 == flag)
    {
        if (size + m_size <= BUFFER_SIZE)
        {
			memcpy(m_buff + m_size, stream, size);
			m_size += size;
			int send_time   = m_size / (SEND_SIZE * 2);
			int remain_size = m_size % (SEND_SIZE * 2);//remain data size
			int i = 0;
			for (; i < send_time; i++)//encode and send to device repeatedly
			{
				encode_send(m_buff + SEND_SIZE * 2 * i);
			}
			if (i == send_time)
			{
				memcpy(m_buff, m_buff + SEND_SIZE * 2 * send_time, remain_size);
				m_size = remain_size;//reset m_buff size
			}

        }
		return;
    }
	if(20 == flag)//do not need encode,used by GB28181,see gsession.cpp
	{
		if (size + m_size <= BUFFER_SIZE)
		{
			memcpy(m_buff + m_size, stream, size);
			m_size += size;
			int send_time   = m_size / (SEND_SIZE * 1);//send data size
			int remain_size = m_size % (SEND_SIZE * 1);//remain data size
			int i = 0;
			for (; i < send_time; i++)//encode and send to device repeatedly
			{
				NET_DVR_VoiceComSendData(m_dualHandle, (char*)(m_buff + SEND_SIZE * 1 * i), SEND_SIZE);
			}
			if (i == send_time)
			{
				memcpy(m_buff, m_buff + SEND_SIZE * 1 * send_time, remain_size);
				m_size = remain_size;//reset m_buff size
			}

		}
		return;
	}
}

void
HikSu::encode_send(void* data, int size)//encode and send to device
{
	if (NULL != m_audio_encode && NULL != data && SEND_SIZE * 2 == size)//encoder may be empty,size must be SEND_SIZE * 2
	{
		uint64_t pts = 0;
		int  m_index = 0;
		m_audio_encode->encode(data, size, m_workspace, m_index, pts);
		if (SEND_SIZE == m_index)
		{
			NET_DVR_VoiceComSendData(m_dualHandle, (char*)m_workspace, m_index);
			//printf("audio s:%d, \n", m_index);
		}
	}
}

void
HikSu::doStatus(int code)
{

}

void
HikSu::onTimer()
{
    Su::onTimer();
}

void
HikSu::recvloop()
{

}

void *
HikSu::threadEntry(ThreadInfo * info)
{
    HikSu * su = (HikSu*)(info->data);
    switch (info->id)
    {
    case 0:
        su->recvloop();
        break;
    default:
        break;
    }


    return NULL;
}
