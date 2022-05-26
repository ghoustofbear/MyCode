/**
 * @file      streampl.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-12 14:31:31
 *
 * @note
 *  common stream play defines
 */
#ifndef __STREAMPL_H__
#define __STREAMPL_H__
#include "pl.h"
#include "render.h"
#include "tvector.h"
#include "decoder.h"
class StreamPlay : public tvector<Render *>
{
public:
    StreamPlay();
    virtual ~StreamPlay();

    int                width();
    int                hight();

    int                addWnd();
    int                delWnd();
    HWND               wnd(int idx);

    int                startRecord();
    int                stopRecord();

    int                capture(const string path, int type);
    int                captureRGB(void *buf, int &w, int h);
    int                captureYUV(void *buf, int &w, int h);

    int                openSound();
    void               closeSound();
    int                setVolume(int vol);

    virtual int        start();
    virtual void       stop();

    virtual int        pause(bool pause);
    virtual int        fast();
    virtual int        slow();

private:
    VideoDecoder     * m_vdec;
    AudioDecoder     * m_adec;

    int                m_speed;
};
#endif
