#include "play_nfstream.h"
#include "tls_error.h"
#include "decoder_ff.h"
#include "render_sdl.h"
#include "frame.h"
#include "mempool.h"
NFStreamPlay::NFStreamPlay(int mode)
    : StreamPlay(STREAM_FILE_NET, mode)
{
    m_refpts  = 0;
    m_reftime = 0;
    m_pause   = false;;
    m_speed   = 0;
    m_bufsize = 0;
}

NFStreamPlay::~NFStreamPlay()
{

}

int
NFStreamPlay::inputData(void *data, int size, bool block)
{
    ut_frame_s *hdr = (ut_frame_s *)data;

    if (ENCODE_H264 == hdr->encode ||
        ENCODE_HEVC == hdr->encode)
    {
        if (!m_vdectrd.active())
        {
            m_vdectrd.setEntry(threadProc, this, T_VDEC);
            m_vdectrd.start();
        }

        AutoLock locker(m_frameQu._mtx);
        //if (m_frameQu.size() > MAX_FRAMEQU_SIZE)
        //{
        //    LOGW_PLY("real buff queue is too large!!!");
        //    while ((int)m_frameQu.size() >= m_minThreshold)
        //    {
        //        mempool->free(m_frameQu.front());
        //        m_frameQu.pop();
        //    }
        //}

        uint8_t *buf = (uint8_t *)mempool->alloc(size);
        if (!buf)
        {
            ERRE_PLY("mempool->alloc err! %d", size);
            return -1;
        }

        memcpy(buf, data, size);
        m_frameQu.push(buf);

        m_bufsize += size;
        if (m_bufsize >= MAX_BUFFER_SIZE)
        {
            if (m_maxThresholdCB)
            {
                m_maxThresholdCB(1, 0, m_maxThresholdData);
            }
        }
    }
    else if (ENCODE_G711A == hdr->encode ||
             ENCODE_G711U == hdr->encode)
    {
        /* LOGI_PLY("frame type: %c", hdr->type); */
        if (!m_adec)
        {
            /* TODO:  <26-01-18, yourname> */
            /* if (!m_adectrd.active()) */
            /* { */
            /*     m_adectrd.setEntry(threadProc, this, T_ADEC); */
            /*     m_adectrd.start(); */
            /* } */
        }
    }

    return 0;
}

int
NFStreamPlay::pause(bool pause)
{
    m_pause = pause;

    return 0;
}

int
NFStreamPlay::fast(int &speed)
{
    if (m_speed++ > 4)
    {
        m_speed = 4;
    }

    speed = m_speed;

    return 0;
}

int
NFStreamPlay::slow(int &speed)
{
    if (m_speed-- < -4)
    {
        m_speed = -4;
    }

    speed = m_speed;

    return 0;
}

void
NFStreamPlay::delay(uint64_t pts)
{
#if 0
    {
        AutoLock locker(m_frameQu._mtx);
        if ((int)m_frameQu.size() < m_minThreshold)
        {
            if (m_minThresholdCB)
            {
                m_minThresholdCB(MIN_THRESHOLD, m_frameQu.size(), m_minThresholdData);
            }
        }
    }
#endif

    if (0 == m_refpts)
    {
        m_refpts  = pts;
        m_reftime = sys->currentTimeMillis();
    }
    else
    {
        uint64_t current = sys->currentTimeMillis();
        int64_t d       = (int64_t)((pts - m_refpts) * pow(2, -m_speed)) - (current - m_reftime);

        if (d < 0)
        {
            m_refpts = 0;
            d = 0;
        }

        if (d > 500)
        {
            m_refpts = 0;
            d = 500;
        }

        LOGT_PLY("media player d: %d", d);
        sys->sleep((uint32_t)d);
    }
}

void
NFStreamPlay::delay(uint32_t fps)
{
    uint32_t delay = (uint32_t)(1000 / fps * pow(2, -m_speed));
    sys->sleep(delay);
}

void
NFStreamPlay::vdecloop()
{
    while (m_vdectrd.active())
    {
        bool empty = false;
        {
            AutoLock locker(m_frameQu._mtx);
            empty = m_frameQu.empty();
        }

        if (empty)
        {
            sys->sleep(50);
            continue;
        }

        if (m_pause)
        {
            sys->sleep(100);
            continue;
        }

        uint64_t pts = 0;
        uint8_t  fps = 0;

        ut_frame_s *hdr = NULL;
        {
            AutoLock locker(m_frameQu._mtx);
            do
            {
                uint8_t *buf = m_frameQu.front();
                hdr = (ut_frame_s *)buf;
                pts = hdr->pts;
                fps = hdr->video.fps;

                m_bufsize -= (hdr->size + sizeof(ut_frame_s));
                if (m_minThresholdCB != NULL && m_bufsize <= MIN_BUFFER_SIZE)
                {
                    m_minThresholdCB(1, 0, m_minThresholdData);
                }

                if (m_iflag)
                {
                    if ('I' != hdr->type)
                    {
                        break;
                    }

                    m_iflag = false;
                }

                int      size = 0;
                int      w    = 0;
                int      h    = 0;

                if (ENCODE_H264 == hdr->encode ||
                        ENCODE_HEVC == hdr->encode)
                {
                    if (setVCodec(hdr->encode, hdr->video.width, hdr->video.height, hdr->video.fps))
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
                    if (setACodec(hdr->encode, hdr->audio.sps, hdr->audio.channels, hdr->audio.bps))
                    {
                        break;
                    }

                    if (!m_adec)
                    {
                        break;
                    }

                    m_adec->decode(hdr->buf, hdr->size, NULL, size);
                }
            }
            while (0);

            mempool->free(m_frameQu.front());
            m_frameQu.pop();
        }

        if (0 == m_mode)
        {
            delay(pts);
        }
        else
        {
            delay((uint32_t)fps);
        }
        m_time = pts;
    }
}

void
NFStreamPlay::adecloop()
{
    while (m_adectrd.active())
    {
        sys->sleep(40);
    }
}
