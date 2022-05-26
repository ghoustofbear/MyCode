/**
 * @file      backrecorder.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-06 11:22:54
 *
 * @note
 *  backrecorder.h defines
 */
#ifndef __BACKRECORDER_H__
#define __BACKRECORDER_H__
#include "def.h"
#include "streamlistener.h"
#include "c2sdk.h"

class IWriter;
class Cu;

class BackRecorder : public StreamListener
{
public:
    BackRecorder(Cu *cu);
    virtual ~BackRecorder();

    int                start(string json, string filepath);
    void               stop();

    int                capture(string filepath, int type);
    int                startREC(string filepath);
    void               stopREC();

    void               timerdriver();
protected:
    virtual void       onStream(void *stream, uint32_t size);
    virtual void       onStatus(int code);

private:
    Cu               * m_cu;
    string             m_json;
    IWriter          * m_writer;
    string             m_filePath;
};

#endif
