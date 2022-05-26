/**
 * @file      lfstreampl.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-12 14:32:51
 *
 * @note
 *  local file stream play defines
 */
#ifndef __PLAY_LFSTRAM_H__
#define __PLAY_LFSTRAM_H__
#include "play_stream.h"

class IReader;
class LFStreamPlay : public StreamPlay
{
public:
    LFStreamPlay(int mode);
    virtual ~LFStreamPlay();

public:
    int                openFile(const char *filename);
    void               closeFile();
    int                getFileTime(datetime_s* sTime, datetime_s* eTime);

    int                setpos(int pos);
    int                getpos();

    virtual int        pause(bool pause);
    virtual int        fast(int &speed);
    virtual int        slow(int &speed);
protected:
    virtual void       vdecloop();
    virtual void       adecloop();

    virtual void       delay(uint64_t pts);
    virtual void       delay(uint32_t fps);

private:
    IReader          * m_rd;
    uint64_t           m_refpts;                           /**< reference pts */
    uint64_t           m_reftime;                          /**< reference time */

    bool               m_pause;

    uint8_t          * m_frame;
};

#endif
