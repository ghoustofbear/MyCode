/**
 * @file      bitmap.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-23 12:56:09
 *   
 * @note
 *  bitmap.h defines 
 */
#ifndef __BITMAP_H__
#define __BITMAP_H__
#include <stdint.h>

#ifdef _WIN32
#pragma pack(2)
#endif
typedef struct
{
    uint16_t           type;
    uint32_t           size;
    uint16_t           reserved1;
    uint16_t           reserved2;
    uint32_t           offBits;
}
#ifdef _WIN32
#pragma pack()
#else
__attribute__((packed))
#endif
bmp_file_hdr_s;

typedef struct
{
    uint32_t           size;
    int32_t            width;
    int32_t            height;
    uint16_t           planes;
    uint16_t           bitcount;
    uint32_t           compression;
    uint32_t           sizeImage;
    int32_t            xPelsPerMeter;
    int32_t            yPelsPerMeter;
    uint32_t           clrUsed;
    uint32_t           clrImportant;
} bmp_info_hdr_s;

typedef struct
{
    uint8_t            blue;
    uint8_t            green;
    uint8_t            red;
    uint8_t            reserved;
} rgbquad_s;

typedef struct
{
    bmp_info_hdr_s     header;
    rgbquad_s          colors;
} bmp_info_s;

#endif
