#include "hs_rtspsession.h"
#include "rtpsession.h"
#include "tls_error.h"
#include "json/json.h"
#include "pu.h"

#define  HS_RTSP_SDP_FORMAT \
		"v=0\r\n" \
		"o=- 0 0 IN IP4 %s\r\n" \
		"s=-\r\n" \
		"c=IN IP4 %s\r\n" \
		"t=0 0\r\n" \
		"a=control:*\r\n" \
		"m=video %d RTP/AVP 100\r\n" \
		"a=rtpmap:100 H264/90000\r\n" \
		"a=fmtp:100 packetization-mode=1; profile-level-id=6742E0; sprop-parameter-sets=Z0LgHtoCwEkQAA==,aM4wpIAA\r\n" \
		"a=recvonly\r\n" \
		"a=control:trackID=101\r\n\r\n"

#define HS_RTSP_ECHO_200 \
		"RTSP/1.0 200 OK\r\n" \
		"Server: Unitech RTSP. <caiwang213@qq.com>\r\n\r\n"

#define HS_RTSP_ECHO_400 \
		"RTSP/1.0 400  Bad Request\r\n" \
		"Server: Unitech RTSP. <caiwang213@qq.com>\r\n\r\n"

#define HS_RTSP_ECHO_404 \
		"RTSP/1.0 404 Not Found\r\n" \
		"Server: Unitech RTSP. <caiwang213@qq.com>\r\n\r\n"

HSRTSPSession::HSRTSPSession()
    : RTSPSession()
{

}

HSRTSPSession::~HSRTSPSession()
{

}

void
HSRTSPSession::onOption(stk::RtspMsg &rtsp)
{
    string url = rtsp.GetReqUrl();
    string seq = rtsp.GetCSeq();

    stk::RtspMsg resp(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));

    resp.SetCSeq(seq.c_str());
    resp.SetPublic("DESCRIBE");
    resp.SetPublic("SETUP");
    resp.SetPublic("TEARDOWN,PLAY,GET_PARAMETER,OPTIONS,PAUSE,ANNOUNCE");

    string data;
    resp.to_str(data);

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onDescribe(stk::RtspMsg &rtsp)
{
    string url = rtsp.GetReqUrl();
    string seq = rtsp.GetCSeq();

    stk::RtspMsg resp(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));

    resp.SetCSeq(seq.c_str());
    resp.SetContentBase(url.c_str());
    resp.SetCacheControl("no-cache");
    resp.SetContentType("application/sdp");

    char sdp[500] = {0};
    /* snprintf(sdp, sizeof(sdp), HS_RTSP_SDP_FORMAT, Bufferev::srcip().c_str(), Bufferev::srcip().c_str(), RTPSRCPort()); */
	snprintf(sdp, sizeof(sdp), HS_RTSP_SDP_FORMAT, m_bev->srcip().c_str(), m_bev->srcip().c_str(), 0);

    resp.SetBody(sdp);
    resp.SetContentLength(strlen(sdp));

    string data;
    resp.to_str(data);

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onSetup(stk::RtspMsg &rtsp)
{
    string seq = rtsp.GetCSeq();

    int    code    = 200;
    string suburl  = "";
    string cliport = "";
    do
    {
        string url = rtsp.GetReqUrl();
        char *requrl  = (char *)strchr(url.c_str(), '@');
        if (!requrl)
        {
            LOGW_MSV("invalidate url: %s", url.c_str());
            code = 400;
            break;
        }

        requrl += 1;

        /* truncate the "/trackId" */
        char *trackid = (char *)strstr(url.c_str(), "/track");
        if (!trackid)
        {
            LOGW_MSV("invalidate url: %s", url.c_str());
            code = 400;
            break;
        }

        string path;
        path.assign(requrl, trackid - requrl);
        suburl = string("rtsp:") + path;

        stk::Transport * trans = rtsp.GetTransport(0);
        if (!trans)
        {
            LOGW_MSV("not find transport");
            code = 400;
            break;
        }

        cliport = trans->GetClientPort();
        uint32_t port1, port2;
        sscanf(cliport.c_str(), "%d-%d", &port1, &port2);
        m_destport = port1;
    }
    while (0);

    if (RTPStart(suburl) < 0)
    {
        LOGW_MSV(getError());
        code = 404;
    }

    stk::RtspMsg resp;
    switch (code)
    {
    case 200:
    {
        resp.parse(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));

        char servport[40] = {0};
        snprintf(servport, 40, "%d-%d", RTPSRCPort(), RTPSRCPort() + 1);

        stk::Transport trans;
        trans.SetMediaType("RTP/AVP");
        trans.SetCastType("unicast");
		trans.SetDestination(m_bev->peerip().c_str());
        trans.SetClientPort(cliport.c_str());
		trans.SetSource(m_bev->srcip().c_str());
        trans.SetServerPort(servport);
        resp.SetTransport(trans);
        resp.SetSession(m_sessid.c_str());
    }
    break;
    case 400:
        resp.parse(HS_RTSP_ECHO_400, strlen(HS_RTSP_ECHO_400));
        break;
    case 404:
        resp.parse(HS_RTSP_ECHO_404, strlen(HS_RTSP_ECHO_404));
        break;
    default:
        break;
    }

    resp.SetCSeq(seq.c_str());

    string data;
    resp.to_str(data);

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onPlay(stk::RtspMsg &rtsp)
{
    string url    = rtsp.GetReqUrl();
    string seq    = rtsp.GetCSeq();
    string sessid = rtsp.GetSession();
    string range  = rtsp.GetRange();
    string scale  = rtsp.GetScale();

    stk::RtspMsg resp;
    if (m_sessid != sessid)
    {
        LOGW_MSV("invalidate sessid: %s, this sessid is: %s", sessid.c_str(), m_sessid.c_str());
        resp.parse(HS_RTSP_ECHO_400, strlen(HS_RTSP_ECHO_400));
    }
    else
    {
        if ("" == scale)
        {
            scale   = "1";
        }
        else
        {
            if ("2.000000" == scale)
            {
                scale = "2";
            }
            else if ("4.000000" == scale)
            {
                scale = "4";
            }
            else if ("8.000000" == scale)
            {
                scale = "8";
            }
            else if ("1.000000" == scale)
            {
                scale = "1";
            }
            else if ("0.500000" == scale)
            {
                scale = "0.5";
            }
            else if ("0.250000" == scale)
            {
                scale = "0.25";
            }
            else if ("0.125000" == scale)
            {
                scale = "0.125";
            }
        }

        if ("" == range)
        {
            range = "npt=now-";
        }

        RTPlay(scale, range,url);
        resp.parse(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));
        resp.SetSession(sessid.c_str());
        resp.SetRange(range.c_str());
        resp.SetScale(scale.c_str());
    }

    resp.SetCSeq(seq.c_str());

    string data;
    resp.to_str(data);
    /* data += "RTP-Info:url=;seq=0;"; */

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onPause(stk::RtspMsg &rtsp)
{
    string url    = rtsp.GetReqUrl();
    string seq    = rtsp.GetCSeq();
    string sessid = rtsp.GetSession();

    stk::RtspMsg resp;
    if (m_sessid != sessid)
    {
        LOGW_MSV("invalidate sessid: %s, this sessid is: %s", sessid.c_str(), m_sessid.c_str());
        resp.parse(HS_RTSP_ECHO_400, strlen(HS_RTSP_ECHO_400));
    }
    else
    {

        RTPause();
        resp.parse(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));
        resp.SetSession(sessid.c_str());
    }

    resp.SetCSeq(seq.c_str());

    string data;
    resp.to_str(data);

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onTeardown(stk::RtspMsg &rtsp)
{
    string url    = rtsp.GetReqUrl();
    string seq    = rtsp.GetCSeq();
    string sessid = rtsp.GetSession();

    stk::RtspMsg resp;
    if (m_sessid != sessid)
    {
        LOGW_MSV("invalidate sessid: %s, this sessid is: %s", sessid.c_str(), m_sessid.c_str());
        resp.parse(HS_RTSP_ECHO_400, strlen(HS_RTSP_ECHO_400));
    }
    else
    {
        RTPStop();
        resp.parse(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));
        resp.SetSession(sessid.c_str());
    }

    resp.SetCSeq(seq.c_str());

    string data;
    resp.to_str(data);

	m_bev->write((char *)data.c_str(), data.size());
}

void
HSRTSPSession::onGetParameter(stk::RtspMsg &rtsp)
{
    string seq    = rtsp.GetCSeq();
    string sessid = rtsp.GetSession();

    stk::RtspMsg resp;
    if (m_sessid != sessid)
    {
        LOGW_MSV("invalidate sessid: %s, this sessid is: %s", sessid.c_str(), m_sessid.c_str());
        resp.parse(HS_RTSP_ECHO_400, strlen(HS_RTSP_ECHO_400));
    }
    else
    {
        resp.parse(HS_RTSP_ECHO_200, strlen(HS_RTSP_ECHO_200));
    }

    resp.SetSession(sessid.c_str());
    resp.SetCSeq(seq.c_str());

    string data;
    resp.to_str(data);

    m_bev->write((char *)data.c_str(), data.size());
}
