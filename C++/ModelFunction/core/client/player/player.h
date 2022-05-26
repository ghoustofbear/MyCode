/**
 * @file      player.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-07-06 15:08:54
 *
 * @note
 *  player.h defines
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "def.h"
#include <mutex>

#define MAX_PVPOOL_SIZE	     (1*1024*1024)                 /* preview player buffer size */

#define MAX_PBPOOL_SIZE	     (10*1024*1024)                 /* remote playback player buffer size */
#define MIN_PBPOOL_MARK      (2*1024*1024)                 /* remote playback player minimun threshold */
#define MAX_PBPOOL_MARK      (7*1024*1024)                 /* remote playback player maximun threshold */

class PlayerListener;
class Player
{
    friend class Playback;
    friend class Preview;

public:
    enum
    {
        T_C2,
        T_HIK,
        T_TIANDY,
        T_DAHUA,
    };

protected:
    enum
    {
        STREAM_REAL,
        STREAM_FILE,
        LOCAL_FILE,
    };
public:
    enum
    {
        S_STOP,
        S_PLAY,
        S_PAUSE,
    };

public:
    Player(PlayerListener *listener);
    virtual ~Player();
    void               setPlayerListener(PlayerListener* listener);

public:
    static  int        initLib(const char* jvm2nativePath = "");
    static  void       uninitLib();

    virtual int        open(HWND wnd, string filename, int poolSize) = 0;
    virtual void       close() = 0;
    virtual int        control(int code, void *val, int valSize) = 0;
    virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL) = 0;

    int                type();

    virtual void       enableDraw(bool enable = true) = 0;
    virtual void       enableDecode(bool enable = true) = 0;

    virtual int        startREC(string filepath) = 0;
    virtual void       stopREC() = 0;
    virtual int        capture(string filepath, int type) = 0;

    virtual int        addWnd(HWND wnd) { return -1; }
    virtual int        delWnd(HWND wnd) { return -1; }

    virtual int        changeWndSize(int width, int height);
    virtual int        changeWndPos(int x, int y);
    virtual int        setDisplayRegion(float left, float right, float top, float bottom){ return 0; }


    virtual int        opensound() = 0;
    virtual int        closesound() = 0;
    virtual int        openReverse(HWND wnd, int streamMode, int encode, int poolSize, void *head = NULL, int size = 0){ return -1; }
    virtual int        setVolume(int vol) = 0;

protected:
    virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head = NULL, int size = 0) = 0;
    virtual int        inputData(void *stream, uint32_t size) = 0;

    virtual int        time(datetime_s &time) = 0;
    virtual void       timerdriver() = 0;
    virtual void       REC(void *frame, size_t size) = 0;

    virtual void       reposEnd(bool end);

    void               notifyEmpty(uint32_t size);
    void               notifyEnd();

    void               notifyYUV(int type, int w, int h, void *yuv, int size);
    void               notifyHDC(HDC hdc, HWND wnd, int w, int h);


protected:
    static string      m_jvm2nativePath;
    int                m_type;
    
    int                m_encode;
    bool               m_isOpen;
    int                m_streamMode;

    HWND               m_wnd;
    int                m_pos;

    int                m_speed;                            /*< -4~4, actual speed = 2^-4~2^4 */
    bool               m_repos;                            /*< is reposing */

    int                m_picw;                             /*< decode picture width */
    int                m_pich;                             /*< decode picture height */

    int                m_sound;                            /*< 0-close sound  1-open sound */
    PlayerListener   * m_listener;

    mutex              m_wrmtx;

	uint64_t           m_tmcnt; 
public:
    int                m_status;
    void               notifyFull(uint32_t size);
    void               notifyError();
};

#endif
