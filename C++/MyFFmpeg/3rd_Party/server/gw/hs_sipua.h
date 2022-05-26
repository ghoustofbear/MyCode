/**
 * @file      hs_sip_ua.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-15 14:45:56
 *
 * @note
 *  hs_sip_ua.h defines
 */
#ifndef __HS_SIP_UA_H__
#define __HS_SIP_UA_H__
#include "sipua.h"

class Superior;
class HSSipUA : public SipUA
{
public:
    HSSipUA(int transport, string bindip, int port);
    virtual ~HSSipUA();

public:
	int                query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
                                      string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                query_record(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                    const string &begin, const string &end, uint32_t type, string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                query_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                   const string &begin, const string &end, uint32_t type, int level, string &content, int jx = 0, int fromidx = 1, int toidx = 100);

    int                push_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, 
                                   const string &time, int32_t type);

    int                push_status(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, int status);
    int                push_ivs(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, int status);
    int                push_vi(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code, int status);

    int                push_ivs(const string &time, string &jivs, int did);
    int                push_vi (const string &time, string &jvi,  int did);

public:
	virtual int        push_alarm(const string &time, string &jalarms, int did, Superior *super);
	virtual int        push_status(const string &code, string &jstatus, int did, Superior *super);

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
    virtual int        on_invite   (osip_message_t *sip);
    virtual int        on_ack      (osip_message_t *sip);
    virtual int        on_bye      (osip_message_t *sip);

protected:
    virtual int        on_parse_request (osip_message_t *sip, string &xml,  Superior *super = NULL);
    virtual int        on_parse_response(osip_message_t *sip, string &json, Superior *super = NULL);

    int                on_resource    (const string &json, string &xml, Superior *super = NULL);
    int                on_dv_resource (const string &json, string &xml, Superior *super = NULL);
    int                on_vi_resource (const string &json, string &xml, Superior *super = NULL);
    int                on_axf_resource(const string &json, string &xml, Superior *super = NULL);
    int                on_est_resource(const string &json, string &xml, Superior *super = NULL);

    int                on_ctrl_camera  (const string &json, string &xml, Superior *super = NULL);
    int                on_history_video(const string &json, string &xml, Superior *super = NULL);

	int                on_subscribe_alarm(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
	int                on_subscribe_status(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
    int                on_subscribe_ivs   (const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
    int                on_subscribe_vi    (const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
private:
};

#endif
