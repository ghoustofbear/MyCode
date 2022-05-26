#ifndef __ONVIF_CLIENT_RECORDING__
#define __ONVIF_CLIENT_RECORDING__

#include <string>
#include <map>
#include <iostream>
#include <list>

#include "onvifclientdevice.h"
#include "soapStub.h"
#include "soapRecordingBindingProxy.h"
#include "wsseapi.h"


using namespace std;

class OnvifClientRecording
{
public:
    OnvifClientRecording(OnvifClientDevice &device);
    ~OnvifClientRecording();
public:
	/* Add function to here */
	int GetRecordings(_trc__GetRecordingsResponse &recordings);
				
private:
	OnvifClientDevice &m_Device;
	RecordingBindingProxy  recordingProxy;

};
#endif