#include "track_wave.h"
#include "mempool.h"
#include "tls_error.h"

WaveTrack::WaveTrack(uint32_t sps, uint8_t channels, uint8_t bps)
    : AudioTrack(sps, channels, bps)
{
    m_waveout = NULL;
    m_hdridx  = 0;
}

WaveTrack::~WaveTrack()
{
    if (m_waveout != NULL)
    {
        waveOutReset(m_waveout);

        for (int i = 0; i < MAX_WAVEHDR_NUM; i++)
        {
            waveOutUnprepareHeader(m_waveout, m_wavehdrs[i], sizeof(WAVEHDR));
            mempool->free(m_wavehdrs[i]->lpData);
            delete m_wavehdrs[i];
        }

        waveOutClose(m_waveout);
        m_waveout = NULL;
    }
}

int
WaveTrack::init()
{
    if (!m_waveout)
    {
        WAVEFORMATEX wfx;

        wfx.wFormatTag      = WAVE_FORMAT_PCM;
        wfx.nChannels       = m_channels;
        wfx.wBitsPerSample  = m_bps;
        wfx.nSamplesPerSec  = m_sps;
        wfx.nBlockAlign     = wfx.wBitsPerSample / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize          = 0;

        MMRESULT result;
        result = waveOutOpen(&m_waveout, WAVE_MAPPER, &wfx, (DWORD)0, (DWORD)0, CALLBACK_NULL);
        if (result != MMSYSERR_NOERROR)
        {
            //ERRE_PLY("waveOutOpen failed, err = %u", result);
            m_waveout = NULL;
            return -1;
        }

        for (int i = 0; i < MAX_WAVEHDR_NUM; i++)
        {
            m_wavehdrs[i] = new WAVEHDR;
            memset(m_wavehdrs[i], 0, sizeof(WAVEHDR));

            m_wavehdrs[i]->lpData         = (LPSTR)mempool->alloc(MAX_WAVEBUF_SIZE);
            m_wavehdrs[i]->dwBufferLength = MAX_WAVEBUF_SIZE;
            memset(m_wavehdrs[i]->lpData, 0, MAX_WAVEBUF_SIZE);

            waveOutPrepareHeader(m_waveout, m_wavehdrs[i], sizeof(WAVEHDR));
        }
    }

    m_init = true;
    return 0;
}

int
WaveTrack::openSound()
{
    return waveOutRestart(m_waveout);
}

int
WaveTrack::closeSound()
{
    return waveOutPause(m_waveout);
}

int
WaveTrack::setVolume(int vol)
{
    unsigned short a = 0xffff / 0xa;
    DWORD v = (a*vol) | (a*vol) << 16;
    return waveOutSetVolume(m_waveout, v);
}

void
WaveTrack::onADPCM(void *data, int size)
{
    if (!m_init)
    {
        init();
    }

    int hdrNum = size / MAX_WAVEBUF_SIZE;
    int offset = 0;

    for (int i = 0; i < hdrNum; i++)
    {
        if (m_waveout)
        {
            memcpy(m_wavehdrs[m_hdridx]->lpData, (LPSTR)data + offset, MAX_WAVEBUF_SIZE);
            waveOutWrite(m_waveout, m_wavehdrs[m_hdridx], sizeof(WAVEHDR));
            if (++m_hdridx >= MAX_WAVEHDR_NUM)
            {
                m_hdridx = 0;
            }
        }

        offset += MAX_WAVEBUF_SIZE;
    }
}
