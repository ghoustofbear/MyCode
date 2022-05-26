#include "stream.h"
#include <stdarg.h>
#ifdef _WIN32
#include <direct.h>
#include<io.h>
#else
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<cstddef>
#endif
/**
 * FileStream
 */
FileStream::FileStream()
{
    file = NULL;
}

FileStream::~FileStream()
{
    if (file)
    {
        fclose(file);
    }
};

bool
FileStream::isopen()
{
    return file != NULL;
}

void
FileStream::openReadOnly(const char *fn)
{
    file = fopen(fn, "rb");

    if (!file)
    {
        throw StreamException("Unable to open file");
    }
}

void
FileStream::openWriteReplace(const char *fn)
{
	string path = string(fn);
	int index =  path.find_last_of('/');
	string dir = path.substr(0, index);
#ifdef _WIN32
	if (0 != access(dir.c_str(), 0))
	{
		mkdir(dir.c_str());
	}
#else
	if (NULL == opendir(dir.c_str()))
	{
		mkdir(dir.c_str(), 0775);
	}
#endif
    file = fopen(fn, "wb");
    if (!file)
    {
        throw StreamException("Unable to open file");
    }
}

void
FileStream::openWriteAppend(const char *fn)
{
    file = fopen(fn, "ab");

    if (!file)
    {
        throw StreamException("Unable to open file");
    }
}

void
FileStream::close()
{
    if (file)
    {
        fclose(file);
        file = NULL;
    }
}

void
FileStream::rewind()
{
    if (file)
    {
        fseek(file, 0, SEEK_SET);
    }
}

int
FileStream::length()
{
    int len = 0;

    if (file)
    {
        int old = ftell(file);
        fseek(file, 0, SEEK_END);
        len = ftell(file);
        fseek(file, old, SEEK_SET);

    }

    return len;
}


bool
FileStream::eof()
{
    if (file)
    {
        return (feof(file) != 0);
    }
    else
    {
        return true;
    }
}

int
FileStream::read(void *ptr, int len)
{
    if (!file)
    {
        return -1;
    }

    if (feof(file))
    {
        return 0;
        //throw StreamException("End of file");
    }

    int r = (int)fread(ptr, 1, len, file);

    //	updateTotals(r, 0);
    return r;
}


int
FileStream::write(const void *ptr, int len)
{
    if (!file)
    {
        return -1;
    }

    return (int)fwrite(ptr, 1, len, file);
    //	updateTotals(0, len);

}

void
FileStream::flush()
{
    if (!file)
    {
        return;
    }

    fflush(file);
}

int
FileStream::pos()
{
    if (!file)
    {
        return 0;
    }

    return ftell(file);
}

int
FileStream::copyTo(const char* des)
{
    FileStream fdes;
    fdes.openWriteReplace(des);
    if (fdes.isopen())
    {
        char buf[1024];
        int cnt = 0;
        while ((cnt = read(buf, 1024)) != 0)
        {
            fdes.write(buf, cnt);
        }
        fdes.close();
        return 0;
    }
    else
    {
        return -1;
    }
}

int
FileStream::seekTo(int pos)
{
    if (!file)
    {
        return -1;
    }

    return fseek(file, pos, SEEK_SET);
}


/**
 * Stream
 */
bool
Stream::eof()
{
    throw StreamException("Stream can`t eof");
    return false;
}

void
Stream::rewind()
{
    throw StreamException("Stream can`t rewind");
}

void
Stream::setPollRead(bool)
{
}

char
Stream::readChar()
{
    char v;
    read(&v, 1);
    return v;
}

short
Stream::readShort()
{
    short v;
    read(&v, 2);
    return v;
}

long
Stream::readLong()
{
    long v;
    read(&v, 4);
    return v;
}

int
Stream::readInt()
{
    return readLong();
}

int
Stream::readInt24()
{
    int v = 0;
    read(&v, 3);
    return v;
}

long
Stream::readTag()
{
    long v = readLong();
    return SWAP4(v);
}

int
Stream::readString(char *s, int max)
{
    int cnt = 0;

    while (max)
    {
        char c = readChar();
        *s++ = c;
        cnt++;
        max--;

        if (!c)
        {
            break;
        }
    }

    return cnt;
}

bool
Stream::readReady()
{
    return true;
}

int
Stream::numPending()
{
    return 0;
}


#if 0
void
Stream::writeID4(ID4 id)
{
    write(id.getData(), 4);
}
#endif

void
Stream::writeChar(char v)
{
    write(&v, 1);
}

void
Stream::writeShort(short v)
{
    write(&v, 2);
}

void
Stream::writeLong(long v)
{
    write(&v, 4);
}

void
Stream::writeInt(int v)
{
    writeLong(v);
}

void
Stream::writeTag(long v)
{
    writeLong(SWAP4(v));
}

void
Stream::writeTag(char id[4])
{
    write(id, 4);
}

int
Stream::seekTo(int)
{
    throw StreamException("Stream can`t seek");
}

void
Stream::writeTo(Stream &out, int len)
{
    char tmp[4096];

    while (len)
    {
        int rlen = sizeof(tmp);

        if (rlen > len)
        {
            rlen = len;
        }

        read(tmp, rlen);
        out.write(tmp, rlen);

        len -= rlen;
    }
}

int
Stream::writeUTF8(unsigned int code)
{
    if (code < 0x80)
    {
        writeChar(code);
        return 1;
    }
    else if (code < 0x0800)
    {
        writeChar(code >> 6 | 0xC0);
        writeChar((code & 0x3F) | 0x80);
        return 2;
    }
    else if (code < 0x10000)
    {
        writeChar(code >> 12 | 0xE0);
        writeChar((code >> 6 & 0x3F) | 0x80);
        writeChar((code & 0x3F) | 0x80);
        return 3;
    }
    else
    {
        writeChar(code >> 18 | 0xF0);
        writeChar((code >> 12 & 0x3F) | 0x80);
        writeChar((code >> 6 & 0x3F) | 0x80);
        writeChar((code & 0x3F) | 0x80);
        return 4;
    }

}

void
Stream::skip(int len)
{
    char tmp[4096];

    while (len)
    {
        int rlen = sizeof(tmp);

        if (rlen > len)
        {
            rlen = len;
        }

        read(tmp, rlen);
        len -= rlen;
    }

}


#if 0
void
Stream::updateTotals(unsigned int in, unsigned int out)
{
    totalBytesIn += in;
    totalBytesOut += out;

    unsigned int tdiff = sys->getTime() - lastUpdate;

    if (tdiff >= 5)
    {
        bytesInPerSec = (totalBytesIn - lastBytesIn) / tdiff;
        bytesOutPerSec = (totalBytesOut - lastBytesOut) / tdiff;
        lastBytesIn = totalBytesIn;
        lastBytesOut = totalBytesOut;
        lastUpdate = sys->getTime();
    }
}
#endif

int
Stream::readLine(char *in, int max)
{
    int i = 0;
    max -= 2;

    while (max--)
    {
        char c;

        if (read(&c, 1) < 0)
        {
            return -1;
        }

        if (c == '\n')
        {
            break;
        }

        if (c == '\r')
        {
            continue;
        }

        in[i++] = c;
    }

    in[i] = 0;
    return i;
}

void
Stream::write(const char *fmt, va_list ap)
{
    char tmp[16 * 1024] = {0};
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    write(tmp, (int)strlen(tmp));
}

void
Stream::writeStringF(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    write(fmt, ap);
    va_end(ap);
}

void
Stream::writeString(const char *str)
{
    write(str, (int)strlen(str));
}

void
Stream::writeLineF(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    write(fmt, ap);

    if (writeCRLF)
    {
        write("\r\n", 2);
    }
    else
    {
        write("\n", 1);
    }

    va_end(ap);
}

void
Stream::writeLine(const char *str)
{
    writeString(str);

    if (writeCRLF)
    {
        write("\r\n", 2);
    }
    else
    {
        write("\n", 1);
    }
}

int
Stream::readWord(char *in, int max)
{
    int i = 0;

    while (!eof())
    {
        char c = readChar();

        if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
        {
            if (i)
            {
                break;    // stop reading
            }
            else
            {
                continue;    // skip whitespace
            }
        }

        if (i >= (max - 1))
        {
            break;
        }

        in[i++] = c;
    }

    in[i] = 0;
    return i;
}

#if 0
int
Stream::readBase64(char *p, int max)
{
    char vals[4];

    int cnt = 0;

    while (cnt < (max - 4))
    {
        read(vals, 4);
        int rl = String::base64WordToChars(p, vals);

        if (!rl)
        {
            break;
        }

        p += rl;
        cnt += rl;
    }

    *p = 0;
    return cnt;
}
#endif



int
Stream::readBits(int cnt)
{
    int v = 0;

    while (cnt)
    {
        if (!bitsPos)
        {
            bitsBuffer = readChar();
        }

        cnt--;

        v |= (bitsBuffer & (1 << (7 - bitsPos))) ? (1 << cnt) : 0;

        bitsPos = (bitsPos + 1) & 7;
    }

    return v;
}
