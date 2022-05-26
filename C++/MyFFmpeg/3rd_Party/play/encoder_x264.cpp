#include "encoder_x264.h"
#include "tls_error.h"
#include "sys.h"

extern "C"
{
#include "x264.h"
};

X264Encoder::X264Encoder()
{
    m_param = NULL;
    m_enc   = NULL;
    m_pic   = NULL;
    m_seq   = 0;
}

X264Encoder::~X264Encoder()
{
    m_init = false;

    if (m_pic)
    {
        /**
         * void x264_picture_clean( x264_picture_t *pic )
         * {
         *     x264_free( pic->img.plane[0] );
         *
         *   memset( pic, 0, sizeof( x264_picture_t ) );
         *   }
         */

        /* x264_picture_clean(m_pic); */
        delete m_pic;
        m_pic = NULL;
    }

    if (m_param)
    {
        delete m_param;
        m_param = NULL;
    }

    x264_encoder_close(m_enc);
    m_enc = NULL;
}

int
X264Encoder::init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode)
{
    if (VideoEncoder::init(avfmt, width, height, gop, fps, bitrate, mode) < 0)
    {
        return -1;
    }

    int csp = X264_CSP_I420;
    switch (avfmt)
    {
    case FMT_YUV420P:
        csp = X264_CSP_I420;
        break;

    case FMT_YUV444:
        csp = X264_CSP_I444;
        break;

    case FMT_YV12:
        csp = X264_CSP_I420;
        break;

    case FMT_NV12:
        csp = X264_CSP_NV12;
        break;

    case FMT_RGB24:
        csp = X264_CSP_RGB;
        break;

    case FMT_BGR24:
        csp = X264_CSP_BGR;
        break;

    case FMT_BGRA:
        csp = X264_CSP_BGR;
        break;

    default:
        csp = X264_CSP_I420;
        break;
    }

    m_param = new x264_param_t;
    x264_param_default(m_param);

    /* fps */
    m_param->i_fps_num                  = fps;
    m_param->i_fps_den                  = 1;

    /* res */
    m_param->i_csp                      = csp;
    m_param->i_width                    = width;
    m_param->i_height                   = height;

    /* bitrate */
    m_param->rc.i_bitrate               = bitrate;
    m_param->rc.i_rc_method             = X264_RC_ABR;

    /* GOP */
    m_param->i_keyint_min               = gop;
    m_param->i_keyint_max               = gop;

    /* preset: ultrafast */
    m_param->i_frame_reference          = 1;
    m_param->i_scenecut_threshold       = 0;
    m_param->b_deblocking_filter        = 0;
    m_param->b_cabac                    = 0;
    m_param->i_bframe                   = 0;
    m_param->analyse.intra              = 0;
    m_param->analyse.inter              = 0;
    m_param->analyse.b_transform_8x8    = 0;
    m_param->analyse.i_me_method        = X264_ME_DIA;
    m_param->analyse.i_subpel_refine    = 0;
    m_param->rc.i_aq_mode               = 0;
    m_param->analyse.b_mixed_references = 0;
    m_param->analyse.i_trellis          = 0;
    m_param->i_bframe_adaptive          = X264_B_ADAPT_NONE;
    m_param->rc.b_mb_tree               = 0;

#if 0
    /* preset: superfast */
    m_param->analyse.inter              = X264_ANALYSE_I8x8 | X264_ANALYSE_I4x4;
    m_param->analyse.i_me_method        = X264_ME_DIA;
    m_param->analyse.i_subpel_refine    = 1;
    m_param->i_frame_reference          = 1;
    m_param->analyse.b_mixed_references = 0;
    m_param->analyse.i_trellis          = 0;
    m_param->rc.b_mb_tree               = 0;
    m_param->analyse.i_weighted_pred    = X264_WEIGHTP_SIMPLE;
    m_param->rc.i_lookahead             = 0;
#endif

#if 0
    /* tune: zerolatency */
    m_param->rc.i_lookahead             = 0;
    m_param->i_sync_lookahead           = 0;
    m_param->i_bframe                   = 0;
    m_param->b_sliced_threads           = 1;
    m_param->b_vfr_input                = 0;
    m_param->rc.b_mb_tree               = 0;
#endif

    /* tune: ssim */
    m_param->rc.i_aq_mode               = X264_AQ_AUTOVARIANCE;
    m_param->analyse.b_psy              = 0;


    m_enc = x264_encoder_open(m_param);
    if (m_enc == NULL)
    {
        ERRE_PLY("x264_encoder_open err");
        delete m_param;
        return -1;
    }

    /*
     * Automatically reduce reference frame count to match the user's target level
     * if the user didn't explicitly set a reference frame count.
     */
    int mbs = (((m_param->i_width) + 15) >> 4) * (((m_param->i_height) + 15) >> 4);
    int i;
    for (i = 0; x264_levels[i].level_idc != 0; i++)
    {
        if (m_param->i_level_idc == x264_levels[i].level_idc)
        {
            while (mbs * 384 * m_param->i_frame_reference > (int)x264_levels[i].dpb
                    && m_param->i_frame_reference > 1)
            {
                m_param->i_frame_reference--;
            }
            break;
        }
    }

    m_enc = x264_encoder_open(m_param);
    if (!m_enc)
    {
        ERRE_PLY("x264_encoder_open fail.");
        delete m_param;
        return -1;
    }

    m_pic = new x264_picture_t;
    if (x264_picture_alloc(m_pic, csp, m_param->i_width, m_param->i_height) < 0)
    {
        ERRE_PLY("x264_picture_alloc fail.");

        delete m_param;
        delete m_pic;
        return -1;
    }

    m_init = true;

    return 0;
}

int
X264Encoder::encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts)
{
    if (!m_init)
    {
        ERRE_PLY("x264 encoder is not inited");
        return -1;
    }

    if (!src)
    {
        ERRE_PLY("invlidate source data");
        return -1;
    }

    int w = m_param->i_width;
    int h = m_param->i_height;

    if (X264_CSP_I420 == m_param->i_csp)
    {
        uint8_t * buf = (uint8_t *)src;

        m_pic->img.plane[0] = buf;
        m_pic->img.plane[1] = buf + w * h;
        m_pic->img.plane[2] = buf + w * h * 5 / 4;
    }
    else if (X264_CSP_I444 == m_param->i_csp)
    {
        uint8_t * buf = (uint8_t *)src;

        m_pic->img.plane[0] = buf;
        m_pic->img.plane[1] = buf + w * h;
        m_pic->img.plane[2] = buf + w * h * 2;
    }

	m_pic->i_pts     = (int64_t)m_seq* m_param->i_fps_den;
	m_pic->i_type    = X264_TYPE_AUTO;
	m_pic->i_qpplus1 = 0;

    x264_nal_t * nals    = NULL;
    int          i_nal   = 0;
    int          i_total = 0;

    x264_picture_t out;

    if (x264_encoder_encode(m_enc, &nals, &i_nal, m_pic, &out) < 0)
    {
        ERRE_PLY("x264_encoder_encode err");

        return -1;
    }

    int offset = 0;
    for (int i = 0; i < i_nal; i++)
    {
        i_total += nals[i].i_payload;
        if (dsize < (int)i_total)
        {
            ERRE_PLY("buffer is too small.");

            return -1;
        }

        memcpy((uint8_t *)dest + offset, nals[i].p_payload, nals[i].i_payload);
        offset += nals[i].i_payload;
    }

    dsize = i_total;
    m_seq++;

    pts = sys->currentTimeMillis();

    return 0;
}
