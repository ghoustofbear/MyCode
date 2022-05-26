/**
 * @file      hik_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 15:14:00
 *
 * @note
 *  hik_gu.h defines
 */
#ifndef __HIK_EHOME_GU_H__
#define __HIK_EHOME_GU_H__
#include "gu.h"
#include "thread.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "3rd/hikehome/HCISUPStream.h"
#define CBK            CALLBACK 

class Pu;
class HikEhomeRealGu : public RealGu
{
	friend void H264streamdatacb(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime);
	friend void CALLBACK PreviewDataCallback(LONG iPreviewHandle, NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg, void *pUserData);//PS流
public:
    HikEhomeRealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    HikEhomeRealGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~HikEhomeRealGu();
	virtual int        start(int offset);
    virtual void       stop();
protected:
    virtual void       onTimer();
public:
	LONG               m_lRealHandle;  //预览回调函数返回的实时预览句柄
	LONG               m_lSessionID;   //设备分配给该取流会话的ID
	char              *m_deviceID;
private:
	int  liveStart();
	int  liveStop();
	//void               *psparser;  //PS解封装对象
};
#endif
