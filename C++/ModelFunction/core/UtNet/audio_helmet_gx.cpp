#include "commonc1.h"
#include "audio_helmet_gx.h"

AudioHelmetGx::AudioHelmetGx()
{
	m_hAudioCapEnc = NULL;
	m_trdAudio     = NULL;
	m_bExit        = FALSE;
	m_fnHelmetAudioDataCallback = NULL;
	m_pUser        = NULL;

	m_pFrameData   = new char[1024 * 1024];
}

AudioHelmetGx::~AudioHelmetGx()
{
    Stop();
}

VOID * HelmetGxAudioDataProc(VOID * pParam)
{
    AudioHelmetGx * pThis = (AudioHelmetGx *)pParam;

	int nLen    = 0;
    int nBufLen = 1024 * 1024;

	while(!pThis->m_bExit)
	{
        {
			nLen = fnAudioCapEnc_ReadFrame(pThis->m_hAudioCapEnc, pThis->m_pFrameData, nBufLen);
			if ((NULL != pThis->m_fnHelmetAudioDataCallback) && (nLen > 0))
			{
				pThis->m_fnHelmetAudioDataCallback(pThis->m_pFrameData, nLen, pThis->m_pUser);
			}
		}

		Sleep(20);
	}

	return NULL;
}

int AudioHelmetGx::Start()
{
    int rv = fnAudioCapEnc_Open(1, "G711U", 0, &m_hAudioCapEnc);
	if (0 != rv)
	{
		return -1;
	}

    CreateJoinableThread(HelmetGxAudioDataProc, this, &m_trdAudio);
	return 0;
}

void AudioHelmetGx::Stop()
{
	m_bExit = TRUE;
    if (NULL != m_trdAudio)
	{
        ThreadJoin(m_trdAudio);
		m_trdAudio = NULL;
	}

	if (NULL != m_hAudioCapEnc)
	{
		fnAudioCapEnc_Close(m_hAudioCapEnc);
	}

	if (NULL != m_pFrameData)
	{
		delete [] m_pFrameData;
		m_pFrameData = NULL;
	}
}

void AudioHelmetGx::SetAudioDataCallback(pfnHelmetAudioDataCallback fnHelmetAudioDataCallback, void *puser)
{
    m_fnHelmetAudioDataCallback = fnHelmetAudioDataCallback;
	m_pUser = puser;
}