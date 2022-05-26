#ifndef _H264_PARSE_H_
#define _H264_PARSE_H_

unsigned int Ue(unsigned char *pu8Data, unsigned int u32Len, unsigned int &u32StartBit);
int Se(unsigned char *pu8Data, unsigned int u32Len, unsigned int &u32StartBit);
long U(unsigned int u32BitCount, unsigned char *pu8Data, unsigned int &u32StartBit);

int parseh264resolution(unsigned char *pu8Data, int s32Len, int &s32Height, int &s32Width);

#endif
