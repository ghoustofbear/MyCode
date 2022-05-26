/**
 * @file      gw_record_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-29 13:51:04
 *
 * @note
 *  gw_record_gu.h defines
 */
#ifndef __GW_RECORD_GU_H__
#define __GW_RECORD_GU_H__

#include "gu.h"
#include "rtspclient.h"
#include "thread.h"
#include "tlist.h"

class GwRecordGu : public RecordGu
{
public:
    GwRecordGu(Pu *pu, string fileurl, int st);
    virtual ~GwRecordGu();

	enum GW_RECORD_THREAD
	{
		GW_RECORD_HEART,
		GW_RECORD_UPLOAD
	};

protected:
	virtual int        start(int offset);
    virtual void       stop();

    virtual int        play(string scale, string range);
    virtual int        pause();
    virtual int        continuex();

    virtual int        startRepos();
    virtual int        setPos(int pos);
    virtual int        setTime(datetime_s &t);

    void               sendHeartbeat();
	void               pushRecord(const uint8_t * data, uint32_t size, uint8_t type);
	void               uploadRecordStream();

protected:
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

    static  void     * threadEntry(ThreadInfo *arg);

private:
    /* data */
    std::string m_sessid;
    RtspClient       * m_rtspClient;
    Thread           * m_keepLive;
    string             m_strScale;
    string             m_strRange;

	Thread           * m_uploadStream;

	typedef struct stRecordGw
	{
		uint8_t * data;
		uint32_t size;
		uint8_t type;
	}stRecordGw;

	tlist<stRecordGw*>   m_gwRecordList;
};
#endif
