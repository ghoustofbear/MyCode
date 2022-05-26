/**
 * @file      superior.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-15 11:24:40
 *
 * @note
 *  superior.h defines
 */
#ifndef __SUPERIOR_H__
#define __SUPERIOR_H__
#include "triggerlistener.h"
#include "thread.h"
#include "tlist.h"
#include "gw_pu.h"


class Superior : public TriggerListener
{
public:
    enum
    {
        T_REGISTER,
		T_KEEPALIVE
    };

    Superior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort, int httpPort, int expires, string ifupIp);
    virtual ~Superior();
    
public:
    string             addrcode();
	string             host();
    string             name();
	int                hport();
    int                sport();
    int                status();
    int                protocol();
    string             ifnetip();
    bool               isSame(string host, int port);
    int                useivalocal() { return m_ivauselocal; }

    virtual int        push_resource();

	void               setregisterstatus(bool bRegister);
    void               setuseivalocalcfg(bool bUseivalocal);

    int                push(const string &type, const string &json);

    int                push_alarm(const string &json);

    int                push_status(const string &json);

    int                 push_resource(const string &json);

    virtual int        on_RecordInfo(const string &json, string &xml) { return 0; }
    virtual int        on_resource(const string &json, string &xml);

public:
    virtual int        start();
    virtual void       stop();

    virtual void       timerdriver();

protected:
    virtual void       onAlarm(string &json, void *data, uint32_t size);
    virtual void       onAIYX(string &json, void *data, uint32_t size);
    virtual void       onAIYC(string &json, void *data, uint32_t size);

protected:
    void               registloop();
    virtual int        keepaliveLoop();
    static void *      threadProc(ThreadInfo *);


protected:
    int                m_protocol;
    int                m_status;
    int                m_rid;
    bool               m_registed;
    int                m_ivauselocal;
protected:
    string             m_addrcode;
    string             m_name;
    string             m_user;
    string             m_pswd;

    string             m_host;
    int                m_sport;
    int                m_rport;
    int                m_hport;
    int                m_expires;

    string             m_ifnetip;

    Thread             m_registThread;
    Thread             m_keekaliveThread;


public:
    int                m_subscribedid;                         /**< subscribe alarm dialog id */
    int                m_subscribeCatalogDid;                         /**< subscribe catalog dialog id */

    uint32_t           m_alramtype;
	uint32_t           m_statustype;
	bool               m_bUnregister;
    bool               m_subCatalog;
    datetime_s         m_beginTimeAlarm;
    datetime_s         m_endTimeAlarm;
    datetime_s         m_beginTimeCatalog;
    datetime_s         m_endTimeCatalog;

    typedef struct
    {
        string type;
        string json;
    }AlarmInfo;
    tlist<AlarmInfo>  m_lsAlarmData;
};

#endif
