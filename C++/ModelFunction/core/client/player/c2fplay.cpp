#include "def.h"
#include "c2sdk.h"
#include "c2fplay.h"
#include "tls_error.h"
#include "c2_player.h"
#ifndef ANDROID
#include "hik_player.h"
#include "dahua_player.h"
#endif
/* #include "dh_player.h" */
/* #include "td_player.h" */

FPLY_API long
FPLY_set_error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    long ret =  vsetError(fmt, arg);
    va_end(arg);

    return ret;
}

FPLY_API const char *
FPLY_get_error()
{
    return getError();
}

FPLY_API HANDLE
FPLY_openfile(HWND wnd, const char *filename)
{
    if (!filename)
    {
        ERRE_PLY("invalidate filename.");
        return NULL;
    }

    string postfix = strrchr(filename, '.');

    Player *ply = NULL;
    if (".utv" == postfix)
    {
        ply = new C2Player(NULL);
    }
#ifndef ANDROID
    else if (".hkv" == postfix)
    {
        ply = new HikPlayer(NULL, false);
    }
    else if (".dav" == postfix)
    {
        ply = new Dahualayer(NULL, false);
    }
    else if (".sdv" == postfix)
    {
        /* ply = new TDPlayer(NULL, false); */
    }
#endif

    if (ply->open(wnd, filename, MAX_PBPOOL_SIZE) < 0)
    {
        delete ply;
        return NULL;
    }

    return (HANDLE)ply;
}

FPLY_API void
FPLY_closefile(HANDLE fp)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return;
    }

    Player *ply = (Player *)fp;
    ply->close();

    delete ply;
}

FPLY_API void 
FPLY_setPlayerListener(HANDLE fp, PlayerListener* listener)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return;
    }

    Player *ply = (Player *)fp;
    ply->setPlayerListener(listener);
}

FPLY_API long
FPLY_addwnd(HANDLE fp, HWND wnd, int rtype)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    if (Player::T_C2 != ply->type())
    {
        ERRE_PLY("play type: %d have no addwnd", ply->type());
        return -1;
    }

    ERRE_PLY("not implement.");
    return -1;
}

FPLY_API long
FPLY_delwnd(HANDLE fp, HWND wnd)
{
    ERRE_PLY("not implement.");
    return -1;
}

FPLY_API long
FPLY_pause(HANDLE fp, bool pause)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    if (pause)
    {
        return ply->control(PLAY_PAUSE, NULL, 0);
    }
    else
    {
        return ply->control(PLAY_RESTART, NULL, 0);
    }
}

FPLY_API long
FPLY_fast(HANDLE fp, int *speed)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_FAST, speed, sizeof(int));
}

FPLY_API long
FPLY_slow(HANDLE fp, int *speed)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_SLOW, speed, sizeof(int));
}

FPLY_API long
FPLY_normal(HANDLE fp)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_NORMAL, NULL, 0);
}

FPLY_API long
FPLY_openAudio(HANDLE fp)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_OPENAUDIO, NULL, 0);
}

FPLY_API long
FPLY_closeAudio(HANDLE fp)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_CLOSEAUDIO, NULL, 0);
}

FPLY_API long 
FPLY_setVolume(HANDLE fp, int vol)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_SETVOLUME, &vol, sizeof(int));

}


FPLY_API long
FPLY_capture(HANDLE fp, const char *path, int type)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->capture(path, type);
}

FPLY_API long
FPLY_capture_buf(HANDLE fp, char *pic, int *size, int *w, int *h, int type)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    ERRE_PLY("not implement.");
    return -1;
#if 0
    Player *ply = (Player *)fp;

    return ply->capture(path, type);
#endif
}

FPLY_API long
FPLY_setpos(HANDLE fp, int pos)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    if (pos < 0 || pos > 100)
    {

        ERRE_PLY("invalidate pos, must 0~100.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->control(PLAY_SETPOS, &pos, sizeof(int));
}

FPLY_API long
FPLY_getpos(HANDLE fp)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }

    Player *ply = (Player *)fp;

    return ply->pos();
}


FPLY_API long
FPLY_change_wndsize(HANDLE fp, HWND hwnd, int width, int height)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }
    Player *ply = (Player *)fp;
    return ply->changeWndSize(width, height);
}


FPLY_API long
FPLY_change_wndpos(HANDLE fp, HWND hwnd, int width, int height)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }
    Player *ply = (Player *)fp;
    return ply->changeWndPos(width, height);
}


FPLY_API long 
FPLY_enable_drawcb(HANDLE fp, bool enable)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }
    Player *ply = (Player *)fp;
    ply->enableDraw(enable);
    return 0;
}

FPLY_API long 
FPLY_enable_decodecb(HANDLE fp, bool enable)
{
    if (!fp)
    {
        ERRE_PLY("invalidate play handle.");
        return -1;
    }
    Player *ply = (Player *)fp;
    ply->enableDecode(enable);
    return 0;
}

