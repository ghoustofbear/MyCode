#ifndef _HTTP_ACTION_H_
#define _HTTP_ACTION_H_

#include "bufferev.h"
#include "buffevlistener.h"
#include "stk/stk_message_http.h"

class HttpAction : public Bufferev, public BuffevListener
{
public:
	HttpAction(event_base *eventbase);
	virtual ~HttpAction();

protected:
	virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
	virtual void       onWrite(Bufferev *bev) {}

	virtual void       onClose(Bufferev *bev) {}

	void               onRequest(stk::HttpMsg &httpMsg);
	void               onResponse(stk::HttpMsg &httpMsg);

	int			       onUpdate(stk::HttpMsg &httpMsg);

	int                onHeartbeat(stk::HttpMsg &httpMsg);

	int                onQueryResourceReq(stk::HttpMsg &httpMsg);

	int                onQueryRecordReq(stk::HttpMsg &httpMsg);

	int                onQueryAlarmLogReq(stk::HttpMsg &httpMsg);

	int                onQueryFlowReq(stk::HttpMsg &httpMsg);
private:
	string             m_strBuffer;
	int                m_s8ReNum;
	int                m_iContentLen;
	int                m_iRecvLen;
};

#endif