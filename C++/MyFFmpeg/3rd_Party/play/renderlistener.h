/**
 * @file      renderlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-14 10:05:03
 *   
 * @note
 *  renderlistener.h defines 
 */
#include "def.h"
#ifndef __RENDERLISTENER_H__
#define __RENDERLISTENER_H__

class RenderListener
{
public:
    virtual void       onHDC(HDC hdc, HWND wnd, int w, int h) = 0;
};

#endif
