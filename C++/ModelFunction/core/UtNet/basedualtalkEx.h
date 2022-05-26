#ifndef  __BASEDUALTALKEX_H__
#define  __BASEDUALTALKEX_H__

#include "ut_net.h"

class BaseDualTalkEx
{
public:
    BaseDualTalkEx(){}
    virtual ~BaseDualTalkEx(){}
	
		/* �����Խ��ŵ� */
	virtual	S32    Start() = 0;

		/* �رնԽ����� */
	virtual	VOID   Stop() = 0;

		/* ���ͶԽ����� */
	virtual	BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type){return TRUE;};

		/* ���ͶԽ����� */
	virtual	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize){return TRUE;};

        /* ��ʼ����װ�ļ�����*/
    virtual BOOL InitMuxAVF(_NET_DEV_MUXAVF_PARAM_ *pParam){return TRUE;}

        /* ȥ��ʼ����װ�ļ�*/
    virtual VOID UninitMuxAVF(){;}
	
	
protected: 
	
private:

};

#endif 
