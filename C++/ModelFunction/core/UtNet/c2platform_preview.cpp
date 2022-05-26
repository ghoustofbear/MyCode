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
//�������ƣ�C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata)
//����˵�����û��б����ݻص�
//����������
//    (in) hdc ����
//    (in) wnd ���ھ��
//    (in) w ���
//    (in) h �߶�
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
void C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata)
{	
	C2PlatformPreview *pView = (C2PlatformPreview *)userdata;
	
	pView->m_pfnDrawCallback(hdc, w, h, pView->m_pUserDraw);
}

//************************************************************************
//�������ƣ�C2StatusCallback(int code, void *userdata)
//����˵����״̬�ص�
//����������
//    (in) code     ״̬��
//    (in) userdata �û����� 
//����ֵ��void
//************************************************************************/
void C2PreviewStatusCallback(int code, void *userdata)
{
	
}

//************************************************************************
//�������ƣ�C2RealDataCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//����˵�����������ݻص�����
//����������
//    (in) wnd      ��ǰ��Ԥ��/�طž��
//    (in) data     ������ݵĻ�����ָ��, ut_frame_s+����Ƶ����
//    (in) size     ��������С
//    (in) userdata �û�����
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�Start(const NET_PVIEW_PARAM_S *pParam)
//����˵��������Ԥ��
//����������
//    (in) pParam   Ԥ������
//����ֵ���ɹ�����Ԥ�����ھ��/ʧ�ܷ���NULL
//************************************************************************/
PreviewClient * C2PlatformPreview::Start(const NET_PVIEW_PARAM_S *pParam)
{
	if((NULL != pParam) && (NULL != m_pConn->m_hLogin))
	{		 
		string ip="", subIp = "", addrcode=""; 
		m_prevParam.channelNo = pParam->u32ChnNO;//ͨ����
		m_prevParam.obtainType = 0;//pParam->u8Content;	//����-����  aaaaaaaaaa
		m_prevParam.streamType = pParam->u8Type;	//��������
		
		if(NULL != pParam->ps8DeviceIp)//�豸IP
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
//�������ƣ�Stop( )
//����˵����ֹͣԤ��
//������������
//����ֵ���ɹ�����Ԥ�����ھ��/ʧ�ܷ���NULL
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
//�������ƣ�OpenSound()
//����˵������Ԥ������, Ĭ�Ϲر�
//������������ 
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�CloseSound()
//����˵�����ر�Ԥ������
//������������ 
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�GetResponse
//����˵���� 
//����������
//    (in) pRsp  
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformPreview::GetResponse(PVIEW_RSP_S *pRsp)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode)
//����˵����ʵʱԤ��ץͼ
//����������
//    (in) ps8PicFileName ץ��ͼ�񱣴�·��
//    (in) u32CaptueMode ץ������0-BMP 1-JPEG
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�SaveRealData(S8 *ps8FileName)
//����˵������ʼʵʱ¼��
//����������
//    (in) view     c2_start_preview����ֵ
//    (in) filepath ¼�񱣴�·��
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�StopSaveRealData()
//����˵����ֹͣʵʱ¼��
//������������
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�IsSameStream(const NET_PVIEW_PARAM_S *pParam)
//����˵�����ж��Ƿ������ͬ����
//����������
//    (in) pParam  Ԥ����Ϣ�ṹ��
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser)
//����˵������������ص�
//����������
//    (in) hDecCb  ����ص�
//    (in) hUser  �û�����
//����ֵ��void
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
