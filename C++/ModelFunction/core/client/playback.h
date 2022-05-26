/**
 * @file      playback.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-04 11:41:56
 *
 * @note
 *  playback.h defines
 */
#ifndef __PLAYBACK_H__
#define __PLAYBACK_H__
#include "recordstream.h"
#include "playerlistener.h"
#include "c2sdk.h"

class Player;
class Playback : public RecordStream, public PlayerListener
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
    Playback(string filename, Cu *cu);
    Playback(datetime_s &start, datetime_s &end, Cu *cu);
    virtual ~Playback();

    void               init(HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

    long               pos(datetime_s *start, datetime_s *end);
    long               getPlayTime(datetime_s &time);
    long               control(int code, void *val, int valSize);
    long               capture(string filepath, int type);
    int                changeWndSize(int width, int height);
    int                changeWndPos(int x, int y);
    string             getSpeedStr(int speed);
    int                setDecCallback(c2_decode_cb decodeCB, void *userData);

    int                addWnd(HWND wnd);
    int                delWnd(HWND wnd);

    int                opensound();
    int                closesound();
    int                setVolume(int vol);

protected:
    virtual void       stop();

    virtual void       onStream(void *stream, uint32_t size);
    virtual void       onStatus(int code);
    
    virtual void       onEmpty(uint32_t size);
    virtual void       onFull(uint32_t size);
    virtual void       onEnd();
    virtual void       onError();

    virtual void       onYUV(int type, int w, int h, void *yuv, int size);
    virtual void       onHDC(HDC hdc, HWND wnd, int w, int h);

    virtual void       onReposEnd();

    virtual void       timerdriver();

protected:
    Player           * m_player;
    int                m_speed;                            /*< -4~4, actual speed = 2^-4~2^4 */

private:
    bool               m_byname;
    string             m_filename;
    datetime_s         m_start;
    datetime_s         m_end;

    HWND               m_wnd;
    c2_stream_cb       m_streamCB;
    void             * m_streamData;
    c2_status_cb       m_statusCB;
    void             * m_statusData;

    int                m_pos;                              /*< note: play pos and stream pos */
    Semaphore          m_reposem;
    
};

#endif
