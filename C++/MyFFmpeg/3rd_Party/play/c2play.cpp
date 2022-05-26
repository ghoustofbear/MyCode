#include "c2play.h"
#include "icommon.h"
#include "mempool.h"
#include "tls_error.h"
#include "concurrency.h"
#include "play_stream.h"
#include "play_lfstream.h"
#include "play_nfstream.h"

#ifdef __linux__
#include "SDL2/SDL.h"
#endif


static bool  init = false;
static Mutex mtx;

static
void PLAY_init()
{
    mempool = new Mempool(MAX_MEMPOOL_SIZE);
    if (icommon::instance()->start() < 0)
    {
        ERRF_SDK("common start fail.");
        abort();
    }

#ifdef __linux__
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ERRF_PLY("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }
#endif


    init = true;

#if _DEBUG
    DEBUG_LEVEL_PLY(LOG_LEVEL_DEBUG);
#else
    DEBUG_LEVEL_PLY(LOG_LEVEL_INFO);
#endif
}

PLAY_API long
PLAY_set_error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    long ret =  vsetError(fmt, arg);
    va_end(arg);

    return ret;
}

PLAY_API const char *
PLAY_get_error()
{
    return getError();
}

PLAY_API HANDLE
PLAY_create(int type, int mode)
{
    {
        AutoLock locker(mtx);
        if (!init)
        {
            PLAY_init();
        }
    }

    StreamPlay *sp = NULL;
    switch (type)
    {
    case STREAM_REAL:
        sp = new StreamPlay(STREAM_REAL, mode);
        break;
    case STREAM_FILE_NET:
        sp = new NFStreamPlay(mode);
        break;
    case STREAM_FILE_LOCAL:
        sp = new LFStreamPlay(mode);
        break;
    case STREAM_AUDIO:
        break;
    default:
        break;
    }

    return (HANDLE)sp;
}

PLAY_API void
PLAY_destroy(HANDLE hp)
{
    if (hp)
    {
        StreamPlay *sp = (StreamPlay *)hp;
        delete sp;
    }
}

PLAY_API long
PLAY_openfile(HANDLE hp, const char *filename)
{
    if (!hp || !filename)
    {
        ERRE_PLY("invalidate play args.");
        return -1;
    }

    LFStreamPlay *lp = (LFStreamPlay *)hp;

    return lp->openFile(filename);
}

PLAY_API void
PLAY_closefile(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return;
    }

    LFStreamPlay *lp = (LFStreamPlay *)hp;

    lp->closeFile();
}

PLAY_API long 
PLAY_getFileTime(HANDLE hp, datetime_s* start_time, datetime_s* end_time)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    LFStreamPlay *lp = (LFStreamPlay *)hp;
    lp->getFileTime(start_time, end_time);
    
    return 0;
}

PLAY_API long
PLAY_addwnd(HANDLE hp, HWND wnd, int rtype)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->addWnd(wnd, rtype);
}

PLAY_API long
PLAY_delwnd(HANDLE hp, HWND wnd)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->delWnd(wnd);
}

PLAY_API long
PLAY_inputdata(HANDLE hp, void *data, int size, bool block)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->inputData(data, size, block);
}

PLAY_API long
PLAY_start(HANDLE hp, c2_status_cb statusCB, void *userdata)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->start(statusCB, userdata);
}

PLAY_API long
PLAY_stop(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->stop();
}


PLAY_API long 
PLAY_openSound(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->openSound();
}


PLAY_API long 
PLAY_closeSound(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    sp->closeSound();
    return 0;
}


PLAY_API long 
PLAY_setVolume(HANDLE hp, int vol)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->setVolume(vol);
}

PLAY_API long
PLAY_pause(HANDLE hp, bool pause)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->pause(pause);
}

PLAY_API long
PLAY_fast(HANDLE hp, int *speed)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    int spd = 1;
    if (sp->fast(spd) < 0)
    {
        return -1;
    }

    *speed = spd;

    return 0;
}

PLAY_API long
PLAY_slow(HANDLE hp, int *speed)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    int spd = 1;
    if (sp->slow(spd) < 0)
    {
        return -1;
    }

    *speed = spd;

    return 0;
}

PLAY_API long
PLAY_capture(HANDLE hp, const char *path, int type)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->capture(path, type);
}

PLAY_API long
PLAY_capture_buf(HANDLE hp, char *pic, int *size, int *w, int *h, int type)
{
    if (!hp || !pic || !size || !w || !h)
    {
        ERRE_PLY("invalidate play handle or args.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    int legnth = *size;
    int width  = 0;
    int height = 0;
    int result = -1;

    if (1 == type)
    {
        result = sp->captureYUV(pic, legnth, width, height);
    }
    if (2 == type)
    {
        /* result = sp->captureBMP(pic, legnth, width, height); */
    }
    if (3 == type)
    {
        /* result = sp->captureJPEG(pic, legnth, width, height); */
    }
    else
    {
        result = sp->captureRGB(pic, legnth, width, height);
    }

    if (result < 0)
    {
        return -1;
    }

    *size = legnth;
    *w    = width;
    *h    = height;

    return 0;
}

PLAY_API long
PLAY_remain_buffer(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->remainFrame();
}

PLAY_API long
PLAY_reset_buffer(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->resetFrame();
}

PLAY_API long
PLAY_current_time(HANDLE hp, datetime_s *time)
{
    if (!hp || !time)
    {
        ERRE_PLY("invalidate play handle or args.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->currentTime(*time);
}

PLAY_API long
PLAY_set_threshold(HANDLE hp, int type, int fcnt, c2_threshold_cb threshold_cb, void *userdata)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    if (0 == type)
    {
        sp->setMinThreshold(fcnt, threshold_cb, userdata);
    }
    else
    {
        sp->setMaxThreshold(fcnt, threshold_cb, userdata);
    }

    return 0;
}

PLAY_API long
PLAY_change_wndsize(HANDLE hp, HWND wnd, int width, int height)
{
    if (!hp || !(width > 0 && width < 8000) || !(height > 0 && height < 6000))
    {
        ERRE_PLY("invalidate play handle or args.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->changeWnnSize(wnd, width, height);
}

PLAY_API long
PLAY_change_position(HANDLE hp, HWND wnd, int left, int top)
{
    if (!hp /*|| !(left > 0 && left < 8000) || !(top > 0 && top < 6000)*/)
    {
        ERRE_PLY("invalidate play handle or args.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    return sp->changeWnnPos(wnd, left, top);
}

PLAY_API long
PLAY_setpos(HANDLE hp, int pos)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    if (pos < 0 || pos > 100)
    {
        ERRE_PLY("invalidate pos, must 0~100.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    if (STREAM_FILE_LOCAL != sp->type())
    {
        LOGW_PLY("not suport, play type: %d", sp->type());
        return 0;
    }

    return ((LFStreamPlay *)sp)->setpos(pos);
}

PLAY_API long
PLAY_getpos(HANDLE hp)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;

    if (STREAM_FILE_LOCAL != sp->type())
    {
        LOGW_PLY("not suport, play type: %d", sp->type());
        return 0;
    }

    return ((LFStreamPlay *)sp)->getpos();
}

PLAY_API long
PLAY_set_drawcb(HANDLE hp, HWND wnd, c2_draw_cb draw_cb, void *userdata)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;
    sp->setDrawCB(wnd, draw_cb, userdata);

    return 0;
}

PLAY_API long
PLAY_set_decodecb(HANDLE hp, c2_decode_cb decode_cb, void *userdata)
{
    if (!hp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    StreamPlay *sp = (StreamPlay *)hp;
    sp->setDecodeCB(decode_cb, userdata);

    return 0;
}
