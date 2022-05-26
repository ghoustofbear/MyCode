#include "cap_audio_wave.h"
#include "tls_error.h"
#include "encoder_g711a.h"
#include "encoder_g711u.h"
#include "sys.h"

void CALLBACK
waveInProc(HWAVEIN hWaveIn, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    WaveCapAudio * cap = (WaveCapAudio *)dwInstance;

    if (WIM_OPEN == uMsg)
    {
        LOGI_CAP("open mic ok");
    }
    else if (WIM_DATA == uMsg)
    {
        WAVEHDR* wavHdr = (WAVEHDR*)dwParam1;

        do
        {
            if (!cap->m_audioCB)
            {
                break;
            }

            if (0x1 & cap->m_mode)
            {
                cap->m_audioCB(0, wavHdr->lpData, wavHdr->dwBufferLength, sys->currentTimeMillis(), cap->m_audioData);
            }
            else if (0x2 & cap->m_mode)
            {
                if (!cap->m_g711a)
                {
                    break;
                }

                int size = wavHdr->dwBufferLength / 2;
                if (!cap->m_g711ab)
                {
                    cap->m_g711ab = (uint8_t *)malloc(size);
                }

                uint64_t pts = 0;
                cap->m_g711a->encode(wavHdr->lpData, wavHdr->dwBufferLength, cap->m_g711ab, size, pts);

                cap->m_audioCB(1, cap->m_g711ab, size, pts, cap->m_audioData);
            }
            else if (0x4 & cap->m_mode)
            {
                if (!cap->m_g711u)
                {
                    break;
                }

                int size = wavHdr->dwBufferLength / 2;
                if (!cap->m_g711ub)
                {
                    cap->m_g711ub = (uint8_t *)malloc(size);
                }

                uint64_t pts = 0;
                cap->m_g711u->encode(wavHdr->lpData, wavHdr->dwBufferLength, cap->m_g711ub, size, pts);

                cap->m_audioCB(2, cap->m_g711ub, size, pts, cap->m_audioData);
            }
#if 0
            else if ((0x8 & cap->m_mode) && cap->_encoder)
            {
                char encData[80];
                NET_DVR_EncodeG722Frame(cap->_encoder, (unsigned char *)wavHdr->lpData, (unsigned char *)encData);

                cap->_dataCallback(3, encData, 80, cap->_userData);
            }
#endif
        }
        while (0);

        waveInPrepareHeader(hWaveIn, wavHdr, sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, wavHdr, sizeof(WAVEHDR));
    }
}

WaveCapAudio::WaveCapAudio(int mode, int sps, int channels, int bps, int ssize)
    : CapAudio(mode, sps, channels, bps, ssize)
{
    m_g711a  = NULL;
    m_g711u  = NULL;
    m_g722   = NULL;

    m_g711ab = NULL;
    m_g711ub = NULL;
}

WaveCapAudio::~WaveCapAudio()
{

}

int
WaveCapAudio::start()
{
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels  = m_channels;

    /* g711 8KHz sampling, g722 16Kz */
    wfx.nSamplesPerSec = m_sps;
    wfx.wBitsPerSample = m_bps;

    /* ust be equal to the product of nChannels and wBitsPerSample divided by 8 (bits per byte). */
    wfx.nBlockAlign     = (wfx.wBitsPerSample / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nChannels * wfx.nBlockAlign;

    /* must be set to 0 */
    wfx.cbSize = 0;


    if (MMSYSERR_NOERROR != waveInOpen(&m_waveIn, WAVE_MAPPER, &wfx, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION))
    {
        ERRE_CAP("waveInOpen failed");
        return -1;
    }

    for (int i = 0; i < MAX_AUDIO_BUFFERS; i++)
    {
        memset(&m_waveHdr[i], 0, sizeof(WAVEHDR));

        m_waveBuf[i] = new char[m_ssize];

        m_waveHdr[i].dwBufferLength = m_ssize;
        m_waveHdr[i].lpData         = m_waveBuf[i];

        if (waveInPrepareHeader(m_waveIn, &m_waveHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            ERRE_CAP("waveInPrepareHeader failed, i: %d", i);
            return -1;
        }

        if (waveInAddBuffer(m_waveIn, &m_waveHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            ERRE_CAP("waveInAddBuffer failed, i: %d", i);
            return -1;
        }
    }

    if (0x2 & m_mode)
    {
        if (!m_g711a)
        {
            m_g711a = new G711AEncoder();
        }
    }
    else if (0x4 & m_mode)
    {
        if (!m_g711u)
        {
            m_g711u = new G711UEncoder();
        }
    }
    else if (0x8 & m_mode)
    {
#if 0
        NET_DVR_Init();
        _encoder = NET_DVR_InitG722Encoder();

        if (-1 == (DWORD)_encoder)
        {
            waveInClose(m_waveIn);
            return -1;
        }
#endif
    }

    waveInStart(m_waveIn);

    return 0;
}

void
WaveCapAudio::stop()
{
    waveInStop(m_waveIn);
    waveInClose(m_waveIn);

    for (int i = 0; i < MAX_AUDIO_BUFFERS; i++)
    {
        if (m_waveBuf[i])
        {
            delete[]m_waveBuf[i];
        }
    }

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

#if 0
    if (_encoder)
    {

        NET_DVR_ReleaseG722Encoder(_encoder);
        _encoder = NULL;
        NET_DVR_Cleanup();
    }
#endif

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
}
