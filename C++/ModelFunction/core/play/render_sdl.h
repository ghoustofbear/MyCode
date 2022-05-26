/**
 * @file      sdl_render.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 16:52:19
 *
 * @note
 *  sdl_render.h defines
 */
#ifndef __SDL_RENDER_H__
#define __SDL_RENDER_H__
#include "render.h"
#include "SDL2/SDL.h"

class SDLRender : public Render
{
public:
    SDLRender(HWND wnd, bool useTxt = true);
    virtual ~SDLRender();

    virtual int        changeWndSize(int width, int height);
    virtual int        changeWndPos(int left, int top);

#if 0
    void               handleEvent(SDL_Event& e);
#endif
protected:
    virtual int        init();    

protected:
    virtual void       onYUV(void *data, int size, int w, int h);
    virtual void       onRGB(void *data, int size, int w, int h);

private:
    bool               m_useTxt;
    SDL_Window       * m_window;
    SDL_Renderer     * m_renderer;
    SDL_Texture      * m_texture;
    SDL_GLContext      m_context;
    uint32_t           m_windowID;
    int m_left;
    int m_top;
};
#endif
