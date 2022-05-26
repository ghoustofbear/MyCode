#ifndef _G711_H_
#define _G711_H_

int encode_g711u_audio_data(unsigned char *dest, unsigned char *src, int len_src);
int decode_g711u_audio_data(unsigned char *dest, unsigned char *src, int len_src);

int encode_g711a_audio_data(unsigned char *dest, unsigned char *src, int len_src);
int decode_g711a_audio_data(unsigned char *dest, unsigned char *src, int len_src);

#endif
