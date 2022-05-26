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

	//远程回放
	BOOL Start(const NET_RECPLAY_PARAM_S *pParam);

	//停止某台设备某个通道的远程回放
	VOID Stop();

	//控制录像回放的状态
	BOOL PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize);

	//获取播放时间
	DWORD GetPlayedTime();

	//设置录像回放状态回调函数
	BOOL SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser);

	//打开声音
	BOOL OpenSound();

	//关闭声音
	BOOL CloseSound();

	//录像数据接收主线程
	VOID RecordPlayLoop();

	//录像数据请求
	BOOL OpenStream();

	//回放状态回调函数
	friend void C2RecordStatusCallback(int code, void* userdata);

	//回放码流回调函数
    friend bool C2RecordStreamCallback(HWND wnd, void *data, uint32_t size, void *userdata);



private:
	pfnRecordDataCallback m_pfnRecordDataCallback; //查询记录回调函数
	pfnRecordPlaybackStatusCallback m_pfnRecordPlaybackStatusCallback;
	C2PlatformConn* m_pConn; //连接对象
	HANDLE m_hRecord; //查询句柄
	void * m_pUserRecord;	//用户数据
	void * m_pUserStatus;

	time_t m_tmBeginSec; /**< 开始时间 */
	time_t m_tmEndSec; /**< 结束时间时间 */
	DWORD m_dwPlayedTime;
};

#endif // !defined(AFX_C2PLATFORM_RECORD_H__A650624C_AA38_4C5C_B7DA_3F8A8B91B670__INCLUDED_)
