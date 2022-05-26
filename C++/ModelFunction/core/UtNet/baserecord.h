#ifndef  __BASERECORD_H__
#define  __BASERECORD_H__

#include "ut_net.h"

class BaseRecord
{
public:
    BaseRecord(){}
    virtual ~BaseRecord(){}

	/* 开始回放 */
	virtual BOOL Start(const NET_RECPLAY_PARAM_S *pParam) = 0;

	/* 停止回放 */
	virtual VOID Stop() = 0;

	/* 播放控制 */
	virtual BOOL PlayCtrl(U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL) = 0;

	/* 录像已播放时间 */
	virtual DWORD GetPlayedTime() = 0;

    /* 设置录像回放状态回调  */
	virtual BOOL  SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser) = 0;

	/* 打开声音*/
	virtual BOOL  OpenSound() = 0;

	/* 关闭声音*/
    virtual BOOL  CloseSound() = 0;

	/* 录像数据接收主线程 */
	virtual VOID   RecordPlayLoop() = 0;

	/* 录像数据请求 */
	virtual BOOL   OpenStream() = 0;

protected: 
	
private:

};

#endif 
