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

		/* ��ʼԤ�� */
	virtual PreviewClient *Start(const NET_PVIEW_PARAM_S *pParam) = 0;

		/* ֹͣԤ�� */
	virtual BOOL           Stop(PreviewClient* pPViewClient) = 0;

		/* ����ص����� */
    virtual VOID           SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser) =0;

		/* ʵʱץ�� */
	virtual BOOL           CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode) = 0;

		/* ������ */
	virtual BOOL  			OpenSound() = 0;
		/* �ر����� */
	virtual BOOL 			CloseSound() =0;

	//virtual static VOID *  PreviewFun(VOID *pArg) = 0;

		/* ʵʱ���ݽ����߳� */
	virtual VOID 			PreviewLoop() =0;

		/* ʵʱ����������� */
	virtual BOOL 			OpenStream() = 0;

		/* ʵʱ���ݽ��ղ��� */
	virtual	VOID 			RecvStream() =0;
	
		/* ��ȡԤ����Ӧ�ṹ�� */
	virtual	BOOL           GetResponse(PVIEW_RSP_S *pRsp){return TRUE;}
	
		/* ��ʼʵʱ¼�� */
	virtual	BOOL           SaveRealData(S8 *ps8FileName){return TRUE;}
	
		/* ͣ��ʵʱ¼�� */
	virtual	BOOL           StopSaveRealData(){return TRUE;}



protected: 
	
private:

};

#endif 
