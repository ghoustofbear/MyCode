/**
 * @file      msv_record_gu.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-09 16:20:05
 *   
 * @note
 *  msv_record_gu.h defines 
 */
#ifndef __MSV_RECORD_GU_H__
#define __MSV_RECORD_GU_H__
#include "gu.h"
#include "c2sdk.h"
class MsvRecordGu : public RecordGu
{
public:
	MsvRecordGu(Pu *pu, string filename, string addrcode, int stream, int streamType, int mode);
	MsvRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int stream, int streamType, int mode);

    virtual ~MsvRecordGu();

protected:
	virtual int        start(int offset);
    virtual void       stop();

    virtual int        pause();
    virtual int        continuex();

    virtual int        startRepos();
    virtual int        setPos(int pos);
    virtual int        setTime(datetime_s &t);

    virtual void       onTimer();

protected:
    static bool        stream_cb(HWND wnd, void *data, uint32_t size, void *userData);
    static void        status_cb(int code, void* userData);

private:
    HANDLE             m_record;
    int                m_stream;
};
#endif
