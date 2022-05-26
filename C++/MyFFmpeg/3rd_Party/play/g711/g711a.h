/**
 * @file      g711a.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-06 16:33:39
 *
 * @note
 *  g711a.h defines
 */
#ifndef __G711A_H__
#define __G711A_H__

#include "g711_util.h"
int g711a_encode(const char *src, char *dest, int srclen, int *dstlen);
int g711a_decode(const char *src, char *dest, int srclen, int *dstlen);

#endif
