#ifndef  __BASEPVIEW_H__
#define  __BASEPVIEW_H__

#include "ut_net.h"

class BasePView;
class Connection;
struct PreviewClient
{
	NET_PVIEW_PARAM_S stParam;
	BOOL              bNeedKeyFrame;
	BasePView         * pPView;
};

class BasePView
{
public:
    BasePView(){}
    virtual ~BasePView(){}

		/* 开始预览 */
	virtual PreviewClient *Start(const NET_PVIEW_PARAM_S *pParam) = 0;

		/* 停止预览 */
	virtual BOOL           Stop(PreviewClient* pPViewClient) = 0;

		/* 解码回调函数 */
    virtual VOID           SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser) =0;

		/* 实时抓拍 */
	virtual BOOL           CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode) = 0;

		/* 打开声音 */
	virtual BOOL  			OpenSound() = 0;
		/* 关闭声音 */
	virtual BOOL 			CloseSound() =0;

	//virtual static VOID *  PreviewFun(VOID *pArg) = 0;

		/* 实时数据接收线程 */
	virtual VOID 			PreviewLoop() =0;

		/* 实时数据请求操作 */
	virtual BOOL 			OpenStream() = 0;

		/* 实时数据接收操作 */
	virtual	VOID 			RecvStream() =0;
	
		/* 获取预览响应结构体 */
	virtual	BOOL           GetResponse(PVIEW_RSP_S *pRsp){return TRUE;}
	
		/* 开始实时录像 */
	virtual	BOOL           SaveRealData(S8 *ps8FileName){return TRUE;}
	
		/* 停下实时录像 */
	virtual	BOOL           StopSaveRealData(){return TRUE;}



protected: 
	
private:

};

#endif 
