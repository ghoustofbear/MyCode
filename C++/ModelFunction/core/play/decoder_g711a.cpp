#include "decoder_g711a.h"
#include "g711/g711a.h"

G711ADecoder::G711ADecoder()
{
    m_codec = CODEC_G711A;
}

G711ADecoder::~G711ADecoder()
{

}

int
G711ADecoder::decode(void *src, int ssize, void *dest, int &dsize)
{
    if (!src || ssize <= 0)
    {
        return -1;
    }

    int   size = ssize * 2;
    char *buf  = new char[size];

    g711a_decode((const char *)src, buf, ssize, &size);

    if(dest)
    {
        memcpy(dest, buf, dsize);
        dsize = size;
    }

    notifyADPCM(buf, size);

    delete []buf;

    return 0;
}
