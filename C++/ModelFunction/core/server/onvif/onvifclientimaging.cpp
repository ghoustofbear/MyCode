#include "onvifclientimaging.h"

OnvifClientImaging::OnvifClientImaging(OnvifClientDevice &device)
: m_Device(device)
{

}

OnvifClientImaging::~OnvifClientImaging()
{

}

//(focus)
int OnvifClientImaging::Stop(_timg__StopResponse &StopResponse,string VideoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__Stop StopReq;
	StopReq.VideoSourceToken = VideoSourceToken;
	return ImageProxy.Stop(&StopReq,StopResponse);
}

//(focus)
int OnvifClientImaging::GetMoveOptions(_timg__GetMoveOptionsResponse &GetMoveOptionsResponse,string VideoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__GetMoveOptions GetMoveOptionsReq;
	GetMoveOptionsReq.VideoSourceToken = VideoSourceToken;
	return ImageProxy.GetMoveOptions(&GetMoveOptionsReq,GetMoveOptionsResponse);
}

//move (focus)
int OnvifClientImaging::Move(_timg__MoveResponse &MoveResponse, tt__FocusMove &Focus, string VideoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__Move MoveReq;
	MoveReq.Focus = &Focus;
	MoveReq.VideoSourceToken = VideoSourceToken;
	return ImageProxy.Move(&MoveReq,MoveResponse);
}

int OnvifClientImaging::GetStatus(_timg__GetStatusResponse &GetImagingStatusResponse, string VideoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__GetStatus GetImagingStatusReq;
	GetImagingStatusReq.VideoSourceToken = VideoSourceToken;

	return ImageProxy.GetStatus(&GetImagingStatusReq,GetImagingStatusResponse);

}

int OnvifClientImaging::SetImagingSettings(_timg__SetImagingSettingsResponse &SetImagingSettingsResponse, string VideoSourceToken, tt__ImagingSettings20 &ImagingSettings,bool ForcePersistence)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__SetImagingSettings SetImagingSettingsReq;
	SetImagingSettingsReq.VideoSourceToken = VideoSourceToken;
	SetImagingSettingsReq.ImagingSettings = &ImagingSettings;
	SetImagingSettingsReq.ForcePersistence = &ForcePersistence;
	return ImageProxy.SetImagingSettings(&SetImagingSettingsReq,SetImagingSettingsResponse);

}

int OnvifClientImaging::GetServiceCapabilities(_timg__GetServiceCapabilitiesResponse &GetServiceCapResponse)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__GetServiceCapabilities GetServiceCapReq;

	return ImageProxy.GetServiceCapabilities(&GetServiceCapReq,GetServiceCapResponse);

}

int OnvifClientImaging::GetOptions(_timg__GetOptionsResponse &GetOptionsResponse, string videoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__GetOptions GetOptionsReq;
	GetOptionsReq.VideoSourceToken = videoSourceToken;

	return ImageProxy.GetOptions(&GetOptionsReq,GetOptionsResponse);


}

int OnvifClientImaging::GetImagingSettings(_timg__GetImagingSettingsResponse &ImagingSettingsResponse, string videoSourceToken)
{
	string strUrl;
	string strUser;
	string strPass;
	if (m_Device.GetUserPasswd(strUser, strPass) == false 
		|| m_Device.GetImagingUrl(strUrl) == false)
	{
		return SOAP_ERR;
	}
	ImageProxy.soap_endpoint =  strUrl.c_str();
	soap_wsse_add_Security(&ImageProxy);
	soap_wsse_add_UsernameTokenDigest(&ImageProxy, "Id", strUser.c_str() , strPass.c_str());

	_timg__GetImagingSettings ImagingSettingsReq;
	ImagingSettingsReq.VideoSourceToken = videoSourceToken;

	return ImageProxy.GetImagingSettings(&ImagingSettingsReq,ImagingSettingsResponse);

}


