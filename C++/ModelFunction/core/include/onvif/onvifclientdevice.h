#ifndef __ONVIF_CLIENT_DEVICE__
#define __ONVIF_CLIENT_DEVICE__

#include <string>
#include <map>
#include <iostream>
#include <list>
#include <ctime>

#include "soapStub.h"
#include "soapDeviceBindingProxy.h"
#include "wsseapi.h"

using namespace std;

class OnvifClientDevice
{
public:
	OnvifClientDevice(string &strUrl, string &strUser, string &strPass);
	~OnvifClientDevice();
public:
	int GetCapabilities(_tds__GetCapabilitiesResponse &cap);
	int GetCapabilities();
	bool GetUserPasswd(string &strUser, string &strPass);
	bool SetUrlUserPasswd(string &strUrl, string &strUser, string &strPass);
	bool GetUrl(string &_strUrl);
public:
	bool GetMediaUrl(string &strUrl);
	bool GetPTZUrl(string &strUrl);
	bool GetImagingUrl(string &strUrl);
	bool GetReceiverUrl(string &strUrl);
	bool GetRecordingUrl(string &strUrl);
	bool GetSearchUrl(string &strUrl);
	bool GetReplayUrl(string &strUrl);
	bool GetEventUrl(string &strUrl);

	
	//Device Service Functions
	int GetDeviceInformation(_tds__GetDeviceInformationResponse &DeviceInformationResponse);
	int GetSystemDateAndTime(_tds__GetSystemDateAndTimeResponse &SystemDateAndTimeResponse);
	int SetSystemDateAndTime(_tds__SetSystemDateAndTimeResponse &SetSystemDateAndTimeResponse, tt__SetDateTimeType &DateTimeType, bool DayLightSavings, tt__TimeZone &Timezone, tt__DateTime &UTCDateTime);
	int GetHostname(_tds__GetHostnameResponse &GetHostnameResponse);
	int SetHostname(_tds__SetHostnameResponse &SetHostnameResponse, string Name);
	int GetDNS(_tds__GetDNSResponse &GetDNSResponse);
	int SetDNS(_tds__SetDNSResponse &SetDNSResponse, bool FromDHCP, vector<string,allocator<string>> SearchDomain, vector<tt__IPAddress*,allocator<tt__IPAddress*>> &DNSManual);
	int GetNTP(_tds__GetNTPResponse &GetNTPResponse);
	int SetNTP(_tds__SetNTPResponse &SetNTPResponse, bool FromDHCP, vector<tt__NetworkHost*,allocator<tt__NetworkHost*>> &NTPManual);
	int GetDynamicDNS(_tds__GetDynamicDNSResponse &GetDynamicDNSResponse);
	int SetDynamicDNS(_tds__SetDynamicDNSResponse &SetDynamicDNSResponse,tt__DynamicDNSType &Type,tt__DNSName &Name, LONG64 &durationTTL);
	int GetNetworkInterfaces(_tds__GetNetworkInterfacesResponse &GetNetworkInterfacesResponse);
	int SetNetworkInterfaces(_tds__SetNetworkInterfacesResponse &SetNetworkInterfacesResponse,string InterfaceToken,tt__NetworkInterfaceSetConfiguration &NetworkInterface);
	int GetNetworkProtocols(_tds__GetNetworkProtocolsResponse &GetNetworkProtocolsResponse);
	int SetNetworkProtocols(_tds__SetNetworkProtocolsResponse &SetNetworkProtocolsResponse,vector<tt__NetworkProtocol*,allocator<tt__NetworkProtocol*>> &NetworkProtocols);
	int GetNetworkDefaultGateway(_tds__GetNetworkDefaultGatewayResponse &GetNetworkDefaultGatewayResponse);
	int SetNetworkDefaultGateway(_tds__SetNetworkDefaultGatewayResponse &SetNetworkDefaultGatewayResponse,vector<string,allocator<string>> &IPv4,vector<string,allocator<string>> &IPv6);
	int SystemReboot(_tds__SystemRebootResponse &SystemRebootResponse);

	int SynchronizeDateAndTimeWithCamera(string &strUrl,string &strUser,string &strPass,_tds__SetSystemDateAndTimeResponse &SetSystemDateAndTimeResponse);
	int SynchronizeDateAndTimeWithCamera(_tds__SetSystemDateAndTimeResponse &SetSystemDateAndTimeResponse);
	
private:	
	double OnvifClientDevice::findDiffTime(struct tm local_sys,tt__DateTime cameraTime,bool isDST);
	int OnvifClientDevice::LocalAddUsernameTokenDigest(struct soap *soapOff,double cam_pc_offset);
	
private:
	string m_strUrl;
	string m_strUser;
	string m_strPass;
	bool m_hasGetCap;

	/* If the Device support blow service */
	bool m_hasMedia;
	bool m_hasPTZ;
	bool m_hasImaging;
	bool m_hasReceiver;
	bool m_hasRecording;
	bool m_hasSearch;
	bool m_hasReplay;
	bool m_hasEvent;

	/* The Url of blow service */
	string m_strMedia;
	string m_strPTZ;
	string m_strImaging;
	string m_strReceiver;
	string m_strRecording;
	string m_strSearch;
	string m_strReplay;
	string m_strEvent;
	DeviceBindingProxy deviceBindProxy;
};
#endif
	
