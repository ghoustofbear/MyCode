#ifndef __ONVIF_CLIENT_MEDIA__
#define __ONVIF_CLIENT_MEDIA__

#include <string>
#include <map>
#include <iostream>
#include <list>

#include "onvifclientdevice.h"
#include "soapStub.h"
#include "soapMediaBindingProxy.h"
#include "wsseapi.h"


using namespace std;

class OnvifClientMedia
{
public:
    OnvifClientMedia(OnvifClientDevice &device);
    ~OnvifClientMedia();
public:
	//Profile Functions
    int OnvifClientMedia::GetProfiles(_trt__GetProfilesResponse &profiles);
	int OnvifClientMedia::GetProfile(_trt__GetProfileResponse &profileResponse,string profileToken);
	int OnvifClientMedia::CreateProfile(_trt__CreateProfileResponse &CreateProfileResponse,string Name,string token);
	//Uri and streaming functions
	int OnvifClientMedia::GetSnapshotUri(_trt__GetSnapshotUriResponse &SnapshotUriResponse,string profileToken);
	int OnvifClientMedia::GetStreamUri(_trt__GetStreamUriResponse &StreamUriResponse,tt__StreamSetup &StreamSetup, string profileToken);
	int OnvifClientMedia::GetServiceCapabilities(_trt__GetServiceCapabilitiesResponse &GetServiceCapResponse);
	int OnvifClientMedia::StartMulticastStreaming(_trt__StartMulticastStreamingResponse &StartMulticastResponse,string profileToken);
	int OnvifClientMedia::StopMulticastStreaming(_trt__StopMulticastStreamingResponse &StopMulticastResponse,string profileToken);
	//Video Source Functions
	int OnvifClientMedia::GetVideoSources(_trt__GetVideoSourcesResponse &GetVideoSourcesResponse);
	int OnvifClientMedia::GetVideoSourceConfigurations(_trt__GetVideoSourceConfigurationsResponse &GetVideoSourceConfigResponse);
	int OnvifClientMedia::SetVideoSourceConfiguration(_trt__SetVideoSourceConfigurationResponse &SetVideoSourceConfigResponse);
	int OnvifClientMedia::GetVideoSourceConfiguration(_trt__GetVideoSourceConfigurationResponse &GetVideoSourceConfigResponse,string ConfigurationToken);
	int OnvifClientMedia::GetVideoSourceConfigurationOptions(_trt__GetVideoSourceConfigurationOptionsResponse &GetVideoSourceConfigOptionsResponse, string ConfigurationToken,string profileToken);
	//Meta data functions
	int OnvifClientMedia::GetMetadataConfigurations(_trt__GetMetadataConfigurationsResponse &GetMetadataConfigurationsResponse);
	int OnvifClientMedia::GetMetadataConfiguration(_trt__GetMetadataConfigurationResponse &GetMetadataConfigurationResponse,string profileToken);
	int OnvifClientMedia::GetMetadataConfigurationOptions(_trt__GetMetadataConfigurationOptionsResponse &GetMetadataConfigurationOptionsResponse,string ConfigToken,string profileToken);

private:
	OnvifClientDevice &m_Device;
	MediaBindingProxy  mediaProxy;

};
#endif