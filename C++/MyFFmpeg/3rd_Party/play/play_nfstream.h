/**
 * @file      play_nfstream.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-26 15:06:42
 *   
 * @note
 *  play_nfstream.h defines 
 */
#ifndef __NFSTREAMPLAY_H__
#define __NFSTREAMPLAY_H__
#include "play_stream.h"

#define MAX_FRAMEQU_SIZE        (800)

#define MAX_BUFFER_SIZE         (6*1024*1024)
#define MIN_BUFFER_SIZE         (512*1024)

class NFStreamPlay : public StreamPlay 
{
public:
    NFStreamPlay(int mode);
    virtual ~NFStreamPlay();

public:
    virtual int        inputData(void *data, int size, bool block = false);

    virtual int        pause(bool pause);
    virtual int        fast(int &speed);
    virtual int        slow(int &speed);

protected:
    virtual void       vdecloop();
    virtual void       adecloop();

protected:
    void               delay(uint64_t pts);
    void               delay(uint32_t fps);

private:
    uint64_t           m_refpts;                           /**< reference pts */
    uint64_t           m_reftime;                          /**< reference time */

    bool               m_pause;
	uint64_t           m_bufsize;
};
#endif
