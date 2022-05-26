/**
 * @file      wav.h
 * @copyright (C) Copyright 2013, xxx Co., Ltd.
 * @author    Zhong caiwang <caiwang213@qq.com>
 * @date      Fri Dec 16 10:24:48 CST 2016
 * @version   
 *
 * @brief     wav file header defines.
 */
#ifndef __WAV_H__
#define __WAV_H__

#include "def.h"

enum
{
	FORMAT_NONE   = 0,                                   /**< Unknown  */
	FORMAT_MPCM   = 1,                                   /**< Microsoft PCM  */
	FORMAT_MADPCM = 2,                                   /**< Microsoft ADPCM  */
	FORMAT_ALAW   = 6,                                   /**< G711A */
	FORMAT_MULAW  = 7,                                   /**< G711u */
};

typedef struct
{
	uint8_t        riff[4];                              /**< "RIFF" */
	uint32_t       size;                                 /**< length of rest of file = size of rest of header(36) + data length */
	uint8_t        wave[4];                              /**< "WAVE" */
}wav_riff_s;


typedef struct
{
	uint8_t        fmt[4];                               /**< "fmt" */
	uint32_t       fmtLen;                               /**< length of rest of fmt chunk = 16 */

	uint16_t       tag;                                  /**< 0x1-PCM 0x6-g711A 0x7-g711u */
	uint16_t       channels;                             /**< channels, mono = 1, stereo = 2 */

	uint32_t       samplesRate;                          /**< sample rate */
	uint32_t       bytesRate;                            /**< bytes per second = samplesRate * bytesPerSample  */

	uint16_t       bytesPerSample;                       /**< block align (bytes per sample) = channels * bits_per_sample / 8 */
	uint16_t       bitsPerSample;                        /**< bits per sample = 16 for MS PCM (format specific) */
}wav_format_s;


typedef struct
{
	wav_riff_s     riff;
	wav_format_s   format;

	uint8_t        data[4];                               /**< "data" */
	uint32_t       length;                                /**< data length (bytes) */
}wav_head_s;

#endif


