/**
 * @file      rtparser.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-20 11:50:16
 *
 * @note
 *  RTP packetization for H.264 (RFC3984)
 *  RTP packetizer for HEVC/H.265 payload format (draft version 6)
 *  https://www.ffmpeg.org/doxygen/2.5/rtpenc__h264_8c_source.html
 *  https://www.ffmpeg.org/doxygen/2.5/rtpenc__hevc_8c_source.html
 */
#ifndef __RTPARSER_H__
#define __RTPARSER_H__
#include <stdint.h>
#include <stdlib.h>

#define MAX_PAYLOAD_SIZE      (1400 - 12)
#define MAX_FRAME_SIZE        (1024 * 1024)
#define RTP_HEADLEN           (12)

class RTParser
{
public:
	RTParser();

	virtual ~RTParser();

    virtual int        onSendata(void *data, size_t size, bool mark) = 0;

    virtual void       onNalUnit(void *data, size_t size) {};
    virtual void       onPayload(void *data, size_t size) {};
    virtual void       onVideo  (void *data, size_t size, int encode, int type, int w, int h, int fps, uint64_t pts) {};
    virtual void       onAudio  (void *data, size_t size, int encode, int sps,  int channels, int bps, uint64_t pts) {};

protected:
    int                h2642nal(const uint8_t *data, size_t size); 
    int                hevc2nal(const uint8_t *data, size_t size); 

	int                rtp2nal(const uint8_t *data, size_t size, bool &bMark);

	int                nal2h264(const uint8_t *data, size_t size, uint8_t **pNalu, uint32_t &naluLen, uint8_t &naluType, bool &bMark);
    int                nal2hevc(); 

    int                nal2payload(const uint8_t *nal, size_t size, bool last);
protected:
    int                m_encode;
    int                m_type;
    uint64_t           m_pts;
	uint8_t            *m_frame;
	uint32_t           m_framelen;

private:
    uint8_t            m_payload[MAX_PAYLOAD_SIZE];    
    uint8_t            m_sps[MAX_PAYLOAD_SIZE];    
    uint8_t            m_pps[MAX_PAYLOAD_SIZE];   	
	uint32_t           m_spslen;
	uint32_t           m_ppslen;	
};
#endif
