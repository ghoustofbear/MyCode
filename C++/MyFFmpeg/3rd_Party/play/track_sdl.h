/**
 * @file      track_sdl.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-07 15:38:02
 *
 * @note
 *  track_sdl.h defines
 */
#ifndef __TRACK_SDL_H__
#define __TRACK_SDL_H__
#include "audio_track.h"
#include "tqueue.h"

class SDLTrack : public AudioTrack
{
    friend void        mix_audio(void *udata, uint8_t *stream, int len);
public:
    SDLTrack(uint32_t sps, uint8_t channels, uint8_t bps);
    virtual ~SDLTrack();

protected:
    virtual int        init();
    virtual int        openSound();
    virtual int        closeSound();
    virtual int        setVolume(int vol);

protected:
    virtual void       onADPCM(void *data, int size);

protected:
    tqueue<uint8_t *>  m_pcmQu;

private:
    int                m_audioid;
};

#endif
