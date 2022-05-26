/**
 * @file      playerlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-04 09:34:13
 *   
 * @note
 *  playerlistener.h defines 
 */
#ifndef __PLAYERLISTENER_H__
#define __PLAYERLISTENER_H__

#if defined(DLL_EXPORT)
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef void*                HANDLE;
typedef unsigned long        HWND;
typedef void*                HDC;
#endif

#else

#include "def.h"

#endif

class PlayerListener
{
public:
    virtual void       onEmpty(uint32_t size) = 0;
    virtual void       onFull(uint32_t size ) = 0;
    virtual void       onEnd()                = 0;
    virtual void       onError()              = 0;

    virtual void       onYUV(int type, int w, int h, void *yuv, int size) = 0;
    virtual void       onHDC(HDC hdc, HWND wnd, int w, int h) = 0;
};

#endif
