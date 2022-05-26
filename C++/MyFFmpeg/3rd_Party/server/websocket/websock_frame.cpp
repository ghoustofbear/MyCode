#include "websock_frame.h"
#include <string.h>

DataFrameHeader::DataFrameHeader(byte buffer[], int nBufLen)
{
    if (nBufLen < 2)
    {
        return;
    }

    //第一个字节
    _fin = (buffer[0] & 0x80) == 0x80;
    _rsv1 = (buffer[0] & 0x40) == 0x40;
    _rsv2 = (buffer[0] & 0x20) == 0x20;
    _rsv3 = (buffer[0] & 0x10) == 0x10;
    _opcode = (sbyte)(buffer[0] & 0x0f);

    //第二个字节
    _maskcode = (buffer[1] & 0x80) == 0x80;
    _payloadlength = (sbyte)(buffer[1] & 0x7f);

}

//发送封装数据
DataFrameHeader::DataFrameHeader(bool fin, bool rsv1, bool rsv2, bool rsv3, sbyte opcode, bool hasmask, int length)
{
    _fin = fin;
    _rsv1 = rsv1;
    _rsv2 = rsv2;
    _rsv3 = rsv3;
    _opcode = opcode;
    //第二个字节
    _maskcode = hasmask;
    _payloadlength = (sbyte)length;
}

//返回帧头字节
byte* DataFrameHeader::GetBytes()
{
    byte* buffer = new byte[2];
    memset(buffer, 0, 2);

    if (_fin)
    {
        buffer[0] ^= 0x80;
    }

    if (_rsv1)
    {
        buffer[0] ^= 0x40;
    }

    if (_rsv2)
    {
        buffer[0] ^= 0x20;
    }

    if (_rsv3)
    {
        buffer[0] ^= 0x10;
    }

    buffer[0] ^= (byte)_opcode;

    if (_maskcode)
    {
        buffer[1] ^= 0x80;
    }

    buffer[1] ^= (byte)_payloadlength;

    return buffer;
}

WebSockFrame::WebSockFrame(char* content, int nContentSize)
{
    _mask = new byte[0];
    nMaskLen = 0;
    _content = content;
    int length = nContentSize;
    nContentLen = nContentSize;

    if (length == 0)
    {
        _extend = new byte[0];
        nExtendLen = 0;
        _header = DataFrameHeader(true, false, false, false, 0x0A, false, length);
    }
    else if (length < 126)
    {
        _extend = new byte[0];
        nExtendLen = 0;
        _header = DataFrameHeader(true, false, false, false, 0x02, false, length);
    }
    else if (length < 65536)
    {
        _extend = new byte[2];
        nExtendLen = 2;
        _header = DataFrameHeader(true, false, false, false, 0x02, false, 126);
        _extend[0] = (byte)(length / 256);
        _extend[1] = (byte)(length % 256);
    }
    else
    {
        _extend = new byte[8];
        memset(_extend, 0, 8);

        nExtendLen = 8;
        _header = DataFrameHeader(true, false, false, false, 0x02, false, 127);

        int left = length;
        int unit = 256;

        for (int i = 7; i > 1; i--)
        {
            _extend[i] = (byte)(left % unit);
            left = left / unit;

            if (left == 0)
            {
                break;
            }
        }
    }
}

WebSockFrame::~WebSockFrame()
{
    if (_mask)
    {
        delete[] _mask;
    }

    if (_extend)
    {
        delete[] _extend;
    }
}

byte* WebSockFrame::GetBytes(int& nTotalSize)
{
    nTotalSize = 2 + nExtendLen + nMaskLen + nContentLen;
    byte* buffer = new byte[nTotalSize];
    if (NULL == buffer)
    {
        return NULL;
    }
    memset(buffer, 0, nTotalSize);
    memcpy(buffer, _header.GetBytes(), 2);
    memcpy(buffer + 2, _extend, nExtendLen);
    memcpy(buffer + 2 + nExtendLen, _mask, nMaskLen);
    memcpy(buffer + 2 + nExtendLen + nMaskLen, _content, nContentLen);

    delete[] _header.GetBytes();

    return buffer;
}

byte* WebSockFrame::Mask(byte* data, int nDataLen, byte* mask)
{
    for (int i = 0; i < nDataLen; i++)
    {
        data[i] = (byte)(data[i] ^ mask[i % 4]);
    }

    return data;
}
