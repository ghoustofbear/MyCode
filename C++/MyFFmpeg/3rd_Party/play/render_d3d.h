/**
 * @file      D3DRender.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 17:12:58
 *
 * @note
 *  d3d_render.h defines
 */
#ifndef __D3D_RENDER_H__
#define __D3D_RENDER_H__
#include "render.h"
#include <d3d9.h>
class PLAY_API D3DRender : public Render
{
public:
    D3DRender(HWND wnd);
    virtual ~D3DRender();

protected:
    virtual int        init();    

protected:
    virtual void       onYUV(void *data, int size, int w, int h);
    virtual void       onRGB(void *data, int size, int w, int h);

private:
    IDirect3D9       * m_d3d9;
    IDirect3DDevice9 * m_d3dd;
    IDirect3DSurface9* m_d3ds;
    RECT               m_view;
};
#endif
