#ifndef  __BASEDUALTALKEX_H__
#define  __BASEDUALTALKEX_H__

#include "ut_net.h"

class BaseDualTalkEx
{
public:
    BaseDualTalkEx(){}
    virtual ~BaseDualTalkEx(){}
	
		/* 启动对讲信道 */
	virtual	S32    Start() = 0;

		/* 关闭对讲链接 */
	virtual	VOID   Stop() = 0;

		/* 发送对讲申请 */
	virtual	BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type){return TRUE;};

		/* 发送对讲数据 */
	virtual	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize){return TRUE;};

        /* 初始化封装文件参数*/
    virtual BOOL InitMuxAVF(_NET_DEV_MUXAVF_PARAM_ *pParam){return TRUE;}

        /* 去初始化封装文件*/
    virtual VOID UninitMuxAVF(){;}
	
	
protected: 
	
private:

};

#endif 
