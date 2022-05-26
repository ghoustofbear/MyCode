#include "rtparser.h"
#include <string.h>
#include "PsProcessApi.h"
#include <stdio.h>

RTParser::RTParser()
{
	m_spslen = 0;
	m_ppslen = 0;
	m_framelen = 0;
    m_frame = new uint8_t[MAX_FRAME_SIZE];
}

RTParser::~RTParser()
{
	if (NULL != m_frame)
	{
		delete[] m_frame;
		m_frame = NULL;
    }
}

int
RTParser::rtp2ps(const uint8_t *data, size_t size, bool &bMark)
{
    if (data == NULL ||
        size < RTP_HEADLEN)
    {
        return -1;
    }

    if (data[1] & 0x80)
        bMark = true;
    else
        bMark = false;

    uint32_t *pu32Start = (uint32_t*)(data + RTP_HEADLEN);

    memcpy(m_frame + m_framelen, data + RTP_HEADLEN, size - RTP_HEADLEN);
    m_framelen += size - RTP_HEADLEN;

    return 0;
}

int 
RTParser::rtp2nal(const uint8_t *data, size_t size, bool &bMark)
{
	uint8_t * naluStart = NULL;
	uint8_t   naluType = 0;
	uint32_t  naluLen = 0;

	int ret = nal2h264(data, size, &naluStart, naluLen, naluType, bMark);
	if (ret < 0)
	{
		return -1;
	}

	if (!bMark)
	{
		memcpy(m_frame + m_framelen, naluStart, naluLen);
		m_framelen += naluLen;

		return -1;
	}
	else
	{
		if (7 == naluType)
		{
			memcpy(m_sps, naluStart, naluLen);
			m_spslen = naluLen;
			m_framelen = 0;

			return -1;
		}
		if (8 == naluType)
		{
			memcpy(m_pps, naluStart, naluLen);
			m_ppslen = naluLen;
			m_framelen = 0;

			return -1;
		}

		memcpy(m_frame + m_framelen, naluStart, naluLen);
		m_framelen += naluLen;

		if (5 == naluType)
		{
			int movsize = (m_spslen + m_ppslen);
			memmove(m_frame + movsize, m_frame, m_framelen);
			memcpy(m_frame, m_sps, m_spslen);
			memcpy(m_frame + m_spslen, m_pps, m_ppslen);

			m_framelen += movsize;
		}
		else 
		{ }
	}

	return 0;
}

int 
RTParser::nal2h264(const uint8_t *data, size_t size, uint8_t **pNalu, uint32_t &naluLen, uint8_t &naluType, bool &bMark)
{
	if (size < RTP_HEADLEN)
	{
		return -1;
	}

	naluLen = 0;
	bMark = false;
	naluType = 1;

	uint8_t *pu8Start = (uint8_t*)data + RTP_HEADLEN;
	uint8_t  u8FuIndicator = *pu8Start;
	uint8_t  u8FuHeader = *(pu8Start + 1);
	uint8_t  u8FuIndictType = u8FuIndicator & 0X1F;
	uint8_t  u8FuHeaderType = u8FuHeader & 0XE0;
	uint8_t  u8NaluHeader = (u8FuIndicator & 0XE0) | (u8FuHeader & 0X1F);

	if (u8FuIndictType == 0X1C)
	{
		/* 判断NAL的类型为0x1c=28，说明是FU-A分片 */
		if (u8FuHeaderType == 0x80)
		{
			/* 开始 */
			*pNalu = pu8Start - 3;

			*((uint32_t *)(*pNalu)) = 0x01000000;        /* 大模式会有问题 */
			*((uint8_t *)(*pNalu) + 4) = u8NaluHeader;
			naluLen = size - RTP_HEADLEN + 3;

			naluType = u8NaluHeader & 0x1F;
		}
		else if (u8FuHeaderType == 0x40)
		{
			/* 结束 */
			*pNalu = pu8Start + 2;
			naluLen = size - RTP_HEADLEN - 2;
			naluType = u8NaluHeader & 0X1F;
		}
		else
		{
			/* 中间 */
			*pNalu = pu8Start + 2;
			naluLen = size - RTP_HEADLEN - 2;
		}
	}
	else
	{
		/* 单一模式数据, 小于一个MTU的P帧、SPS、PPS走这种模式 */
		*pNalu = pu8Start - 4;
		*((uint32_t*)(*pNalu)) = 0x01000000;               /* 大模式会有问题 */
		naluLen = size - RTP_HEADLEN + 4;
		naluType = pu8Start[0] & 0X1F;
	}

	if (data[1] & 0x80)
	{
		bMark = true;     /* rtp mark */
	}
	else
	{
		bMark = false;
	}

	return 0;
}

int                
RTParser::h264ToPs(const uint8_t *data, size_t size, int type, uint64_t pts)
{
    inputstreamdata(m_psPacketor, (uint8_t *)data, size, type, pts,pts);
    return 0;
}

int
RTParser::nal2payload(const uint8_t *nal, size_t size, bool last)
{
#if 1 
    if (size <= MAX_PAYLOAD_SIZE)
    {
        /* single nal unit packet */
        onSendata((void *)nal, size, true);
    }
    else
    {
        uint8_t type = nal[0] & 0x1F;//0001 1111
        uint8_t nri  = nal[0] & 0x60;//0110 0000

        uint8_t * offset = (uint8_t *)nal;
        int       left   = size;

        offset++;
        left--;

        /* FU-A fragmentation unit without interleaving */
        int cnt = (left + (MAX_PAYLOAD_SIZE - 2) - 1) / (MAX_PAYLOAD_SIZE - 2);

        for (int i = 0; i < cnt; i++)
        {
            int len = (left < (MAX_PAYLOAD_SIZE - 2)) ? left : (MAX_PAYLOAD_SIZE - 2);

            /* FU indicator */
            m_payload[0] = nri | 28;

            /* FU header */
            m_payload[1] = (i == 0 ? 0x80 : 0x00) | ((i == cnt - 1) ? 0x40 : 0x00) | type;

            /* FU payload */
            memcpy(&m_payload[2], offset, len);

            onSendata(m_payload, len + 2, (i == cnt - 1) && last);

            left   -= len;
            offset += len;
        }
    }

    return size;
#else
    if (size <= MAX_PAYLOAD_SIZE)
    {
        onSendata((void *)nal, size, last);//
    }
    else
    {
        uint8_t type = nal[0] & 0x1F;
        uint8_t nri  = nal[0] & 0x60;

        /* FU Indicator; Type = 28 ---> FU-A */
        m_payload[0]  = 28;
        m_payload[0] |= nri;
        m_payload[1]  = type;
        m_payload[1] |= 1 << 7;

        nal  += 1;
        size -= 1;

        while (size + 2 > MAX_PAYLOAD_SIZE)
        {
            memcpy(&m_payload[2], nal, MAX_PAYLOAD_SIZE - 2);
            onSendata(m_payload, MAX_PAYLOAD_SIZE, false);//

            nal  += MAX_PAYLOAD_SIZE - 2;
            size -= MAX_PAYLOAD_SIZE - 2;
            m_payload[1] &= ~(1 << 7);
        }

        m_payload[1] |= 1 << 6;
        memcpy(&m_payload[2], nal, size);

        onSendata(m_payload, size + 2, last);//
    }

    return 0;
#endif
}

int
RTParser::h2642nal(const uint8_t *data, size_t size)
{
    /* invalid frame */
    if (!data || size <= 3)
    {
        return -1;
    }

    uint8_t * buf  = (uint8_t *)data;
    size_t    left = size;

    while (left > 4 && (buf[0] != 0 || buf[1] != 0 || buf[2] != 1))
    {
        left--;
        buf++;
    }

    /* split to nal units */
    int result = 0;
    while (left > 4)
    {
        size_t offset;
        size_t len  = left;
        size_t skip = left;

        /* search nal end */
        for (offset = 4; offset + 2 < left; offset++)
        {
            if (buf[offset] == 0 && buf[offset + 1] == 0 && buf[offset + 2] == 1)
            {
                /* we found another startcode */
                len  = offset - (buf[offset - 1] == 0 ? 1 : 0);
                skip = offset;

                break;
            }
        }

        result += nal2payload(buf + 3, len - 3, len >= left);
        left   -= skip;
        buf    += skip;
    }

    return result;
}

int
RTParser::hevc2nal(const uint8_t *data, size_t size)
{
#if 0
#define RTP_HEVC_HEADERS_SIZE 3

    static void nal_send(AVFormatContext * ctx, const uint8_t *buf, int len, int last_packet_of_frame)
    {
        RTPMuxContext *rtp_ctx = ctx->priv_data;
        int rtp_payload_size   = rtp_ctx->max_payload_size - RTP_HEVC_HEADERS_SIZE;
        int nal_type           = (buf[0] >> 1) & 0x3F;

        /* send it as one single NAL unit? */
        if (len <= rtp_ctx->max_payload_size)
        {
            /* use the original NAL unit buffer and transmit it as RTP payload */
            ff_rtp_send_data(ctx, buf, len, last_packet_of_frame);
        }
        else
        {
            /*
               create the HEVC payload header and transmit the buffer as fragmentation units (FU)

               0                   1
               0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               |F|   Type    |  LayerId  | TID |
               +-------------+-----------------+

               F       = 0
               Type    = 49 (fragmentation unit (FU))
               LayerId = 0
               TID     = 1
               */
            rtp_ctx->buf[0] = 49 << 1;
            rtp_ctx->buf[1] = 1;

            /*
               create the FU header

               0 1 2 3 4 5 6 7
               +-+-+-+-+-+-+-+-+
               |S|E|  FuType   |
               +---------------+

               S       = variable
               E       = variable
               FuType  = NAL unit type
               */
            rtp_ctx->buf[2]  = nal_type;
            /* set the S bit: mark as start fragment */
            rtp_ctx->buf[2] |= 1 << 7;

            /* pass the original NAL header */
            buf += 2;
            len -= 2;

            while (len > rtp_payload_size)
            {
                /* complete and send current RTP packet */
                memcpy(&rtp_ctx->buf[RTP_HEVC_HEADERS_SIZE], buf, rtp_payload_size);
                ff_rtp_send_data(ctx, rtp_ctx->buf, rtp_ctx->max_payload_size, 0);

                buf += rtp_payload_size;
                len -= rtp_payload_size;

                /* reset the S bit */
                rtp_ctx->buf[2] &= ~(1 << 7);
            }

            /* set the E bit: mark as last fragment */
            rtp_ctx->buf[2] |= 1 << 6;

            /* complete and send last RTP packet */
            memcpy(&rtp_ctx->buf[RTP_HEVC_HEADERS_SIZE], buf, len);
            ff_rtp_send_data(ctx, rtp_ctx->buf, len + 2, last_packet_of_frame);
        }
    }

    void ff_rtp_send_hevc(AVFormatContext * ctx, const uint8_t *frame_buf, int frame_size)
    {
        const uint8_t *next_NAL_unit;
        const uint8_t *buf_ptr, *buf_end = frame_buf + frame_size;
        RTPMuxContext *rtp_ctx = ctx->priv_data;

        /* use the default 90 KHz time stamp */
        rtp_ctx->timestamp = rtp_ctx->cur_timestamp;

        if (rtp_ctx->nal_length_size)
        {
            buf_ptr = ff_avc_mp4_find_startcode(frame_buf, buf_end, rtp_ctx->nal_length_size) ? frame_buf : buf_end;
        }
        else
        {
            buf_ptr = ff_avc_find_startcode(frame_buf, buf_end);
        }

        /* find all NAL units and send them as separate packets */
        while (buf_ptr < buf_end)
        {
            if (rtp_ctx->nal_length_size)
            {
                next_NAL_unit = ff_avc_mp4_find_startcode(buf_ptr, buf_end, rtp_ctx->nal_length_size);
                if (!next_NAL_unit)
                {
                    next_NAL_unit = buf_end;
                }

                buf_ptr += rtp_ctx->nal_length_size;
            }
            else
            {
                while (!*(buf_ptr++))
                    ;
                next_NAL_unit = ff_avc_find_startcode(buf_ptr, buf_end);
            }
            /* send the next NAL unit */
            nal_send(ctx, buf_ptr, next_NAL_unit - buf_ptr, next_NAL_unit == buf_end);

            /* jump to the next NAL unit */
            buf_ptr = next_NAL_unit;
        }
    }
#endif
    return -1;
}

#if 0
static void nal_send(AVFormatContext *s1, const uint8_t *buf, int size, int last)
{
    RTPMuxContext *s = s1->priv_data;
    enum AVCodecID codec = s1->streams[0]->codecpar->codec_id;

    av_log(s1, AV_LOG_DEBUG, "Sending NAL %x of len %d M=%d\n", buf[0] & 0x1F, size, last);
    if (size <= s->max_payload_size)
    {
        int buffered_size = s->buf_ptr - s->buf;
        int header_size;
        int skip_aggregate = 0;

        if (codec == AV_CODEC_ID_H264)
        {
            header_size = 1;
            skip_aggregate = s->flags & FF_RTP_FLAG_H264_MODE0;
        }
        else
        {
            header_size = 2;
        }

        // Flush buffered NAL units if the current unit doesn't fit
        if (buffered_size + 2 + size > s->max_payload_size)
        {
            flush_buffered(s1, 0);
            buffered_size = 0;
        }
        // If we aren't using mode 0, and the NAL unit fits including the
        // framing (2 bytes length, plus 1/2 bytes for the STAP-A/AP marker),
        // write the unit to the buffer as a STAP-A/AP packet, otherwise flush
        // and send as single NAL.
        if (buffered_size + 2 + header_size + size <= s->max_payload_size &&
                !skip_aggregate)
        {
            if (buffered_size == 0)
            {
                if (codec == AV_CODEC_ID_H264)
                {
                    *s->buf_ptr++ = 24;
                }
                else
                {
                    *s->buf_ptr++ = 48 << 1;
                    *s->buf_ptr++ = 1;
                }
            }
            AV_WB16(s->buf_ptr, size);
            s->buf_ptr += 2;
            memcpy(s->buf_ptr, buf, size);
            s->buf_ptr += size;
            s->buffered_nals++;
        }
        else
        {
            flush_buffered(s1, 0);
            ff_rtp_send_data(s1, buf, size, last);
        }
    }
    else
    {
        int flag_byte, header_size;
        flush_buffered(s1, 0);
        if (codec == AV_CODEC_ID_H264 && (s->flags & FF_RTP_FLAG_H264_MODE0))
        {
            av_log(s1, AV_LOG_ERROR,
                   "NAL size %d > %d, try -slice-max-size %d\n", size,
                   s->max_payload_size, s->max_payload_size);
            return;
        }
        av_log(s1, AV_LOG_DEBUG, "NAL size %d > %d\n", size, s->max_payload_size);
        if (codec == AV_CODEC_ID_H264)
        {
            uint8_t type = buf[0] & 0x1F;
            uint8_t nri = buf[0] & 0x60;

            s->buf[0] = 28;        /* FU Indicator; Type = 28 ---> FU-A */
            s->buf[0] |= nri;
            s->buf[1] = type;
            s->buf[1] |= 1 << 7;
            buf  += 1;
            size -= 1;

            flag_byte   = 1;
            header_size = 2;
        }
        else
        {
            uint8_t nal_type = (buf[0] >> 1) & 0x3F;
            /*
             * create the HEVC payload header and transmit the buffer as fragmentation units (FU)
             *
             *    0                   1
             *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
             *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             *   |F|   Type    |  LayerId  | TID |
             *   +-------------+-----------------+
             *
             *      F       = 0
             *      Type    = 49 (fragmentation unit (FU))
             *      LayerId = 0
             *      TID     = 1
             */
            s->buf[0] = 49 << 1;
            s->buf[1] = 1;

            /*
             *     create the FU header
             *
             *     0 1 2 3 4 5 6 7
             *    +-+-+-+-+-+-+-+-+
             *    |S|E|  FuType   |
             *    +---------------+
             *
             *       S       = variable
             *       E       = variable
             *       FuType  = NAL unit type
             */
            s->buf[2]  = nal_type;
            /* set the S bit: mark as start fragment */
            s->buf[2] |= 1 << 7;

            /* pass the original NAL header */
            buf  += 2;
            size -= 2;

            flag_byte   = 2;
            header_size = 3;
        }

        while (size + header_size > s->max_payload_size)
        {
            memcpy(&s->buf[header_size], buf, s->max_payload_size - header_size);
            ff_rtp_send_data(s1, s->buf, s->max_payload_size, 0);
            buf  += s->max_payload_size - header_size;
            size -= s->max_payload_size - header_size;
            s->buf[flag_byte] &= ~(1 << 7);
        }
        s->buf[flag_byte] |= 1 << 6;
        memcpy(&s->buf[header_size], buf, size);
        ff_rtp_send_data(s1, s->buf, size + header_size, last);
    }
}

void ff_rtp_send_h264_hevc(AVFormatContext *s1, const uint8_t *buf1, int size)
{
    const uint8_t *r, *end = buf1 + size;
    RTPMuxContext *s = s1->priv_data;

    s->timestamp = s->cur_timestamp;
    s->buf_ptr   = s->buf;
    if (s->nal_length_size)
    {
        r = ff_avc_mp4_find_startcode(buf1, end, s->nal_length_size) ? buf1 : end;
    }
    else
    {
        r = ff_avc_find_startcode(buf1, end);
    }
    while (r < end)
    {
        const uint8_t *r1;

        if (s->nal_length_size)
        {
            r1 = ff_avc_mp4_find_startcode(r, end, s->nal_length_size);
            if (!r1)
            {
                r1 = end;
            }
            r += s->nal_length_size;
        }
        else
        {
            while (!*(r++));
            r1 = ff_avc_find_startcode(r, end);
        }
        nal_send(s1, r, r1 - r, r1 == end);
        r = r1;
    }
    flush_buffered(s1, 1);
}
#endif
