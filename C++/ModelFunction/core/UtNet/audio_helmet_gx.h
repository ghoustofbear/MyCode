#ifndef  __AUDIO_HELMET_GX_H__
#define  __AUDIO_HELMET_GX_H__

#include "helmet_interface_player.h"


typedef void (* pfnHelmetAudioDataCallback)(void* data, int size, void* userData);

class AudioHelmetGx
{
public:
	AudioHelmetGx();

	~AudioHelmetGx();

    int Start();

    void Stop();

	void SetAudioDataCallback(pfnHelmetAudioDataCallback fnHelmetAudioDataCallback, void *puser);



public:
	AUDIOCAPENC_HANDLE m_hAudioCapEnc;

	HANDLE  m_trdAudio;
	BOOL    m_bExit;
	pfnHelmetAudioDataCallback m_fnHelmetAudioDataCallback;
	void * m_pUser;
	char * m_pFrameData;
};

#endif