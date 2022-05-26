#include "hik_ehome_su.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "encoder_g711u.h"


HikEhomeSu::HikEhomeSu(Pu * pu, int channelNo)
    : Su(pu, channelNo)
{
	m_audio_encode = new G711UEncoder();
	m_workspace = new unsigned char[SEND_SIZE];
	m_buff = new unsigned char[BUFFER_SIZE];
	m_size = 0;
	m_deviceID = -1;
	m_lTalkHandle  = -1;
}

HikEhomeSu::HikEhomeSu(Pu *pu, string addrcode)
    : Su(pu, addrcode)
{
	m_audio_encode = new G711UEncoder();
	m_workspace = new unsigned char[SEND_SIZE];
	m_buff = new unsigned char[BUFFER_SIZE];
	m_size = 0;
	m_deviceID = -1;
	m_lTalkHandle = -1;
}

HikEhomeSu::~HikEhomeSu()
{
	stop();
	if (m_audio_encode)
	{
		delete m_audio_encode;
		m_audio_encode = NULL;
	}
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
}

int
HikEhomeSu::start()
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	if (!pu->online())
	{
		ERRE_MSV("dualtalk device ip: %s is not on-line.", pu->ip().c_str());
		return -1;
	}
	m_deviceID = pu->guname(m_channelNo);//保存为成员变量，避免死锁
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToUserId.find(m_deviceID);
	if (pos == pu->m_deviceIDToUserId.end())
	{
		return -1;
	}
	LONG m_lUserID = pos->second;
	if (-1 == m_lUserID)
	{
		return -1;
	}

	//获取语音转发支持的音频格式
	m_deviceInfo.dwSize = sizeof(NET_EHOME_DEVICE_INFO);
	NET_EHOME_CONFIG struCfg = { 0 };//配置参数
	struCfg.pOutBuf = &m_deviceInfo;
	struCfg.dwOutSize = sizeof(NET_EHOME_DEVICE_INFO);
	NET_ECMS_GetDevConfig(m_lUserID, NET_EHOME_GET_DEVICE_INFO, &struCfg, sizeof(NET_EHOME_CONFIG));
	DWORD audioEncType = m_deviceInfo.dwAudioEncType;//语音对讲音频格式，0-G722，1-G711U，2-G711A，3-G726，4-AAC，5-MP2L2,6-PCM,7-MP3

	//开始语音对讲
	NET_EHOME_VOICETALK_PARA struVoiceTalkPara = {0};  //语音转发参数结构体
	struVoiceTalkPara.bNeedCBNoEncData         = 0;    //需要回调的语音类型：0- 编码后语音，1- 编码前语音(语音转发时不支持) 
	struVoiceTalkPara.cbVoiceDataCallBack      = NULL; //音频数据回调函数
	struVoiceTalkPara.dwEncodeType             = 1;    //SDK赋值，SDK的语音编码类型：0- G722，1- G711U，2- G711A，3- G726，4- AAC，5- MP2L2，6- PCM
	struVoiceTalkPara.byVoiceTalk              = 1;    //0- 语音对讲，1- 语音转发，2-语音广播
	struVoiceTalkPara.byDevAudioEnc;                   //（输出参数）设备的音频编码类型：0-OggVorbis，1-G.711U，2-G.711A，3-G.726，4-AAC，5-MP2L2， 6-PCM。
	DWORD dwVoiceChan                          = 1;    //语音对讲通道号
	m_lTalkHandle = NET_ECMS_StartVoiceTalk(m_lUserID, dwVoiceChan, &struVoiceTalkPara);
	if (m_lTalkHandle < 0)
	{
		ERRE_MSV("NET_ECMS_StartVoiceTalk failed, error code: %d\n", NET_ECMS_GetLastError());
		return -1;
	}
	{
		AutoLock locker(pu->m_deviceIDToTalkHandle._mtx);
		pu->m_deviceIDToTalkHandle[m_deviceID] = m_lTalkHandle;
	}
	return 0;
}

void
HikEhomeSu::stop()
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToTalkHandle.find(m_deviceID);
	if (pos == pu->m_deviceIDToTalkHandle.end())
	{
		return;
	}
	m_lTalkHandle = pos->second;//音频转发句柄
	if (-1 != m_lTalkHandle)
	{
		//停止语音对讲
		bool iRet = NET_ECMS_StopVoiceTalk(m_lTalkHandle);
		pu->m_deviceIDToTalkHandle[m_deviceID] = -1;
	}
    return;
}

int
HikEhomeSu::reset()
{
    return 0;
}

void
HikEhomeSu::encode_send(void* data, int size)//encode and send to device
{
	if (NULL != m_audio_encode && NULL != data && SEND_SIZE * 2 == size)//encoder may be empty,size must be SEND_SIZE * 2
	{
		uint64_t pts = 0;
		int  m_index = 0;
		m_audio_encode->encode(data, size, m_workspace, m_index, pts);
		if (SEND_SIZE == m_index)
		{
			NET_ECMS_SendVoiceTransData(m_lTalkHandle, (char*)m_workspace, m_index);
			//printf("audio s:%d, \n", m_index);
		}
	}
}

void
HikEhomeSu::doVideoStream(void *stream, uint32_t size)
{

}

void
HikEhomeSu::doAudioStream(void *stream, uint32_t size, int flag)
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	string deviceID = pu->guname(m_channelNo);
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToTalkHandle.find(deviceID);
	if (pos == pu->m_deviceIDToTalkHandle.end())
	{
		return;
	}
	m_lTalkHandle = pos->second;
	if (-1 == m_lTalkHandle)
	{
		return;
	}
	m_lastpts = sys->currentTimeMillis();
	//将来自客户端软件的语音转发到设备
	if (0 == flag)//contain 40 bytes header
	{
		ut_frame_s* hdr = (ut_frame_s*)stream;
		stream = hdr->buf;
		size = hdr->size;
		flag = 1;
	}
	if (1 == flag)
	{
		if (int(size) + m_size <= BUFFER_SIZE)
		{
			memcpy(m_buff + m_size, stream, size);
			m_size += size;
			int send_time = m_size / (SEND_SIZE * 2);
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
}

void
HikEhomeSu::doStatus(int code)
{

}

void
HikEhomeSu::onTimer()
{
	Su::onTimer();
}