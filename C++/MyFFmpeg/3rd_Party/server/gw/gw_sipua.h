/**
 * @file      gw_sipua.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-12 10:46:19
 *
 * @note
 * int jx;   0: xml 1:json
 */
#ifndef __GW_SIPUA_H__
#define __GW_SIPUA_H__
#include "sipua.h"


class Superior;
class GWSipUA : public SipUA
{
public:
    GWSipUA(int transport, string bindip, int port);
    virtual ~GWSipUA();

    enum
    {
        T_ON_CTRL,
    };

    struct ParamTh_T
    {
        GWSipUA *pThis;
        Superior *pSuper;
        string json;
        int tid;
    };

    friend void cb_message_rsp(string response, int code, int tid, void *userdata);
public:
	int                query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
                                      string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                query_record(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                    const string &begin, const string &end, uint32_t type, string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                query_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                   const string &begin, const string &end, uint32_t type, int level, string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                ctrl_camera(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
                                   int cmd, int param1 = 0, int param2 = 0, int param3 = 0);

    int                subscribe_event(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
                                   int subsType, int expires = 0, int alarmType = 0);

    int                push_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                  const string &time, int32_t type, int did = -1);

    int                push_status(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                   int status, int did = -1);

public:
	virtual int        push_alarm(const string &time, string &jalarms, int did, Superior *super);
	virtual int        push_status(const string &code, string &jstatus, int did, Superior *super);

	int                push_ivs(const string &time, string &jivs, int did);
	int                push_vi(const string &time, string &jvi, int did);

protected:
    virtual int        regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first);

protected:
    virtual int        on_notify_response   (osip_message_t *sip);
    virtual int        on_invite_response   (osip_message_t *sip);
    virtual int        on_message_response  (osip_message_t *sip);
    virtual int        on_subscribe_response(osip_message_t *sip);

protected:
    virtual int        on_register (osip_message_t *sip);
    virtual int        on_notify   (osip_message_t *sip);
    virtual int        on_subscribe(osip_message_t *sip);
    virtual int        on_message  (osip_message_t *sip);
    virtual int        on_info     (osip_message_t *sip){ return 0; };
    virtual int        on_invite   (osip_message_t *sip);
    virtual int        on_ack      (osip_message_t *sip);
    virtual int        on_bye      (osip_message_t *sip);

protected:
    virtual int        on_parse_request (osip_message_t *sip, string &xml,  Superior *super = NULL);
    virtual int        on_parse_response(osip_message_t *sip, string &json, Superior *super = NULL);

    int                on_resource     (const string &json, string &xml, Superior *super = NULL);
    int                on_ctrl_camera  (const string &json, string &xml, Superior *super = NULL);
    int                on_history_video(const string &json, string &xml, Superior *super = NULL);
	int                on_history_alarm(const string &json, string &xml, Superior *super = NULL);
	int                on_query_flow(const string &json, string &xml, Superior *super = NULL);
	int                on_push_resource(const string &json, string &xml, Superior *super = NULL);

	int                on_subscribe_alarm(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
	int                on_subscribe_status(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);

    int                onQueryAlgAbility(const string &json, string &xml, Superior *super = NULL);
    int                onTaskDispatch(const string &json, string &xml, Superior *super = NULL);
    int                onReqAllTask(const string &json, string &xml, Superior *super = NULL);
    int                onReqSignalTask(const string &json, string &xml, Superior *super = NULL);
    int                onCameraSnap(const string &json, string &xml, Superior *super = NULL);

    int                setPuMessageCB(string json);
private:
    bool               m_bFirstRegister;
    bool               m_bRegisted;

};
#endif
