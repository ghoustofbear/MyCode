#include "cap_audio_wave_alsa.h"
#include "tls_error.h"
#include "encoder_g711a.h"
#include "encoder_g711u.h"
#include "sys.h"

#ifdef __linux__

WaveCapAudioAlsa::WaveCapAudioAlsa(int mode, int sps, int channels, int bps, int ssize)
	: CapAudio(mode, sps, channels, bps, ssize)
{
	m_g711a = NULL;
	m_g711u = NULL;
	m_g722 = NULL;

	m_g711ab = NULL;
	m_g711ub = NULL;
	m_handle = NULL;
	m_capThread = NULL;
    m_frame = 0;
}

WaveCapAudioAlsa::~WaveCapAudioAlsa()
{

}

int
WaveCapAudioAlsa::start()
{
	if (opendevice() < 0)
	{
		return -1;
	}
	if (initdeviceparam() < 0)
	{
		return -1;
	}

	if (!m_capThread)
	{
		m_capThread = new Thread;
		m_capThread->setEntry(threadProc, this, T_CAP_AUDIO);
		m_capThread->start();
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
	return 0;
}

void
WaveCapAudioAlsa::stop()
{
    if (NULL != m_capThread)
    {
        m_capThread->stop();
        delete m_capThread;
        m_capThread = NULL;
    }

	if (NULL != m_handle)
	{
		snd_pcm_close(m_handle);
		m_handle = NULL;
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

bool
WaveCapAudioAlsa::opendevice(int mode)
{
	if (snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0)
	{
		return false;
    }

	return true;
}

bool
WaveCapAudioAlsa::initdeviceparam()
{
    int err = 0;
	snd_pcm_hw_params_t *param;
	snd_pcm_hw_params_malloc(&param);

    err = snd_pcm_hw_params_any(m_handle, param);
    if (err < 0)
    {
        return false;
    }
    err = snd_pcm_hw_params_set_access(m_handle, param, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        return false;
    }
    err = snd_pcm_hw_params_set_format(m_handle, param, SND_PCM_FORMAT_S16_LE);
    if (err < 0)
    {
        return false;
    }
    unsigned int rate = m_sps;
    err = snd_pcm_hw_params_set_rate_near(m_handle, param, &rate, 0);
    if (err < 0)
    {
        return false;
    }
    err = snd_pcm_hw_params_set_channels(m_handle, param, m_channels);
    if (err < 0)
    {
        return false;
    }

    m_frame = 160;
    snd_pcm_hw_params_set_period_size_near(m_handle,
                                  param, &m_frame, 0);

    err = snd_pcm_hw_params(m_handle, param);
    if (err < 0)
    {
        return false;
    }

	snd_pcm_hw_params_free(param);

	return true;
}

void*
WaveCapAudioAlsa::threadProc(ThreadInfo *info)
{
	WaveCapAudioAlsa * cap = (WaveCapAudioAlsa *)(info->data);

	switch (info->id)
	{
	case T_CAP_AUDIO:
		cap->capture();
		break;
	}
   
	return NULL;
}

void
WaveCapAudioAlsa::capture()
{
    char buf[10*1024] = {0};

    while (m_capThread->active())
	{
        m_frame = m_ssize /2;
        int err = snd_pcm_readi(m_handle, buf, m_frame);    
        if (err == -EPIPE)
		{
            snd_pcm_prepare(m_handle);
		    continue;
		}
        else if (err < 0)
        {
            sys->sleep(10);
            continue;
        }

		do
		{
			if (!this->m_audioCB)
			{
				break;
			}

			if (0x1 & this->m_mode)
			{
				this->m_audioCB(0, buf, m_ssize, sys->currentTimeMillis(), this->m_audioData);
			}
            if (0x2 & this->m_mode)
			{
				if (!this->m_g711a)
				{
					break;
				}

				int size = m_ssize / 2;
				if (!this->m_g711ab)
				{
					this->m_g711ab = (uint8_t *)malloc(size);
				}

				uint64_t pts = 0;
				this->m_g711a->encode(buf, m_ssize, this->m_g711ab, size, pts);

				this->m_audioCB(1, this->m_g711ab, size, pts, this->m_audioData);
			}
            if (0x4 & this->m_mode)
			{
				if (!this->m_g711u)
				{
					break;
				}

				int size = m_ssize / 2;
				if (!this->m_g711ub)
				{
					this->m_g711ub = (uint8_t *)malloc(size);
				}

				uint64_t pts = 0;
				this->m_g711u->encode(buf, m_ssize, this->m_g711ub, size, pts);

				this->m_audioCB(2, this->m_g711ub, size, pts, this->m_audioData);
			}
#if 0
			else if ((0x8 & this->m_mode) && this->_encoder)
			{
				char encData[80];
				NET_DVR_EncodeG722Frame(cap->_encoder, (unsigned char *)wavHdr->lpData, (unsigned char *)encData);

				this->_dataCallback(3, encData, 80, this->_userData);
			}
#endif
		}
		while (0);

		sys->sleep(10);
    }
}

#endif

