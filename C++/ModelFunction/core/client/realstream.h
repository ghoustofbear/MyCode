/**
 * @file      realstream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-07 15:36:30
 *
 * @note
 *  realstream.h defines
 */
#ifndef __REALSTREAM_H__
#define __REALSTREAM_H__
#include "def.h"
#include "tlist.h"
#include "thread.h"

#define DELAY_CLOSE_TIME     (10) 
#define MAX_FRAME_SIZE       (2*1024*1024) 

class StreamListener;
class Preview;
class Cu;
class Socket;

class RealStream : public tlist<StreamListener *>
{
public:
    enum
    {
        S_UNINIT,
        S_STARTING,
        S_WAITING_LOGIN,
        S_CONNECTING,
        S_OPENING,
        S_RECVING,
        S_WATTING_CLOSE,
        S_CLOSED,
    };

    RealStream(Cu *cu);
    virtual ~RealStream();

    bool               isSame(Cu *cu, string json);
    int                start(string json);
    void               stop();

    void               addStreamListener(StreamListener *listener);
    void               delStreamListener(StreamListener *listener);

    int                openTalk();
    void               closeTalk();
    void               doAudioStream(void* frame, int size);


    int                status();

    Cu               * getCu();
    string           & getJson();

    void               timerdriver();
#ifdef UNIX_DRAW_RECT
	int                getChannelNo(){ return m_channelNo; }
	string             getDeviceIp() { return m_devIp; }
	int                getDevicePort() { return m_devPort; }
	int               drawROI(string alarmMsg);
    void              clearROI(string strMsg);
#endif
protected:
    virtual int        open() = 0;
    virtual int        recv() = 0;

protected:
    void               notifyStream(void *stream, uint32_t size);
    void               notifyStatus(int code);
    void               recvLoop();
    static  void     * threadProc(ThreadInfo *);

protected:
    volatile int       m_status;
    Thread             m_recvThread;
    Socket           * m_sock;

    Cu               * m_cu;
    string             m_json;

    uint64_t           m_lastViewPTS;

    char               m_frame[MAX_FRAME_SIZE];
    int                m_frameSize;

    bool               m_bTalking;

    uint64_t           m_u64ViewTickTime;
#ifdef UNIX_DRAW_RECT
	int                m_channelNo;
	string             m_devIp;
	int                m_devPort;
#endif
	string             m_strIvaMsg;
};

#endif
