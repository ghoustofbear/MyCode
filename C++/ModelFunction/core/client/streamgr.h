/**
 * @file      streammgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-07 17:04:13
 *
 * @note
 *  streammgr.h defines
 */
#ifndef __STREAMGR_H__
#define __STREAMGR_H__
#include "realstream.h"

class TalkStream
{
public:
    TalkStream();
    ~TalkStream();
    int                    start();
    void                   stop();
    bool                   isCapture();

    int                    addTalkStream(RealStream *listener);
    void                   delTalkStream(RealStream *listener);

    void                   notifyAudioStream(void* data, int size, uint64_t pts);

private:
    void                   buildFrame(uint32_t tag, void* data,  int size, uint64_t pts);
    static void            cap_audio(int type, void *data, int size, uint64_t pts, void *userdata);

private:
    static  uint64_t       m_seq;
    char                 * m_aframe;
    HANDLE                 m_capAudio;
    tlist<RealStream *>    m_audios;
};

class StreamMgr : public tlist<RealStream *>
{
    enum
    {
        T_COLLECTOR,
        T_TIMER,
        T_RELEASE_REALSESSION
    };

public:
    StreamMgr();
    virtual ~StreamMgr();

    int                start();
    void               stop();

    int                addRealStream(Cu *cu, string json, StreamListener *listener);
    void               delRealStream(Cu *cu, string json, StreamListener *listener);
#ifdef UNIX_DRAW_RECT
	void               drawRoi(string devIp, int devPort, int chnNo, string alarmStr, int dwtype);
    void               clearRoi(string msg, string devIp, int devPort, int chnNo);
#endif
    void               release(Cu *cu);

protected:
    int                collector();
    void               timerdriver();

    static void      * threadEntry(ThreadInfo *arg);

private:
    Thread             m_collecThread;
    Thread             m_timerThread;
};

extern StreamMgr *streamMgr;
#endif
