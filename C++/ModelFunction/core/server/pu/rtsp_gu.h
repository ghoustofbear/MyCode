/**
* @file      rtsp_gu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    songhongliang@ut.cn
* @date      2022-02-20 15:14:00
*
* @note
*  rtsp_gu.h defines
*/
#ifndef __RTSP_GU_H__
#define __RTSP_GU_H__

#include "gu.h"
#include "liveMedia.hh"



class DummySink;
class RtspGu
{
public:


	RtspGu();
	virtual ~RtspGu();

	virtual int        startGu(int offset);
	virtual void       stopGu();


	typedef void(*pfnstreamcallback)(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
	bool createMyRTSPClient(string url, UsageEnvironment* live555_env, pfnstreamcallback m_streamcallback,void* puser);

protected:
	virtual void       onTimer();

protected:

	static void        continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
	static void        continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
	static void        continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);


	static void        setupNextSubsession(RTSPClient* rtspClient);
	static void        shutdownStream(RTSPClient* rtspClient, int exitCode = 1);
	static void        streamTimerHandler(void* clientData);
	static void        subsessionAfterPlaying(void* clientData);
	static void        subsessionByeHandler(void* clientData, char const* reason);



private:
	DummySink * Sink;

	class MyRTSPClient : public RTSPClient
	{
	public:
		static MyRTSPClient* createNew(char const* rtspURL);
		static MyRTSPClient* createNew(char const * rtspURL, UsageEnvironment* live555_env);
		void  setDatacalllbaek(pfnstreamcallback callback);
	protected:
		MyRTSPClient(char const* rtspURL);
		MyRTSPClient(char const* rtspURL, UsageEnvironment* live555_env);
	public:
		virtual ~MyRTSPClient();


		MediaSubsessionIterator* iter;
		MediaSession* session;
		MediaSubsession* subsession;
		TaskToken streamTimerTask;
		double duration;
		pfnstreamcallback m_streamcallback;
		void* puser;
	} *m_rtspclient;

	class DummySink : public MediaSink
	{
	public:
		static DummySink* createNew(UsageEnvironment& env,
			MediaSubsession& subsession, // identifies the kind of data that's being received
			char const* streamId = NULL); // identifies the stream itself (optional)

	private:
		DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
		// called only by "createNew()"
		virtual ~DummySink();
		
		static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes,
		struct timeval presentationTime,
			unsigned durationInMicroseconds);
		void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime, unsigned durationInMicroseconds);

	private:
		// redefined virtual functions:
		virtual Boolean continuePlaying();

	private:
		u_int8_t* fReceiveBuffer;
		u_int8_t* H264data;
		MediaSubsession& fSubsession;
		char* fStreamId;
		bool m_needKeyframe;
	public:
		pfnstreamcallback m_treamcallback;
		void* puser;
	};

};
//UsageEnvironment* live555_env;
#endif
