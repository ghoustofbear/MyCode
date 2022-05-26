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
#ifndef __ATALK_SESSION_H__
#define __ATALK_SESSION_H__

#include "playerlistener.h"
#include "c2sdk.h"
#include "streamlistener.h"
#include "socket.h"
#include "thread.h"

class Cu;
class ATalkSession : public AudioStreamListener
{
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

    ATalkSession(Cu *cu);
    virtual ~ATalkSession();

    void               init(c2_status_cb cb, void* userdata);
    int                start(string json);
    void               stop(string json);

    int                talkTo(HANDLE view);
    int                endTalk(HANDLE view);


    

    bool               isSame(Cu*, string);
    
    

protected:
    virtual int        open() = 0;
    virtual int        heartbeat();

    void               statusCallback(int status);
    int                sendRequest(string url, string body, string& rspJson);
    int                recvHttpMsg(string &method, string &type, int &status, string &json);
    static  void     * threadProc(ThreadInfo *);
    void               recvLoop();

protected:
    Cu*                m_cu;
    string             m_json;

    Socket           * m_sock;
    Thread             m_thread;
    int                m_status;

    c2_status_cb       m_statusCB;
    void*              m_statusData;

    int                m_laststatus;
};

class Preview;
class HttpATalkSession :  public ATalkSession
{
public:

    HttpATalkSession(Preview *view);
    HttpATalkSession(Cu *cu);
    virtual ~HttpATalkSession();

    virtual int        open();
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);

private:
    char               m_aframe[1024];    
};








#endif
