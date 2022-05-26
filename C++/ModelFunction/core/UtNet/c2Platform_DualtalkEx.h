// c2Platform_DualtalkEx.h: interface for the c2Platform_DualtalkEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DUALTALKEX_H__80068F11_41C2_4665_AFF7_A5480062EB6E__INCLUDED_)
#define AFX_C2PLATFORM_DUALTALKEX_H__80068F11_41C2_4665_AFF7_A5480062EB6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedualtalkEx.h"

class C2PlatformConn;
class C2PlatformDualtalkEx : public BaseDualTalkEx
{
public:
	C2PlatformDualtalkEx(C2PlatformConn *pConn, int audioType);
	virtual ~C2PlatformDualtalkEx();

	// 启动对讲信道 
	S32 Start();
	
	// 关闭对讲链接 */
	VOID Stop();
	
	// 发送对讲申请 */
	BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type);
	
	// 发送对讲数据 */
	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);
	
	friend void C2DevTalkStatus_cb(int code, void* userdata);
private:
	C2PlatformConn* m_pConn;	//连接对象	
	HANDLE m_devTalkExHandle;
	int                 m_iAudioType;                /**< 语音编码格式 */
};

#endif // !defined(AFX_C2PLATFORM_DUALTALKEX_H__80068F11_41C2_4665_AFF7_A5480062EB6E__INCLUDED_)
