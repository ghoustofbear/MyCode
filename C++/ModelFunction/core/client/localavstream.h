/**
 * @file      dualtalkstream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 10:12:51
 *   
 * @note
 *  dualtalkstream.h defines 
 */
#ifndef __LOCAL_AV_STREAM_H__
#define __LOCAL_AV_STREAM_H__

#include "def.h"
#include "tlist.h"
#include "thread.h"
#include "c2sdk.h"

class ATalkSession;
class Preview;


class LocalAVStream
{
public:
    LocalAVStream();
    ~LocalAVStream();

    int                start(int flag);
    int                stop();
    
    HANDLE             addAudioTalk(Preview* cu, c2_status_cb cb, void* userdata);
    void               delAudioTalk(Preview* cu);

    void               notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts);
    void               notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);

protected:
    int                startAudio(int sps = 8000, int bps = 16, int channnels = 1);
    int                stopAudio();

    static void        capVideoCB(int type, void *data, int size, uint64_t pts, int fps, void *userdata);
    static void        capAudioCB(int type, void *data, int size, uint64_t pts, void *userdata);

    int                addAudioListener(ATalkSession*);
    int                delAudioListener(ATalkSession*);

    static  void     * threadProc(ThreadInfo *);
    void               timeDriver();

private:
    int                m_bps;
    int                m_sps;
    int                m_channels;

    HANDLE             m_audioHandle;
    HANDLE             m_videoHandle;

    Thread             m_thread;

    tlist<ATalkSession*> m_audioListener;

    int                m_capFlag;

    FILE*                m_tmp;
    
};


//extern LocalAVStream *localAVSteamMgr;

#endif
