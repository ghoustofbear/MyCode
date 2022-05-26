#include <stdio.h>
#include "audio_record.h"
#include "3rd/hik/HCNetSDK.h"



void CALLBACK waveInProc(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	AudioRecord* record = (AudioRecord *)dwInstance;

	if(WIM_OPEN == uMsg)
	{
		printf("open mic\n");
	}
	else if(WIM_DATA == uMsg)
	{
#if defined(_WIN32)
		WAVEHDR* pWavHead = (WAVEHDR*)dwParam1; 

		if (3 == record->_encodeType && record->_encoder)
		{
			char encData[80];
			NET_DVR_EncodeG722Frame(record->_encoder, (unsigned char *)pWavHead->lpData, (unsigned char *)encData);
			//NET_DVR_EncodeG722Frame(record->_encoder, (unsigned char *)pWavHead->lpData);

			if (record->_dataCallback)
			{
				record->_dataCallback(3, encData, 80, record->_userData);
			}
		}
		
		waveInPrepareHeader(hWaveIn, pWavHead, sizeof(WAVEHDR)); 
		waveInAddBuffer(hWaveIn, pWavHead, sizeof(WAVEHDR)); 
#endif
	} 
}

AudioRecord::AudioRecord(int encodeType, int channels, int samplesPerSec, int bitPerSample, int samplesSize)
{
	_encodeType    = encodeType;
	_channels      = channels;
	_samplesPerSec = samplesPerSec;
	_bitPerSample  = bitPerSample;
	_samplesSize   = samplesSize;
		
	_wavIn        = NULL;
	_encoder      = NULL;
	_dataCallback = NULL;
	_userData     = NULL;
}

AudioRecord::~AudioRecord()
{

}

int
AudioRecord::start()
{
	WAVEFORMATEX wfx;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= _channels;
	wfx.nSamplesPerSec	= _samplesPerSec;				                        // g711 8KHz sampling, g722 16Kz
	wfx.wBitsPerSample	= _bitPerSample;
	wfx.nBlockAlign		= (wfx.wBitsPerSample/8) * wfx.nChannels;				// ust be equal to the product of nChannels and wBitsPerSample divided by 8 (bits per byte). 
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nChannels * wfx.nBlockAlign;
	wfx.cbSize			= 0;	                                                // must be set to 0
	
	
	if(MMSYSERR_NOERROR != waveInOpen(&_wavIn, WAVE_MAPPER, &wfx, (DWORD)waveInProc, (DWORD)this, CALLBACK_FUNCTION))
	{
		printf("waveInOpen failed\n");
	}

	for (int i = 0; i < MAX_AUDIO_BUFFERS; i++)
	{
		memset(&_wavHead[i], 0, sizeof(WAVEHDR));

		_wavData[i] = new char[_samplesSize];

		_wavHead[i].dwBufferLength = _samplesSize;
		_wavHead[i].lpData         = _wavData[i];

		int res = 0;
		if( res = waveInPrepareHeader(_wavIn, &_wavHead[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			printf("i = %d", i);
			return -1;
		}
		
		if( waveInAddBuffer(_wavIn, &_wavHead[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			printf("i = %d", i);
			return -1;
		}
	}
	
	if (3 == _encodeType)
	{
		 NET_DVR_Init();
		_encoder = NET_DVR_InitG722Encoder();

		if (-1 == (DWORD)_encoder)
		{
			waveInClose(_wavIn);
			return -1;
		}
	}

	waveInStart(_wavIn);

	return 0;
}

void
AudioRecord::stop()
{
	waveInStop(_wavIn);
	waveInClose(_wavIn);

	for (int i = 0; i < MAX_AUDIO_BUFFERS; i++)
	{
		if (_wavData[i])
		{
			delete []_wavData[i];
		}
	}
	
	if (3 == _encodeType)
	{
		NET_DVR_ReleaseG722Encoder(_encoder);
		_encoder = NULL;
		NET_DVR_Cleanup();
	}
}

void 
AudioRecord::setAudioDataCallback(lpAudioDataCallback callback, void* userData)
{
	_dataCallback = callback;
	_userData     = userData;
}