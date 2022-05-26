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
#ifndef _HIK_RECORD_GU_H__
#define _HIK_RECORD_GU_H__
#include "gu.h"
#include "3rd/hik/sdk-plat.h"

#define CBK            CALLBACK 
class HikRecordGu : public RecordGu
{
public:
	HikRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode);
	HikRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode);

	HikRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode);
	HikRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);

    virtual ~HikRecordGu();

    int      play(string scale, string range);

protected:
    virtual int        start(int offset);
    virtual void       stop();

    virtual int        pause();
    virtual int        continuex();

    virtual int        startRepos();
    virtual int        setPos(int pos);
    virtual int        setTime(datetime_s &t);
    virtual int        insertRecordLabel(string name);
    virtual int        playFrame();
    virtual int        playNormal();
    virtual int        playReverse(datetime_s &t);
    virtual int        playFoward(datetime_s &t);

    virtual void       onTimer();

protected:
    static void CBK    streamcb(LONG realID, DWORD type, uint8_t *data, DWORD size, void *userData);
    static void CBK    rawdatacb(LONG recordID, NET_DVR_PACKET_INFO_EX *packet, void *userData);

private:
    long               m_recordID;
    bool               m_bStop;
};
#endif
