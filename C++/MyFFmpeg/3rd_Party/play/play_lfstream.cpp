#include "play_lfstream.h"
#include "fsrw.h"
#include "tls_error.h"
#include "decoder_ff.h"
#include "render_sdl.h"
#include "frame.h"
#include "mempool.h"

#define MAX_FRAME_SIZE    (1024*1024)

LFStreamPlay::LFStreamPlay(int mode)
    : StreamPlay(STREAM_FILE_LOCAL, mode)
{
    m_rd      = NULL;
    m_refpts  = 0;
    m_reftime = 0;
    m_pause   = false;
    m_frame   = NULL;
}

LFStreamPlay::~LFStreamPlay()
{

}

int
LFStreamPlay::openFile(const char *filename)
{
    if (!filename)
    {
        return -1;
    }

    m_rd = new RecordReader();
    if (m_rd->open(filename))
    {
        delete m_rd;
        m_rd = NULL;
        return -1;
    }

    if (!m_frame)
    {
        m_frame = (uint8_t *)mempool->alloc(MAX_FRAME_SIZE);
        if (!m_frame)
        {
            m_rd->close();
            delete m_rd;
            m_rd = NULL;
            return -1;
        }
    }

    /* uint32_t type = ((RecordReader *)m_rd)->type(); */
    /* if (FS_RECORD_AUDI == type) */
    /* { */
    /* } */
    /* else */
    /* { */
    /* } */

    if (!m_vdectrd.active())
    {
        m_vdectrd.setEntry(threadProc, this, T_VDEC);
        return m_vdectrd.start();
    }

    /* if (!m_adectrd.active()) */
    /* { */
    /*     m_adectrd.setEntry(threadProc, this, T_ADEC); */
    /*     result += m_adectrd.start(); */
    /* } */

    return 0;
}

void
LFStreamPlay::closeFile()
{
    if (!m_rd)
    {
        return;
    }

    m_rd->close();
    delete m_rd;
    m_rd = NULL;

    if (m_frame)
    {
        mempool->free((void *)m_frame);
        m_frame = NULL;
    }
}

int
LFStreamPlay::getFileTime(datetime_s* sTime, datetime_s* eTime)
{
    if (!m_rd)
    {
        return -1;
    }

    uint64_t u64sTime = 0;
    uint64_t u64eTime = 0;
    m_rd->range(u64sTime, u64eTime);

    if (u64sTime == 0 || u64eTime == 0)
    {
        return -1;
    }

    *sTime = u64Time2DataTime(u64sTime);
    *eTime = u64Time2DataTime(u64eTime);

    return 0;
    

}

int
LFStreamPlay::pause(bool pause)
{
    m_pause = pause;

    return 0;
}

int
LFStreamPlay::fast(int &speed)
{
    if (m_speed++ > 4)
    {
        m_speed = 4;
    }

    speed = m_speed;

    return 0;
}

int
LFStreamPlay::slow(int &speed)
{
    if (m_speed-- < -4)
    {
        m_speed = -4;
    }

    speed = m_speed;

    return 0;
}

int
LFStreamPlay::setpos(int pos)
{
    uint64_t begin = 0;
    uint64_t end   = 0;
    uint64_t repos = 0;

    m_rd->range(begin, end);
    repos = begin + (uint64_t)((end - begin) * pos / (float)100);

    m_pause = true;

    int result = m_rd->seek(repos);

    m_pause = false;

    return result;
}

int
LFStreamPlay::getpos()
{
    uint64_t begin = 0;
    uint64_t end   = 0;
    uint64_t cur   = 0;

    m_rd->range(begin, end);

    cur = m_rd->current();

    return (int)((cur - begin) / float(end - begin) * 100);
}

void
LFStreamPlay::delay(uint64_t pts)
{
    if (0 == m_refpts)
    {
        m_refpts  = pts;
        m_reftime = sys->currentTimeMillis();
    }
    else
    {
        uint64_t current = sys->currentTimeMillis();
        int64_t  d       = (int64_t)((pts - m_refpts) * pow(2, -m_speed)) - (current - m_reftime);

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
LFStreamPlay::delay(uint32_t fps)
{
    uint32_t delay = (uint32_t)(1000 / fps * pow(2, -m_speed));
    sys->sleep(delay);
}

void
LFStreamPlay::vdecloop()
{
    while (m_vdectrd.active())
    {
        if (m_pause)
        {
            sys->sleep(100);
            continue;
        }

        if (m_rd->readFrame(m_frame, MAX_FRAME_SIZE) < 0)
        {
            break;
        }

        ut_frame_s *hdr = (ut_frame_s *)m_frame;

        if (m_iflag)
        {
            if ('I' != hdr->type)
            {
                continue;
            }

            m_iflag = false;
        }

        int      size = 0;
        int      w    = 0;
        int      h    = 0;
        uint64_t pts  = 0;

        if ('A' != hdr->type)
        {
            LOGT_PLY("frame type: %c, pts: %lld", hdr->type, hdr->pts);
            if (setVCodec(hdr->encode, hdr->video.width, hdr->video.height, hdr->video.fps) < 0)
            {
                sys->sleep(50);
                continue;
            }

            if (!m_vdec)
            {
                sys->sleep(50);
                continue;
            }

            m_vdec->decode(hdr->buf, hdr->size, NULL, size, w, h, pts);

            if (0 == m_mode)
            {
                delay(hdr->pts);
            }
            else
            {
                delay((uint32_t)hdr->video.fps);
            }

            m_time = hdr->pts;
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
    }
}

void
LFStreamPlay::adecloop()
{
    while (m_adectrd.active())
    {
        sys->sleep(40);
    }
}
