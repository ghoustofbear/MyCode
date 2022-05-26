#include "OnvifClientRecording.h"

OnvifClientRecording::OnvifClientRecording(OnvifClientDevice &device)
: m_Device(device), recordingProxy(SOAP_C_UTFSTRING)
{

}

OnvifClientRecording::~OnvifClientRecording()
{

}

int OnvifClientRecording::GetRecordings(_trc__GetRecordingsResponse &recordings)
{
	_trc__GetRecordings req;
	string strUrl;
	string strUser;
	string strPass;
	
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetRecordingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	
	recordingProxy.soap_endpoint =  strUrl.c_str();
	
	soap_wsse_add_Security(&recordingProxy);
	soap_wsse_add_UsernameTokenDigest(&recordingProxy, "Id", 
		strUser.c_str() , strPass.c_str());
		
	return recordingProxy.GetRecordings( &req, recordings);
}
