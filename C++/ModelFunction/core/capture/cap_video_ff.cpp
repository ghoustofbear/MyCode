#include "cap_video_ff.h"
#include "encoder_x264.h"
#include "encoder_x265.h"
#include "tls_error.h"
#include "sys.h"

#ifdef WIN32
#include "render_d3d.h"
#elif defined UNIX
#include "render_sdl.h"
#elif defined ANDROID
#include "render.h"
#endif

class FFAVFormtInitializer
{
public:
    FFAVFormtInitializer()
    {
        av_register_all();
        avformat_network_init();
        avdevice_register_all();
    }
};

static FFAVFormtInitializer ffCodecInitializer;
FFCapVideo::FFCapVideo(int type, int width, int height, int gop, int fps, int bps, int mode)
    : CapVideo(type, width, height, gop, fps, bps, mode)
{
    m_videoIdx = -1;
    m_codecCtx = NULL;
    m_fmtCtx   = NULL;
    m_yuv      = NULL;
    m_render   = NULL;

    m_encoder  = NULL;
    m_frame    = NULL;
}

FFCapVideo::~FFCapVideo()
{

}

int
FFCapVideo::start(HWND wnd)
{
    AVCodec	*codec = NULL;
    m_fmtCtx = avformat_alloc_context();
    if (NULL == m_fmtCtx)
    {
        ERRE_PLY("avformat_alloc_context fail.");
        return -1;
    }

#ifdef _WIN32
#if USE_DSHOW
    AVInputFormat *ifmt = av_find_input_format("dshow");

    if (avformat_open_input(&m_fmtCtx, "video=USB Camera", ifmt, NULL) != 0)
    {
        ERRE_PLY("Couldn't open input stream.");
        goto FAIL0;
    }
#else
    AVInputFormat *ifmt = av_find_input_format("vfwcap");
    if (avformat_open_input(&m_fmtCtx, "0", ifmt, NULL) != 0)
    {
        ERRE_PLY("Couldn't open input stream.");
        goto FAIL0;
    }
#endif
#elif defined(__linux__)
    AVInputFormat *ifmt = av_find_input_format("video4linux2");
    if (avformat_open_input(&m_fmtCtx, "/dev/video0", ifmt, NULL) != 0)
    {
        ERRE_PLY("Couldn't open input stream.");
        goto FAIL0;
    }
#else
    AVInputFormat *ifmt = av_find_input_format("avfoundation");
    if (avformat_open_input(&m_fmtCtx, "0", ifmt, NULL) != 0)
    {
        ERRE_PLY("Couldn't open input stream.");
        goto FAIL0;
    }
#endif

    m_fmtCtx->probesize = 2048;
    m_fmtCtx->max_analyze_duration = 1000;

    if (avformat_find_stream_info(m_fmtCtx, NULL) < 0)
    {
        ERRE_PLY("Couldn't find stream information.");
        goto FAIL1;
    }

    for (uint32_t i = 0; i < m_fmtCtx->nb_streams; i++)
    {
        if (m_fmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoIdx = i;
            break;
        }
    }

    if (m_videoIdx == -1)
    {
        ERRE_PLY("Couldn't find a video stream.");
        goto FAIL1;
    }

    m_codecCtx = m_fmtCtx->streams[m_videoIdx]->codec;
    codec = avcodec_find_decoder(m_codecCtx->codec_id);
    if (codec == NULL)
    {
        ERRE_PLY("Codec not found.");
        goto FAIL1;
    }

    if (avcodec_open2(m_codecCtx, codec, NULL) < 0)
    {
        ERRE_PLY("Could not open codec.");
        goto FAIL1;
    }

    m_vsize = 3 * m_codecCtx->width * m_codecCtx->height;
    m_vbuf  = (uint8_t *)malloc(m_vsize);
    if (!m_vbuf)
    {
        ERRE_PLY("malloc fail.");
        goto FAIL2;
    }

#ifdef _WIN32
    m_render = new D3DRender(wnd);
#elif defined UNIX
    m_render = new SDLRender(wnd);
#endif

    m_captrd.setEntry(threadProc, this, 0);
    m_captrd.start();

    return 0;

FAIL2:
    avcodec_close(m_codecCtx);
    m_codecCtx = NULL;

FAIL1:
    avformat_close_input(&m_fmtCtx);

FAIL0:
    avformat_free_context(m_fmtCtx);
    m_fmtCtx = NULL;

    return -1;
}

void
FFCapVideo::stop()
{
    m_captrd.stop();

    if (m_render)
    {
        delete m_render;
        m_render = NULL;
    }

    if (m_codecCtx)
    {
        avcodec_close(m_codecCtx);
        m_codecCtx = NULL;
    }

    if (m_fmtCtx)
    {
        avformat_close_input(&m_fmtCtx);
        avformat_free_context(m_fmtCtx);
        m_fmtCtx = NULL;
    }

    if (m_vbuf)
    {
        free(m_vbuf);
        m_vbuf = NULL;
    }
}

void
FFCapVideo::caploop()
{
    m_yuv = av_frame_alloc();

    avpicture_fill(&m_yuv420, NULL, AV_PIX_FMT_YUV420P, m_codecCtx->width, m_codecCtx->height);
    avpicture_alloc(&m_yuv420, AV_PIX_FMT_YUV420P, m_codecCtx->width, m_codecCtx->height);

    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    int ret, got_picture;

    SwsContext * swsctx = sws_getContext(
                              m_codecCtx->width,
                              m_codecCtx->height,
                              m_codecCtx->pix_fmt,
                              m_codecCtx->width,
                              m_codecCtx->height,
                              AV_PIX_FMT_YUV420P,
                              SWS_BICUBIC, NULL, NULL, NULL);

    while (m_captrd.active())
    {

        if (av_read_frame(m_fmtCtx, packet) >= 0)
        {
            if (packet->stream_index == m_videoIdx)
            {
                ret = avcodec_decode_video2(m_codecCtx, m_yuv, &got_picture, packet);
                if (ret < 0)
                {
                    LOGE_PLY("Decode Error.");
                    break;
                }

                if (got_picture)
                {
                    sws_scale(swsctx, (const unsigned char* const*)m_yuv->data, m_yuv->linesize, 0, m_codecCtx->height, m_yuv420.data, m_yuv420.linesize);

                    if (!m_vbuf)
                    {
                        LOGE_PLY("No buffer.");
                        continue;
                    }

                    uint8_t *yuv420 = m_vbuf;
                    memcpy(yuv420, m_yuv420.data[0], m_yuv420.linesize[0] * m_codecCtx->height);
                    if (m_yuv420.data[1])
                    {
                        yuv420 += m_yuv420.linesize[0] * m_codecCtx->height;
                        memcpy(yuv420, m_yuv420.data[1], m_yuv420.linesize[1] * m_codecCtx->height / 2);
                    }

                    if (m_yuv420.data[2])
                    {
                        yuv420 += m_yuv420.linesize[1] * m_codecCtx->height / 2;
                        memcpy(yuv420, m_yuv420.data[2], m_yuv420.linesize[2] * m_codecCtx->height / 2);
                    }

                    m_render->onYUV(m_vbuf, m_vsize, m_codecCtx->width, m_codecCtx->height);

                    if (!m_encoder)
                    {
                        if (ENC_H264 == m_type)
                        {
                            m_encoder = new X264Encoder();
                        }
                        else
                        {
                            m_encoder = new X265Encoder();
                        }

                        m_encoder->init(VideoEncoder::FMT_YUV420P, m_codecCtx->width, m_codecCtx->height, m_gop, m_fps, m_bps, m_mode);

                        m_frame = (uint8_t *)malloc(512 * 1024);
                    }

                    if (m_encoder && m_frame)
                    {
                        int size = 512 * 1024;
                        uint64_t pts;
                        m_encoder->encode(m_vbuf, m_vsize, m_frame, size, pts);

                        int type = (ENC_H264 == m_type) ? 2 : 3;
                        if (m_videoCB)
                        {
                            m_videoCB(type, m_frame, size, sys->currentTimeMillis(), m_fps, m_videoData);
                        }
                    }
                }
            }

            av_free_packet(packet);
        }
    }

    if (m_encoder)
    {
        delete m_encoder;
        m_encoder = NULL;
    }

    if (m_frame)
    {
        free(m_frame);
        m_frame = NULL;
    }

    sws_freeContext(swsctx);
    av_free(m_yuv);
}

int
FFCapVideo::enumDevice(int &count, cap_device_s *devices)
{
    AVFormatContext *m_fmtCtx = avformat_alloc_context();

    AVDictionary* options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    avformat_open_input(&m_fmtCtx, "video=dummy", iformat, &options);

    /* AVDeviceInfoList *device_list = NULL; */
    /* avdevice_list_input_sources(iformat, "dshow", options, &device_list); */
    /* m_fmtCtx->iformat = iformat; */
    /* avdevice_list_devices(m_fmtCtx, &device_list); */

    return 0;
}
