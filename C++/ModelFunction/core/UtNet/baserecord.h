#ifndef  __BASERECORD_H__
#define  __BASERECORD_H__

#include "ut_net.h"

class BaseRecord
{
public:
    BaseRecord(){}
    virtual ~BaseRecord(){}

	/* ��ʼ�ط� */
	virtual BOOL Start(const NET_RECPLAY_PARAM_S *pParam) = 0;

	/* ֹͣ�ط� */
	virtual VOID Stop() = 0;

	/* ���ſ��� */
	virtual BOOL PlayCtrl(U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL) = 0;

	/* ¼���Ѳ���ʱ�� */
	virtual DWORD GetPlayedTime() = 0;

    /* ����¼��ط�״̬�ص�  */
	virtual BOOL  SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser) = 0;

	/* ������*/
	virtual BOOL  OpenSound() = 0;

	/* �ر�����*/
    virtual BOOL  CloseSound() = 0;

	/* ¼�����ݽ������߳� */
	virtual VOID   RecordPlayLoop() = 0;

	/* ¼���������� */
	virtual BOOL   OpenStream() = 0;

protected: 
	
private:

};

#endif 
