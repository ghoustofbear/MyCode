/**
 * @file      stream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:27:52
 *
 * @note
 *  stream.h defines
 */
#ifndef __STREAM_H__
#define __STREAM_H__

#include "excep.h"
#include "def.h"

#define SWAP2(v) ( ((v&0xff)<<8) | ((v&0xff00)>>8) )
#define SWAP3(v) (((v&0xff)<<16) | ((v&0xff00)) | ((v&0xff0000)>>16) )
#define SWAP4(v) (((v&0xff)<<24) | ((v&0xff00)<<8) | ((v&0xff0000)>>8) | ((v&0xff000000)>>24))
#define TOUPPER(c) ((((c) >= 'a') && ((c) <= 'z')) ? (c)+'A'-'a' : (c))
#define TONIBBLE(c) ((((c) >= 'A')&&((c) <= 'F')) ? (((c)-'A')+10) : ((c)-'0'))
#define BYTES_TO_KBPS(n) (float)(((((float)n)*8.0f)/1024.0f))

class CM_API Stream
{
public:
    Stream()
        : writeCRLF(true)
        , bitsBuffer(0)
        , bitsPos(0)
        , totalBytesIn(0)
        , totalBytesOut(0)
        , lastBytesIn(0)
        , lastBytesOut(0)
        , bytesInPerSec(0)
        , bytesOutPerSec(0)
        , lastUpdate(0)
    {
    }
    virtual        ~Stream() {}

    virtual int    readUpto(void *, int)
    {
        return 0;
    }
    virtual int    read(void *, int) = 0;
    virtual int    write(const void *, int) = 0;
    virtual void   close() = 0;

    virtual bool   eof();
    virtual void   rewind();
    virtual int    seekTo(int);

    void           writeTo(Stream &out, int len);
    virtual void   skip(int i);

    virtual void   setPollRead(bool);

    char           readChar();
    short          readShort();
    long           readLong();
    int            readInt();

    int	           readInt24();
    long           readTag();
    int	           readString(char *s, int max);
    virtual bool   readReady();
    virtual int    numPending();
    void	       writeChar(char v);
    void	       writeShort(short v);
    void	       writeLong(long v);
    void           writeInt(int v);
    void	       writeTag(long v);
    void	       writeTag(char id[4]);

    int	           writeUTF8(unsigned int);

    int	           readLine(char *in, int max);

    int		       readWord(char *, int);
//	int		       readBase64(char *, int);

    void	       write(const char *, va_list);
    void	       writeLine(const char *);
    void	       writeLineF(const char *, ...);
    void	       writeString(const char *);
    void	       writeStringF(const char *, ...);


    int		       readBits(int);

//	void	       updateTotals(unsigned int,unsigned int);

    bool	       writeCRLF;
    unsigned char  bitsBuffer;
    unsigned int   bitsPos;

    unsigned int   totalBytesIn, totalBytesOut;
    unsigned int   lastBytesIn, lastBytesOut;
    unsigned int   bytesInPerSec, bytesOutPerSec;
    unsigned int   lastUpdate;
};

class CM_API FileStream : public Stream
{
public:
    FileStream();
    virtual 	   ~FileStream();

    void	       openReadOnly(const char *);
    void	       openWriteReplace(const char *);
    void	       openWriteAppend(const char *);
    bool	       isopen();
    int		       length();
    int		       pos();
    int            copyTo(const char*);


    virtual int    seekTo(int);
    virtual void   flush();
    virtual int	   read(void *, int);
    virtual int    write(const void *, int);
    virtual bool   eof();
    virtual void   rewind();
    virtual void   close();

    

public:
    FILE         * file;
};

#endif
