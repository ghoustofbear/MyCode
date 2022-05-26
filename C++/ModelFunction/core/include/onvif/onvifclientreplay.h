#ifndef ONVIFCLIENTDEVICE_H
#define ONVIFCLIENTDEVICE_H
#include <list>

#include "onvifclientdevice.h"
#include "soapStub.h"
#include "soapReplayBindingProxy.h"
#include "wsseapi.h"


using namespace std;

class OnvifClientReplay
{
public:
    OnvifClientReplay(OnvifClientDevice &device);
    ~OnvifClientReplay();
public:
	/* Add function to here */
	int GetReplayUri(string recordingToken, string  &replayUrl);
	
				
private:
	OnvifClientDevice &m_Device;
	ReplayBindingProxy  replayProxy;

};

#endif