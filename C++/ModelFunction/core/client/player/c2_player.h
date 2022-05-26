/**
 * @file      c2_player.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 15:10:29
 *
 * @note
 *  c2_player.h defines
 */
#ifndef __C2_PLAYER_H__
#define __C2_PLAYER_H__
#include "player.h"

#define MIN_FRAME_MARK       (25)                          /* remote playback player minimun frame threshold */
#define MAX_FRAME_MARK       (200)                         /* remote playback player maximun frame threshold */

class PlayerListener;
class IWriter;
class C2Player : public Player
{

    friend void        decode_cb(int type, int w, int h, void *yuv, int size, void *userdata);
    friend void        draw_cb(HDC hdc, HWND wnd, int w, int h, void *userdata);
public:
    C2Player(PlayerListener *listener);
    ~C2Player();

    virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);
    virtual int        open(HWND wnd, string filename, int poolSize);
    virtual void       close();

    virtual int        inputData(void *stream, uint32_t size);
    virtual int        control(int code, void *val, int valSize);
    virtual int        capture(string filepath, int type);
    virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL);
    virtual int        time(datetime_s &time);



    virtual int        addWnd(HWND wnd);
    virtual int        delWnd(HWND wnd);


    virtual int        changeWndSize(int width, int height);
    virtual int        changeWndPos(int width, int height);

    virtual int        startREC(string filepath);
    virtual void       stopREC();

    virtual int        opensound();
    virtual int        closesound();
    virtual int        setVolume(int vol);

    

    virtual void       enableDraw(bool enable = true);
    virtual void       enableDecode(bool enable = true);
protected:
    virtual void       REC(void *frame, size_t sizev);
    virtual void       timerdriver();

private:
    HANDLE             m_hp;
    IWriter          * m_wr;
};
#endif
