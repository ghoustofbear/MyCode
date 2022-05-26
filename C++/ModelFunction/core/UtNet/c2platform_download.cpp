// c2platform_download.cpp: implementation of the c2platform_download class.
//
//////////////////////////////////////////////////////////////////////
 

#include "c2platform_download.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformDownload::C2PlatformDownload(C2PlatformConn *pConn)
:BaseDownload()
{
	m_pConn = pConn;
	m_hDownload = NULL;
	m_pUserData = NULL; 
}

C2PlatformDownload::~C2PlatformDownload()
{

}

//************************************************************************
//�������ƣ�C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//����˵�������ļ��������ݻص�����
//����������
//    (in) wnd      ��ǰ��Ԥ��/�طž��
//    (in) data     ������ݵĻ�����ָ��, ut_frame_s+����Ƶ����
//    (in) size     ��������С
//    (in) userdata �û�����
//����ֵ���ɹ�����true/ʧ�ܷ���false
//************************************************************************/
bool C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
{
	C2PlatformDownload *pDownload = (C2PlatformDownload *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadDataCallback)
	{
		if(TRUE == pDownload->m_pfnRecordDownloadDataCallback(data, size, pDownload->m_pUserData))
		{
			return true;
		}
	}
	return false;
}

//************************************************************************
//�������ƣ�C2StatusCallback(int code, void* userdata)
//����˵�������ļ�����״̬�ص�
//����������
//    (in) code ״̬��
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
void C2StatusCallback(int code, void* userdata)
{
	C2PlatformDownload *pDownload = (C2PlatformDownload *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadStatusCallback)
	{
		int iCode = 0;
		switch(code)
		{
		case CODE_STREAM_END:
			break;	
		}
		pDownload->m_pfnRecordDownloadStatusCallback(iCode, pDownload->m_pUserData);
	}
}



//************************************************************************
//�������ƣ�Start(const NET_RECDOWNLOAD_PARAM_S *pParam)
//����˵�������ļ�����ʼ����¼��
//����������
//    (in) pParam      ¼��������Ϣ�ṹ��
//����ֵ���ɹ�����true/ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformDownload::Start(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
	if((NULL != m_pConn) && (NULL != pParam))
	{
		c2_download_s downloadInfo = {0};
		downloadInfo.savepath = pParam->ps8FileName;
		downloadInfo.filename = pParam->ps8RecFileName;
		downloadInfo.obtainType = 0; //aaaaa����
		downloadInfo.deviceHost = pParam->ps8DeviceIp;
		downloadInfo.devicePort = pParam->u16DevicePort;
		downloadInfo.subServHost = pParam->ps8SubServIp;
		downloadInfo.subServPort = pParam->u16SubServPort;

		downloadInfo.addrcode = pParam->ps8FrontAddrCode;	//aaaaaaǰ�˵�ַ��
		m_pfnRecordDownloadStatusCallback = pParam->fStatusCallback;
		m_pfnRecordDownloadDataCallback = pParam->fRecordDataCallback;

		m_pUserData = pParam->pUser;
	
		if(NULL != m_hDownload)
		{
			m_hDownload = NULL;
		}
		m_hDownload = c2_start_download(m_pConn->m_hLogin, &downloadInfo, C2StreamCallback, this, C2StatusCallback, this);  

		return TRUE;
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�Stop()
//����˵����ֹͣĳ̨�豸ĳ��ͨ����Զ������
//������������
//����ֵ��void
//************************************************************************/
void C2PlatformDownload::Stop()
{
	if((NULL != m_pConn->m_hLogin) && (NULL != m_hDownload))
	{
		c2_stop_download(m_pConn->m_hLogin, m_hDownload);
	}
}

//************************************************************************
//�������ƣ�GetDownloadPos()
//����˵������ȡ��ǰ����λ��
//������������
//����ֵ����ǰ����λ��
//************************************************************************/
U32 C2PlatformDownload::GetDownloadPos()
{
	if(NULL != m_hDownload)
	{
		long lRet = c2_get_downloadpos(m_hDownload);
		return ((U32)lRet);
	}
	return 0;
}

