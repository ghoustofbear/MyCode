#ifndef  __BASEDUALTALK_H__
#define  __BASEDUALTALK_H__

#include "ut_net.h"

class BaseDualTalk
{
public:
    BaseDualTalk(){}
    virtual ~BaseDualTalk(){}
	
		/* 启动对讲信道 */
	virtual	S32    Start() = 0;

		/* 关闭对讲连接 */
	virtual	VOID   Stop() = 0;
	
		/* 发送对讲信息 */
	virtual	BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList){return TRUE;};

		/* 发送对讲数据 */
	virtual	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize){return TRUE;};

		/* 开始捕获 */
	virtual	BOOL StartCapture(){return TRUE;};
protected: 
	
private:

};

#endif 
