/**
 * (C) Copyright 2015, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@qq.com>
 *
 * Mon Sep 21 12:48:40 CST 2015
 *
 * uuid header define.
 */
#ifndef __UUID_H__
#define __UUID_H__

#include <inttypes.h>
#define LIBUUID_CLOCK_FILE	"/var/lib/libuuid/clock.txt"

struct uuid {
	uint32_t	time_low;
	uint16_t	time_mid;
	uint16_t	time_hi_and_version;
	uint16_t	clock_seq;
	uint8_t	    node[6];
};

typedef unsigned char uuid_t[16];

static const char *fmt_lower =
	"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}";

static const char *fmt_upper =
	"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}";

#define UUID_UNPARSE_DEFAULT_UPPER
#ifdef UUID_UNPARSE_DEFAULT_UPPER
#define FMT_DEFAULT fmt_upper
#else
#define FMT_DEFAULT fmt_lower
#endif

void uuid_generate(uuid_t out);
void uuid_unparse(const uuid_t uu, char *out);

#endif
