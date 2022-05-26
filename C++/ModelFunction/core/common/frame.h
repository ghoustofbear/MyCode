/**
 * @file      frame.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     private video and audio frame define
 * @author    caiwang213@qq.com
 * @date      2017-06-15 16:41:15
 *
 * @note
 *  frame.h defines
 */

#ifndef __FRAME_H__
#define __FRAME_H__

#include <stdint.h>

#define TAG_HI           (*(uint32_t*)"HIFM")              /**< 海思h.264帧 */
#define TAG_HIK          (*(uint32_t*)"HKFM")              /**< 海康老设备帧 */
#define TAG_HIK_N        (*(uint32_t*)"HNFM")              /**< 海康新设备帧 */
#define TAG_DAHUA        (*(uint32_t*)"DHFM")              /**< 大华设备帧 */
#define TAG_TIANDY       (*(uint32_t*)"TDFM")              /**< 天地设备帧 */
#define TAG_STD          (*(uint32_t*)"STFM")              /**< 标准h.264/h.265/音视频帧 */
#define TAG_STD_NALU     (*(uint32_t*)"NAFM")              /**< 标准h.264/h.265/音视频帧  NALU包 */
#define TAG_STD_FU       (*(uint32_t*)"FUFM")              /**< 标准h.264/h.265/音视频帧  RTP FUA包 */
#define TAG_STD_PAYLOAD  (*(uint32_t*)"FPFM")              /**< 标准h.264/h.265/音视频帧  RTP FUA负载包 */
#define TAG_HELMET_GX    (*(uint32_t*)"HTGX")              /**< 安全帽＿国信 */
#define TAG_STD_PS       (*(uint32_t*)"STPS")              /**< PS流 */


#define TAG_POSI         (*(uint32_t*)"POSI")              /**< 进度通知帧 */
#define TAG_RPOS         (*(uint32_t*)"RPOS")              /**< 重定位通知帧 */

#define TAG_STARTTALK    (*(uint32_t*)"STTK")              /**< start dualtalk to dev */
#define TAG_STOPTALK     (*(uint32_t*)"SPTK")              /**< stop dualtalk to dev */
#define TAG_RESPONSETALK (*(uint32_t*)"RSTK")              /**< response dualtalk to dev */
#define TAG_HEARBEAT     (*(uint32_t*)"HEAR")              /**< hearbeat */

static uint8_t hikOldHdr[40] =
{
    0x34, 0x48, 0x4b, 0x48, 0xfe, 0xb3, 0xd0, 0xd6,
    0x08, 0x03, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x10, 0x01, 0x10, 0x01, 0x10, 0x10, 0x00,
    0x80, 0x3e, 0x00, 0x00, 0xC0, 0x02, 0x40, 0x02,
    0x11, 0x10, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00
};

static uint8_t hikNewHdr[40] =
{
    0x49, 0x4d, 0x4b, 0x48, 0x01, 0x01, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x01, 0x21, 0x72, 0x01, 0x10,
    0x80, 0x3e, 0x00, 0x00, 0x80, 0x3e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


enum
{
    ENCODE_UNKNOW,
    ENCODE_H264,
    ENCODE_HEVC,
    ENCODE_JPEG,
    ENCODE_G711A,
    ENCODE_G711U,
    ENCODE_G722,
    ENCODE_PCM,
    ENCODE_PS
};

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint8_t        fps;
    uint8_t        system;                                 /**< 'P' or 'N' */
    uint8_t        camera;                                 /**< 前置or后置摄像头 */
    uint8_t        reserved;
} ut_video_s;

typedef struct
{
    uint32_t       sps;                                    /**< sample rate, sample times int per second */
    uint8_t        channels;                               /**< number of sampling channels */
    uint8_t        bps;                                    /**< bit per sample, bits for each sample */
    uint16_t       packLen;
} ut_audio_s;

typedef struct
{
    uint32_t       tag;                                    /**< 厂商标识 */

    uint8_t        type;                                   /**< 帧类型, 'I':I帧 'P':P帧 'A': A帧 'R': rtp packet */
    uint8_t        encode;                                 /**< 编码类型, 1: h264 2: h265 3: JPEG 4: G711A 5: G711U 6: G722 7:PCM */
    uint8_t        channel: 7;                             /**< 通道号 */
    uint8_t        encrypt: 1;                             /**< 是否加密码. 0: 不加密 1: 加密 */
    uint8_t        reserved;

    uint32_t       seq;                                    /**< 帧序号 */
    uint32_t       size;                                   /**< 数据长度, 不包括帧头自己 */
    uint64_t       time;
    uint64_t       pts;

    union
    {
        ut_video_s video;
        ut_audio_s audio;
    };

    char           buf[0];
} ut_frame_s;

/**
 * 0x000000014e NAL_SEI_PREFIX
 * 0x0000000140 NAL_VPS
 */
#define IS_HEVC_IFRAME(data) \
( \
    (0x00 == data[0] && 0x00 == data[1] && 0x00 == data[2] && 0x01 == data[3] && 0x4e == data[4]) || \
    (0x00 == data[0] && 0x00 == data[1] && 0x00 == data[2] && 0x01 == data[3] && 0x40 == data[4]) \
)

#define IS_HEVC_PFRAME(data) \
( \
    (0x00 == data[0] && 0x00 == data[1] && 0x00 == data[2] && 0x01 == data[3] && 0x02 == data[4]) \
)

#endif
