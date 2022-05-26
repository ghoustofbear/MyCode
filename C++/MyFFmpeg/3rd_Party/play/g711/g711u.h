/**
 * @file      g711u.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-06 16:32:46
 *
 * @note
 *  g711u.h defines
 */
#ifndef	__G711U_H__
#define	__G711U_H__

#include "g711_util.h"
int g711u_encode(const char *src, char *dest, int srclen, int *dstlen);
int g711u_decode(const char *src, char *dest, int srclen, int *dstlen);

#endif
