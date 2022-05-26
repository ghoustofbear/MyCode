/**
 * @file      render.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 16:51:12
 *
 * @note
 *  render.h defines
 */
#ifndef __RENDER_H__
#define __RENDER_H__

#include "def.h"
#include "vdeclistener.h"
#include "concurrency.h"
#include "c2play.h"

class RenderListener;
class PLAY_API Render : public VDecListener
{
public:
    enum
    {
        R_DDRAW,
        R_D3D,
        R_SDL,
        R_OPENGL,
        R_OPENGLES2
    };

    Render(HWND wnd);
    virtual ~Render();

    bool               isSame(HWND wnd);
    void               setRenderListener(RenderListener *listener);

    virtual int        changeWndSize(int width, int height);
    virtual int        changeWndPos(int left, int top);

protected:
    virtual int        init() = 0;    

protected:
    volatile bool      m_init;
    int                m_type;
    HWND               m_wnd;

    int                m_width;
    int                m_height;

    RenderListener   * m_listener;

    Mutex              m_mtx;
};
#endif
