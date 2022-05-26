#include "play_stream.h"
#include "decoder_ff.h"
#include "decoder_g711a.h"
#include "decoder_g711u.h"
#include "render_sdl.h"
#include "track_sdl.h"
#include "frame.h"
#include "mempool.h"
#include "tls_error.h"
#include "bitmap.h"

#if defined _WIN32
#include "render_d3d.h"
#include "track_wave.h"
#elif defined ANDROID
#include "render_gl.h"
#include "render_gles2.h"
#include "track_sles.h"
#elif defined UNIX
#include "render_sdl.h"
#include "track_sdl.h"
#endif

StreamPlay::StreamPlay(int type, int mode)
{
    m_type             = type;
    m_mode             = mode;
    m_vdec             = NULL;
    m_adec             = NULL;
    m_track            = NULL;
    m_speed            = 0;
    m_statusCB         = NULL;
    m_statusData       = NULL;
    m_snap             = false;
    m_snapType         = 0;
    m_time             = 0;
    m_iflag            = true;

    m_minThreshold     = 25;
    m_minThresholdCB   = NULL;
    m_minThresholdData = NULL;;
    m_maxThreshold     = 200;
    m_maxThresholdCB   = NULL;
    m_maxThresholdData = NULL;;

    m_drawCB           = NULL;
    m_drawData         = NULL;
    m_decodeCB         = NULL;
    m_decodeData       = NULL;
}

StreamPlay::~StreamPlay()
{
    for (size_t i = 0; i < m_renders.size(); ++i)
    {
        delete m_renders[i];
    }
    m_renders.clear();

    if (m_vdec)
    {
        delete m_vdec;
    }

    if (m_adec)
    {
        delete m_adec;
    }

    if (m_track)
    {
        delete m_track;
    }

    AutoLock locker(m_frameQu._mtx);
    while (!m_frameQu.empty())
    {
        mempool->free(m_frameQu.front());
        m_frameQu.pop();
    }
}

int
StreamPlay::type()
{
    return m_type;
}

int
StreamPlay::addWnd(HWND wnd, int rtype)
{
    Render *rdr = NULL;

    AutoLock locker(m_renders._mtx);
    tvector<Render *>::const_iterator i;
    for (i = m_renders.begin(); i != m_renders.end(); ++i)
    {
        if ((*i)->isSame(wnd))
        {
            rdr = *i;
            break;
        }
    }

    if (i == m_renders.end())
    {
        switch (rtype)
        {
        case Render::R_D3D:
#ifdef _WIN32
            rdr = new D3DRender(wnd);
#else
            ERRE_PLY("do not suport on linux/android platform.");
            return -1;
#endif
            break;
        case Render::R_SDL:
#ifdef UNIX
            rdr = new SDLRender(wnd);
#else
            ERRE_PLY("do not suport on windows/android platform.");
            return -1;
#endif
            break;
        case Render::R_OPENGLES2:
        case Render::R_OPENGL:
#ifdef ANDROID
            rdr = new GLES2Render(wnd);
#else
            ERRE_PLY("do not suport on windows/linux platform.");
            return -1;
#endif
            break;
        default:
            ERRE_PLY("do not suport render type.");
            return -1;
            break;
        }

        m_renders.push_back(rdr);
    }

    /* rdr->setRenderListener(this); */

    if (m_vdec)
    {
        m_vdec->addDecoderLisntener(rdr);
    }

    return 0;
}

int
StreamPlay::delWnd(HWND wnd)
{
    AutoLock locker(m_renders._mtx);

    auto i = m_renders.begin();
    for (i = m_renders.begin(); i != m_renders.end(); ++i)
    {
        if ((*i)->isSame(wnd))
        {
            break;
        }
    }

    if (i == m_renders.end())
    {
        return -1;
    }

    if (m_vdec)
    {
        m_vdec->delDecoderLisntener(*i);
    }

    /* (*i)->stop(); */
    delete *i;
    m_renders.erase(i);

    return 0;
}

HWND
StreamPlay::wnd(int idx)
{
    /* AutoLock locker(m_renders._mtx); */
    /* return m_renders[idx]->wnd(); */

    return 0;
}

int
StreamPlay::changeWnnSize(HWND wnd, int width, int height)
{
    AutoLock locker(m_renders._mtx);

    tvector<Render *>::const_iterator i;
    for (i = m_renders.begin(); i != m_renders.end(); ++i)
    {
        if ((*i)->isSame(wnd))
        {
            break;
        }
    }

    if (i == m_renders.end())
    {
        return -1;
    }

    return (*i)->changeWndSize(width, height);
}

int
StreamPlay::changeWnnPos(HWND wnd, int left, int top)
{
    AutoLock locker(m_renders._mtx);

    tvector<Render *>::const_iterator i;
    for (i = m_renders.begin(); i != m_renders.end(); ++i)
    {
        if ((*i)->isSame(wnd))
        {
            break;
        }
    }

    if (i == m_renders.end())
    {
        return -1;
    }

    return (*i)->changeWndPos(left, top);
}

int
StreamPlay::inputData(void *data, int size, bool block)
{
    ut_frame_s *hdr = (ut_frame_s *)data;
    if (!block)
    {
        if (!m_vdectrd.active())
        {
            m_vdectrd.setEntry(threadProc, this, T_VDEC);
            m_vdectrd.start();
        }

        /* if (!m_adectrd.active()) */
        /* { */
        /*     m_adectrd.setEntry(threadProc, this, T_ADEC); */
        /*     m_adectrd.start(); */
        /* } */

        AutoLock locker(m_frameQu._mtx);
        int fqcnt = m_frameQu.size();
        if (fqcnt >= m_maxThreshold)
        {
            if (m_maxThresholdCB != NULL)
            {
                m_maxThresholdCB(1, fqcnt, m_maxThresholdData);
            }
           
            LOGW_PLY("real buff queue is too large!!!");
            while (m_frameQu.size() > 0)
            {
                mempool->free(m_frameQu.front());
                m_frameQu.pop();
            }
        }
        else if (fqcnt < m_minThreshold)
        {
            if (m_minThresholdCB != NULL)
            {
                m_minThresholdCB(0, fqcnt, m_minThresholdData);
            }
        }

        uint8_t *buf = (uint8_t *)mempool->alloc(size);
        if (!buf)
        {
            ERRE_PLY("mempool->alloc err! %d", size);
            return -1;
        }
        memcpy(buf, data, size);
        m_frameQu.push(buf);
    }
    else
    {
        if (m_iflag)
        {
            if ('I' != hdr->type)
            {
                return 0;
            }

            m_iflag  = false;
        }

        int      size = 0;
        int      w    = 0;
        int      h    = 0;
        uint64_t pts  = 0;

        if (ENCODE_H264 == hdr->encode ||
                ENCODE_HEVC == hdr->encode)
        {
            if (setVCodec(hdr->encode, hdr->video.width, hdr->video.height, hdr->video.fps))
            {
                return -1;
            }

            if (!m_vdec)
            {
                return -1;
            }

            return m_vdec->decode(hdr->buf, hdr->size, NULL, size, w, h, pts);
        }
        else if ('A' == hdr->type)
        {
            if (setACodec(hdr->encode, hdr->audio.sps, hdr->audio.channels, hdr->audio.bps))
            {
                return -1;
            }

            if (!m_adec)
            {
                return -1;
            }

            return m_adec->decode(hdr->buf, hdr->size, NULL, size);
        }
    }

    return 0;
}

int
StreamPlay::start(c2_status_cb statusCB, void *userData)
{
    m_statusCB   = statusCB;
    m_statusData = userData;

    return 0;
}

int
StreamPlay::stop()
{
    m_vdectrd.stop();
    m_adectrd.stop();

    return 0;
}

int
StreamPlay::pause(bool pause)
{
    return -1;
}

int
StreamPlay::fast(int &speed)
{
    return -1;
}

int
StreamPlay::slow(int &speed)
{
    return -1;
}

int
StreamPlay::capture(const string path, int type)
{
    m_snapType = type;
    tevent<string, struct snap_s> event;

    struct snap_s snap;

    if (0 == type)
    {
        event.init("snap-rgb", snap);
    }
    else
    {
        event.init("snap-jpg", snap);
    }

    m_snapQu.add(event);
    m_snap = true;

    if (!event.wait(5000))
    {
        ERRE_PLY("capture timeout!");
        return -1;
    }

    if (!snap.buf || snap.size <= 0)
    {
        ERRE_PLY("capture err!");
        return -1;
    }

    /* dsize = w * h * 3 + sizeof(bmp_file_hdr_s) + sizeof(bmp_info_s); */
    bmp_file_hdr_s bmpFileHdr;
    memset(&bmpFileHdr, 0, sizeof(bmpFileHdr));

    /* MB */
    bmpFileHdr.type    = 0x4D42;
    bmpFileHdr.size    = snap.size;
    bmpFileHdr.offBits = sizeof(bmp_file_hdr_s) + sizeof(bmp_info_hdr_s);

//    bmp_info_s bmpInfo;
//    memset(&bmpInfo, 0, sizeof(bmpFileHdr));
//    bmpInfo.header.size          = sizeof(bmp_info_hdr_s);
//    bmpInfo.header.planes        = 1;
//    bmpInfo.header.bitcount      = 24;
//    bmpInfo.header.compression   = 0;
//    bmpInfo.header.width         = snap.width;
//    bmpInfo.header.height        = snap.height;
//    bmpInfo.header.sizeImage     = snap.width * snap.height * 3;
//    bmpInfo.header.xPelsPerMeter = 0;
//    bmpInfo.header.yPelsPerMeter = 0;

    bmp_info_hdr_s bmpInfoHdr;
    memset(&bmpInfoHdr, 0, sizeof(bmp_info_hdr_s));
    bmpInfoHdr.size = sizeof(bmp_info_hdr_s);
    bmpInfoHdr.planes = 1;
    bmpInfoHdr.bitcount = 24;
    bmpInfoHdr.compression = 0;
    bmpInfoHdr.width = snap.width;
    bmpInfoHdr.height = snap.height;
    bmpInfoHdr.sizeImage = snap.width * snap.height * 3;
    bmpInfoHdr.xPelsPerMeter = 0;
    bmpInfoHdr.yPelsPerMeter = 0;

    FILE *file = fopen(path.c_str(), "wb");
    if (!file)
    {
        ERRE_PLY("create file %s fail!", path.c_str());
        return -1;
    }

    fwrite(&bmpFileHdr, sizeof(bmp_file_hdr_s), 1, file);
    fwrite(&bmpInfoHdr, sizeof(bmp_info_hdr_s), 1, file);
    fwrite(snap.buf, snap.size, 1, file);
    fflush(file);
    fclose(file);
    file = NULL;

    mempool->free(snap.buf);

    return 0;
}

int
StreamPlay::captureRGB(void *buf, int &size, int &w, int &h)
{
    m_snapType = 0;
    tevent<string, struct snap_s> event;

    struct snap_s snap;
    event.init("snap-rgb", snap);

    m_snapQu.add(event);
    m_snap = true;

    if (!event.wait(5000))
    {
        ERRE_PLY("capture timeout!");
        return -1;
    }

    if (!snap.buf || snap.size <= 0)
    {
        ERRE_PLY("capture err!");
        return -1;
    }

    if (size < (int)snap.size)
    {
        ERRE_PLY("user data Insufficient: %d", size);
        return -1;
    }

    memcpy(buf, snap.buf, snap.size);
    size = snap.size;
    w    = snap.width;
    h    = snap.height;

    mempool->free(snap.buf);

    return 0;
}

int
StreamPlay::captureYUV(void *buf, int &size, int &w, int &h)
{
    m_snapType = 2;
    tevent<string, struct snap_s> event;

    struct snap_s snap;
    event.init("snap-yuv", snap);

    m_snapQu.add(event);
    m_snap = true;

    if (!event.wait(5000))
    {
        ERRE_PLY("capture timeout!");
        return -1;
    }

    if (!snap.buf || snap.size <= 0)
    {
        ERRE_PLY("capture err!");
        return -1;
    }

    if (size < (int)snap.size)
    {
        ERRE_PLY("user data Insufficient: %d", size);
        return -1;
    }

    memcpy(buf, snap.buf, snap.size);
    size = snap.size;
    w    = snap.width;
    h    = snap.height;

    mempool->free(snap.buf);

    return 0;
}

int
StreamPlay::openSound()
{
    if (!m_track)
    {
        return -1;
    }

    return m_track->openSound();
}


void
StreamPlay::closeSound()
{
    if (!m_track)
    {
        return;
    }
    m_track->closeSound();
}


int
StreamPlay::setVolume(int vol)
{
    if (!m_track)
    {
        return -1;
    }
    return m_track->setVolume(vol);
}

void
StreamPlay::onYUV(void *data, int size, int w, int h)
{
    do
    {
        if (!m_vdec)
        {
            break;
        }

        if (m_snap)
        {
            struct snap_s snap;
            snap.width  = w;
            snap.height = h;

            if (0 == m_snapType)
            {
                int       dsize = w * h * 3;
                uint8_t * dest  = (uint8_t *)mempool->alloc(dsize);

                m_vdec->convert(VideoDecoder::FMT_YV12, VideoDecoder::FMT_RGB24, data, size, dest, dsize, w, h);
                snap.buf  = dest;
                snap.size = dsize;

                m_snapQu.response("snap-rgb", snap);
            }
            else if (1 == m_snapType)
            {
                m_snapQu.response("snap-jpg", snap);
            }
            else
            {
                uint8_t * yuv = (uint8_t *)mempool->alloc(size);
                memcpy(yuv, data, size);

                snap.buf  = yuv;
                snap.size = size;

                m_snapQu.response("snap-yuv", snap);
            }

            m_snap = false;
        }
    }
    while (0);

    if (m_decodeCB)
    {
        m_decodeCB(0, w, h, data, size, m_decodeData);
    }
}

void
StreamPlay::onRGB(void *data, int size, int w, int h)
{
    if (m_decodeCB)
    {
        m_decodeCB(2, w, h, data, size, m_decodeData);
    }
}

void
StreamPlay::onHDC(HDC hdc, HWND wnd, int w, int h)
{
    if (m_drawCB)
    {
        m_drawCB(hdc, wnd, w, h, m_drawData);
    }
}

void
StreamPlay::onADPCM(void *data, int size)
{

}

int
StreamPlay::remainFrame()
{
    AutoLock locker(m_frameQu._mtx);
    return m_frameQu.size();
}

int
StreamPlay::resetFrame()
{
    AutoLock locker(m_frameQu._mtx);
    while (!m_frameQu.empty())
    {
        mempool->free(m_frameQu.front());
        m_frameQu.pop();
    }
    /* m_frameQu.clear(); */
    m_iflag = true;

    return 0;
}

void
StreamPlay::setMinThreshold(uint32_t min, c2_threshold_cb threshold_cb, void *userdata)
{
    if (min > 100)
    {
        min = 100;
    }

    m_minThreshold     = min;
    m_minThresholdCB   = threshold_cb;
    m_minThresholdData = userdata;
}

void
StreamPlay::setMaxThreshold(uint32_t max, c2_threshold_cb threshold_cb, void *userdata)
{
    if (max > 5000)
    {
        max = 5000;
    }

    if (max < 100)
    {
        max = 100;
    }

    m_maxThreshold     = max;
    m_maxThresholdCB   = threshold_cb;
    m_maxThresholdData = userdata;
}

int
StreamPlay::currentTime(datetime_s &time)
{
    time = u64Time2DataTime(m_time);
    return 0;
}

datetime_s         
StreamPlay::u64Time2DataTime(uint64_t ti)
{
    time_t t = (time_t)(ti / 1000 + 0.5);
    struct tm ttm;

    datetime_s time = {0};

#ifdef _WIN32
    if (localtime_s(&ttm, &t) != 0)
#else
    if (!localtime_r(&t, &ttm))
#endif
    {
        return time;
    }

    time.year   = ttm.tm_year + 1900;
    time.month  = ttm.tm_mon  + 1;
    time.day    = ttm.tm_mday;
    time.hour   = ttm.tm_hour;
    time.minute = ttm.tm_min;
    time.second = ttm.tm_sec;
    return time;
}

void
StreamPlay::setDrawCB(HWND wnd, c2_draw_cb draw_cb, void *userdata)
{
    Render *rdr = NULL;

    AutoLock locker(m_renders._mtx);
    tvector<Render *>::const_iterator i;
    for (i = m_renders.begin(); i != m_renders.end(); ++i)
    {
        if ((*i)->isSame(wnd))
        {
            rdr = *i;
            break;
        }
    }

    if (i == m_renders.end())
    {
        LOGW_PLY("not find the wnd.");
        return;
    }

    if (!draw_cb)
    {
        rdr->setRenderListener(NULL);
    }
    else
    {
        rdr->setRenderListener(this);
    }

    m_drawCB   = draw_cb;
    m_drawData = userdata;
}

void
StreamPlay::setDecodeCB(c2_decode_cb decode_cb, void *userdata)
{
    m_decodeCB   = decode_cb;
    m_decodeData = userdata;
}

void
StreamPlay::vdecloop()
{
    while (m_vdectrd.active())
    {
        size_t fsize = 0;
        {
            AutoLock locker(m_frameQu._mtx);
            fsize = m_frameQu.size();
        }

        if (fsize <= 0)
        {
            sys->sleep(5);

            continue;
        }

        AutoLock locker(m_frameQu._mtx);
        do
        {
            uint8_t    * buf = m_frameQu.front();
            ut_frame_s * hdr = (ut_frame_s *)buf;

            if (m_iflag)
            {
                if ('I' != hdr->type)
                {
                    break;
                }

                m_iflag  = false;
            }

            int      size = 0;
            int      w    = 0;
            int      h    = 0;
            uint64_t pts  = 0;


            if (ENCODE_H264 == hdr->encode ||
                    ENCODE_HEVC == hdr->encode)
            {
                if (setVCodec(hdr->encode, hdr->video.width, hdr->video.height, hdr->video.fps) < 0)
                {
                    break;
                }

                if (!m_vdec)
                {
                    break;
                }

                m_vdec->decode(hdr->buf, hdr->size, NULL, size, w, h, pts);
            }
            else if ('A' == hdr->type)
            {
                if (setACodec(hdr->encode, hdr->audio.sps, hdr->audio.channels, hdr->audio.bps) < 0)
                {
                    break;
                }

                if (!m_adec)
                {
                    break;
                }

                m_adec->decode(hdr->buf, hdr->size, NULL, size);
            }

            m_time = hdr->pts;
        }
        while (0);

        mempool->free(m_frameQu.front());
        m_frameQu.pop();
    }
}

void
StreamPlay::adecloop()
{
    while (m_adectrd.active())
    {
        sys->sleep(40);
    }
}

int
StreamPlay::setVCodec(uint8_t encode, uint32_t w, uint32_t h, uint32_t fps)
{
    if (!m_vdec)
    {
        int codec = VideoDecoder::CODEC_H264;
        if (ENCODE_H264 == encode)
        {
            codec = VideoDecoder::CODEC_H264;
        }
        else if (ENCODE_HEVC == encode)
        {
            codec = VideoDecoder::CODEC_HEVC;
        }

        m_vdec = new FFVideoDecoder();
        if (m_vdec->init(codec, VideoDecoder::FMT_YUV420P, w, h, fps) < 0)
        {
            delete m_vdec;
            return -1;
        }

        m_vdec->addDecoderLisntener(this);

        AutoLock locker(m_renders._mtx);
        for (size_t i = 0; i < m_renders.size(); ++i)
        {
            m_vdec->addDecoderLisntener(m_renders[i]);
        }
    }
    else
    {
        bool reset = false;
        int  codec = VideoDecoder::CODEC_H264;
        if ((ENCODE_H264 == encode) && (VideoDecoder::CODEC_H264 != m_vdec->codec()))
        {
            codec = VideoDecoder::CODEC_H264;
            reset = true;
        }
        else if ((ENCODE_HEVC == encode) && (VideoDecoder::CODEC_HEVC != m_vdec->codec()))
        {
            codec = VideoDecoder::CODEC_HEVC;
            reset = true;
        }

        if (!reset)
        {
            return 0;
        }

        delete m_vdec;

        m_vdec = new FFVideoDecoder();
        if (m_vdec->init(codec, VideoDecoder::FMT_YUV420P, w, h, fps) < 0)
        {
            delete m_vdec;
            return -1;
        }

        m_vdec->addDecoderLisntener(this);

        AutoLock locker(m_renders._mtx);
        for (size_t i = 0; i < m_renders.size(); ++i)
        {
            m_vdec->addDecoderLisntener(m_renders[i]);
        }

        reset = false;
    }

    return 0;
}

int
StreamPlay::setACodec(uint8_t encode, uint32_t sps, uint8_t channels, uint8_t bps)
{
    if (!m_adec)
    {
#if 1
        if (ENCODE_G711A == encode)
        {
            m_adec = new G711ADecoder();
        }
        else if (ENCODE_G711U == encode)
        {
            m_adec = new G711UDecoder();
        }
        else
        {
            m_adec = new G711ADecoder();
        }
#else
        int codec = AudioDecoder::CODEC_G711A;
        if (ENCODE_G711A == encode)
        {
            codec = AudioDecoder::CODEC_G711A;
        }
        else if (ENCODE_G711U == encode)
        {
            codec = AudioDecoder::CODEC_G711U;
        }

        m_adec = new FFAudioDecoder();
        if (m_adec->init(codec, sps, bps, channels) < 0)
        {
            delete m_adec;
            ERRE_PLY("%s", getError());
            return -1;
        }
#endif
        m_adec->addDecoderLisntener(this);

        if (!m_track)
        {
#if defined(_WIN32)
            m_track = new WaveTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(ANDROID)
            m_track = new SlesTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(UNIX)
            m_track = new SDLTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#endif
        }
    }
    else
    {
        if ((ENCODE_G711A == encode) && (AudioDecoder::CODEC_G711A != m_adec->codec()))
        {
            delete m_adec;
            delete m_track;

#if 1
            m_adec = new G711ADecoder();
#else
            m_adec = new FFAudioDecoder();
            if (m_adec->init(AudioDecoder::CODEC_G711A, sps, bps, channels) < 0)
            {
                delete m_adec;
                ERRE_PLY("%s", getError());
                return -1;
            }
#endif


#if defined(_WIN32)
            m_track = new WaveTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(ANDROID)
            m_track = new SlesTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(UNIX)
            m_track = new SDLTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#endif
            m_adec->addDecoderLisntener(this);
            
        }
        else if ((ENCODE_G711U == encode) && (AudioDecoder::CODEC_G711U != m_adec->codec()))
        {
            delete m_adec;
            delete m_track;
#if 1
            m_adec = new G711UDecoder();
#else
            m_adec = new FFAudioDecoder();
            if (m_adec->init(AudioDecoder::CODEC_G711U, sps, bps, channels) < 0)
            {
                delete m_adec;
                ERRE_PLY("%s", getError());
                return -1;
            }
#endif

#if defined(_WIN32)
            m_track = new WaveTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(ANDROID)
            m_track = new SlesTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#elif defined(UNIX)
            m_track = new SDLTrack(sps, channels, bps);
            m_adec->addDecoderLisntener(m_track);
#endif
            m_adec->addDecoderLisntener(this);   
        }
    }

    return 0;
}

void *
StreamPlay::threadProc(ThreadInfo *info)
{
    StreamPlay *sp = (StreamPlay*)(info->data);

    switch (info->id)
    {
    case T_VDEC:
        sp->vdecloop();
        break;

    case T_ADEC:
        sp->adecloop();
        break;

    default:
        break;
    }

    return NULL;
}
