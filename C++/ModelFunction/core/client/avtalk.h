/**
 * @file      avtalk.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-04 11:41:56
 *
 * @note
 *  avtalk.h defines
 */
#ifndef __AVTALK_H__
#define __AVTALK_H__

#include "playerlistener.h"
#include "c2sdk.h"
#include "streamlistener.h"
#include "socket.h"
#include "tqueue.h"
#include "thread.h"

class Cu;
class Player;

class AVTalk : public StreamSpeaker
{
#define STATUS(code) \
    do \
    { \
        if (m_statusCB) \
        { \
            m_statusCB(code, m_statusData); \
        } \
    } while(0);

public:
    enum
    {
        DTS_UNINIT,
        DTS_STARTING,
        DTS_WAITING_LOGIN,
        DTS_CONNECTING,
        DTS_CONNECT_SUCCESS,
        DTS_REQNING,
        DTS_REFUSE,
        DTS_TALKING,
        DTS_WATTING_CLOSE,
        DTS_CLOSED,
    };

    AVTalk(Cu *cu);
    virtual ~AVTalk();

    int                init();
    void               uninit();

    int                start(string json);
    void               stop(string json);


    virtual int        open();

    int                pause();
    int                resume();

    //request dualtalk to dev
    //return 0 success, other refuse
    virtual int        requestDualTalk(string devListJson);

    //disconnect dualtalk
    virtual int        disconnDualTalk(string devListJson);


    virtual bool       isSame(Cu* cu);
protected:
    int                recvHttpMsg(string &method, string &type, int &status, string &json);
    int                sendRequest(string url, string body, string& rspJson);

    
    

    virtual int        initDualTalk();
    virtual int        reqDualTalk(string devListJson);


    
    virtual void       onStream(void *stream, uint32_t size);
    virtual void       onStatus(int code);
public:
    //speaker
    virtual void       doStream(void *stream, uint32_t size);

private:
    int                recv();
    void               recvLoop();
    
    static  void     * threadProc(ThreadInfo *);   


private:
    char               m_frame[1024];
    int                m_frameSize;

    string             m_devListJson;
    Cu               * m_cu;
    volatile int       m_status;

    int                m_talkStatus;   //0:notalk  1:talking

    tqueue<uint8_t *>  m_wque;
    Socket           * m_sock;
    Thread             m_thread;
    HWND               m_wnd;
    c2_stream_cb       m_streamCB;
    void             * m_streamData;
    c2_status_cb       m_statusCB;
    void             * m_statusData;
};

class Preview;
class Intercom
{
public:
    Intercom(HANDLE view);
    virtual ~Intercom();

    int start();
    int stop();

private:
    static  void     * threadProc(ThreadInfo *);
    void               recvLoop();

private:
    Preview          * m_view;
    Socket           * m_sock;
    Thread             m_thread;
};


#endif
