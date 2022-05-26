/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Feb 26 16:45:16 PST 2013
 *
 * frame header define.
 */
#ifndef __FRAME_H__
#define __FRAME_H__
#include "typec1.h"

#define  MAX_FRAME_SIZE               (512*1024)
/**  
 * ˽������Ƶ֡ͷ����
 */

/* ϵͳ��ʽ */
#define SYSTEM_PAL	  'P'
#define SYSTEM_NTSC	  'N'

typedef struct
{
	WORD Width;
	WORD Height;
	BYTE FrameRate;
	BYTE System;
	WORD Reserved;
}UT_VIDEO_HEAD_S;

typedef struct
{
	UINT SampleRate;
	BYTE Channels;
	BYTE BitsPerSample;
	WORD PackLen;
}UT_AUDIO_HEAD_S;

/* UT_FRAME_HEAD_S.FrameTag */
#define FRAME_TAG_UT	               (*(UINT*)"UTFM")
#define FRAME_TAG_HIK8                 (*(UINT*)"HKFM8")  /**< �������豸֡8ϵ�� */
#define FRAME_TAG_HIK                  (*(UINT*)"HKFM")   /**< �������豸֡ */
#define FRAME_TAG_HIK_N                (*(UINT*)"HNFM")   /**< �������豸֡ */
#define FRAME_TAG_DAHUA                (*(UINT*)"DHFM")
#define FRAME_TAG_TIANDY               (*(UINT*)"TDFM")
#define FRAME_TAG_HI_H264              (*(UINT*)"HIFM")   /**< ��˼h.264֡ */
#define FRAME_TAG_STD_H264             (*(UINT*)"STFM")   /**< ��׼h.264֡ */
#define FRAME_TAG_STD_H264_NALU        (*(UINT*)"NAFM")   /**< ��׼h.264 NALU�� */
#define FRAME_TAG_STD_H264_FU          (*(UINT*)"FUFM")   /**< ��׼h.264 RTP FUA�� */
#define FRAME_TAG_STD_H264_FU_PAYLOAD  (*(UINT*)"FPFM")   /**< ��׼h.264 RTP FUA���ذ� */
#define FRAME_TAG_HELMET_GX            (*(UINT*)"HTGX")   /**< ��ȫñ�߹��� */

/* UT_FRAME_HEAD_S.FrameType */
#define FRAME_I	            'I'
#define FRAME_P	            'P'
#define FRAME_A	            'A'
#define FRAME_END           'E'
#define FRAME_REPOS         'R'

/* UT_FRAME_HEAD_S.EncodeType */
/* video */
#define ENCODE_TYPE_HI_H264	    (0x01)         /**< ��˼ */
#define ENCODE_TYPE_STD_H264	(0x02)         /**< ��׼ */
 
/* audio */
#define ENCODE_TYPE_G711A	(0x01)
#define ENCODE_TYPE_ADPCM   (0x02)

typedef struct
{
	UINT FrameTag;

	BYTE FrameType;
	BYTE EncodeType;    /* EncodeType Flag. h264: 0x15; h265: 0x17; motionjpeg: 0x29 */
	BYTE Channel:7;
	BYTE Encrypted:1;   /* Encrypt Flag. 0:no Encrypt; 1:Encrypted. */
	BYTE Reserved;

	UINT FrameNo;
	UINT FrameSize;
	U64  Time;          /* (microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC */
	U64  Pts;

	union
	{
		UT_VIDEO_HEAD_S VideoHead;
		UT_AUDIO_HEAD_S AudioHead;
	};
}UT_FRAME_HEAD_S;      /* sizeof(UT_FRAME_HEAD_S) = 40 */

#endif
