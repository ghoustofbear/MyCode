/**
 * @file      streamlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 09:14:04
 *   
 * @note
 *  streamlistener.h defines 
 */
#ifndef __STREAMLISTENER_H__
#define __STREAMLISTENER_H__
#include "def.h"

class StreamListener
{
public:
    virtual void       onStream(void *stream, uint32_t size) = 0;
    virtual void       onStatus(int code) = 0;
#ifdef UNIX_DRAW_RECT
	virtual void       onDrawRoi(string strAlarmMsg) {}
	virtual void       onCtrlROI(string strAlarmMsg) {}
#endif
};

class StreamSpeaker
{
public:
    enum SS_TYPE
    {
        SS_A_SPEAKER,
        SS_V_SPEAKER,
        SS_AV_SPEAKER,
    };

    StreamSpeaker(int t = SS_A_SPEAKER) :m_ssType(t){}
    
    int speakerType() const { return m_ssType; }

    virtual void       doStream(void *stream, uint32_t size) = 0;
    //virtual void       doStatus(int code) = 0;
private:
    int                m_ssType;
};


class AudioStreamListener
{
public:
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts) = 0;
    
};

#endif
