/**
 * @file      helmet_player.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-07-06 17:00:59
 *
 * @note
 *  helmet_player.h defines
 */
#ifndef __HELMET_PLAYER_H__
#define __HELMET_PLAYER_H__


#ifndef __ANDROID__


#include "player.h"
#include "3rd/helmet/VARender.h"
#include "tqueue.h"
#include "thread.h"

//#define MAX_PVPOOL_SIZE	     (1*1024*1024)                 /* preview player buffer size */
//
//#define MAX_PBPOOL_SIZE	     (6*1024*1024)                 /* remote playback player buffer size */
//#define MIN_PBPOOL_MARK      (1*1024*1024)                 /* remote playback player minimun threshold */
//#define MAX_PBPOOL_MARK      (4*1024*1024)                 /* remote playback player maximun threshold */

#define CBK                  __stdcall
class PlayerListener;
class HelmetPlayer : public Player
{
public:

    friend void VARENDER_STDCALL vRenderVideoCallback(VARENDERUSER_HANDLE hUser, int nWidth, int nHeight, char *pBuf, int iBufLen, char cFormat);

    HelmetPlayer(PlayerListener *listener);
    ~HelmetPlayer();

    virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head = NULL, int size = 0);
    virtual int        open(HWND wnd, string filename, int poolSize);
    virtual void       close();

    virtual int        inputData(void *stream, uint32_t size);
    virtual int        control(int code, void *val, int valSize);
    virtual int        capture(string filepath, int type);
    virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL);
    virtual int        time(datetime_s &time);

    virtual int        startREC(string filepath);
    virtual void       stopREC();

    virtual int        changeWndSize(int width, int height);

    virtual int        opensound();
    virtual int        closesound();
    virtual int        setVolume(int vol){ return -1; }

    virtual void       enableDraw(bool enable = true);
    virtual void       enableDecode(bool enable = true);
protected:
    virtual void       REC(void *frame, size_t size);
    virtual void       timerdriver();

    static void *      threadProc(ThreadInfo *);
    void               onInputdata();

private:
    VARENDER_HANDLE    m_playHandle;
    FILE             * m_recfile;

    tqueue<uint8_t *>  m_recvQue;
    Thread             m_inputThread;
    uint64_t           m_currentPlayTime;

    static  bool       m_initPlayLib;
};
#endif

#endif //__ANDROID__


