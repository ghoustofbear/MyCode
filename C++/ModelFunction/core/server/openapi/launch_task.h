#ifndef  __LAUNCH_TASK_H__
#define  __LAUNCH_TASK_H__

#include "task.h"
#include "tqueue.h"
#include "tevqueue.h"
#include "launch_server.h"

class LaunchTask : public Task
{
public:
	LaunchTask(LaunchServer *server);
	virtual ~LaunchTask();
	int response(string type, int code, string body);
	void setPu(Pu*pu);
protected:
	virtual void       stop();

	virtual int        onLogin(string json) { return 0; }
	virtual int        onSubscribe(string json) { return 0; }
	virtual int        onSubscribeStatus(string json) { return 0; }
	virtual int        onQueryDevice(string json) { return 0; }
	virtual int        onPreview(string json) { return 0; }
	virtual int        onPtzCtrl(string json) { return 0; }

	virtual int        onQueryRecord(string json) { return 0; }
	virtual int        onPlayback(string json) { return 0; }
	virtual int        onDownload(string json) { return 0; }

	virtual int        onSyncTime(string json) { return 0; }
	virtual int        onWindowDecoder(string json){ return -1; };
	virtual int        onDualTalk(string json){ return -1; };

	virtual int        onSetFace(string json) { return 0; }
	virtual int        onGetConfig(string json) { return 0; }

protected:
	virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
	virtual void       onWrite(Bufferev *bev);
	virtual void       onClose(Bufferev *bev);

	virtual int        process();
private:
	int                m_s8ReNum;
	string             m_strBuffer;
    int                m_bodySize;

    int                m_bodyTotal;
	Pu                *m_pu;
	LaunchServer      *launchSVR;
};

#endif