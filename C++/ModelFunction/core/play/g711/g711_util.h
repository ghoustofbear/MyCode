/*
 * This source code is quick table lookup implementation of
 * convert 16 bit linear PCM and A-law u-law (ITU G.711) codings
 * Tables are generated using ITU G.711 example code from
 * Sun Microsystems, Inc.
 *
 * (C)2001 Karsten Keil kkeil@suse.de
 *
 */

#ifndef __H_G711_UTIL_H__
#define __H_G711_UTIL_H__
unsigned char  linear2ulaw(signed short l);
unsigned char  linear2alaw(signed short l);

signed   short ulaw2linear(unsigned char u);
signed   short alaw2linear(unsigned char a);

unsigned char  ulaw2alaw(unsigned char u);
unsigned char  alaw2ulaw(unsigned char a);

#endif
