#ifndef  __BASEDUALTALK_H__
#define  __BASEDUALTALK_H__

#include "ut_net.h"

class BaseDualTalk
{
public:
    BaseDualTalk(){}
    virtual ~BaseDualTalk(){}
	
		/* �����Խ��ŵ� */
	virtual	S32    Start() = 0;

		/* �رնԽ����� */
	virtual	VOID   Stop() = 0;
	
		/* ���ͶԽ���Ϣ */
	virtual	BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList){return TRUE;};

		/* ���ͶԽ����� */
	virtual	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize){return TRUE;};

		/* ��ʼ���� */
	virtual	BOOL StartCapture(){return TRUE;};
protected: 
	
private:

};

#endif 
