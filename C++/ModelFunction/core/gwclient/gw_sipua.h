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
#include "gwsdk.h"

class GWSipUA : public SipUA
{
public:
	GWSipUA(string bindip, int port, char *addrcode);
    virtual ~GWSipUA();

public:
	int                query_resource(const string &ton, const string &toh, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser);

	int                query_record(const string &ton, const string &toh, int top, const string &usercode, 
		                 const string &begin, const string &end, uint32_t type, pfnRecordCallback fnCb, void *pUser);

    int                query_alarm(const string &ton, const string &toh, int top, const string &code, 
		                     const string &begin, const string &end, uint32_t type, int level, pfnAlarmCallback fnCb, void *pUser);

    int                ctrl_camera(const string &ton, const string &toh, int top, const string &code,
                                   int cmd, int param1 = 0, int param2 = 0, int param3 = 0);

    int                subscribe_event(const string &ton, const string &toh, int top, const string &code,
		                     int subsType, int alarmType = 0, int expires = 3600);
							 
	int                refresh_subevent(const string &ton, const string &toh, int top, const string &code,
		                     int subsType, int alarmType = 0, int expires = 3600);

	int                query_dev_ability(const string &ton, const string &toh, int top, const string &code, pfnDevAbilityCallback fnCb, void *pUser);

	int                do_algorithm_task(const string &ton, const string &toh, int top, const string &code, const char *json, int size);

	int                do_picture_snap(const string &ton, const string &toh, int top, const string &code, const char *json, int siz);

	int                query_dev_rule(const string &ton, const string &toh, int top, const string &code, pfnDevRuleCallback fnCb, void *pUser);

	int                query_all_task(const string &ton, const string &toh, int top, pfnAllTaskCallback fnCb, void *pUser);

	int                query_flow(const string &ton, const string &toh, int toport, const string &usercode, int &bandWidth, int &streamNum);

protected:
    virtual int        on_notify_response   (osip_message_t *sip);
    virtual int        on_invite_response   (osip_message_t *sip);
    virtual int        on_message_response  (osip_message_t *sip);
    virtual int        on_subscribe_response(osip_message_t *sip);
	virtual int        on_register(osip_message_t *sip);
	virtual int        on_notify(osip_message_t *sip);

private:
	bool               m_bFirstRegister;
	bool               m_bRegisted;
    bool               m_bSubAlarm;
    bool               m_bSubStatus;
};
#endif
