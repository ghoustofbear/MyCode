#include "cap_audio_sles.h"
#include "tls_error.h"
#include "encoder_g711a.h"
#include "encoder_g711u.h"
#include "sys.h"

#ifdef ANDROID



void
SlesCapAudio::AudioRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context)
{
    SlesCapAudio* pthis = (SlesCapAudio*)context;

    do
    {
        if (!pthis->m_audioCB)
        {
            break;
        }

        if (0x1 & pthis->m_mode)
        {
            //ERRE_SDK("audio call back s:%d\n", pthis->m_audioBuffSize);
            pthis->m_audioCB(0, pthis->m_audioBuff, pthis->m_audioBuffSize, sys->currentTimeMillis(), pthis->m_audioData);
        }
        else if (0x2 & pthis->m_mode)
        {
            if (!pthis->m_g711a)
            {
                break;
            }

            int size = pthis->m_audioBuffSize / 2;
            if (!pthis->m_g711ab)
            {
                pthis->m_g711ab = (uint8_t *)malloc(size);
            }

            uint64_t pts = 0;
            pthis->m_g711a->encode(pthis->m_audioBuff, pthis->m_audioBuffSize, pthis->m_g711ab, size, pts);

            pthis->m_audioCB(1, pthis->m_g711ab, size, pts, pthis->m_audioData);
        }
        else if (0x4 & pthis->m_mode)
        {
            if (!pthis->m_g711u)
            {
                break;
            }

            int size = pthis->m_audioBuffSize / 2;
            if (!pthis->m_g711ub)
            {
                pthis->m_g711ub = (uint8_t *)malloc(size);
            }

            uint64_t pts = 0;
            pthis->m_g711u->encode(pthis->m_audioBuff, pthis->m_audioBuffSize, pthis->m_g711ub, size, pts);

            pthis->m_audioCB(2, pthis->m_g711ub, size, pts, pthis->m_audioData);
        }

    }while (0);

    SLresult result;
    SLuint32 state;
    result = (*pthis->m_recorderRecord)->GetRecordState(pthis->m_recorderRecord, &state);
    if (SL_RESULT_SUCCESS == result || state == SL_RECORDSTATE_RECORDING)
    {
        (*bufferQueueItf)->Enqueue(bufferQueueItf, pthis->m_audioBuff, pthis->m_audioBuffSize);
    }
}


SlesCapAudio::SlesCapAudio(int mode, int sps, int channels, int bps, int ssize)
	: CapAudio(mode, sps, channels, bps, ssize)
{
	m_g711a = NULL;
	m_g711u = NULL;
	m_g722 = NULL;
	m_g711ab = NULL;
	m_g711ub = NULL;


    m_audioBuffSize = ssize;
    m_audioBuff = new char[ssize];

    ERRE_SDK("SlesCapAudio::SlesCapAudio sps:%d chn:%d bps:%d ssize:%d\n", sps, channels, bps, ssize);

    m_engineObject = NULL;
    m_engineEngine = NULL;
    m_recorderObject = NULL;
    m_configItf = NULL;
    m_recorderBuffQueueItf = NULL;
    m_recorderRecord = NULL;
}

SlesCapAudio::~SlesCapAudio()
{

}

int
SlesCapAudio::start()
{
    if (!createEngine())
    {
        return -1;
    }

    if (!createRecorder())
    {
        return -1;
    }

    return 0;
}

void
SlesCapAudio::stop()
{
    if (m_recorderRecord != NULL)
    {
        (*m_recorderRecord)->SetRecordState(m_recorderRecord, SL_RECORDSTATE_STOPPED);
    }

    destroyRecorder();
    destroyEngine();

    if (m_g711a)
    {
        delete m_g711a;
        m_g711a = NULL;
    }

    if (m_g711u)
    {
        delete m_g711u;
        m_g711u = NULL;
    }
    if (m_g711ab)
    {
        free(m_g711ab);
        m_g711ab = NULL;
    }

    if (m_g711ub)
    {
        free(m_g711ub);
        m_g711ub = NULL;
    }

    if (m_audioBuff)
    {
        delete [] m_audioBuff;
        m_audioBuff = NULL;
        m_audioBuffSize = 0;
    }
}


bool SlesCapAudio::createEngine()
{
    SLEngineOption EngineOption[] = {
                {(SLuint32) SL_ENGINEOPTION_THREADSAFE, (SLuint32) SL_BOOLEAN_TRUE}};
    SLresult result;
    result = slCreateEngine(&m_engineObject, 1, EngineOption, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("slCreateEngine failed \n");
        return false;
    }

    /* Realizing the SL Engine in synchronous mode. */
    result = (*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("engineObject Realize failed \n");
        return false;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engineEngine);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("engineObject GetInterface failed \n");
        return false;
    }
    return true;
}

bool SlesCapAudio::destroyEngine()
{
    if (m_engineObject != NULL)
    {
        (*m_engineObject)->Destroy(m_engineObject);
        m_engineObject = NULL;
        m_engineEngine = NULL;
    }
    return true;
}

bool SlesCapAudio::createRecorder()
{
    SLresult result;

    /* setup the data source*/
    SLDataLocator_IODevice ioDevice = {
            SL_DATALOCATOR_IODEVICE,
            SL_IODEVICE_AUDIOINPUT,
            SL_DEFAULTDEVICEID_AUDIOINPUT,
            NULL
    };

    SLDataSource recSource = {&ioDevice, NULL};

    SLDataLocator_AndroidSimpleBufferQueue recBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            1
    };

    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,    // pcm 格式的数据
            (SLuint32)m_channels, // 声道
            SL_SAMPLINGRATE_8,    // 8000hz 的采样频率
            m_bps,
            m_bps,
            SL_SPEAKER_FRONT_CENTER,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSink dataSink = { &recBufferQueue, &pcm };
    SLInterfaceID iids[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    SLboolean required[1] = {SL_BOOLEAN_TRUE};

    /* Create the audio recorder */
    result = (*m_engineEngine)->CreateAudioRecorder(m_engineEngine, &m_recorderObject , &recSource, &dataSink,
                                                  1, iids, required);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("CreateAudioRecorder failed chn:%d bps:%d\n", m_channels, m_bps);
        return false;
    }

    /* get the android configuration interface*/
    result = (*m_recorderObject)->GetInterface(m_recorderObject, SL_IID_ANDROIDCONFIGURATION, &m_configItf);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("GetInterface failed \n");
        //return false;
    }

    /* Realize the recorder in synchronous mode. */
    result = (*m_recorderObject)->Realize(m_recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderObject Realize failed \n");
        return false;
    }

    /* Get the buffer queue interface which was explicitly requested */
    result = (*m_recorderObject)->GetInterface(m_recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, (void*) &m_recorderBuffQueueItf);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderObject GetInterface failed \n");
        return false;
    }

    /* get the record interface */
    result = (*m_recorderObject)->GetInterface(m_recorderObject, SL_IID_RECORD, &m_recorderRecord);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderObject GetInterface failed \n");
        return false;
    }

    result = (*m_recorderBuffQueueItf)->RegisterCallback(m_recorderBuffQueueItf, AudioRecorderCallback, this);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderBuffQueueItf RegisterCallback failed \n");
        return false;
    }

    /* Enqueue buffers to map the region of memory allocated to store the recorded data */
    result = (*m_recorderBuffQueueItf)->Enqueue(m_recorderBuffQueueItf, m_audioBuff, m_audioBuffSize);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderBuffQueueItf Enqueue failed \n");
        return false;
    }

    /* Start recording */
    // 开始录制音频
    result = (*m_recorderRecord)->SetRecordState(m_recorderRecord, SL_RECORDSTATE_RECORDING);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRE_SDK("m_recorderRecord SetRecordState failed \n");
        return false;
    }
    return true;
}

bool SlesCapAudio::destroyRecorder()
{
    if (m_recorderObject != NULL)
    {
        (*m_recorderObject)->Destroy(m_recorderObject);
        m_recorderObject = NULL;
        m_recorderBuffQueueItf = NULL;
        m_configItf = NULL;
    }
    return true;
}


#endif

