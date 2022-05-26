/**
 * @file      uuid.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:30:45
 *   
 * @note
 *  uuid.h defines 
 */
#ifndef __UUID_H__
#define __UUID_H__

#include <inttypes.h>
#define LIBUUID_CLOCK_FILE	"/var/lib/libuuid/clock.txt"

struct uuid
{
    uint32_t	time_low;
    uint16_t	time_mid;
    uint16_t	time_hi_and_version;
    uint16_t	clock_seq;
    uint8_t	    node[6];
};

typedef unsigned char uuid_t[16];


#define UUID_UNPARSE_DEFAULT_UPPER
#ifdef UUID_UNPARSE_DEFAULT_UPPER
#define FMT_DEFAULT fmt_upper
#else
#define FMT_DEFAULT fmt_lower
#endif

void uuid_generate(uuid_t out);
void uuid_unparse(const uuid_t uu, char *out);

#endif
