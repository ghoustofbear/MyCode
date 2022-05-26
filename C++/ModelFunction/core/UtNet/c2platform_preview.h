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
	//码流数据回调函数
	friend bool C2RealDataCallback(HWND wnd, void *data, uint32_t size, void *userdata);
	
	//状态回调
	friend void C2PreviewStatusCallback(int code, void* userdata);
	
	//绘制回调
	friend void C2DrawCallback(HDC hdc, HWND wnd, int w, int h, void *userdata);
	
	//解码后数据回调
	friend void C2DecodeCallback(int type, int w, int h, void *yuv, int size, void *userdata);
	
	C2PlatformPreview(C2PlatformConn* pConn);
	~C2PlatformPreview();

	//启动预览
	PreviewClient * Start(const NET_PVIEW_PARAM_S *pParam);

	//停止预览
	BOOL Stop(PreviewClient* pPViewClient); 
 
	//解码回调函数
	VOID SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser);

	//实时预览抓图
	BOOL CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode);
	
	//打开预览声音
	BOOL OpenSound();

	//关闭预览声音
	BOOL CloseSound();

	// 实时数据接收线程
	VOID PreviewLoop();
	
	//实时数据请求操作
	BOOL OpenStream();
	
	//实时数据接收操作
	VOID RecvStream();
	
	// 获取预览响应结构体
	BOOL GetResponse(PVIEW_RSP_S *pRsp);

	//开始实时录像
	BOOL SaveRealData(S8 *ps8FileName);

	//停止实时录像
	BOOL StopSaveRealData();
	
	//判断是否存在相同的流
	BOOL IsSameStream(const NET_PVIEW_PARAM_S* pParam);

	long SetDrawCallBack(HANDLE hPreview, const NET_REGDRAW_PARAM *pParam);
	
	HANDLE m_hPreview;	//预览句柄	
	void * m_pUserReadData;		//用户数据
	c2_preview_s m_prevParam;	//预览参数 
	PreviewClient m_PViewClient;
private:
	C2PlatformConn* m_pConn;	//连接对象
	void * m_pUserDec;	//解码用户数据
	void * m_pUserDraw; //绘制数据		 
	pfnRealDataCallback m_pfnRealDataCallback;	//码流数据回调函数
	pfnDecDataCallback m_pfnDecDataCallback;	//解码回调函数
	pfnDrawFun m_pfnDrawCallback; //绘图回调

	void * m_previewUser;


};

#endif // !defined(AFX_C2PLATFORM_PREVIEW_H__B51DE96B_00E2_4E6D_93D9_9143088096D8__INCLUDED_)
