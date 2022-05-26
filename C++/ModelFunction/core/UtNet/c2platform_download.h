// c2platform_download.h: interface for the c2platform_download class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_)
#define AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedownload.h"

class C2PlatformConn;
class C2PlatformDownload : public BaseDownload
{
public:

	C2PlatformDownload(C2PlatformConn *pConn);
	virtual ~C2PlatformDownload();

	//���ļ�����ʼ����¼��
	BOOL Start(const NET_RECDOWNLOAD_PARAM_S *pParam);
	
	//ֹͣԶ������
	void Stop();
	
	//��ȡ��ǰ����λ��
	U32 GetDownloadPos();

	//���ļ�������״̬�ص�
	friend void C2StatusCallback(int code, void* userdata);
	
	//���ļ������������ص�
    friend bool C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata);

private:
	C2PlatformConn *m_pConn; //���Ӷ���	

	pfnRecordDownloadStatusCallback m_pfnRecordDownloadStatusCallback; //����״̬�ص�����
	pfnRecordDownloadDataCallback m_pfnRecordDownloadDataCallback; //��������״̬�ص����� 

	HANDLE m_hDownload; //���ؾ��
 
	void * m_pUserData;		//���ļ������û�����
};

#endif // !defined(AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_)
