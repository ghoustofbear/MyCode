// c2Platform_DownloadByTime.h: interface for the c2Platform_DownloadByTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_)
#define AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedownloadbytime.h"

class C2PlatformConn;
class C2PlatformDownloadByTime : public BaseDownloadByTime 
{
public:

	C2PlatformDownloadByTime(C2PlatformConn *pConn);
	virtual ~C2PlatformDownloadByTime();
	
	//��ʱ�俪ʼ����¼��
	BOOL Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);
	
	//ֹͣ��ʱ������
	void Stop();
	
	//��ȡ��ǰ����λ��
	U32 GetDownloadPos();
	
	//��ʱ������״̬�ص�
	friend void C2StatusByTimeCallback(int code, void* userdata);
	
	//��ʱ�����������ص�
	friend bool C2StreamByTimeCallback(HWND wnd, void *data, uint32_t size, void *userdata);

private:
	C2PlatformConn *m_pConn; //���Ӷ���	
	
	pfnRecordDownloadStatusCallback m_pfnRecordDownloadByTimeStatusCallback;  //����ʱ������״̬�ص�����
	pfnRecordDownloadDataCallback m_pfnRecordDownloadDataByTimeCallback;	//����ʱ���������ݻص�����
	
	HANDLE m_hDownload; //���ؾ��
	
	void * m_pUserData;	//��ʱ�������û�����
};

#endif // !defined(AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_)
