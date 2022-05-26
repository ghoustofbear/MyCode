/**
 * @file      hs_rtspsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-28 10:41:20
 *
 * @note
 *  gw_rtspsession.h defines
 */
#ifndef __HS_RTSPSESSION_H__
#define __HS_RTSPSESSION_H__
#include "rtspsession.h"

class GWRTSPSession : public RTSPSession
{
public:
    GWRTSPSession();
    virtual ~GWRTSPSession();

protected:
    virtual void       onOption  (stk::RtspMsg &rtsp);
    virtual void       onDescribe(stk::RtspMsg &rtsp);
    virtual void       onSetup   (stk::RtspMsg &rtsp);
    virtual void       onPlay    (stk::RtspMsg &rtsp);
    virtual void       onPause   (stk::RtspMsg &rtsp);
    virtual void       onTeardown(stk::RtspMsg &rtsp);
    virtual void       onGetParameter(stk::RtspMsg &rtsp);

	virtual void       onWrite(Bufferev *bev);
	virtual void       onClose(Bufferev *bev);
	

private:
    /* data */
};

#endif
