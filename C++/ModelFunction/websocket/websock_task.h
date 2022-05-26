/**
* @file      privtask.h
* @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2018-05-11 17:04:20
*
* @note
*  privtask.h defines
*/

#ifndef  __WEBSOCK_TASK_H__
#define  __WEBSOCK_TASK_H__

#include "task.h"
#include "tqueue.h"
#include "tevqueue.h"


class WebSocketTask : public Task
{
public:
    WebSocketTask();
    virtual ~WebSocketTask();

protected:
    virtual void       stop();

    virtual int        onLogin(string json);
    virtual int        onSubscribe(string json);
	virtual int        onSubscribeStatus(string json);
    virtual int        onQueryDevice(string json);
    virtual int        onPreview(string json);
    virtual int        onPtzCtrl(string json);

    virtual int        onQueryRecord(string json);
    virtual int        onPlayback(string json);
    virtual int        onDownload(string json);

    virtual int        onSyncTime(string json);
    virtual int        onWindowDecoder(string json){ return -1; };
    virtual int        onDualTalk(string json){ return -1; };

	virtual int        onSetFace(string json);
	virtual int        onGetConfig(string json);
	virtual int        onCustomCommand(string json);

	virtual int        onGateCtrl(string json);
protected:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onClose(Bufferev *bev);

    virtual int        process();

protected:
    int                response(string type, int code, string &body);

private:
    void               handShake(Bufferev *bev, void *data, uint32_t size);

private:

    typedef tevqueue<string, int> RespQueue;
    typedef struct
    {
        string             type;
        int                code;
        string             buff;
    } resp_s;

    std::string        m_req;
	vector<uint8_t>    m_recvBuf;
};

#endif
