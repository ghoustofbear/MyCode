#ifndef __ONVIF_CLIENT_IMAGING__
#define __ONVIF_CLIENT_IMAGING__

#include <string>
#include <map>
#include <iostream>
#include <list>

#include "onvifclientdevice.h"
#include "soapStub.h"
#include "soapImagingBindingProxy.h"
#include "wsseapi.h"


using namespace std;

class OnvifClientImaging
{
public:
    OnvifClientImaging(OnvifClientDevice &device);
    ~OnvifClientImaging();

public:
	int GetImagingSettings(_timg__GetImagingSettingsResponse &ImagingSettingsResponse, string videoSourceToken);
	int GetOptions(_timg__GetOptionsResponse &GetOptionsResponse, string videoSourceToken);
	int GetServiceCapabilities(_timg__GetServiceCapabilitiesResponse &GetServiceCapResponse);
	int SetImagingSettings(_timg__SetImagingSettingsResponse &SetImagingSettingsResponse, string VideoSourceToken, tt__ImagingSettings20 &ImagingSettings,bool ForcePersistence);
	int OnvifClientImaging::GetStatus(_timg__GetStatusResponse &GetStatusResponse, string VideoSourceToken);
	//focus control functions
	int Move(_timg__MoveResponse &MoveResponse, tt__FocusMove &FocusMove, string VideoSourceToken);
	int GetMoveOptions(_timg__GetMoveOptionsResponse &MoveOptionsResponse,string VideoSourceToken);
	int Stop(_timg__StopResponse &StopResponse,string VideoSourceToken);

private:
	OnvifClientDevice &m_Device;
	ImagingBindingProxy ImageProxy;

};

#endif