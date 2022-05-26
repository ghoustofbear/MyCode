#ifndef __ONVIF_CLIENT_PTZ__
#define __ONVIF_CLIENT_PTZ__

#include <string>
#include <map>
#include <iostream>
#include <list>

#include "onvifclientdevice.h"
#include "soapStub.h"
#include "soapPTZBindingProxy.h"
#include "wsseapi.h"
//#include "soapC.h"


using namespace std;

class OnvifClientPTZ
{
public:
    OnvifClientPTZ(OnvifClientDevice &device);
    ~OnvifClientPTZ();

	bool m_bHasRelativePTMove;
	bool m_bHasRelativeZoomMove;
	bool m_bHasAbsolutePTMove;
	bool m_bHasAbsoluteZoomMove;
	bool m_bHasContinuousPTMove;
	bool m_bHasContinuousZoomMove;

public:
	/* Add function to here */
	//Configuration functions
	int OnvifClientPTZ::GetConfiguration(_tptz__GetConfigurationResponse &configurationResp); // John 7/2
	int OnvifClientPTZ::GetConfigurations(_tptz__GetConfigurationsResponse &configurationsResp);
	int OnvifClientPTZ::GetConfigurationOptions(_tptz__GetConfigurationOptionsResponse &configOptions, string configToken); // John 7/2
	int OnvifClientPTZ::SetConfiguration(_tptz__SetConfigurationResponse &SetConfiguration, tt__PTZConfiguration &PTZConfiguration,bool ForcePersist);
	//Move Functions (includes Pan, Tilt, and Zoom Moves)
	int OnvifClientPTZ::RelativeMove(_tptz__RelativeMoveResponse &relMoveResponse, tt__PTZVector &Translation, tt__PTZSpeed &Speed, string profileToken);
	int OnvifClientPTZ::ContinuousMove(_tptz__ContinuousMoveResponse &ContMoveResponse, tt__PTZSpeed &Speed, string profileToken);
	//int OnvifClientPTZ::ContinuousMove(_tptz__ContinuousMoveResponse &ContMoveResponse, tt__PTZSpeed &Speed, LONG64 &Timeout, string profileToken);
	int OnvifClientPTZ::AbsoluteMove(_tptz__AbsoluteMoveResponse &AbsoluteMoveResponse, tt__PTZSpeed &Speed, tt__PTZVector &position, string profileToken);
	int OnvifClientPTZ::Stop(_tptz__StopResponse &StopResponse,bool PanTilt,bool Zoom,string profileToken);
	//Zoom Functions
	//TODO:

	//Node, status, and service capabilities
	int OnvifClientPTZ::GetNodes(_tptz__GetNodesResponse &GetNodesResponse);
	int OnvifClientPTZ::GetNode(_tptz__GetNodeResponse &GetNodeResponse, string nodeToken);
	int OnvifClientPTZ::GetStatus(_tptz__GetStatusResponse &StatusResponse,string profileToken);
	int OnvifClientPTZ::GetServiceCapabilities(_tptz__GetServiceCapabilitiesResponse &ServiceCapResponse);

	//present function
	int OnvifClientPTZ::GetPresent(_tptz__GetPresetsResponse &GetPresetsResponse, std::string ProfileToken);
	int OnvifClientPTZ::SetPresent(_tptz__SetPresetResponse &tptz__SetPresetResponse, std::string ProfileToken, std::string PresetToken, std::string PresetName);
	int OnvifClientPTZ::GotoPresent(_tptz__GotoPresetResponse& GotoPresetRespons, std::string ProfileToken, std::string PresetToken);
	int OnvifClientPTZ::DeletePresent(_tptz__RemovePresetResponse& RemovePresetResponse, std::string ProfileToken, std::string PresetToken);

	int OnvifClientPTZ::Initialize();

private:
	OnvifClientDevice &m_Device;
	PTZBindingProxy ptzProxy;

};
#endif