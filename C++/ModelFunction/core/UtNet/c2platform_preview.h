// c2platform_preview.h: interface for the c2platform_preview class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_PREVIEW_H__B51DE96B_00E2_4E6D_93D9_9143088096D8__INCLUDED_)
#define AFX_C2PLATFORM_PREVIEW_H__B51DE96B_00E2_4E6D_93D9_9143088096D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h"
#include "c2sdkc1.h"
#include "basepview.h"

class C2PlatformConn;
class C2PlatformPreview : public BasePView 
{
public:
	//�������ݻص�����
	friend bool C2RealDataCallback(HWND wnd, void *data, uint32_t size, void *userdata);
	
	//״̬�ص�
	friend void C2PreviewStatusCallback(int code, void* userdata);
	
	//���ƻص�
	friend void C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata);
	
	//��������ݻص�
	friend void C2DecodeCallback(int type, int w, int h, void *yuv, int size, void *userdata);
	
	C2PlatformPreview(C2PlatformConn* pConn);
	~C2PlatformPreview();

	//����Ԥ��
	PreviewClient * Start(const NET_PVIEW_PARAM_S *pParam);

	//ֹͣԤ��
	BOOL Stop(PreviewClient* pPViewClient); 
 
	//����ص�����
	VOID SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser);

	//ʵʱԤ��ץͼ
	BOOL CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode);
	
	//��Ԥ������
	BOOL OpenSound();

	//�ر�Ԥ������
	BOOL CloseSound();

	// ʵʱ���ݽ����߳�
	VOID PreviewLoop();
	
	//ʵʱ�����������
	BOOL OpenStream();
	
	//ʵʱ���ݽ��ղ���
	VOID RecvStream();
	
	// ��ȡԤ����Ӧ�ṹ��
	BOOL GetResponse(PVIEW_RSP_S *pRsp);

	//��ʼʵʱ¼��
	BOOL SaveRealData(S8 *ps8FileName);

	//ֹͣʵʱ¼��
	BOOL StopSaveRealData();
	
	//�ж��Ƿ������ͬ����
	BOOL IsSameStream(const NET_PVIEW_PARAM_S* pParam);

	long SetDrawCallBack(HANDLE hPreview, const NET_REGDRAW_PARAM *pParam);
	
	HANDLE m_hPreview;	//Ԥ�����	
	void * m_pUserReadData;		//�û�����
	c2_preview_s m_prevParam;	//Ԥ������ 
	PreviewClient m_PViewClient;
private:
	C2PlatformConn* m_pConn;	//���Ӷ���
	void * m_pUserDec;	//�����û�����
	void * m_pUserDraw; //��������		 
	pfnRealDataCallback m_pfnRealDataCallback;	//�������ݻص�����
	pfnDecDataCallback m_pfnDecDataCallback;	//����ص�����
	pfnDrawFun m_pfnDrawCallback; //��ͼ�ص�

	void * m_previewUser;


};

#endif // !defined(AFX_C2PLATFORM_PREVIEW_H__B51DE96B_00E2_4E6D_93D9_9143088096D8__INCLUDED_)
