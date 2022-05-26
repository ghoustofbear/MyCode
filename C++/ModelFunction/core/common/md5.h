#ifndef __MD5_H__
#define __MD5_H__

/* MD5.H - header file for MD5C.C
 *

 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express ed in any copies of any part of this
 * documentation and/or software.
 */

/* MD5 context. */
#include "def.h"
typedef struct
{
    unsigned int  state[4];                                /* state (ABCD) */
    unsigned int  count[2];                                /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                              /* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(MD5_CTX *, unsigned char [16]);
CM_API void MD5(unsigned char *in, unsigned char *crypto, int size);

#endif
