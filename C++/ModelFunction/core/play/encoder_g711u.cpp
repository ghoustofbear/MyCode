#include "encoder_g711u.h"
#include "g711/g711u.h"
#include "sys.h"

G711UEncoder::G711UEncoder()
{

}

G711UEncoder::~G711UEncoder()
{

}

int
G711UEncoder::encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts)
{
     if (!src || ssize <= 0)
    {
        return -1;
    }

    int size = ssize / 2;

    if (dest)
    {
        g711u_encode((const char *)src, (char *)dest, ssize, &size);
    }
    else
    {
        char *buf = new char[size];
        g711u_encode((const char *)src, buf, ssize, &size);

        delete []buf;
    }

    dsize = size;
    pts   = sys->currentTimeMillis();

    return 0;
   return 0;
}
