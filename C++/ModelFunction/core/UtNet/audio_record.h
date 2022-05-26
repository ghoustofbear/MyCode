#ifndef __AUDIO_RECORD_H__
#define __AUDIO_RECORD_H__ 

#include <Windows.h>
#include <MMSystem.h>
//#include "def.h"

#define  MAX_AUDIO_BUFFERS   1

typedef void (* lpAudioDataCallback)(int type, void* data, int size, void* userData);

class AudioRecord
{
	friend void CALLBACK waveInProc(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
public:
	AudioRecord(int encodeType, int channels, int samplesPerSec, int bitPerSample, int samplesSize);
	~AudioRecord();

	int     start();
	void    stop();

	void    setAudioDataCallback(lpAudioDataCallback callback, void* userData);

private:
    int    _channels; 
	int    _samplesPerSec;
	int    _bitPerSample ;
	int    _samplesSize;

	HWAVEIN  _wavIn;
	WAVEHDR  _wavHead[MAX_AUDIO_BUFFERS];
	char   * _wavData[MAX_AUDIO_BUFFERS];

protected:
	void   * _encoder;
	int      _encodeType;               /**< 0-pcm 1-g711A 2-g711u 3-hkg722 */

	lpAudioDataCallback _dataCallback;
	void   *            _userData;
};
#endif
