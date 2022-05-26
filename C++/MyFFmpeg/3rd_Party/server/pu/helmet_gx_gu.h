#ifndef __HELMET_GX_GU_H__
#define __HELMET_GX_GU_H__

#include "gu.h"
#include "3rd/helmet/iCVSC.h"
#include "3rd/helmet/iCVSC_base.h"
#include "thread.h"

class HelmetGXGu : public RealGu 
{
public:
    HelmetGXGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    HelmetGXGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~HelmetGXGu();


    virtual int        start(int offset);
    virtual void       stop();
    virtual int        reset();

    virtual int        startDualTalk();
    virtual void       stopDualTalk();

protected:
    virtual void       onTimer();

    static  void     * threadEntry(ThreadInfo *arg);

private:
    int                LiveStart();
    int                LiveStop();


    void               RealDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType);
    void               RawDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType, uint32_t nKeyFrmFlag);
    int                StartVideoGather();
    void               VideoGatherLoop();

//
//    virtual S32 RequestIFrame();
//    virtual S32 PtzCtrl(S32 s32Cmd, S32 s32Speed, S32 s32Stop);
//


private:
    IC_HANDLE  m_lRealHandle;
    IC_HANDLE  m_lAudioHandle;
    IC_HANDLE  m_dualtalkHandle;
    
    
    Thread     m_thread;

    typedef struct X2Q_AUDIO_ALG_HEAD_H
    {
        unsigned char alg;						// 音频帧算法
        unsigned char rsv[3];                   // 保留区域
    }X2Q_AUDIO_ALG_HEAD;

    typedef struct X2Q_VIDEO_HEAD_H
    {
        unsigned short width;                   // 视频宽度
        unsigned short height;                  // 视频高度
        unsigned short producer_id;             // 厂商ID
        unsigned char frm_rate;                 // 帧率
        unsigned char rsv[9];                   // 保留区域
    }X2Q_VIDEO_HEAD;


    typedef struct X2Q_AUDIO_HEAD_H
    {
        uint16_t  block_align;  //每帧字节数
        uint8_t   channel;      //通道数
        uint8_t   bits;         //采样精度
        uint16_t  simple;       //采样率
        uint16_t  frm_num;      //算法帧个数
        uint16_t  producer_id;  //厂商ID
        uint16_t  pcm_len;
        uint8_t   energy;
        uint8_t   rsv[3];
    }X2Q_AUDIO_HEAD;
};

#endif
