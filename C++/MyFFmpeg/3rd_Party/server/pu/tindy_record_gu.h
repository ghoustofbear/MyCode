/**
* @file      hik_record_gu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-08-02 15:08:13
*
* @note
*  hik_record_gu.h defines
*/
#ifndef _TD_RECORD_GU_H__
#define _TD_RECORD_GU_H__
#include "gu.h"
#include "3rd/tindy/sdk-plat.h"

#ifndef CBK
#define CBK            __stdcall
#endif


class TindyRecordGu : public RecordGu
{
public:
	TindyRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode);
	TindyRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode);

	TindyRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode);
	TindyRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);

    virtual ~TindyRecordGu();

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
    static void CBK    streamcb(unsigned int userID , unsigned char* data, int len, int flag, void* userData); 
    static void CBK    rawdatacb(unsigned int userID, unsigned char* data, int size, RAWFRAME_INFO*  frameInfo, void* userData);


private:
    long               m_recordID;
    unsigned long      m_recvID;
};
#endif
