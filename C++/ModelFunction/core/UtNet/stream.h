/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * stream.h
 */
#ifndef _STREAM_H
#define _STREAM_H

// -------------------------------------

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "excep.h"
#include "defc1.h"

#define SWAP2(v) ( ((v&0xff)<<8) | ((v&0xff00)>>8) )
#define SWAP3(v) (((v&0xff)<<16) | ((v&0xff00)) | ((v&0xff0000)>>16) )
#define SWAP4(v) (((v&0xff)<<24) | ((v&0xff00)<<8) | ((v&0xff0000)>>8) | ((v&0xff000000)>>24))
#define TOUPPER(c) ((((c) >= 'a') && ((c) <= 'z')) ? (c)+'A'-'a' : (c))
#define TONIBBLE(c) ((((c) >= 'A')&&((c) <= 'F')) ? (((c)-'A')+10) : ((c)-'0'))
#define BYTES_TO_KBPS(n) (float)(((((float)n)*8.0f)/1024.0f))

#ifndef _WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// -------------------------------------
class Stream
{
	public:
		Stream()
			:writeCRLF(true)
			 ,bitsBuffer(0)
			 ,bitsPos(0)
			 ,totalBytesIn(0)
			 ,totalBytesOut(0)
			 ,lastBytesIn(0)
			 ,lastBytesOut(0)
			 ,bytesInPerSec(0)
			 ,bytesOutPerSec(0)
			 ,lastUpdate(0)
	{
	}
		virtual      ~Stream() {}

		virtual int  readUpto(void *,int) {return 0;}
		virtual int  read(void *,int)=0;
		virtual int  write(const void *,int) = 0;
		virtual bool eof()
		{
			throw StreamException("Stream can`t eof");
			return false;
		}

		virtual void rewind()
		{
			throw StreamException("Stream can`t rewind");
		}

		virtual int seekTo(int)
		{
			throw StreamException("Stream can`t seek");
		}

		void            writeTo(Stream &out, int len);
		virtual void    skip(int i);

		virtual void    close() { }

		virtual void	setReadTimeout(unsigned int ) { }
		virtual void	setWriteTimeout(unsigned int ) { }
		virtual void	setPollRead(bool) { }

		virtual int		getPosition() {return 0;}


		// binary
		char	readChar()
		{
			char v;
			read(&v,1);
			return v;
		}

		short	readShort()
		{
			short v;
			read(&v,2);
			return v;
		}

		long	readLong()
		{
			long v;
			read(&v,4);
			return v;
		}

		int readInt()
		{
			return readLong();
		}
#if 0
		ID4 readID4()
		{
			ID4 id;
			read(id.getData(),4);
			return id;
		}
#endif
		int	readInt24()
		{
			int v=0;
			read(&v,3);
			return v;
		}

		long readTag()
		{
			long v = readLong();
			return SWAP4(v);
		}

		int	readString(char *s, int max)
		{
			int cnt=0;
			while (max)
			{
				char c = readChar();
				*s++ = c;
				cnt++;
				max--;
				if (!c)
					break;
			}
			return cnt;
		}

		virtual bool readReady()  { return true; }
		virtual int  numPending() { return 0; }


#if 0
		void writeID4(ID4 id)
		{
			write(id.getData(),4);
		}
#endif

		void	writeChar(char v)    { write(&v,1); }
		void	writeShort(short v)  { write(&v,2); }
		void	writeLong(long v)    { write(&v,4); }
		void    writeInt(int v)      { writeLong(v);} 
		void	writeTag(long v)     { writeLong(SWAP4(v)); }

		void	writeTag(char id[4]) { write(id,4); }

		int	    writeUTF8(unsigned int);

		// text
		int	    readLine(char *in, int max);

		int		readWord(char *, int);
		//		int		readBase64(char *, int);

		void	write(const char *,va_list);
		void	writeLine(const char *);
		void	writeLineF(const char *,...);
		void	writeString(const char *);
		void	writeStringF(const char *,...);


		int		readBits(int);

		//	void	updateTotals(unsigned int,unsigned int);


		bool	      writeCRLF;
		unsigned char bitsBuffer;
		unsigned int  bitsPos;

		unsigned int  totalBytesIn,totalBytesOut;
		unsigned int  lastBytesIn,lastBytesOut;
		unsigned int  bytesInPerSec,bytesOutPerSec;
		unsigned int  lastUpdate;

};


// -------------------------------------
class FileStream : public Stream
{
	public:
		FileStream() {file = NULL;}
		virtual 	~FileStream() { if (file) fclose(file); };

		void	openReadOnly(const char *);
		void	openWriteReplace(const char *);
		void	openWriteAppend(const char *);
		bool	isOpen(){return file!=NULL;}
		int		length();
		int		pos();

		virtual int     seekTo(int);
		virtual int		getPosition() {return pos();}
		virtual void	flush();
		virtual int		read(void *,int);
		virtual int     write(const void *,int);
		virtual bool	eof();
		virtual void	rewind();
		virtual void	close();

	public:
		FILE           *file;
};

#endif

