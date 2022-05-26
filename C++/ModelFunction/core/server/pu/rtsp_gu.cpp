#include "tls_error.h"
#include "rtsp_gu.h"
#include <functional>
#include <stdio.h>


#define DELAY_CLOSE_TIME 15

//extern UsageEnvironment* live555_env;


RtspGu::RtspGu()
{

}

RtspGu::~RtspGu()
{
	//LOGD("销毁RtspGu:%s", m_url.c_str());
	shutdownStream(m_rtspclient);	
}
bool RtspGu::createMyRTSPClient(string url, UsageEnvironment* live555_env, pfnstreamcallback m_streamcallback, void* puser)
{
	m_rtspclient = MyRTSPClient::createNew(url.c_str(), live555_env);
	if (m_rtspclient)
	{
		m_rtspclient->m_streamcallback = m_streamcallback;
		m_rtspclient->puser = puser;
		return true;
	}
	return false;
}

int  RtspGu::startGu(int offset)
{
	m_rtspclient->sendDescribeCommand(continueAfterDESCRIBE);
	return 0;
}


void RtspGu::stopGu()
{

}

void RtspGu::continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias		
		if (resultCode != 0) {
			ERRE_MSV("获取sdp失败: %d", resultCode);
			delete[] resultString;
			break;
		}

		char* const sdpDescription = resultString;
		LOGE_MSV("sdp信息为: \n%s", sdpDescription);

		((MyRTSPClient*)rtspClient)->session = MediaSession::createNew(env, sdpDescription);
		delete[] sdpDescription; // because we don't need it anymore

		if (((MyRTSPClient*)rtspClient)->session == NULL) {
			ERRE_MSV("通过sdp创建媒体会话失败, errmsg:%s", env.getResultMsg());
			break;
		}
		else if (!((MyRTSPClient*)rtspClient)->session->hasSubsessions()) {
			ERRE_MSV("SDP未包含媒体信息(i.e., no m= lines)");
			break;
		}

		// Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
		// calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
		// (Each 'subsession' will have its own data source.)
		((MyRTSPClient*)rtspClient)->iter = new MediaSubsessionIterator(*((MyRTSPClient*)rtspClient)->session);
		setupNextSubsession(rtspClient);
		return;
	} while (0);

	// An unrecoverable error occurred with this stream.
	shutdownStream(rtspClient);
}


void RtspGu::continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias		
		if (resultCode != 0) {
			ERRE_MSV("SETUP命令失败，会话（%s:%s）, errmsg:%s", ((MyRTSPClient*)rtspClient)->subsession->mediumName(), ((MyRTSPClient*)rtspClient)->subsession->codecName(), resultString);
			break;
		}

		// Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
		// (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
		// after we've sent a RTSP "PLAY" command.)

		((MyRTSPClient*)rtspClient)->subsession->sink = DummySink::createNew(env, *((MyRTSPClient*)rtspClient)->subsession, rtspClient->url());
		((DummySink*)((MyRTSPClient*)rtspClient)->subsession->sink)->m_treamcallback = ((MyRTSPClient*)rtspClient)->m_streamcallback;
		((DummySink*)((MyRTSPClient*)rtspClient)->subsession->sink)->puser = ((MyRTSPClient*)rtspClient)->puser;
		// perhaps use your own custom "MediaSink" subclass instead
		if (((MyRTSPClient*)rtspClient)->subsession->sink == NULL) {
			ERRE_MSV("创建媒体接收器失败,会话（%s:%s）, errmsg:%s", ((MyRTSPClient*)rtspClient)->subsession->mediumName(), ((MyRTSPClient*)rtspClient)->subsession->codecName(), env.getResultMsg());

			break;
		}

		LOGE_MSV("创建媒体接收器成功，会话（%s:%s）", ((MyRTSPClient*)rtspClient)->subsession->mediumName(), ((MyRTSPClient*)rtspClient)->subsession->codecName());

		((MyRTSPClient*)rtspClient)->subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession 
		((MyRTSPClient*)rtspClient)->subsession->sink->startPlaying(*(((MyRTSPClient*)rtspClient)->subsession->readSource()),
			subsessionAfterPlaying, ((MyRTSPClient*)rtspClient)->subsession);
		// Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
		if (((MyRTSPClient*)rtspClient)->subsession->rtcpInstance() != NULL) {
			((MyRTSPClient*)rtspClient)->subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, ((MyRTSPClient*)rtspClient)->subsession);
		}
	} while (0);
	delete[] resultString;

	// Set up the next subsession, if any:
	setupNextSubsession(rtspClient);
}


void RtspGu::continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
	Boolean success = False;

	do {
		UsageEnvironment& env = rtspClient->envir(); // alias	
		if (resultCode != 0) {
			ERRE_MSV("PLAY命令失败, %s", resultString);
			break;
		}

		// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
		// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
		// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
		// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
		if (((MyRTSPClient*)rtspClient)->duration > 0) {
			unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
			((MyRTSPClient*)rtspClient)->duration += delaySlop;
			unsigned uSecsToDelay = (unsigned)(((MyRTSPClient*)rtspClient)->duration * 1000000);
			((MyRTSPClient*)rtspClient)->streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
		}

		LOGE_MSV("开始播放会话");
		if (((MyRTSPClient*)rtspClient)->duration > 0) {
			LOGE_MSV("等待：%d秒开始播放...", ((MyRTSPClient*)rtspClient)->duration);
		}
		success = True;
	} while (0);
	delete[] resultString;

	if (!success) {
		// An unrecoverable error occurred with this stream.
		shutdownStream(rtspClient);
	}
}

#define REQUEST_STREAMING_OVER_TCP true
void RtspGu::setupNextSubsession(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias  

	((MyRTSPClient*)rtspClient)->subsession = ((MyRTSPClient*)rtspClient)->iter->next();
	if (((MyRTSPClient*)rtspClient)->subsession != NULL) {
		if (!((MyRTSPClient*)rtspClient)->subsession->initiate()) {
			ERRE_MSV("发起子会话(%s:%s)失败, errmsg:%s", ((MyRTSPClient*)rtspClient)->subsession->mediumName(), ((MyRTSPClient*)rtspClient)->subsession->codecName(), env.getResultMsg());
			setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
		}
		else {
			LOGE_MSV("发起(%s:%s)会话成功", ((MyRTSPClient*)rtspClient)->subsession->mediumName(), ((MyRTSPClient*)rtspClient)->subsession->codecName());

			if (((MyRTSPClient*)rtspClient)->subsession->rtcpIsMuxed()) {
				LOGE_MSV("客户端端口:%d", ((MyRTSPClient*)rtspClient)->subsession->clientPortNum());
			}
			else {
				LOGE_MSV("客户端端口(%d:%d)", ((MyRTSPClient*)rtspClient)->subsession->clientPortNum(), ((MyRTSPClient*)rtspClient)->subsession->clientPortNum() + 1);
			}


			// Continue setting up this subsession, by sending a RTSP "SETUP" command:
			rtspClient->sendSetupCommand(*((MyRTSPClient*)rtspClient)->subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
		}
		return;
	}

	// We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
	if (((MyRTSPClient*)rtspClient)->session->absStartTime() != NULL) {
		// Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
		rtspClient->sendPlayCommand(*((MyRTSPClient*)rtspClient)->session, continueAfterPLAY, ((MyRTSPClient*)rtspClient)->session->absStartTime(), ((MyRTSPClient*)rtspClient)->session->absEndTime());
	}
	else {
		((MyRTSPClient*)rtspClient)->duration = ((MyRTSPClient*)rtspClient)->session->playEndTime() - ((MyRTSPClient*)rtspClient)->session->playStartTime();
		rtspClient->sendPlayCommand(*((MyRTSPClient*)rtspClient)->session, continueAfterPLAY);
	}
}


void RtspGu::shutdownStream(RTSPClient* rtspClient, int exitCode) {
	UsageEnvironment& env = rtspClient->envir(); // alias


	// First, check whether any subsessions have still to be closed:
	if (((MyRTSPClient*)rtspClient)->session != NULL) {
		Boolean someSubsessionsWereActive = False;
		MediaSubsessionIterator iter(*((MyRTSPClient*)rtspClient)->session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				Medium::close(subsession->sink);
				subsession->sink = NULL;

				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
				}

				someSubsessionsWereActive = True;
			}
		}

		if (someSubsessionsWereActive) {
			// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
			// Don't bother handling the response to the "TEARDOWN".
			rtspClient->sendTeardownCommand(*((MyRTSPClient*)rtspClient)->session, NULL);
		}
	}

	ERRE_MSV("关闭流：%s", rtspClient->url());

	Medium::close(rtspClient);
	// Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.
}

void RtspGu::streamTimerHandler(void* clientData) {
	MyRTSPClient* rtspClient = (MyRTSPClient*)clientData;
	((MyRTSPClient*)rtspClient)->streamTimerTask = NULL;
	shutdownStream(rtspClient);
}

void RtspGu::subsessionAfterPlaying(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

	// Begin by closing this subsession's stream:
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	// Next, check whether *all* subsessions' streams have now been closed:
	MediaSession& session = subsession->parentSession();
	MediaSubsessionIterator iter(session);
	while ((subsession = iter.next()) != NULL) {
		if (subsession->sink != NULL) return; // this subsession is still active
	}

	// All subsessions' streams have now been closed, so shutdown the client:
	shutdownStream(rtspClient);
}


void RtspGu::subsessionByeHandler(void* clientData, char const* reason) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RtspGu::MyRTSPClient* rtspClient = (RtspGu::MyRTSPClient*)subsession->miscPtr;

	ERRE_MSV("接收到RTCP BYE命令，会话(%s:%s)关闭", rtspClient->subsession->mediumName(), rtspClient->subsession->codecName());

	if (reason != NULL) {
		ERRE_MSV("reason:%s", reason);
		delete[] reason;
	}
	// Now act as if the subsession had closed:
	subsessionAfterPlaying(subsession);
}

void RtspGu::onTimer()
{

}

RtspGu::MyRTSPClient * RtspGu::MyRTSPClient::createNew(char const * rtspURL)
{
	return NULL;
}

RtspGu::MyRTSPClient * RtspGu::MyRTSPClient::createNew(char const * rtspURL, UsageEnvironment* live555_env)
{
	return new MyRTSPClient(rtspURL, live555_env);
}

RtspGu::MyRTSPClient::MyRTSPClient(char const * rtspURL, UsageEnvironment* live555_env)
	:RTSPClient(*live555_env, rtspURL, 1, nullptr, 0, -1),iter(nullptr), session(nullptr), subsession(nullptr),
	streamTimerTask(nullptr), duration(0.0)
{
	puser = NULL;
	m_streamcallback = NULL;
}

//RtspGu::MyRTSPClient::MyRTSPClient(char const * rtspURL)
//	:RTSPClient(*live555_env, rtspURL, 1, nullptr, 0, -1)
//{
//}

RtspGu::MyRTSPClient::~MyRTSPClient()
{

}

RtspGu::DummySink * RtspGu::DummySink::createNew(UsageEnvironment & env, MediaSubsession & subsession, char const * streamId)
{
	return new DummySink(env, subsession, streamId);
}

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 1000000
RtspGu::DummySink::DummySink(UsageEnvironment & env, MediaSubsession & subsession, char const * streamId) :
MediaSink(env), fSubsession(subsession)
{
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
	H264data = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE+1000];
    m_needKeyframe = true;
	m_treamcallback = NULL;
}




RtspGu::DummySink::~DummySink()
{
	delete[] fReceiveBuffer;
	delete[] fStreamId;
	delete[] H264data;
}

void RtspGu::DummySink::afterGettingFrame(void * clientData, unsigned frameSize, unsigned numTruncatedBytes, timeval presentationTime, unsigned durationInMicroseconds)
{
	DummySink* sink = (DummySink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}
void RtspGu::DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, timeval presentationTime, unsigned durationInMicroseconds)
{
	// We've just received a frame of data.  (Optionally) print out information about it:
		
	if (m_needKeyframe)
	{
		if (fReceiveBuffer[0] == 0x65)
		{
			// For H.264 video stream, we use a special sink that insert start_codes:
			unsigned int num;
			SPropRecord *sps = parseSPropParameterSets(fSubsession.fmtp_spropparametersets(), num);
			unsigned char start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
			memcpy(H264data, start_code,4);
			memcpy(H264data+4, sps[0].sPropBytes, sps[0].sPropLength);
			memcpy(H264data+ 4 + sps[0].sPropLength, start_code, 4);
			memcpy(H264data + 4 + sps[0].sPropLength+4, sps[1].sPropBytes, sps[1].sPropLength);
			int size = sps[0].sPropLength + sps[1].sPropLength + 4 + 4;
			if (m_treamcallback)
			{
				this->m_treamcallback(H264data, size, 1, puser);
			}
			delete[] sps;
		}
		else
		{
			continuePlaying();
			return;
		}
	}


	if (fReceiveBuffer[0] == 0x67 || fReceiveBuffer[0] == 0x68 || frameSize == 0)
	{
		continuePlaying();
		return;
	}

	if (strcmp(fSubsession.mediumName(), "video") == 0 &&
		(strcmp(fSubsession.codecName(), "H264") == 0))
	{
		//printf("recevi a frame, %d\n", frameSize);
		char head[4] = { 0x00, 0x00, 0x00, 0x01 };
		memcpy(H264data, head, 4);
		memcpy(H264data + 4, fReceiveBuffer, frameSize);
		int size = frameSize + 4;
		if (m_treamcallback)
		{
			this->m_treamcallback(H264data, size, 3, puser);
		}
	}



	// Then continue, to request the next frame of data:
	continuePlaying();
}

Boolean RtspGu::DummySink::continuePlaying()
{
	// Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
		afterGettingFrame, this,
		onSourceClosure, this);
	return True;
}
