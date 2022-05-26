/**
 * @file      tindy_player.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    
 * @date      2018-2-27
 *
 * @note
 *  tindy_player.h defines
 */

#ifndef __TINDY_PLAYER_H__
#define __TINDY_PLAYER_H__

#include "player.h"
#include "thread.h"
#include "tlist.h"
#include "3rd/tindy/sdk-plat.h"

//#define MAX_PVPOOL_SIZE	   (1*1024*1024)                 /* preview player buffer size */
//#define MAX_PBPOOL_SIZE	   (6*1024*1024)                 /* remote playback player buffer size */
//#define MIN_PBPOOL_MARK      (1*1024*1024)                 /* remote playback player minimun threshold */
//#define MAX_PBPOOL_MARK      (4*1024*1024)                 /* remote playback player maximun threshold */

            
class PlayerListener;
class TindyPlayer : public Player
{
    //enum 
    //{
    //    TD_RECORD
    //};

    //struct frame_data
    //{
    //    unsigned char * data;
    //    int len;
    //};

public:
    TindyPlayer(PlayerListener *listener, bool isOld);
    ~TindyPlayer();

    virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);
    virtual int        open(HWND wnd, string filename, int poolSize);
    virtual void       close();

    virtual int        inputData(void *stream, uint32_t size);
    virtual int        control(int code, void *val, int valSize);
    virtual int        capture(string filepath, int type);
    virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL);
    virtual int        time(time_t& time);
    virtual int        time(datetime_s &time){ return  0; };

    virtual int        startREC(string filepath);
    virtual void       stopREC();

    virtual int        opensound();
    virtual int        closesound();
    virtual int        setVolume(int vol){ return -1; }

    virtual void       enableDraw(bool enable = true);
    virtual void       enableDecode(bool enable = true);

protected:
    virtual void       REC(void *frame, size_t size);
    virtual void       timerdriver();
    //static  void*      threadEntry(ThreadInfo *arg);
    //int                recordProc();
protected:

    static int  __cdecl drawdatacb(long handle, PLAYHDC hdc, void* userData);
    static void __cdecl decdatacb(unsigned int _ulID, unsigned char *_cData, int _iLen, const FRAME_INFO_TD *_pFrameInfo, void* _iUser);


private:
    bool                m_isOld;
    long                m_playID;
    Thread              m_thread;
    bool                m_bstop;
    //tlist<frame_data*>  m_listFrame;

public:
    bool                m_bPlayBack;
    FILE              * m_recfile;
};
#endif
