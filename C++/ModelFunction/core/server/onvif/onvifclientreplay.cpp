#include "OnvifClientReplay.h"

 OnvifClientReplay::OnvifClientReplay(OnvifClientDevice &device)
: m_Device(device), replayProxy(SOAP_C_UTFSTRING)
{

}

 OnvifClientReplay::~OnvifClientReplay()
{

}

 int OnvifClientReplay::GetReplayUri(string recordingToken, string  &replayUrl)
{
	string strUrl;
	string strUser;
	string strPass;
	_trp__GetReplayUri req;
	_trp__GetReplayUriResponse resp;
	tt__StreamSetup StreamSetup;
	tt__Transport Transport;
	Transport.Protocol = tt__TransportProtocol__RTSP;
	StreamSetup.Stream = tt__StreamType__RTP_Unicast;
	StreamSetup.Transport = &Transport;
	
	
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetReplayUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	
	replayProxy.soap_endpoint =  strUrl.c_str();
	
	soap_wsse_add_Security(&replayProxy);
	soap_wsse_add_UsernameTokenDigest(&replayProxy, "Id", 
		strUser.c_str() , strPass.c_str());
		
	req.RecordingToken = recordingToken;
	req.StreamSetup = &StreamSetup;
		
	int ret = replayProxy.GetReplayUri( &req, resp);
	if (ret == SOAP_OK)
	{
		replayUrl = resp.Uri;
	}
	return ret;

}

