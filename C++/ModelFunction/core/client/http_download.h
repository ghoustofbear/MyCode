/**
 * @file      http_download.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-03 15:58:55
 *   
 * @note
 *  http_download.h defines 
 */
#ifndef __HTTP_DOWNLOAD_H__
#define __HTTP_DOWNLOAD_H__
#include "recordstream.h"
#include "c2sdk.h"

class Player;
class IWriter;

class HttpDownload : public RecordStream
{
#define STATUS(code) \
    do \
    { \
        if (m_statusCB) \
        { \
            m_statusCB(code, m_statusData); \
        } \
    } while(0);

public:
    HttpDownload(Cu *cu, string savepath, bool standard);
    virtual ~HttpDownload();

    void               init(c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

    virtual int        streamCtrl(int code, void *val, int valSize);
protected:
    virtual int        open();
    virtual void       stop();
    virtual int        recv();

    virtual int        pause();
    virtual int        continuex();
    virtual int        heartbeat();

    virtual void       onStream(void *stream, uint32_t size);
    virtual void       onStatus(int code);
    virtual void       onReposEnd();

    virtual void       timerdriver();
protected:
    int                recvMsg(string &method, string &url, int &status, string &json);

    int                recvFrame();

private:
    string             m_savepath;
    bool               m_standard;
    FILE             * m_savefile;
    IWriter          * m_saveStandardFile;

    c2_stream_cb       m_streamCB;
    void             * m_streamData;
    c2_status_cb       m_statusCB;
    void             * m_statusData;
};

#endif
