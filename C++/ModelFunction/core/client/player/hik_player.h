/**
 * @file      hik_player.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-07-06 17:00:59
 *
 * @note
 *  hik_player.h defines
 */
#ifndef __HIK_PLAYER_H__
#define __HIK_PLAYER_H__


#ifndef __ANDROID__


#include "player.h"
#include "3rd/hik/sdk-plat.h"

//#define MAX_PVPOOL_SIZE	     (1*1024*1024)                 /* preview player buffer size */
//
//#define MAX_PBPOOL_SIZE	     (6*1024*1024)                 /* remote playback player buffer size */
//#define MIN_PBPOOL_MARK      (1*1024*1024)                 /* remote playback player minimun threshold */
//#define MAX_PBPOOL_MARK      (4*1024*1024)                 /* remote playback player maximun threshold */

#define CBK                  __stdcall
class PlayerListener;
class HikPlayer : public Player
{
public:
    HikPlayer(PlayerListener *listener, bool isOld);
    ~HikPlayer();

    virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);
    virtual int        open(HWND wnd, string filename, int poolSize);
    virtual void       close();
    virtual int        openReverse(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);

    virtual int        inputData(void *hdr, uint32_t size);
    virtual int        control(int code, void *val, int valSize);
    virtual int        capture(string filepath, int type);
    virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL);
    virtual int        time(datetime_s &time);

    virtual int        startREC(string filepath);
    virtual void       stopREC();

    virtual int        changeWndSize(int width, int height);
    virtual int        setDisplayRegion(float left, float right, float top, float bottom);

    virtual int        opensound();
    virtual int        closesound();
    virtual int        setVolume(int vol);

    virtual void       enableDraw(bool enable = true);
    virtual void       enableDecode(bool enable = true);
protected:
    virtual void       REC(void *frame, size_t size);
    virtual void       timerdriver();

protected:
#if defined(__linux__)
    static void        decdatacb(int port, char *data, int size, FRAME_INFO *info, void *reserved1, int reserved2);
    static void        drawdatacb(int handle, PLAYM4_HDC hdc, void *userData);
#elif defined(_WIN64)
    static void CBK    decdatacb(long port, char *data, long size, FRAME_INFO *info, HIK_USERDATA reserved1, HIK_USERDATA reserved2);
    static void CBK    drawdatacb(long handle, HDC hdc, HIK_USERDATA userData);
#elif defined(_WIN32)
    static void CBK    decdatacb(long port, char *data, long size, FRAME_INFO *info, long reserved1, long reserved2);
    static void CBK    drawdatacb(long handle, HDC hdc, long userData);
#endif

private:
    bool               m_isOld;
    long               m_playID;
    bool               m_bReverse;
    FILE             * m_recfile;
	bool				m_isHikHead;
};
#endif

#endif //__ANDROID__


