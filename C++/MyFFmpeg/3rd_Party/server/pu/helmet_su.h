/**
 * @file      helmet_su.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 15:14:00
 *
 * @note
 *  helmet_su.h defines
 */
#ifndef __HELMET_SU_H__
#define __HELMET_SU_H__

#include "su.h"
#include "thread.h"
#include "helmet_interface.h"


class Pu;
class AudioEncoder;

class HelmetSu : public Su
{
public:
    HelmetSu(Pu *pu, int channelNo);
    HelmetSu(Pu *pu, string addrcode);
    virtual ~HelmetSu();

    virtual int        start();
    virtual void       stop();
    virtual int        reset();

    virtual void       doVideoStream(void *stream, uint32_t size);
    virtual void       doAudioStream(void *stream, uint32_t size, int flag);
    virtual void       doStatus(int code);

protected:
    void               encode(void* srcData, int size, void* desData, int& desSize);
    virtual void       onTimer();
    static  void     * threadEntry(ThreadInfo *arg);
    void               recvloop();


private:
    IC_HANDLE          m_dualHandle;
    Thread             m_thread;
    AudioEncoder*      m_audio_encode;



    char*              m_makeBuf;
    char*              m_encBuf;
    int                m_useLen;
    const  int         TMPBUFLEN = 1024 * 16;
};
#endif
