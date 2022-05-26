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
#ifndef __HIK_SU_H__
#define __HIK_SU_H__

#include "su.h"
#include "thread.h"
#include "3rd/hik/sdk-plat.h"

class AudioEncoder;
class Pu;
const int SEND_SIZE   = 160;
const int BUFFER_SIZE = 2048;
class HikSu : public Su
{
public:
    HikSu(Pu *pu, int channelNo);
    HikSu(Pu *pu, string addrcode);
    virtual ~HikSu();

    virtual int        start();
    virtual void       stop();
    virtual int        reset();

    virtual void       doVideoStream(void *stream, uint32_t size);
	virtual void       doAudioStream(void *stream, uint32_t size, int flag);
    virtual void       doStatus(int code);

protected:
    void               encode_send(void* data, int size = 320);
    virtual void       onTimer();
    static  void     * threadEntry(ThreadInfo *arg);
    void               recvloop();


private:
    long               m_dualHandle;
    Thread             m_thread;
    int                m_dualtalkCount;
    int                m_vState;   //0:normal   1:OK   2:error

	unsigned char*     m_workspace;

	unsigned char*     m_buff;
	int                m_size;

    AudioEncoder*      m_audio_encode;
};
#endif
