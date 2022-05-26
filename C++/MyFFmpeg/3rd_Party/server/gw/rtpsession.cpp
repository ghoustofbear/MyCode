#include "rtptriggerlistener.h"
#include "rtpsession.h"
#include "frame.h"
#include "tls_error.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpsourcedata.h"
#include "jrtplib3/rtpsessionparams.h"
#include "pu.h"
#include "PsProcessApi.h"

/* #include "jrtplib3/rtppacket.h" */
/* #include "jrtplib3/rtperrors.h" */
/* #include "jrtplib3/rtcprrpacket.h" */
/* #include "jrtplib3/rtcpsrpacket.h" */

int RTPSessionEx::m_baseport = 10000;
tmap<int, int> RTPSessionEx::m_mapSrcPort;

static int g_ssrc = 1;

void h264_to_ps_cb(uint8_t *data, int size, void* userdata, int type, uint64_t pts,uint64_t frame_time)
{
    RTPSessionEx *rtp = (RTPSessionEx*)userdata;
    if (size > 0 && data && rtp)
    {
        int last = size / MAX_PAYLOAD_SIZE;
        for (int index = 0; index <= last; ++index)
        {
            int len = size - index*MAX_PAYLOAD_SIZE;
            if (len > MAX_PAYLOAD_SIZE)
            {
                len = MAX_PAYLOAD_SIZE;
            }
            rtp->onSendata(data + index*MAX_PAYLOAD_SIZE, len, ((index == last) ? true : false));
        }
    }

}


string GenerateSSRC(const string &addrcode)
{
    char str[32] = { 0 };
    sprintf(str, "%04d", g_ssrc++);
    if (g_ssrc > 9999)
    {
        g_ssrc = 1;
    }

    string ssrc = "ABBBBBCCCC";
    ssrc.replace(0, 0, "0");
    ssrc.replace(1, 5, addrcode.substr(3, 7));
    ssrc.replace(6, 9, str);
    return ssrc;
}

void
RTPSessionEx::setBaseport(int baseport)
{
    m_baseport = baseport;
}

RTPSessionEx::RTPSessionEx()
{
    m_istart  = false;
    m_srcport = -1;
	m_pfnstreamcallback = NULL;
	m_puser = NULL;
    m_ssrc = "";
    m_psPacketor = createpsinstance(PS_PACK);
    setpsstreamcallback(m_psPacketor, h264_to_ps_cb, this);
}

RTPSessionEx::~RTPSessionEx()
{
    clear();

    AutoLock lock(m_mapSrcPort._mtx);
    if (-1 != m_srcport)
    {
        m_mapSrcPort.erase(m_srcport);
    }  
}

int
RTPSessionEx::start(RTPTriggerListener *listener)
{
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    sessparams.SetOwnTimestampUnit(1.0 / 9000.0);
    if (!m_ssrc.empty())
    {
        sessparams.SetUsePredefinedSSRC(true);
        uint32_t ssrc = atoi(m_ssrc.c_str());
        sessparams.SetPredefinedSSRC(ssrc);
    }

	uint32_t srcipn = inet_addr("0.0.0.0");
    if (srcipn == INADDR_NONE)
    {
        ERRE_MSV("inet_addr fail!");
		return -1;    
    }

	srcipn = ntohl(srcipn);
	transparams.SetBindIP(srcipn);

	uint32_t sport = srcport();
	transparams.SetPortbase(sport);

    int result = Create(sessparams, &transparams);
    if (result < 0)
    {
		ERRE_MSV("CreatRTPSession Fail! err:%s, srcport: %d", RTPGetErrorString(result).c_str(), sport);
        return -1;
    }

    m_istart = true;

    return 0;
}

int
RTPSessionEx::stop()
{
    if (m_psPacketor)
    {
        destorypsinstance(m_psPacketor);
        m_psPacketor = NULL;
    }
    m_istart = false;
    BYEDestroy(RTPTime(0, 50), NULL, 0);

    return 0;
}

int
RTPSessionEx::addDestination(string destip, int destport, RTPTriggerListener * listener)
{
    if (!m_istart)
    {
        if (start(listener) < 0)
        {
            return -1;
        }
    }

    uint32_t destipn = inet_addr(destip.c_str());
    if (destipn == INADDR_NONE)
    {
        ERRE_MSV("inet_addr fail! %s", destip.c_str());
    }

    /* destipnnet_addr function returns a value in network byte order, but */
    /* we need the IP address in host byte order, so we use a call to ntohl */
    uint32_t destiph = ntohl(destipn);

    /* add destination ip into session */
    RTPIPv4Address destaddr(destiph, destport);

    int result = AddDestination(destaddr);
    if (result < 0)
    {
        ERRE_MSV("AddDestination fail. err: %s\n", RTPGetErrorString(result).c_str());
        /* ReleaseBuffer(); */
        return -1;
    }

    addRTPTriggerListener(listener);

    return 0;
}

int
RTPSessionEx::delDestination(string destip, int destport, RTPTriggerListener *listener)
{
    uint32_t destipn = inet_addr(destip.c_str());
    if (destipn == INADDR_NONE)
    {
        ERRE_MSV("inet_addr fail! %s", destip.c_str());
    }

    uint32_t destiph = ntohl(destipn);

    RTPIPv4Address dest(destiph, destport);

    int result = DeleteDestination(dest);
    if (result < 0)
    {
        ERRW_MSV("DeleteDestination fail. err: %s\n", RTPGetErrorString(result).c_str());
    }

    delRTPTriggerListener(listener);

    return 0;
}

int
RTPSessionEx::srcport()
{
    do 
    {
        if (-1 == m_srcport)
        {
            m_srcport = m_baseport;
            m_baseport += 2;

            if (m_srcport >= 56636)
            {
                m_baseport = 10000;
                continue;
            }

            AutoLock lock(m_mapSrcPort._mtx);
            if (m_mapSrcPort.find(m_srcport) == m_mapSrcPort.end())
            {
                m_mapSrcPort[m_srcport] = 1;
                break;
            }
        }
    } while (0);
    
    return m_srcport;
}

void
RTPSessionEx::setSsrc(string ssrc)
{
    m_ssrc = ssrc;
}

void
RTPSessionEx::addRTPTriggerListener(RTPTriggerListener *listener)
{
    AutoLock locker(_mtx);
    auto i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
RTPSessionEx::delRTPTriggerListener(RTPTriggerListener *listener)
{
    AutoLock locker(_mtx);
    auto i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

void
RTPSessionEx::notifyHeartbeat(string &destip, int destport)
{
    AutoLock locker(_mtx);
    for (list<RTPTriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (((*i)->destip() == destip) && ((*i)->destport() == destport))
        {
            (*i)->onHeartbeat();
        }
    }
}

void
RTPSessionEx::notifyClose(string &destip, int destport)
{
    AutoLock locker(_mtx);
    for (list<RTPTriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (((*i)->destip() == destip) && ((*i)->destport() == destport))
        {
            (*i)->onClose();
        }
    }
}

void
RTPSessionEx::OnPollThreadStep()
{
    BeginDataAccess();

    if (GotoFirstSourceWithData())
    {
        do
        {
            RTPPacket     *pack;
            RTPSourceData *srcdat;

            srcdat = GetCurrentSourceInfo();

            while ((pack = GetNextPacket()) != NULL)
            {
                ProcessRTPPacket(*srcdat, *pack);
                DeletePacket(pack);
            }
        }
        while (GotoNextSourceWithData());
    }

    EndDataAccess();
}

void
RTPSessionEx::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{
	if (8 == rtppack.GetPayloadType())
	{
		if (NULL != m_pfnstreamcallback)
		{
			m_pfnstreamcallback(rtppack.GetPayloadData(), rtppack.GetPayloadLength(), 1, m_puser);
		}
	}
    else if (96 == rtppack.GetPayloadType())
    {
        bool bMark = false;
        int ret = rtp2ps(rtppack.GetPacketData(), rtppack.GetPacketLength(), bMark);
        if (ret == 0 && m_pfnstreamcallback && bMark)
        {
            m_pfnstreamcallback(m_frame, m_framelen, 2, m_puser);
            m_framelen = 0;
        }
    }
	else
	{
		bool bMark = false;
		int ret = rtp2nal(rtppack.GetPacketData(), rtppack.GetPacketLength(), bMark);
		if (ret < 0)
		{
			return;
		}

		if (NULL != m_pfnstreamcallback && bMark)
		{
			m_pfnstreamcallback(m_frame, m_framelen, 0, m_puser);
			m_framelen = 0;
		}
	}
}

void
RTPSessionEx::setstreamcallback(pfnstreamcallback fnstreamcallback, void *puser)
{
	m_pfnstreamcallback = fnstreamcallback;
	m_puser = puser;
}

void
RTPSessionEx::OnNewSource(RTPSourceData *dat)
{
    if (dat->IsOwnSSRC())
    {
        return;
    }

    uint32_t ip;
    uint16_t port;

	const RTPAddress * rtpaddr = dat->GetRTPDataAddress();
	const RTPAddress * rtcpaddr = dat->GetRTCPDataAddress();

	if (rtpaddr != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort();
    }
	else if (rtcpaddr != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort() - 1;
    }
    else
    {
        return;
    }

    RTPIPv4Address dest(ip, port);

    struct in_addr inaddr;
    inaddr.s_addr = htonl(ip);

    LOGI_MSV("destip:%s, destport:%d", inet_ntoa(inaddr), port);
}

void
RTPSessionEx::OnRemoveSource(RTPSourceData *dat)
{
    if (dat->IsOwnSSRC())
    {
        return;
    }

    if (dat->ReceivedBYE())
    {
        return;
    }

    uint32_t ip;
    uint16_t port;

    if (dat->GetRTPDataAddress() != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort();
    }
    else if (dat->GetRTCPDataAddress() != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
        ip = addr->GetIP();
        port = addr->GetPort() - 1;
    }
    else
    {
        return;
    }

    RTPIPv4Address dest(ip, port);
    DeleteDestination(dest);

    struct in_addr inaddr;
    inaddr.s_addr = htonl(ip);

    LOGI_MSV("delete destination, destip:%s, destport:%d", inet_ntoa(inaddr), port);

    string destip = inet_ntoa(inaddr);
    notifyClose(destip, port);
}

void
RTPSessionEx::OnBYEPacket(RTPSourceData *dat)
{
    if (dat->IsOwnSSRC())
    {
        return;
    }

    uint32_t ip;
    uint16_t port;

    if (dat->GetRTPDataAddress() != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
        ip   = addr->GetIP();
        port = addr->GetPort();
    }
    else if (dat->GetRTCPDataAddress() != 0)
    {
        const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
        ip   = addr->GetIP();
        port = addr->GetPort() - 1;
    }
    else
    {
        return;
    }

    RTPIPv4Address dest(ip, port);
    DeleteDestination(dest);

    struct in_addr inaddr;
    inaddr.s_addr = htonl(ip);

    LOGI_MSV("delete destination, destip:%s, destport:%d", inet_ntoa(inaddr), port);

    string destip = inet_ntoa(inaddr);
    notifyClose(destip, port);
}

void RTPSessionEx::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress)
{
    const RTPIPv4Address *addr = (const RTPIPv4Address *)(senderaddress);

    uint32_t ip   = addr->GetIP();
    uint16_t port = addr->GetPort() - 1;

    struct in_addr inaddr;
    inaddr.s_addr = htonl(ip);

    LOGT_MSV("RTCP heartbeat, destination destip:%s, destport:%d", inet_ntoa(inaddr), port);

    string destip = inet_ntoa(inaddr);
    notifyHeartbeat(destip, port);
}

int
RTPSessionEx::sendByFU(int encode, int type, void *frame, size_t size, uint64_t pts)
{
    m_encode = encode;
    m_type   = type;
    m_pts    = pts;

    if (ENCODE_H264 == encode)
    {
        return h2642nal((const uint8_t *)frame, size);
    }
    else if (ENCODE_HEVC == encode)
    {
        return hevc2nal((const uint8_t *)frame, size);
    }
    else if (ENCODE_G711A == encode)
    {
        return onSendata(frame, size, true);
    }
	else if (ENCODE_G711U == m_encode)
	{
		return onSendata(frame, size, true);
	}

    return -1;
}

int
RTPSessionEx::sendByPS(int encode, int type, void *frame, size_t size, uint64_t pts)
{
    m_encode = ENCODE_PS;
    return h264ToPs((const uint8_t *)frame, size, type, pts);
}

int
RTPSessionEx::sendByTS(int encode, int type, void *frame, size_t size, uint64_t pts)
{
    return -1;
}

int
RTPSessionEx::onSendata(void *data, size_t size, bool mark)
{
    if (ENCODE_H264 == m_encode)
    {
        SendPacket(data, size, 100, mark, 3600);
    }
    else if (ENCODE_HEVC == m_encode)
    {

    }
    else if (ENCODE_G711A == m_encode)
    {
		SendPacket(data, size, 8, 1, 10);
    }
	else if (ENCODE_G711U== m_encode)
	{
		SendPacket(data, size, 8, 1, 10);
	}
    else if (ENCODE_PS == m_encode)
    {
        SendPacket(data, size, 96, mark, 3600);
    }
    return 0;
}
