#include "track_sles.h"
#include "tls_error.h"
#include "mempool.h"

void
SlesTrack::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context)
{
#if 0
    static FILE* sfi = NULL;

    if (sfi == NULL)
    {
        sfi = fopen("/sdcard/helmet.pcm", "rb");
    }



    if (!sfi)
    {
        return;
    }

    if (feof(sfi) == 0)
    {
        int len = fread(buf, 1, 960, sfi);
        if (len > 0)
        {
            (*bufferQueueItf)->Enqueue(bufferQueueItf, buf, len);
        }
    }
#endif

    SlesTrack *track = (SlesTrack *)context;
    uint8_t * pcm = NULL;
    while (!track->m_bExitThread)
    {
        {
            AutoLock locker(track->m_pcmQu._mtx);
            if (track->m_pcmQu.size() > 0)
            {
                pcm = track->m_pcmQu.front();
                track->m_pcmQu.pop();
            }
        }
        
        if (pcm)
        {
            FraIf_S * fi = (FraIf_S *)pcm;
            memcpy(track->m_tmpBuf, fi->buf, fi->size);
            (*bufferQueueItf)->Enqueue(bufferQueueItf, track->m_tmpBuf, fi->size);
            mempool->free(pcm);
            break;
        }
        else
        {
            //ERRW_PLY("wait pcm\n");
            sys->sleep(10);
        }
    }
}

SlesTrack::SlesTrack(uint32_t sps, uint8_t channels, uint8_t bps)
    : AudioTrack(sps, channels, bps)
{
    m_engineObject = NULL;
    m_engineEngine = NULL;
    m_playerObject = NULL;
    m_playerPlay   = NULL;
    m_playerVolume = NULL;
    m_outputMixObject = NULL;
    m_playerBufferQueueItf = NULL;
    m_tmpBuf = new char[m_tmpBufSize];

    m_bExitThread = false;

    ERRW_PLY("sps:%d channels:%d bps:%d\n", sps, channels, bps);
}

SlesTrack::~SlesTrack()
{
    m_bExitThread = true;

    if (m_playerPlay != NULL)
    {
        SLresult result;
        result = (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_STOPPED);
        if (SL_RESULT_SUCCESS != result)
        {
            ERRW_PLY("m_playerPlay SetPlayState \n");
        }
    }

    destroyAudioPlayer();
    destroyEngine();

    if (m_tmpBuf)
    {
        delete [] m_tmpBuf;
        m_tmpBuf = NULL;
    }

    AutoLock locker(m_pcmQu._mtx);
    while (!m_pcmQu.empty())
    {
        mempool->free(m_pcmQu.front());
        m_pcmQu.pop();
    }
}

int
SlesTrack::init()
{
    if (!createEngine())
    {
        return -1;
    }
    if (!createAudioPlayer())
    {
        destroyEngine();
        return -1;
    }

    m_bExitThread = false;
    m_init = true;
    return 0;
}

bool SlesTrack::createEngine()
{
    SLEngineOption EngineOption[] = {
                {(SLuint32) SL_ENGINEOPTION_THREADSAFE, (SLuint32) SL_BOOLEAN_TRUE}};
    SLresult result;
    result = slCreateEngine(&m_engineObject, 1, EngineOption, 0, NULL, NULL);

    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("slCreateEngine failed \n");
        return false;
    }

    /* Realizing the SL Engine in synchronous mode. */
    result = (*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("engineObject Realize failed \n");
        return false;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engineEngine);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("engineObject GetInterface failed \n");
        return false;
    }
    return true;
}

bool SlesTrack::createAudioPlayer()
{
    SLresult result;

    // 创建混音器
    const SLInterfaceID mids[1] = {SL_IID_PLAYBACKRATE};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*m_engineEngine)->CreateOutputMix(m_engineEngine, &m_outputMixObject, 0, 0, 0);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_engineEngine CreateOutputMix failed \n");
        return false;
    }

    result = (*m_outputMixObject)->Realize(m_outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_outputMixObjext Realize failed \n");
        return false;
    }

    // 创建播放器
    //createAudioPlayer(engineEngine, outputMixObjext, playerObject);
    SLDataLocator_AndroidSimpleBufferQueue dataSourceLocator = {
                SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                10
    };

    SLDataFormat_PCM dataSourceFormat = {
            SL_DATAFORMAT_PCM,
            1,
            SL_SAMPLINGRATE_8,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource dataSource = {
            &dataSourceLocator,
            &dataSourceFormat
    };

    SLDataLocator_OutputMix dataSinkLocator = {
            SL_DATALOCATOR_OUTPUTMIX, // 定位器类型
            m_outputMixObject // 输出混合
    };

    SLDataSink dataSink = {
            &dataSinkLocator, // 定位器
            0,
    };

    // 需要的接口
    SLInterfaceID interfaceIDs[2] = {
            SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            SL_IID_VOLUME
    };
    SLboolean requiredInterfaces[2] = {
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE
    };

    // 创建音频播放对象
    result = (*m_engineEngine)->CreateAudioPlayer(
            m_engineEngine,
            &m_playerObject,
            &dataSource,
            &dataSink,
            2,
            interfaceIDs,
            requiredInterfaces
    );

    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_engineEngine CreateAudioPlayer failed \n");
        return false;
    }

    result = (*m_playerObject)->Realize(m_playerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerObject Realize failed \n");
        return false;
    }

    result = (*m_playerObject)->GetInterface(m_playerObject, SL_IID_PLAY, &m_playerPlay);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerObject GetInterface SL_IID_PLAY failed \n");
        return false;
    }

    result = (*m_playerObject)->GetInterface(m_playerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                &m_playerBufferQueueItf);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerObject GetInterface failed \n");
        return false;
    }

    result = (*m_playerBufferQueueItf)->RegisterCallback(m_playerBufferQueueItf, AudioPlayerCallback,
                                                    this);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerBufferQueueItf RegisterCallback failed \n");
        return false;
    }

    result = (*m_playerObject)->GetInterface(m_playerObject, SL_IID_VOLUME, &m_playerVolume);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerObject GetInterface SL_IID_VOLUME failed \n");
        return false;
    }

    result = (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerPlay SetPlayState failed \n");
        return false;
    }

    (*m_playerBufferQueueItf)->Enqueue(m_playerBufferQueueItf, "", 1);
	
    return true;
}

bool SlesTrack::destroyEngine()
{
    if (m_engineObject != NULL)
    {
        (*m_engineObject)->Destroy(m_engineObject);
        m_engineObject = NULL;
        m_engineEngine = NULL;
    }
    return true;
}

bool SlesTrack::destroyAudioPlayer()
{
    if (m_playerObject != NULL) {
        (*m_playerObject)->Destroy(m_playerObject);
        m_playerObject = NULL;
        m_playerPlay = NULL;
        m_playerBufferQueueItf = NULL;
        m_outputMixObject = NULL;
    }

}

int
SlesTrack::openSound()
{
    if (SL_RESULT_SUCCESS != (*m_playerVolume)->SetMute(m_playerVolume, SL_BOOLEAN_FALSE))
    {
        ERRW_PLY("m_playerVolume SetVolumeLevel failed \n");
        return -1;
    }
    else
    {
        return 0;
    }
}

int
SlesTrack::closeSound()
{
    if (SL_RESULT_SUCCESS != (*m_playerVolume)->SetMute(m_playerVolume, SL_BOOLEAN_TRUE))
    {
        ERRW_PLY("m_playerVolume SetVolumeLevel failed \n");
        return -1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief SlesTrack::setVolume
 * @param vol [0, 100]
 * @return
 */
int
SlesTrack::setVolume(int vol)
{
    if (NULL == m_playerVolume)
    {
        return -1;
    }

    SLresult result = (*m_playerVolume)->SetVolumeLevel(m_playerVolume, (SLmillibel) ((1.0f - vol / 100.0f) * -5000));
    if (SL_RESULT_SUCCESS != result)
    {
        ERRW_PLY("m_playerVolume SetVolumeLevel failed \n");
        return -1;
    }
    else
    {
        return 0;
    }
}

void
SlesTrack::onADPCM(void *data, int size)
{
    if (!m_init)
    {
        if (0 != init())
        {
            return;
        }
    }

    {
        uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(FraIf_S));
        FraIf_S *hdr = (FraIf_S *)buf;
        memcpy(hdr->buf, data, size);
        hdr->size = size;
        AutoLock locker(m_pcmQu._mtx);
        m_pcmQu.push(buf);
    }
}
