#include "c2cap.h"
#include "icommon.h"
#include "tls_error.h"
#include "concurrency.h"
#include "cap_video_ff.h"
#include "mempool.h"

#if defined(_WIN32)
#include "cap_audio_wave.h"
#elif defined(ANDROID)
#include "cap_audio_sles.h"
#elif defined(UNIX)
#include "cap_audio_wave_alsa.h"
#endif


static bool  init = false;
static Mutex mtx;

static
void CAP_init()
{
    mempool = new Mempool(20*1024*1024);
    if (icommon::instance()->start() < 0)
    {
        ERRF_SDK("common start fail.");
        abort();
    }

    init = true;

#if _DEBUG
    DEBUG_LEVEL_CAP(LOG_LEVEL_DEBUG);
#else
    DEBUG_LEVEL_CAP(LOG_LEVEL_INFO);
#endif
}

CAP_API long
CAP_set_error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    long ret =  vsetError(fmt, arg);
    va_end(arg);

    return ret;
}

CAP_API const char *
CAP_get_error()
{
    return getError();
}

CAP_API long
CAP_enum_device(int *count, cap_device_s *devices)
{
    int cnt = 0;

    FFCapVideo::enumDevice(cnt, devices);
    *count = cnt;

    return 0;
}

CAP_API HANDLE
CAP_video_start(cap_device_s *device, HWND wnd, cap_video_s *encode, uint32_t mode, cap_video_cb video_cb, void *userdata)
{
    {
        AutoLock locker(mtx);
        if (!init)
        {
            CAP_init();
        }
    }

    FFCapVideo *capVideo = new FFCapVideo(
        encode->type,
        encode->width,
        encode->height,
        encode->gop,
        encode->fps,
        encode->bps,
        encode->mode);

    if (capVideo->start(wnd) < 0)
    {
        delete capVideo;
        return NULL;
    }

    capVideo->setVideoCB(video_cb, userdata);

    return (HANDLE)capVideo;
}

CAP_API long
CAP_video_stop(HANDLE cap)
{
    if (!cap)
    {
        ERRE_CAP("invalidate capture handle.");
        return -1;
    }

    FFCapVideo *capVideo = (FFCapVideo *)cap;

    capVideo->stop();

    delete capVideo;

    return 0;
}

CAP_API HANDLE
CAP_start_audio(cap_device_s *device, cap_audio_s *encode, uint32_t mode, cap_audio_cb audio_cb, void *userdata)
{
    {
        AutoLock locker(mtx);
        if (!init)
        {
            CAP_init();
        }
    }

#ifdef _WIN32
    WaveCapAudio *capAudio = new WaveCapAudio(mode, encode->sps, encode->channels, encode->bps, encode->ssize);
    if (capAudio->start() < 0)
    {
        delete capAudio;
        return NULL;
    }
    capAudio->setAudioCB(audio_cb, userdata);
    return (HANDLE)capAudio;
#elif defined(ANDROID)
    SlesCapAudio * capAudio = new SlesCapAudio(mode, encode->sps, encode->channels, encode->bps, encode->ssize);
    if (capAudio->start() < 0)
    {
        delete capAudio;
        return NULL;
    }
    capAudio->setAudioCB(audio_cb, userdata);
    return capAudio;
#elif defined(UNIX)
    WaveCapAudioAlsa * capAudio = new WaveCapAudioAlsa(mode, encode->sps, encode->channels, encode->bps, encode->ssize);
    if (capAudio->start() < 0)
    {
        delete capAudio;
        return NULL;
    }
    capAudio->setAudioCB(audio_cb, userdata);
    return capAudio;
#endif
}

CAP_API long CAP_stop_audio(HANDLE cap)
{
    if (!cap)
    {
        ERRE_CAP("invalidate capture handle.");
        return -1;
    }

#ifdef _WIN32
    WaveCapAudio *capAudio = (WaveCapAudio *)cap;
    capAudio->stop();
    delete capAudio;
#elif defined(ANDROID)
    SlesCapAudio *capAudio = (SlesCapAudio *)cap;
    capAudio->stop();
    delete capAudio;
#elif defined(UNIX)
    WaveCapAudioAlsa *capAudio = (WaveCapAudioAlsa *)cap;
    capAudio->stop();
    delete capAudio;
#endif

    capAudio = NULL;
    return 0;
}
