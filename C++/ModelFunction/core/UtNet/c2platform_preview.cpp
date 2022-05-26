// c2platform_preview.cpp: implementation of the c2platform_preview class.
//
//////////////////////////////////////////////////////////////////////

#include "c2platform_preview.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformPreview::C2PlatformPreview(C2PlatformConn *pConn) 
	: BasePView()
{
	m_pConn = pConn; 

	m_hPreview = NULL;
	memset(&m_prevParam, 0, sizeof(c2_preview_s));
	
	m_pUserDraw = NULL;
	m_pUserDec = NULL;
	m_pUserReadData = NULL;
	m_pfnDecDataCallback = NULL;
	m_pfnRealDataCallback = NULL;
	m_pfnDrawCallback = NULL;
	m_previewUser = NULL;
}

C2PlatformPreview::~C2PlatformPreview()
{ 
	Stop(&m_PViewClient);	

	m_pConn = NULL; 
	
	m_hPreview = NULL;
	memset(&m_prevParam, 0, sizeof(c2_preview_s));

	m_pUserDraw = NULL;
	m_pUserDec = NULL;
	m_pUserReadData = NULL;
	m_pfnDecDataCallback = NULL;
	m_pfnRealDataCallback = NULL;
	m_pfnDrawCallback = NULL;
}

//************************************************************************
//函数名称：C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata)
//函数说明：用户列表数据回调
//函数参数：
//    (in) hdc 画布
//    (in) wnd 窗口句柄
//    (in) w 宽度
//    (in) h 高度
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
void C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata)
{	
	C2PlatformPreview *pView = (C2PlatformPreview *)userdata;
	
	pView->m_pfnDrawCallback(hdc, w, h, pView->m_pUserDraw);
}

//************************************************************************
//函数名称：C2StatusCallback(int code, void *userdata)
//函数说明：状态回调
//函数参数：
//    (in) code     状态码
//    (in) userdata 用户数据 
//返回值：void
//************************************************************************/
void C2PreviewStatusCallback(int code, void *userdata)
{
	
}

//************************************************************************
//函数名称：C2RealDataCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//函数说明：码流数据回调函数
//函数参数：
//    (in) wnd      当前的预览/回放句柄
//    (in) data     存放数据的缓冲区指针, ut_frame_s+音视频数据
//    (in) size     缓冲区大小
//    (in) userdata 用户数据
//返回值：成功返回true，失败返回false
//************************************************************************/
bool C2RealDataCallback(HWND wnd, void *data, uint32_t size, void *userdata)
{
	C2PlatformPreview *pView = (C2PlatformPreview *)userdata;
	
	if (pView && pView->m_pfnRealDataCallback)
	{
	   pView->m_pfnRealDataCallback(wnd, data, size, pView->m_previewUser);
	}  
	
	return true;  
}

void C2DecodeCallback(int type, int w, int h, void *yuv, int size, void *userdata)
{
	C2PlatformPreview *pView = (C2PlatformPreview *)userdata;

	DEC_FRAME_INFO decFrame = {0};
	decFrame.u8Type = type;
	decFrame.s32Width = w;
	decFrame.s32Height = h;
	decFrame.u32Len = size;
	strncpy(decFrame.ps8Data, (char*)yuv, size);

	pView->m_pfnDecDataCallback(&decFrame, pView->m_pUserDec);
}

//************************************************************************
//函数名称：Start(const NET_PVIEW_PARAM_S *pParam)
//函数说明：启动预览
//函数参数：
//    (in) pParam   预览参数
//返回值：成功返回预览窗口句柄/失败返回NULL
//************************************************************************/
PreviewClient * C2PlatformPreview::Start(const NET_PVIEW_PARAM_S *pParam)
{
	if((NULL != pParam) && (NULL != m_pConn->m_hLogin))
	{		 
		string ip="", subIp = "", addrcode=""; 
		m_prevParam.channelNo = pParam->u32ChnNO;//通道号
		m_prevParam.obtainType = 0;//pParam->u8Content;	//主从-码流  aaaaaaaaaa
		m_prevParam.streamType = pParam->u8Type;	//主从码流
		
		if(NULL != pParam->ps8DeviceIp)//设备IP
		{
			ip = pParam->ps8DeviceIp;
			m_prevParam.deviceHost = const_cast<char*>(ip.c_str());	
		}
		else
		{
			m_prevParam.deviceHost = NULL;
		}
		
		m_prevParam.devicePort = pParam->u16DevicePort;
		
		if(NULL != pParam->ps8SubServIp)
		{
			subIp = pParam->ps8SubServIp;
			m_prevParam.subServHost = const_cast<char*>(subIp.c_str());
		}
		else
		{
			m_prevParam.subServHost = NULL;
		}
		m_prevParam.subServPort = pParam->u16SubServPort;
		
		if(NULL != pParam->ps8FrontAddrCode)
		{
			addrcode = pParam->ps8FrontAddrCode;
			m_prevParam.addrcode = const_cast<char*>(addrcode.c_str());
		}
		else
		{
			m_prevParam.addrcode = NULL;
		}
		
		m_prevParam.transport = pParam->u32Proto; //TCP/UDP
		
		m_pfnRealDataCallback = pParam->fRealDataCallback;
		m_previewUser = pParam->pUser;

		
		m_hPreview = c2_start_preview(m_pConn->m_hLogin, &m_prevParam, pParam->hWnd, C2RealDataCallback, this, C2PreviewStatusCallback, this);  //aaaaaaaaaa
	
		memcpy(&(m_PViewClient.stParam), pParam, sizeof(NET_PVIEW_PARAM_S));
		m_PViewClient.pPView  = this;
		m_PViewClient.bNeedKeyFrame = TRUE;
		return &m_PViewClient; 
	}
	return NULL;
}

//************************************************************************
//函数名称：Stop( )
//函数说明：停止预览
//函数参数：无
//返回值：成功返回预览窗口句柄/失败返回NULL
//************************************************************************/
BOOL C2PlatformPreview::Stop(PreviewClient* pPViewClient)
{
	if((NULL != pPViewClient) && (NULL != m_pConn->m_hLogin))
	{	
		C2PlatformPreview *pView = (C2PlatformPreview *)pPViewClient->pPView;
		c2_stop_preview(m_pConn->m_hLogin, pView->m_hPreview);
		
		return TRUE;
	}
	
	return FALSE;
}

//************************************************************************
//函数名称：OpenSound()
//函数说明：打开预览声音, 默认关闭
//函数参数：无 
//返回值：成功返回true，失败返回false
//************************************************************************/
BOOL C2PlatformPreview::OpenSound()
{	 
	if(NULL != m_hPreview)
	{
		int iRet = c2_opensound(m_hPreview);
		
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//函数名称：CloseSound()
//函数说明：关闭预览声音
//函数参数：无 
//返回值：成功返回true，失败返回false
//************************************************************************/

BOOL C2PlatformPreview::CloseSound()
{
	if(NULL != m_hPreview)
	{
		int iRet = c2_colsesound(m_hPreview);
		
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	return FALSE; 
}

VOID C2PlatformPreview::PreviewLoop()
{

}

BOOL C2PlatformPreview::OpenStream()
{
	return FALSE;
}

VOID C2PlatformPreview::RecvStream()
{

}

//************************************************************************
//函数名称：GetResponse
//函数说明： 
//函数参数：
//    (in) pRsp  
//返回值：成功返回true，失败返回false
//************************************************************************/  
BOOL C2PlatformPreview::GetResponse(PVIEW_RSP_S *pRsp)
{
	return FALSE;
}

//************************************************************************
//函数名称：CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode)
//函数说明：实时预览抓图
//函数参数：
//    (in) ps8PicFileName 抓拍图像保存路径
//    (in) u32CaptueMode 抓拍类型0-BMP 1-JPEG
//返回值：成功返回true，失败返回false
//************************************************************************/  
BOOL C2PlatformPreview::CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode)
{
	if((NULL != ps8PicFileName) || (NULL != m_hPreview))
	{
		long lRet = c2_capture(m_hPreview, ps8PicFileName, u32CaptueMode);		
		if(lRet == 0)
		{
			return TRUE;
		}
	}	
	return FALSE;
}

//************************************************************************
//函数名称：SaveRealData(S8 *ps8FileName)
//函数说明：开始实时录像
//函数参数：
//    (in) view     c2_start_preview返回值
//    (in) filepath 录像保存路径
//返回值：成功返回true，失败返回false
//************************************************************************/  
BOOL C2PlatformPreview::SaveRealData(S8 *ps8FileName)
{
	if((NULL != ps8FileName) && (NULL != m_hPreview))
	{
		long lRet = c2_start_realREC(m_hPreview, ps8FileName);
		if(lRet == 0)
		{
			return TRUE;
		}		
	}
	return FALSE;
}

//************************************************************************
//函数名称：StopSaveRealData()
//函数说明：停止实时录像
//函数参数：无
//返回值：成功返回true，失败返回false
//************************************************************************/  
BOOL C2PlatformPreview::StopSaveRealData()
{
	if(NULL != m_hPreview)
	{
		c2_stop_realREC(m_hPreview);
		return TRUE;
	}
	return FALSE;
}

//************************************************************************
//函数名称：IsSameStream(const NET_PVIEW_PARAM_S *pParam)
//函数说明：判断是否存在相同的流
//函数参数：
//    (in) pParam  预览信息结构体
//返回值：成功返回true，失败返回false
//************************************************************************/ 
BOOL C2PlatformPreview::IsSameStream(const NET_PVIEW_PARAM_S *pParam)
{
	if (pParam->ps8FrontAddrCode)
	{
		return (0 == memcmp(m_prevParam.addrcode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN));
	}
	
	U8 u8DeviceIp[MAX_IPV4_LEN] = {0};
	U8 u8SubServIp[MAX_IPV4_LEN]= {0};
	INET_PTON(pParam->ps8DeviceIp,  u8DeviceIp);
	INET_PTON(pParam->ps8SubServIp, u8SubServIp);
	
	return ((m_prevParam.channelNo == pParam->u32ChnNO) && 
		(m_prevParam.streamType == pParam->u8Type) && 
		(m_prevParam.obtainType == pParam->u8Content) &&
		(m_prevParam.transport == pParam->u32Proto) &&
		(m_prevParam.devicePort  == pParam->u16DevicePort) &&
		(m_prevParam.subServPort == pParam->u16SubServPort) &&
		(0 == memcmp(m_prevParam.deviceHost, u8DeviceIp, MAX_IPV4_LEN)) &&
		(0 == memcmp(m_prevParam.subServHost, u8SubServIp, MAX_IPV4_LEN)));
}

//************************************************************************
//函数名称：SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser)
//函数说明：设置译码回调
//函数参数：
//    (in) hDecCb  译码回调
//    (in) hUser  用户数据
//返回值：void
//************************************************************************/ 
VOID C2PlatformPreview::SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser)
{
	if(NULL != m_hPreview )
	{
		m_pUserDec = hUser;
		m_pfnDecDataCallback = (pfnDecDataCallback)hDecCb;

		long lRet = c2_set_decodecb(m_hPreview, C2DecodeCallback, this);
		
		if(0 == lRet)
		{
		 
		}
	}
 
}

long C2PlatformPreview::SetDrawCallBack(HANDLE hPreview, const NET_REGDRAW_PARAM *pParam)
{
	m_pfnDrawCallback = pParam->pfnDraw;
	m_pUserDraw = pParam->hUser;
	long lRet =	c2_set_drawcb(hPreview, C2DrawCallback, this);
	return lRet;
}
