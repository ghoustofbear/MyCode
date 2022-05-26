#include "c2sdk.h"
#include "c2_player.h"
#include "c2play.h"
#include "tls_error.h"
#include "asl.h"
#include "fsrw.h"

typedef void (*c2_status_cb)(int code, void* userData);
typedef void (*c2_threshold_cb)(int type, int fcnt, void* userdata);
extern "C"
{
    typedef long (*_set_error)(const char* fmt, ...);
    typedef const char *(*_get_error)();
    typedef HANDLE(*_create)(int type, int mode);
    typedef void (*_destroy)(HANDLE hp);
    typedef long (*_addwnd)(HANDLE hp, HWND wnd, int rtype);
    typedef long (*_delwnd)(HANDLE hp, HWND wnd);
    typedef long (*_inputdata)(HANDLE hp, void *data, int size, bool block);
    typedef long (*_start)(HANDLE hp, c2_status_cb statusCB, void *userData);
    typedef long (*_stop)(HANDLE hp);
    typedef long (*_opensound)(HANDLE hp);
    typedef long (*_closesound)(HANDLE hp);
    typedef long (*_setvolume)(HANDLE hp, int vol);
    typedef long (*_pause)(HANDLE hp, bool pause);
    typedef long (*_fast)(HANDLE hp, int *speed);
    typedef long (*_slow)(HANDLE hp, int *speed);
    typedef long (*_capture)(HANDLE hp, const char *path, int type);
    typedef long (*_set_threshold)(HANDLE hp, int type, int fcnt, c2_threshold_cb threshold_cb, void *userdata);
    typedef long (*_capture_buf)(HANDLE hp, char *pic, int *size, int *w, int *h, int type);
    typedef long (*_remain_buffer)(HANDLE hp);
    typedef long (*_reset_buffer)(HANDLE hp);
    typedef long (*_current_time)(HANDLE hp, datetime_s *time);
    typedef long (*_change_wndsize)(HANDLE hp, HWND wnd, int width, int height);
    typedef long (*_change_position)(HANDLE hp, HWND wnd, int x, int y);
    typedef long (*_openfile)(HANDLE hp, const char *filename);
    typedef void (*_closefile)(HANDLE hp);
    typedef long (*_setpos)(HANDLE hp, int pos);
    typedef long (*_getpos)(HANDLE hp);
    typedef long (*_set_drawcb)(HANDLE hp, HWND wnd, c2_draw_cb draw_cb, void *userdata);
    typedef long (*_set_decodecb)(HANDLE hp, c2_decode_cb decode_cb, void *userdata);
}

ASL_LIBRARY_BEGIN(C2Play)
ASL_SYMBOL_OPTIONAL(_set_error,      PLAY_set_error)
ASL_SYMBOL_OPTIONAL(_get_error,      PLAY_get_error)
ASL_SYMBOL_EXPLICIT(_create,         PLAY_create)
ASL_SYMBOL_OPTIONAL(_destroy,        PLAY_destroy)
ASL_SYMBOL_OPTIONAL(_addwnd,         PLAY_addwnd)
ASL_SYMBOL_OPTIONAL(_delwnd,         PLAY_delwnd)
ASL_SYMBOL_OPTIONAL(_inputdata,      PLAY_inputdata)
ASL_SYMBOL_OPTIONAL(_start,          PLAY_start)
ASL_SYMBOL_OPTIONAL(_stop,           PLAY_stop)
ASL_SYMBOL_OPTIONAL(_opensound,      PLAY_openSound)
ASL_SYMBOL_OPTIONAL(_closesound,     PLAY_closeSound)
ASL_SYMBOL_OPTIONAL(_setvolume,      PLAY_setVolume)
ASL_SYMBOL_OPTIONAL(_pause,          PLAY_pause)
ASL_SYMBOL_OPTIONAL(_fast,           PLAY_fast)
ASL_SYMBOL_OPTIONAL(_slow,           PLAY_slow)
ASL_SYMBOL_OPTIONAL(_capture,        PLAY_capture)
ASL_SYMBOL_OPTIONAL(_set_threshold,  PLAY_set_threshold)
ASL_SYMBOL_OPTIONAL(_capture_buf,    PLAY_capture_buf)
ASL_SYMBOL_OPTIONAL(_remain_buffer,  PLAY_remain_buffer)
ASL_SYMBOL_OPTIONAL(_reset_buffer,   PLAY_reset_buffer)
ASL_SYMBOL_OPTIONAL(_current_time,   PLAY_current_time)
ASL_SYMBOL_OPTIONAL(_change_wndsize, PLAY_change_wndsize)
ASL_SYMBOL_OPTIONAL(_change_position,PLAY_change_position)
ASL_SYMBOL_OPTIONAL(_openfile,       PLAY_openfile)
ASL_SYMBOL_OPTIONAL(_closefile,      PLAY_closefile)
ASL_SYMBOL_OPTIONAL(_setpos,         PLAY_setpos)
ASL_SYMBOL_OPTIONAL(_getpos,         PLAY_getpos)
ASL_SYMBOL_OPTIONAL(_set_drawcb,     PLAY_set_drawcb)
ASL_SYMBOL_OPTIONAL(_set_decodecb,   PLAY_set_decodecb)
ASL_LIBRARY_END()


void
decode_cb(int type, int w, int h, void *yuv, int size, void *userdata)
{
    C2Player *player = (C2Player *)userdata;
    if (player == NULL)
    {
        return;
    }

    player->notifyYUV(type, w, h, yuv, size);
}

void
draw_cb(HDC hdc, HWND wnd, int w, int h, void *userdata)
{
    C2Player *player = (C2Player *)userdata;
    if (player == NULL)
    {
        return;
    }

    player->notifyHDC(hdc, wnd, w, h);
}

static C2Play c2play;
class C2PLAYInitializer
{
public:
    C2PLAYInitializer(string path)
    {
        LOGE_SDK("load player lib: %s", path.c_str());
        try
        {
#ifdef ANDROID
            string p = path + "libc2play.so";
            LOGE_SDK("load player lib: %s %s",path.c_str(), p.c_str());
            c2play.Load(p.c_str());
#else
            c2play.Load(ASL_LIBRARY_NAME("c2play"));
#endif
            
        }
        catch (const ASL::LibraryNotFoundException& e)
        {
            LOGE_SDK("load Lib failed, lib not found: %s", e.what());
            return;
        }
        catch (const ASL::SymbolNotFoundException& e)
        {
            LOGE_SDK("load Lib failed, Sym not found: %s", e.what());
            return;
        }

        LOGE_SDK("Lib load success path:%s\n", path.c_str());

        ASSERT(c2play.PLAY_create         != NULL);
        ASSERT(c2play.PLAY_destroy        != NULL);
        ASSERT(c2play.PLAY_addwnd         != NULL);
        ASSERT(c2play.PLAY_delwnd         != NULL);
        ASSERT(c2play.PLAY_inputdata      != NULL);
        ASSERT(c2play.PLAY_start          != NULL);
        ASSERT(c2play.PLAY_stop           != NULL);
        ASSERT(c2play.PLAY_pause          != NULL);
        ASSERT(c2play.PLAY_fast           != NULL);
        ASSERT(c2play.PLAY_slow           != NULL);
        ASSERT(c2play.PLAY_capture        != NULL);
        ASSERT(c2play.PLAY_capture_buf    != NULL);
        ASSERT(c2play.PLAY_remain_buffer  != NULL);
        ASSERT(c2play.PLAY_current_time   != NULL);
        ASSERT(c2play.PLAY_change_wndsize != NULL);
        ASSERT(c2play.PLAY_change_position!= NULL);
        ASSERT(c2play.PLAY_openfile       != NULL);
        ASSERT(c2play.PLAY_closefile      != NULL);
        ASSERT(c2play.PLAY_setpos         != NULL);
        ASSERT(c2play.PLAY_getpos         != NULL);
        ASSERT(c2play.PLAY_set_drawcb     != NULL);
        ASSERT(c2play.PLAY_set_decodecb   != NULL);

        m_init = true;
    }

    ~C2PLAYInitializer()
    {
        m_init = false;
    }

    static bool inited()
    {
        return m_init;
    }

private:
    static bool m_init;
};
bool C2PLAYInitializer::m_init = false;

C2Player::C2Player(PlayerListener *listener)
    : Player(listener)
{
    m_type = T_C2;
    m_hp   = NULL;
    m_wnd  = 0;
    m_wr = NULL;
}

C2Player:: ~C2Player()
{

}

int
C2Player::open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
    static C2PLAYInitializer initializer(m_jvm2nativePath);

    m_wnd        = wnd;
    m_streamMode = streamMode;
    m_encode     = encode;

    if (m_isOpen)
    {
        return 0;
    }

    if (!initializer.inited())
    {
        ERRE_SDK("c2play init failed.");
        return -1;
    }

    m_hp = c2play.PLAY_create(streamMode, 0);
    /* m_hp = c2play.PLAY_create(streamMode, 1); */
    if (!m_hp)
    {
        goto exit;
    }

#ifdef _WIN32
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_D3D) < 0)
#elif defined UNIX
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_SDL) < 0)
#elif defined ANDROID
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_OPENGL) < 0)
#endif
    {
        goto exit;
    }

    if (c2play.PLAY_start(m_hp, NULL, NULL) < 0)
    {
        goto exit;
    }

    m_speed = 0;

#if 1
    enableDecode(true);
#endif

    return 0;

exit:
    ERRW_SDK("c2 player opened failed: %s.", c2play.PLAY_get_error());

    if (m_hp)
    {
        c2play.PLAY_destroy(m_hp);
        m_hp = NULL;
    }

    return -1;
}

int
C2Player::open(HWND wnd, string filename, int poolSize)
{
    static C2PLAYInitializer initializer(m_jvm2nativePath);

    int result = 0;

    m_wnd        = wnd;
    m_streamMode = LOCAL_FILE;

    if (m_isOpen)
    {
        return 0;
    }

    do
    {
        if (!initializer.inited())
        {
            ERRE_SDK("c2play init failed.");
            result = -1;
            break;
        }

        m_hp = c2play.PLAY_create(STREAM_FILE_LOCAL, 0);
        if (!m_hp)
        {
            ERRE_SDK("c2play PLAY_create failed.");
            result = -1;
            break;
        }

        if (c2play.PLAY_openfile(m_hp, filename.c_str()) < 0)
        {
            ERRE_SDK("c2play PLAY_openfile failed.");
            result = -1;
            break;
        }

#ifdef _WIN32
        if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_D3D) < 0)
#elif defined UNIX
        if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_SDL) < 0)
#elif defined ANDROID
        if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_OPENGL) < 0)
#endif
        {
            ERRE_SDK("c2play PLAY_addwnd failed.");
            result = -1;
            break;
        }

        if (c2play.PLAY_start(m_hp, NULL, NULL) < 0)
        {
            ERRE_SDK("c2play PLAY_start failed.");
            result = -1;
            break;
        }

        m_speed = 0;

        //opensound();


    #if 1
        enableDecode(true);
    #endif

        return 0;
    }
    while (0);

    return result;
}

void
C2Player::close()
{
    stopREC();

    if (m_hp)
    {
        c2play.PLAY_delwnd(m_hp, m_wnd);
        c2play.PLAY_stop(m_hp);

        if (LOCAL_FILE == m_streamMode)
        {
            c2play.PLAY_closefile(m_hp);
        }

        c2play.PLAY_destroy(m_hp);
        m_hp = NULL;
    }
}

int
C2Player::inputData(void *stream, uint32_t size)
{
    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");
        notifyError();
        return -1;
    }

    if (m_repos)
    {
        ERRD_SDK("Waiting reposing.");
        return 0;
    }


#if 1
    enableDecode(true);
#endif

    int result = 0;
    if (m_streamMode == LOCAL_FILE)
        result = c2play.PLAY_inputdata(m_hp, stream, size, true);
    else
        result = c2play.PLAY_inputdata(m_hp, stream, size, false);
    
    if (0 != result)
    {
        /* if (PLAY_BUF_OVER == result) */
        {
            ERRW_SDK("%s", c2play.PLAY_get_error());
            //notifyError();

            return 0;
        }

        //ERRE_SDK("%s", c2play.PLAY_get_error());
        //notifyError();

        return -1;
    }

    m_status = S_PLAY;

    REC(stream, size);

    if (STREAM_FILE == m_streamMode)
    {
        int remain = c2play.PLAY_remain_buffer(m_hp);
        //if (remain >= MAX_FRAME_MARK)
        if (remain >= 49)
        {
            notifyFull(remain);
        }
    }

    return 0;
}

int
C2Player::control(int code, void *val, int valSize)
{
    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");
        return -1;
    }

    int result = 0;
    switch (code)
    {
    case PLAY_PAUSE:
        result = c2play.PLAY_pause(m_hp, true);
        m_status = S_PAUSE;
        break;

    case PLAY_RESTART:
        result = c2play.PLAY_pause(m_hp, false);
        m_status = S_PLAY;
        break;

    case PLAY_FAST:
        result = c2play.PLAY_fast(m_hp, &m_speed);
        if (val)
        {
            *((int *)val) = m_speed;
        }
        break;

    case PLAY_SLOW:
        result = c2play.PLAY_slow(m_hp, &m_speed);
        if (val)
        {
            *((int *)val) = m_speed;
        }
        break;

    case PLAY_NORMAL:
        while (m_speed > 0)
        {
            result = c2play.PLAY_slow(m_hp, &m_speed);
        }

        while (m_speed < 0)
        {
            result = c2play.PLAY_fast(m_hp, &m_speed);
        }
        m_status = S_PLAY;
        break;

    case PLAY_STARTREPOS:
        m_repos = true;
        result  = c2play.PLAY_reset_buffer(m_hp);
        break;

    case PLAY_SETPOS:
        if (val)
        {
            int pos = *((int *)val);
            result += c2play.PLAY_setpos(m_hp, pos);
        }
        result += c2play.PLAY_reset_buffer(m_hp);
        break;

    case PLAY_SETTIME:
        result = c2play.PLAY_reset_buffer(m_hp);
        break;

    case PLAY_OPENAUDIO:
        result = opensound();
        break;

    case PLAY_CLOSEAUDIO:
        result = closesound();
        break;
    case PLAY_SETVOLUME:
        result = setVolume(*((int *)val));
        break;


    default:
        ERRE_SDK("Unknow control commond.");
        return -1;
    }

    if (result < 0)
    {
        ERRE_SDK("PLAY_xxx call failed err: %d.", c2play.PLAY_get_error());
    }

    return result;
}

int
C2Player::capture(string filepath, int type)
{
    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");
        return -1;
    }

    FILE *pic = NULL;
    pic = fopen(filepath.c_str(), "wb");
    if (!pic)
    {
        ERRE_SDK("open file: %s fail", filepath.c_str());
        return  false;
    }

    return c2play.PLAY_capture(m_hp, filepath.c_str(), type);
}

int
C2Player::pos(datetime_s *start, datetime_s *end)
{
    if (m_repos)
    {
        ERRD_SDK("Waiting reposing.");
        return -1;
    }

    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");
        return -1;
    }

    if (start && end)
    {
        int remain = c2play.PLAY_remain_buffer(m_hp);
        if (0 == remain)
        {
            ERRE_SDK("Play buffer is empty.");
            return -1;
        }

        tm tm_start;
        tm tm_end;

        memset(&tm_start, 0, sizeof(tm));
        memset(&tm_end, 0, sizeof(tm));

        tm_start.tm_year = start->year  - 1900;
        tm_start.tm_mon  = start->month - 1;
        tm_start.tm_mday = start->day;
        tm_start.tm_hour = start->hour;
        tm_start.tm_min  = start->minute;
        tm_start.tm_sec  = start->second;

        time_t t_start   = mktime(&tm_start);

        tm_end.tm_year = end->year  - 1900;
        tm_end.tm_mon  = end->month - 1;
        tm_end.tm_mday = end->day;
        tm_end.tm_hour = end->hour  ;
        tm_end.tm_min  = end->minute;
        tm_end.tm_sec  = end->second;
        time_t t_end   = mktime(&tm_end);

        datetime_s dt = {0};
        if (0 == time(dt))

        {
            tm tm_cur = {0};
            tm_cur.tm_year = dt.year - 1900;
            tm_cur.tm_mon  = dt.month  - 1;
            tm_cur.tm_mday = dt.day;
            tm_cur.tm_hour = dt.hour;
            tm_cur.tm_min  = dt.minute;
            tm_cur.tm_sec  = dt.second;

            time_t current = mktime(&tm_cur);

            /* m_pos = (int)((current - t_start) / float(t_end - t_start + 1) * 100 + 0.5); */
            m_pos = (int)((current - t_start) / float(t_end - t_start + 1) * 100);
        }

        LOGI_SDK("play pos: %d", m_pos);

        return m_pos;
    }
    else
    {
        return c2play.PLAY_getpos(m_hp);
    }

    return m_pos;
}

int
C2Player::time(datetime_s &time)
{
    return c2play.PLAY_current_time(m_hp, &time);
}


int
C2Player::addWnd(HWND wnd)
{
#ifdef _WIN32
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_D3D) < 0)
#elif defined UNIX
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_SDL) < 0)
#elif defined ANDROID
    if (c2play.PLAY_addwnd(m_hp, wnd, C2_RENDER_OPENGL) < 0)
#endif
    {
        return -1;
    }

    return 0;
}

int
C2Player::delWnd(HWND wnd)
{
#ifdef _WIN32
    if (c2play.PLAY_delwnd(m_hp, wnd) < 0)
#elif defined UNIX
    if (c2play.PLAY_delwnd(m_hp, wnd) < 0)
#elif defined ANDROID
    if (c2play.PLAY_delwnd(m_hp, wnd) < 0)
#endif
    {
        return -1;
    }

    return 0;
}


int
C2Player::changeWndSize(int width, int height)
{
    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");

        return -1;
    }

    return c2play.PLAY_change_wndsize(m_hp, m_wnd, width, height);
}

int
C2Player::changeWndPos(int x, int y)
{
    if (!m_hp)
    {
        ERRE_SDK("Invalidate player handle.");

        return -1;
    }

    return c2play.PLAY_change_position(m_hp, m_wnd, x, y);
}

int
C2Player::startREC(string filepath)
{
    lock_guard<mutex> locker(m_wrmtx);
    if (m_wr)
    {
        delete m_wr;
        m_wr = NULL;
    }

    m_wr = new RecordWriter();
    if (!m_wr)
    {
        return -1;
    }

    filepath += ".utv";
    if (m_wr->open(filepath.c_str()) < 0)
    {
        delete m_wr;
        m_wr = NULL;
    }

    return 0;
}

void
C2Player::stopREC()
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_wr)
    {
        return;
    }

    m_wr->close();
    delete m_wr;
    m_wr = NULL;
}

void
C2Player::REC(void *frame, size_t size)
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_wr)
    {
        return;
    }

#if 0
    ut_frame_s *hdr = (ut_frame_s *)frame;
    if (m_switch && FRAME_I == hdr->FrameType)
    {
        IWriter *wr = new RecordWriter;
        if (wr)
        {
            if (wr->open(m_switchPath.c_str()) < 0)
            {
                delete wr;
                wr = NULL;
            }
            else
            {
                delete m_wr;
                m_wr = wr;
            }

            m_switch = false;
        }
    }
#endif
    
    m_wr->write(frame);
}

int
C2Player::opensound()
{
    
    return c2play.PLAY_openSound(m_hp);
}

int
C2Player::closesound()
{
    
    return c2play.PLAY_closeSound(m_hp);
}

int
C2Player::setVolume(int vol)
{
    return c2play.PLAY_setVolume(m_hp, vol);
}

void
C2Player::enableDraw(bool enable)
{
    if (enable)
    {
        c2play.PLAY_set_drawcb(m_hp, m_wnd, draw_cb, this);
    }
    else
    {
        c2play.PLAY_set_drawcb(m_hp, m_wnd, NULL, NULL);
    }
}
void
C2Player::enableDecode(bool enable)
{
    if (enable)
    {
        c2play.PLAY_set_decodecb(m_hp, decode_cb, this);
    }
    else
    {
        c2play.PLAY_set_decodecb(m_hp, NULL, NULL);
    }
}

void
C2Player::timerdriver()
{
    do
    {
        if (!m_hp)
        {
            break;
        }

        if (S_STOP == m_status || S_PAUSE == m_status)
        {
            break;
        }

        if (m_repos)
        {
            break;
        }

        if (STREAM_FILE != m_streamMode)
        {
            break;
        }

        int remain = c2play.PLAY_remain_buffer(m_hp);
        if (remain < 0)
        {
            LOGE_SDK(c2play.PLAY_get_error());
            break;
        }

        if (0 == remain)
        {
            /* end */
            notifyEnd();
        }
        else if (remain >= 50)
        {
            /* maximum threshold */
            notifyFull(remain);
        }
        else if (remain <= 10)
        {
            /* minimun threshold */
            notifyEmpty(remain);//shlshl
        }
    }
    while (0);
}
