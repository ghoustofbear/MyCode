// c2platform_record.h: interface for the c2platform_record class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_RECORD_H__A650624C_AA38_4C5C_B7DA_3F8A8B91B670__INCLUDED_)
#define AFX_C2PLATFORM_RECORD_H__A650624C_AA38_4C5C_B7DA_3F8A8B91B670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "baserecord.h"

class C2PlatformConn;
class C2PlatformRecord : public BaseRecord
{
public:

	C2PlatformRecord(C2PlatformConn *pConn);
	virtual ~C2PlatformRecord();

	//Զ�̻ط�
	BOOL Start(const NET_RECPLAY_PARAM_S *pParam);

	//ֹͣĳ̨�豸ĳ��ͨ����Զ�̻ط�
	VOID Stop();

	//����¼��طŵ�״̬
	BOOL PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize);

	//��ȡ����ʱ��
	DWORD GetPlayedTime();

	//����¼��ط�״̬�ص�����
	BOOL SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser);

	//������
	BOOL OpenSound();

	//�ر�����
	BOOL CloseSound();

	//¼�����ݽ������߳�
	VOID RecordPlayLoop();

	//¼����������
	BOOL OpenStream();

	//�ط�״̬�ص�����
	friend void C2RecordStatusCallback(int code, void* userdata);

	//�ط������ص�����
    friend bool C2RecordStreamCallback(HWND wnd, void *data, uint32_t size, void *userdata);



private:
	pfnRecordDataCallback m_pfnRecordDataCallback; //��ѯ��¼�ص�����
	pfnRecordPlaybackStatusCallback m_pfnRecordPlaybackStatusCallback;
	C2PlatformConn* m_pConn; //���Ӷ���
	HANDLE m_hRecord; //��ѯ���
	void * m_pUserRecord;	//�û�����
	void * m_pUserStatus;

	time_t m_tmBeginSec; /**< ��ʼʱ�� */
	time_t m_tmEndSec; /**< ����ʱ��ʱ�� */
	DWORD m_dwPlayedTime;
};

#endif // !defined(AFX_C2PLATFORM_RECORD_H__A650624C_AA38_4C5C_B7DA_3F8A8B91B670__INCLUDED_)
