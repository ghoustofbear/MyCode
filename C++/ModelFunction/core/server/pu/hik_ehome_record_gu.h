/**
 * @file      hik_ehome_record_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     playback
 * @author    quxiaohui
 * @date      2017-08-02 15:08:13
 *
 * @note
 *  hik_record_gu.h defines
 */
#ifndef _HIK_EHOME_RECORD_GU_H__
#define _HIK_EHOME_RECORD_GU_H__
#include "gu.h"
#include "3rd/hik/sdk-plat.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "3rd/hikehome/HCISUPStream.h"
#define CBK            CALLBACK 
class HikEhomeRecordGu : public RecordGu
{
	friend BOOL CALLBACK PlaybackNewlinkCallback(LONG lPlayBackLinkHandle, NET_EHOME_PLAYBACK_NEWLINK_CB_INFO  *pNewLinkCBMsg, void *pUserData);
public:
	HikEhomeRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode);
	HikEhomeRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode);

	HikEhomeRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode);
	HikEhomeRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);

    virtual ~HikEhomeRecordGu();

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

	virtual int        startPlayBack();
	virtual void       stopPlayBack();
protected:
    static void CBK    streamcb(LONG realID, DWORD type, uint8_t *data, DWORD size, void *userData);
    static void CBK    rawdatacb(LONG recordID, NET_DVR_PACKET_INFO_EX *packet, void *userData);

private:
    long               m_recordID;
    bool               m_bStop;
public:
	LONG               m_lPlaybackLinkHandle;  //回放连接句柄
	LONG               m_lPlaybackListenHandle;//回放监听句柄，由NET_ESTREAM_StartListenPlayBack返回
	LONG               m_lSessionID;           //设备返回的回放请求会话ID,0:无效 
};
#endif
