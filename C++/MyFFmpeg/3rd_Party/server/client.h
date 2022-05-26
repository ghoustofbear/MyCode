/**
 * @file      client.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-14 12:49:01
 *
 * @note
 *  client.h defines
 */
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include "def.h"
#include "thread.h"
#include "bitoperation.h"
#include "tqueue.h"
#include "tmap.h"
#include "tlist.h"
#include "triggerlistener.h"
#include "authlistener.h"
#include "buffevlistener.h"
#include "tevqueue.h"

class Session;
class DSession;

typedef tlist< Session *>  SessMgr;
typedef tlist< DSession *> DSessMgr;

class Client : public TriggerListener, public AuthListener, public BuffevListener
{
public:
    enum
    {
        T_PRIVATE,
        T_WEBSOCK,
    };

    /* right bit define */
    enum
    {
        R_PUSH,
        R_PREVIEW,
        R_PLAYBACK,
        R_DOWNLOAD,
        R_CTRL,
        R_QUERY,
        R_CONFIG,
        R_SYNCTIME,
        R_MATRIX_SWITCH,
        R_WINDOW_CODER,
        R_DUALTALK
    };

    Client(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey);
    virtual ~Client();

public:
    string             key();

    int                type();
    bool               active();
    void               clearPush();
    void               setPush(uint64_t type);

    bool               hasRight(uint32_t rightBit);
    int                writeUserLog(string  info);

	friend void        query_record_cb(int type, string addrcode, string &jout, void *userdata);
    static int	       Gw2PrivRecordJson(const string gwJson, string &privJson, int &fromIndex, int &toIndex);
    static int	       GB281812PrivRecordJson(const string gwJson, string &privJson);

public:
    virtual int        start();
    virtual void       stop();

    virtual Session  * startPreview(string &json, Bufferev *bev);

    virtual int        ptzCtrl(string &json);

    virtual int        queryPreset(string &jin, string &jout);

    virtual int        queryChannelStatus(string &jin, string &jout);

    virtual int        setPreset(string &jin, string &jout);

    virtual int        queryRecord(string &jin, string &jout);

    virtual Session  * startPlayback(string &json, Bufferev *bev);

    virtual Session  * startDownload(string &json, Bufferev *bev);

	virtual DSession * startDualTalk(string &json, Bufferev *bev);

    virtual int        syncTime(string &json);

    virtual int        customCommand(string &json);

	virtual int        requestConfig(string &jin, string &jout);

	virtual int        customCommandEx(string &json);

    virtual void       setbev(Bufferev *bev);

    virtual void       onTimer();

protected:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onClose(Bufferev *bev);
	virtual bool       buildResponsePacket(string &msg, char *stream, int &size) = 0;

protected:
    virtual void       onChanged(const string &json);
    virtual string     peerip();
    virtual int        peerport();
    virtual string     peertime();

protected:
    virtual void       onAlarm(string &json, void *data, uint32_t size);
    virtual void       onAIYX(string &json, void *data, uint32_t size);
    virtual void       onAIYC(string &json, void *data, uint32_t size);

private:
    int                onGateCtrl(string &json);//20200603 zhangshenyu add
    int                onVehicleCtrl(string &json);

protected:
    int                m_type;
    Bufferev         * m_bev;
    uint64_t           m_lastHeart;
    vector<uint8_t>    m_recvBuf;

private:
    volatile bool      m_active;

    //GnuID              m_key;
    string             m_userKey;
    Thread             m_mainThread;
    Thread             m_pushThread;

    string             m_name;
    string             m_userId;
    string             m_userCode;
    uint64_t           m_mright;
    uint64_t           m_oright;

    string             m_ip;
    int                m_port;

    string             m_time;

    uint8_t            m_typeBits[BITS_TO_BYTES(TR_LAST)];

    SessMgr            m_sessMgr;
    DSessMgr           m_dutalkSessMgr;
};
#endif
