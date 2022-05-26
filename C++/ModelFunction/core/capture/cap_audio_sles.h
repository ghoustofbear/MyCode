#ifndef  _CAP_AUDIO_SLES_H_
#define  _CAP_AUDIO_SLES_H_

#include "cap_audio.h"
#include "thread.h"

#ifdef ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


class AudioEncoder;
class SlesCapAudio : public CapAudio
{
public:
    SlesCapAudio(int mode, int sps, int channels, int bps, int ssize);
    virtual ~SlesCapAudio();

    enum 
    {
        T_CAP_AUDIO,
    };

public:
    virtual int        start();
    virtual void       stop();

    static  void       AudioRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

private:
    bool               createEngine();
    bool               destroyEngine();
    bool               createRecorder();
    bool               destroyRecorder();


private:
    AudioEncoder     * m_g711a;
    AudioEncoder     * m_g711u;
    void             * m_g722;
    uint8_t          * m_g711ab;
    uint8_t          * m_g711ub;

    char*              m_audioBuff;
    int                m_audioBuffSize;

    SLObjectItf        m_engineObject;
    SLEngineItf        m_engineEngine;
    SLObjectItf        m_recorderObject;
    SLRecordItf        m_recorderRecord;
    SLAndroidConfigurationItf m_configItf;
    SLAndroidSimpleBufferQueueItf m_recorderBuffQueueItf;
};
#endif

#endif
