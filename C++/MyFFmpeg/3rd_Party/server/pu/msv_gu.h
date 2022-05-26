/**
 * @file      msv_gu.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-06 16:26:17
 *
 * @note
 *  stream    0: private stream 1:standard stream
 */
#ifndef __MSV_GU_H
#define __MSV_GU_H
#include "gu.h"
#include "c2sdk.h"
class Pu;
class MsvRealGu : public RealGu
{
public:
    MsvRealGu(Pu *pu, string addrcode, int streamType, int stream, int streamPotoType);
    MsvRealGu(Pu * pu, int channelNo, string Host, int Port, int streamType, int streamProtoType);





    virtual ~MsvRealGu();

	virtual int        start(int offset);
    virtual void       stop();

protected:
    virtual void       onTimer();

protected:
    static bool        stream_cb(HWND wnd, void *data, uint32_t size, void *userData);
    static void        status_cb(int code, void* userData);

private:
    HANDLE             m_view;
};
#endif
