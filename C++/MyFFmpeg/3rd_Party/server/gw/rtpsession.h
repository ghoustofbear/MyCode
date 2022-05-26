/**
 * @file      rtpsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-20 11:41:58
 *
 * @note
 *  rtpsession.h defines
 */
#ifndef __RTPSESSION_H__
#define __RTPSESSION_H__

#include "tmap.h"
#include "tlist.h"
#include "rtparser.h"
#include "jrtplib3/rtpsession.h"

using namespace jrtplib;
std::string GenerateSSRC(const std::string &addrcode);

class RTPTriggerListener;
class RTPSessionEx : public RTPSession, public RTParser, public tlist<RTPTriggerListener *>
{
public:
    RTPSessionEx();
    virtual ~RTPSessionEx();

public:
    int                start(RTPTriggerListener *listener);
    int                stop();

    int                addDestination(string destip, int destport, RTPTriggerListener *listener);
    int                delDestination(string destip, int destport, RTPTriggerListener *listener);

    int                srcport();

    int                sendByFU(int encode, int type, void *frame, size_t size, uint64_t pts);
    int                sendByPS(int encode, int type, void *frame, size_t size, uint64_t pts);
    int                sendByTS(int encode, int type, void *frame, size_t size, uint64_t pts);

	typedef void(*pfnstreamcallback)(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
	void               setstreamcallback(pfnstreamcallback fnstreamcallback, void *puser);

    void                 setSsrc(string ssrc);
    virtual int        onSendata(void *data, size_t size, bool mark);
protected:
    void               addRTPTriggerListener(RTPTriggerListener *listener);
    void               delRTPTriggerListener(RTPTriggerListener *listener);

    void               notifyHeartbeat(string &destip, int destport);
    void               notifyClose(string &destip, int destport);

protected:

protected:
    virtual void       OnPollThreadStep();
    void 			   OnNewSource(RTPSourceData *dat);
    void               OnRemoveSource(RTPSourceData *dat);
    void			   OnBYEPacket(RTPSourceData *dat);
    void               OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress);
    /* RTP数据包处理函数 */
    void               ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);


public:
    static void        setBaseport(int port);

private:
    volatile bool      m_istart;
    int                m_srcport;

    static int         m_baseport;
	pfnstreamcallback m_pfnstreamcallback;
	void             * m_puser;

    static  tmap<int, int>  m_mapSrcPort;
    string              m_ssrc;
};
#endif
