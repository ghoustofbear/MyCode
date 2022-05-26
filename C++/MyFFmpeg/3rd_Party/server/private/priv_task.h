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
#ifndef __PRIVTASK_H__
#define __PRIVTASK_H__
#include "task.h"
#include "tqueue.h"
#include "tevqueue.h"

typedef tevqueue<string, int> RespQueue;
typedef struct
{
    string             type;
    string             buff;
} resp_s;

class PrivTask : public Task
{
public:
    PrivTask(Bufferev* bev);
    virtual ~PrivTask();

protected:
    virtual void       stop();

    virtual int        onLogin(string json);
    virtual int        onSubscribe(string json);
    virtual int        onSubscribeStatus(string json);
	virtual int        onQueryUser();
    virtual int        onQueryChannelStatus(string json);

    virtual int        onQueryDevice(string json);
    virtual int        onPreview(string json);
    virtual int        onPtzCtrl(string json);
    virtual int        onQueryPreset(string json);
    virtual int        onSetPreset(string json);

    virtual int        onQueryRecord(string json);
    virtual int        onPlayback(string json);
    virtual int        onDownload(string json);

    virtual int        onSyncTime(string json);

    virtual int        onDualTalk(string json);

    virtual int        onCustomCommand(string json);
    virtual int        onGetConfig(string json);

protected:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onClose(Bufferev *bev);

    virtual int        process();
    int                response(string type, int code, string &body);


protected:
    vector<uint8_t>    m_recvBuf;

private:
    string             m_http;
    int                m_bodySize;
    

};

#endif
