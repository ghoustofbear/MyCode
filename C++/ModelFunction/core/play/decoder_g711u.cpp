#include "decoder_g711u.h"
#include "g711/g711u.h"

G711UDecoder::G711UDecoder()
{
    m_codec = CODEC_G711U;
}

G711UDecoder::~G711UDecoder()
{

}

int
G711UDecoder::decode(void *src, int ssize, void *dest, int &dsize)
{
    if (!src || ssize <= 0)
    {
        return -1;
    }

    int   size = ssize * 2;
    char *buf  = new char[size];

    g711u_decode((const char *)src, buf, ssize, &size);

    if(dest)
    {
        memcpy(dest, buf, dsize);
        dsize = size;
    }

    notifyADPCM(buf, size);

    delete []buf;

    return 0;
}
