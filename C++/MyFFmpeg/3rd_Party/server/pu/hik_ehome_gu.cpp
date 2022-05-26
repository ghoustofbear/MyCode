#include "hik_ehome_gu.h"
#include "hik_ehome_pu.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "3rd/hikehome/HCISUPStream.h"
#include "platform.h"
#include "PsProcessApi.h"
#include "json/json.h"


/*H264/G711回调函数*/
void H264streamdatacb(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime)
{
	HikEhomeRealGu *pGu = (HikEhomeRealGu *)userdata;
	switch (type)
	{
	case 'I':
	case 'P':
	{						  
		pGu->notifyVideoStream(data, size, ENCODE_H264, 0, pGu->m_s32Width, pGu->m_s32Height, 25, pts);
	}
		break;
	case 'A':
		pGu->notifyAudioStream(data, size, ENCODE_G711A, 8000, 1, 16, pts);
		break;
	default:
		return;
	}
	pGu->traffic(size);
	pGu->m_lastpts = sys->currentTimeMillis();
}




HikEhomeRealGu::HikEhomeRealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType ,int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
	m_lRealHandle  = -1;
	m_lSessionID   = -1;
	m_deviceID     = NULL;
	//psparser = createpsinstance(PS_UNPACK);               //创建PS解封装对象
	//setpsstreamcallback(psparser, H264streamdatacb, this);//设置回调函数
}

HikEhomeRealGu::HikEhomeRealGu(Pu *pu, string addrcode, int streamType, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
	m_lRealHandle = -1;
	m_lSessionID  = -1;
	m_deviceID    = NULL;
	//psparser = createpsinstance(PS_UNPACK);               //创建PS封装对象
	//setpsstreamcallback(psparser, H264streamdatacb, this);//设置回调函数
}

HikEhomeRealGu::~HikEhomeRealGu()
{
	if (NULL != m_deviceID)//避免内存泄漏
	{
		delete m_deviceID;
		m_deviceID = NULL;
	}
}

int  HikEhomeRealGu::liveStart()
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToUserId.find(m_name);
	if (pos == pu->m_deviceIDToUserId.end())
	{
		return -1;
	}
	long lUserId = pos->second;

	//CMS向设备发送预览开始请求,将SMS的IP地址和端口号发送给设备，并自动为CMS分配一个会话ID
	NET_EHOME_PREVIEWINFO_IN_V11 struPreviewIn = { 0 };//预览请求的输入参数
	struPreviewIn.iChannel =   1;
	struPreviewIn.dwLinkMode   = 0;//接入方式 0:TCP 1:UDP
	struPreviewIn.dwStreamType = m_streamType;//码流类型
    memcpy(struPreviewIn.struStreamSever.szIP, HikEhomePu::ExternalHost.c_str(), sizeof(struPreviewIn.struStreamSever.szIP));
	struPreviewIn.struStreamSever.wPort = (unsigned short)(HikEhomePu::ExternalPort);//SMS端口号，和SMS监听端口号一致
	struPreviewIn.byDelayPreview = 0;
	struPreviewIn.byEncrypt = 0;	

	NET_EHOME_PREVIEWINFO_OUT struPreviewOut = { 0 };//预览请求的输出参数
	if (!NET_ECMS_StartGetRealStreamV11(lUserId, &struPreviewIn, &struPreviewOut))
	{
		int code = NET_ECMS_GetLastError();
		ERRE_MSV("NET_ECMS_StartGetRealStream failed, error code: %d\n", NET_ECMS_GetLastError());
		return -1;
	}
	else
	{
		ERRI_MSV("NET_ECMS_StartGetRealStream succeed\n");
	}
	m_lSessionID = struPreviewOut.lSessionID;
	NET_EHOME_PUSHSTREAM_IN struPushStreamIn = {0};//码流传输请求的输入参数
	struPushStreamIn.dwSize = sizeof(struPushStreamIn);    
	struPushStreamIn.lSessionID = m_lSessionID;//预览请求的会话ID
	NET_EHOME_PUSHSTREAM_OUT struPushStreamOut = {0};//码流传输请求的输出参数
	//CMS向设备发送码流传输请求，设备自动连接SMS，并开始传输实时码流
	if (!NET_ECMS_StartPushRealStream(lUserId, &struPushStreamIn, &struPushStreamOut))
	{        
		int code = NET_ECMS_GetLastError();
		ERRE_MSV("NET_ECMS_StartPushRealStream failed, error code: %d\n", NET_ECMS_GetLastError());
		return -1;    
	}
	else
	{
		ERRI_MSV("NET_ECMS_StartPushRealStream success\n");
	}
	return 0;
}

int
HikEhomeRealGu::liveStop()
{ 
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToUserId.find(m_name);
	if (pos == pu->m_deviceIDToUserId.end())    
	{
		return -1;
	}
	LONG lUserId = pos->second;
	if (!NET_ECMS_StopGetRealStream(lUserId, m_lSessionID))//CMS向设备发送预览停止请求
	{        
		ERRE_MSV("NET_ECMS_StopGetRealStream failed, error code: %d\n", NET_ECMS_GetLastError());
	}

	if (m_lRealHandle != -1)
	{
		//监听端口与多个通道相关联,调用接口,其中一个通道的预览停止
		if (!NET_ESTREAM_StopPreview(m_lRealHandle))//SMS停止转发实时码流从而停止预览
		{
			ERRE_MSV("NET_ESTREAM_StopPreview failed, error code: %d\n", NET_ECMS_GetLastError());
		}
		m_lRealHandle = -1;
	}
	return 0;
}
int
HikEhomeRealGu::start(int offset)
{
 	if (!m_pu->online())
	{
		ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());
		return -1;
	}

	if (!m_pu->validate(m_channelNo))
	{
		ERRE_MSV("device ip: %s, chn: %d, stream:%d  is not exist.", m_pu->ip().c_str(), m_channelNo, m_streamType);
		return -1;
	}
	if (liveStart() < 0 )
	{
		return -1;
	}
	realGuMgr->addGu(this);
	return 0;
}

void
HikEhomeRealGu::stop()
{
	realGuMgr->delGu(this);
	liveStop();
}

void
HikEhomeRealGu::onTimer()
{
    Gu::onTimer();
}
