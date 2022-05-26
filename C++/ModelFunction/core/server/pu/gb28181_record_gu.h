#ifndef __GB28181_RECORD_GU_H__
#define __GB28181_RECORD_GU_H__

#include "gu.h"
#include "rtspclient.h"
#include "thread.h"
#include "tlist.h"

class GB28181RecordGu : public RecordGu
{
    enum
    {
        PLAYBACK_PAUSE,
        PLAYBACK_CONTINUE,
        PLAYBACK_FAST,
        PLAYBACK_SLOW,
        PLAYBACK_SETTIME
    };
public:
    GB28181RecordGu(Pu *pu, string fileurl,string addrcode, int st);
    GB28181RecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);
    virtual ~GB28181RecordGu();

	enum GB28181_RECORD_THREAD
	{
		GB28181_RECORD_HEART,
		GB28181_RECORD_UPLOAD
    };
    bool               start_by_time();
    void               stop_by_time();

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

    void               streamCtrl(int cmd, int param);

protected:
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

    static  void     * threadEntry(ThreadInfo *arg);

private:
    /* data */
    Thread           * m_keepLive;
    string             m_strScale;
    string             m_strRange;

	Thread           * m_uploadStream;

	typedef struct stRecordGb28181
	{
		uint8_t * data;
		uint32_t size;
		uint8_t type;
	}stRecordGb28181;

    tlist<stRecordGb28181*>   m_gb28181RecordList;

    string             m_strCallId;
    int                m_iConId;
    int                m_iDialId;
    string             m_strSessId;
    int                m_streamState; // 0-uninit 1-playing 2-pause 3-fast 4-slow
    int            m_cseq;
    int             m_speed;
};
#endif
