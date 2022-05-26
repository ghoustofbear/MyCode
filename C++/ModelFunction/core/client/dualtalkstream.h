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
#ifndef __DUALTALKSTREAM_H__
#define __DUALTALKSTREAM_H__
#include "def.h"
#include "tlist.h"
#include "tqueue.h"
#include "thread.h"


#define MAX_FRAME_SIZE       (2*1024*1024) 

class Cu;
class Socket;
class AVTalk;

class DualtalkStream
{
public:
    enum 
    {
        DS_HIK,
        DS_DAHUA,
        DS_TINDY,
        DS_HELM,
        DS_PC,
        DS_APP,
    };

    enum
    {
        S_UNINIT,
        S_CAPING,
        S_CLOSED,
    };

    DualtalkStream(Cu* cu);
    virtual ~DualtalkStream();

    virtual int        start(string json);
    virtual void       stop();

    Cu *               getCu() const { return m_cu; }

    HANDLE             addStreamSpeaker(string json);
    int                delStreamSpeaker(string json);

    void               addStreamSpeaker(AVTalk *listener, bool a, bool v);
    void               delStreamSpeaker(AVTalk *listener, bool a, bool v);

    static void        cap_video(int type, void *data, int size, uint64_t pts, int fps, void *userdata);
    static void        cap_audio(int type, void *data, int size, uint64_t pts, void *userdata);

    void               notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts);
    void               notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    void               notifyStatus(int code);
protected:

    virtual void       timerdriver();

    void               timedriverLoop();
    void               recvLoop();

    static  void     * threadProc(ThreadInfo *);

    int                startCap(bool a);
    void               stopCap(bool a);

protected:
    Cu                   * m_cu;
    int                    m_dualType;
    Thread                 m_timeDriver;    
    volatile int           m_status;
    uint64_t               m_tmcnt;                            /**< timer driver count */
    HANDLE                 m_capAudio;
    HANDLE                 m_capVideo;
    tlist<AVTalk *>        m_audioListers;
    tlist<AVTalk *>        m_videoListers;
    uint64_t               m_vsesspts;

    char*                  m_aframe[1024];
};

#endif
