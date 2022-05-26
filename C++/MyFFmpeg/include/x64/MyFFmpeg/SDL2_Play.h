#ifndef SDL2_PLAY_H
#define SDL2_PLAY_H
#include <stdio.h>
#define __STDC_CONSTANT_MACROS
#define SDL_INIT_TIMER          0x00000001u
#define SDL_INIT_AUDIO          0x00000010u
#define SDL_INIT_VIDEO          0x00000020u  

#define SDL_WINDOWPOS_UNDEFINED_MASK    0x1FFF0000u
#define SDL_WINDOWPOS_UNDEFINED_DISPLAY(X)  (SDL_WINDOWPOS_UNDEFINED_MASK|(X))
#define SDL_WINDOWPOS_UNDEFINED         SDL_WINDOWPOS_UNDEFINED_DISPLAY(0)
#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/error.h"
#include "libavutil/avutil.h"
#include"SDL2/SDL.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "libavutil/error.h"
#include <SDL2/SDL.h>
#ifdef __cplusplus
};
#endif
#endif
struct AVFrame;
class SDL2_Play
{
public:
     int screen_w=0,screen_h=0;
    SDL_Window *m_sdlwindows;
    SDL_Renderer* m_sdlrender;
    SDL_Texture* m_sdltexture;
    SDL_Rect m_sdlrect;
    virtual void Init(int width, int height,const char* url) = 0;
    virtual void Repaint(AVFrame *frame,const char* url) = 0;
};
#endif