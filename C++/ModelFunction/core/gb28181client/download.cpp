#include "download.h"
#include "rtpsession.h"
#include "frame.h"
#include "3rd/hik/sdk-plat.h"
#include "stk/stk_message_rtsp.h"


#define  GB28181_DOWNLOAD_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Download\r\n" \
    "u=%s:%d\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=%d %d\r\n" \
    "m=video %d RTP/AVP 96 97 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=recvonly\r\n" \
    "a=filesize:%d\r\n" \
    "y=%d\r\n\r\n"


#define GB28181_RTSP_CONTINUE \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Range: npt=now-\r\n\r\n"
        

#define GB28181_RTSP_PAUSE \
    "PAUSE RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "PauseTime: now\r\n\r\n"
    




Download::Download(GB28181_SipUA *pSipUa)
{
	m_pSipUa = pSipUa;   
    m_cseq = 1;
    m_rtpSession = new RTPSessionEx();
    m_file = NULL;
    m_offset = 0;
    m_fileSize = 0;
}

Download::~Download()
{
    stop();
}

bool Download::start(const char *cameracode, const char *host, int port, const char *savefile, const char *recordfile, uint32_t startt, uint32_t end, uint64_t filesize)
{
	if (!m_pSipUa || !m_rtpSession)
	{
		return false;
	}

    m_file = fopen(savefile, "wb");
    if (m_file == NULL)
    {
        LOGE_SIP("download file:%s can not open!", savefile);
        return false;
    }

	string resp;
	char sdp[1024] = { 0 };
    sprintf(sdp, GB28181_DOWNLOAD_VIDEO_SDP_FORMAT,
		cameracode, m_pSipUa->ip().c_str(), 
        recordfile, 1,
		m_pSipUa->ip().c_str(), 
		startt, end,
		m_rtpSession->srcport(),
        231,
        8989);

	int ret = m_pSipUa->invite(cameracode, host, port, sdp, resp);
	if (ret < 0)
	{
		return false;
	}
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(resp, rvalue, false))
	{
		return  false;
	}

	m_iConId = atoi(rvalue["cid"].asString().c_str());
	m_iDialId = atoi(rvalue["did"].asString().c_str());
	int destport = rvalue["videoport"].asInt();

	ret = m_rtpSession->addDestination(host, destport, this);
	if (ret < 0)
	{
		m_pSipUa->bye(m_iConId, m_iDialId);
		m_iConId = -1;
		m_iDialId = -1;
		return false;
	}
	m_rtpSession->setstreamcallback(stream_cb, this);


    m_fileSize = filesize;
	m_strCamCode = cameracode;
    m_host = host;
    m_port = port;
    

	return true;
}


void Download::status_cb(int code, void* user_data)
{

}

void Download::stop()
{
	if ((-1 != m_iConId) && (-1 != m_iDialId))
	{
		m_pSipUa->bye(m_iConId, m_iDialId);
		m_iConId = -1;
		m_iDialId = -1;
	}

	if (NULL != m_rtpSession)
	{
		m_rtpSession->stop();
		delete m_rtpSession;
		m_rtpSession = NULL;
	}

    fclose(m_file);
    m_file = NULL;
}

void Download::playCtrl(int cmd, int param)
{

}

int
Download::downloadPos()
{
    if (m_fileSize <= 0)
    {
        return -1;
    }

    int process = (m_offset * 100) / m_fileSize;
    return process;
}

void Download::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
	Download * pthis = (Download*)puser;

    if (pthis->m_file)
    {
        //printf("recv frame size:%d type:%d\n", size, type);
        fwrite(stream, size, 1, pthis->m_file);

        pthis->m_offset += size;
    }
}

void Download::onHeartbeat()
{

}

void Download::onClose()
{

}