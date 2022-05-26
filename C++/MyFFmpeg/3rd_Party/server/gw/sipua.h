/**
 * @file      sip_ua.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-15 14:19:27
 *
 * @note
 *  sip_ua.h defines
 *  SuperMgr was created only when SipUA was created
 */
#ifndef __SIP_UA_H__
#define __SIP_UA_H__
#include "server.h"
#include "thread.h"
#include "tevqueue.h"
#include "tmap.h"
#include "json/json.h"
#include "eXosip2/eXosip.h"

class OnUiUpdate;
class Superior;

struct eXosip_t;
struct eXosip_event;
struct osip_uri;

typedef struct osip_message osip_message_t;
typedef tevqueue<std::string, SHL_Json::Value> CallQueue;



typedef struct
{
    int8_t  * ps8AddrCode;          /**< 目标设备地址编码 */
    int32_t   s32FromIndex;         /**< 期望返回的开始记录数目 */
    int32_t   s32ToIndex;           /**< 期望返回的结束记录数目 */
}SIP_RESOURCE_REQ_S;

typedef struct
{
    int8_t  * ps8ChnAddrCode;       /**< 目标设备通道地址编码 */
    int8_t  * ps8UserAddrCode;      /**< 请求用户地址编码 */

    uint32_t  u32RecType;           /**< 录象类型 bit0 - Normal, bit1 - Motion, bit2 - IO-Alarm, 0xffffffff - ALL */
    int8_t  * ps8Begin;             /**< 开始时间 */
    int8_t  * ps8End;	    	    /**< 结束时间 */

    int32_t   s32FromIndex;         /**< 期望返回的开始记录数目 */
    int32_t   s32ToIndex;           /**< 期望返回的结束记录数目 */
}SIP_RECORD_REQ_S;


typedef struct
{
    int8_t  * ps8ChnAddrCode;       /**< 目标设备通道地址编码 */
    uint32_t   u32Cmd;			    /**< 云镜控制  */
    uint32_t   u32Param1;	        /**< 参数1  横向运动速度，1-9 ，或预置位编号1-128 */
    uint32_t   u32Param2;	        /**< 参数2  纵向运动速度，1-9  */
    uint32_t   u32Param3;	        /**< 参数3  保留 */
}SIP_PTZ_REQ_S;

typedef struct
{
    int8_t  * ps8ChnAddrCode;       /**< 目标设备通道地址编码 */
    uint32_t   u32SubsType;          /**< 订阅类型，0 - 事件alarm  1 - 状态presence  */
    uint32_t   u32Mode;              /**< 0 - 启动订阅  1 - 刷新订阅  2 -取消订阅  */
    uint32_t   u32Expires;           /**< 刷新周期 */
    uint32_t   u32AlarmType;         /**< 报警类型 */
}SIP_SUBCRIBLE_REQ_S;

typedef struct
{
    int8_t  * ps8ChnAddrCode;       /**< 目标设备通道地址编码 */
    int8_t  * ps8UserAddrCode;      /**< 请求用户地址编码 */

    uint32_t   u32AlarmType;         /**< 录象类型 bit0 - Normal, bit1 - Motion, bit2 - IO-Alarm, 0xffffffff - ALL */
    uint32_t   u32AlarmLevel;        /**< 告警级别 */
    int8_t  * ps8Begin;             /**< 开始时间 */
    int8_t  * ps8End;	    	    /**< 结束时间 */

    int32_t   s32FromIndex;         /**< 期望返回的开始记录数目 */
    int32_t   s32ToIndex;           /**< 期望返回的结束记录数目 */

}SIP_ALARMLOG_REQ_S;

typedef struct
{
    int8_t  * ps8FrontAddrCode;     /**< 前端设备地址编码 */
    int8_t  * ps8Name;              /**< 前端设备名称 */
    int8_t  * ps8Begin;             /**< 报警开始时间 */
    uint32_t   u32AlarmType;         /**< 报警类型 */
    uint32_t   u32Status;            /**< 活动状态 */
    int8_t  * ps8Plate;             /**< 车牌 */
    uint32_t   u16CarType;           /**< 车辆类型 */
    uint32_t   u16CarDirect;         /**< 进出方向 */
    int8_t  * ps8PlateUrl;          /**< 车牌httpurl */
}SIP_EVENTRPT_REQ_S;

typedef struct
{
    int8_t  * ps8AddrCode;          /**< 目标系统地址编码 */
    int8_t  * ps8UserAddrCode;      /**< 请求用户地址编码 */
}SIP_FLOW_REQ_S;

typedef struct
{
    string   strType;
    union
    {
        SIP_RESOURCE_REQ_S  stResouceReq;
        SIP_RECORD_REQ_S    stRecordReq;
        SIP_PTZ_REQ_S       stPtzReq;
        SIP_SUBCRIBLE_REQ_S stSubscribeReq;
        SIP_ALARMLOG_REQ_S  stAlarmLogReq;
        SIP_EVENTRPT_REQ_S  stEventRptReq;
        SIP_FLOW_REQ_S      stFlowReq;
    };
}SIP_REQ_S;

/**
* 云镜控制
*/
enum {
	PTZ_CMD_0101 = 0x0101,   /**< 光圈关停止  */
	PTZ_CMD_0102 = 0x0102,   /**< 光圈关  */
	PTZ_CMD_0103 = 0x0103,   /**< 光圈开  */
	PTZ_CMD_0104 = 0x0104,   /**< 光圈开停止  */

	PTZ_CMD_0201 = 0x0201,   /**< 近聚焦停止  */
	PTZ_CMD_0202 = 0x0202,   /**< 近聚焦  */
	PTZ_CMD_0203 = 0x0203,   /**< 远聚焦停止  */
	PTZ_CMD_0204 = 0x0204,   /**< 远聚焦  */

	PTZ_CMD_0301 = 0x0301,   /**< 缩小停止  */
	PTZ_CMD_0302 = 0x0302,   /**< 缩小  */
	PTZ_CMD_0303 = 0x0303,   /**< 放大停止  */
	PTZ_CMD_0304 = 0x0304,   /**< 放大  */

	PTZ_CMD_0401 = 0x0401,   /**< 向上停止  */
	PTZ_CMD_0402 = 0x0402,   /**< 向上  */
	PTZ_CMD_0403 = 0x0403,   /**< 向下停止  */
	PTZ_CMD_0404 = 0x0404,   /**< 向下  */

	PTZ_CMD_0501 = 0x0501,   /**< 右转停止  */
	PTZ_CMD_0502 = 0x0502,   /**< 右转  */
	PTZ_CMD_0503 = 0x0503,   /**< 左转停止  */
	PTZ_CMD_0504 = 0x0504,   /**< 左转  */

	PTZ_CMD_0601 = 0x0601,   /**< 预置位保存  */
	PTZ_CMD_0602 = 0x0602,   /**< 预置位调用  */
	PTZ_CMD_0603 = 0x0603,   /**< 预置位删除  */
	PTZ_CMD_0604 = 0x0604,   /**<   */

	PTZ_CMD_0701 = 0x0701,   /**< 左上方向运动停止  */
	PTZ_CMD_0702 = 0x0702,   /**< 左上方向运动  */
	PTZ_CMD_0703 = 0x0703,   /**< 左下方向运动停止  */
	PTZ_CMD_0704 = 0x0704,   /**< 左下方向运动  */

	PTZ_CMD_0801 = 0x0801,   /**< 右上方向运动停止  */
	PTZ_CMD_0802 = 0x0802,   /**< 右上方向运动  */
	PTZ_CMD_0803 = 0x0803,   /**< 右下方向运动停止  */
	PTZ_CMD_0804 = 0x0804,   /**< 右下方向运动  */

	PTZ_CMD_0901 = 0x0901,   /**< 停止当前动作  */


	PTZ_CMD_0A01 = 0x0A01,   /**< 雨刷开  */
	PTZ_CMD_0A02 = 0x0A02,   /**< 雨刷关  */

	PTZ_CMD_0B01 = 0x0B01,   /**< 灯亮  */
	PTZ_CMD_0B02 = 0x0B02,   /**< 灯灭  */

	PTZ_CMD_0C01 = 0x0C01,   /**< 加热开  */
	PTZ_CMD_0C02 = 0x0C02,   /**< 加热关  */

	PTZ_CMD_0D01 = 0x0D01,   /**< 红外开  */
	PTZ_CMD_0D02 = 0x0D02,   /**< 红外关  */

	PTZ_CMD_0E01 = 0x0E01,   /**< 线性扫描开始  */
	PTZ_CMD_0E02 = 0x0E02,   /**< 线性扫描停止  */

	PTZ_CMD_0F01 = 0x0F01,   /**< 轨迹巡航开始  */
	PTZ_CMD_0F02 = 0x0F02,   /**< 轨迹巡航停止  */

	PTZ_CMD_1001 = 0x1001,   /**< 预置位巡航开始  */
	PTZ_CMD_1002 = 0x1002,   /**< 预置位巡航开始  */

	PTZ_CMD_1101 = 0x1101,   /**< 云台锁定  */
	PTZ_CMD_1102 = 0x1102,   /**< 云台解锁  */

	PTZ_CMD_3DXX = 0xFFF0  /**< 3D定位 */
};

class SipUA : public Server
{
    enum
    {
        T_EVENTLOOP,
    };

public:


    SipUA(int transport, string bindIp, int port);
    virtual ~SipUA();

public:
    virtual int        start();
    virtual void       stop();

    virtual int        regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first) = 0;

    virtual int        subscribe(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, 
                                const string &event, int expires, const string &xml, string &jresp, int &subscribeId);

	virtual int        refreshsubscribe(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top,
		                         const string &event, int expires, const string &xml, string &jresp,int &subscribeId);

    virtual int        message(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, 
                               const string &xml, string &xresp);

    virtual int        invite (const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, 
                               const string &sdp, string &jresp);

    virtual int        notify (const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, 
                               const string &event, const string &xml, string &jresp);

    virtual int        notify (const string &event, int did, const string &xml, string &jresp, string contact);

    virtual int        info(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, int did, const string &rtsp, string &jresp);

    virtual int        ack();

	int                baseport();

	virtual void       bye(int32_t s32CallId, int32_t s32DialogId);

	virtual int        push_resource(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top,
		                      const string &xml, string &xresp);
    
public:
    virtual int        push_alarm (const string &time, string &jalarms, int did, Superior *super) = 0;
    virtual int        push_status(const string &code, string &jstatus, int did, Superior *super) = 0;
    virtual int        push_catalog(const string &time, string &xml, int did, Superior *super){ return 0; }
    virtual int        push_mediaStatus(const string& gucode, const string& strType, Superior* super){ return 0; }

protected:
    virtual int        on_register_response (osip_message_t *sip);
    virtual int        on_notify_response   (osip_message_t *sip) = 0;
    virtual int        on_invite_response   (osip_message_t *sip) = 0;
    virtual int        on_message_response  (osip_message_t *sip) = 0;
    virtual int        on_subscribe_response(osip_message_t *sip) = 0;
    virtual int        on_bye_response(osip_message_t *sip);
    virtual int        on_info_response(osip_message_t *sip);

    virtual int        doAnswer(int code);

protected:
    virtual int        on_register (osip_message_t *sip) = 0;
    virtual int        on_notify   (osip_message_t *sip) = 0;
    virtual int        on_subscribe(osip_message_t *sip) = 0;
    virtual int        on_message  (osip_message_t *sip) = 0;
    virtual int        on_info     (osip_message_t *sip) = 0;
    virtual int        on_invite   (osip_message_t *sip) = 0;
    virtual int        on_ack      (osip_message_t *sip) = 0;
    virtual int        on_bye      (osip_message_t *sip) = 0;

	int                on_subscribe_alarm(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
	int                on_subscribe_status(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);

protected:
    virtual int        on_parse_request (osip_message_t *sip, string &xml,  Superior *super = NULL) = 0;
    virtual int        on_parse_response(osip_message_t *sip, string &json, Superior *super = NULL) = 0;

    virtual void       timerdriver();

protected:
    int                on_request (osip_message_t *sip);
    int                on_response(osip_message_t *sip);

    int                makeXmlReq(SIP_REQ_S req, string& xml);
	int                parseXml(const string &toCode, const string &xml, SHL_Json::Value &json);

    string           & osip2str(osip_message_t *sip, string &str);
   
    void               eventloop();
    static void      * threadProc(ThreadInfo *arg);

protected:
    int                m_protocol;
    eXosip_t         * m_eXcontext;
    eXosip_event     * m_eXevent;
    osip_uri         * m_sipUrl;                           /**< SIP request URL path */


    CallQueue          m_cqueue;                           /* call queue */
    tmap<string, int>  m_toPortMap;                        /* invate sdp to port map */
    int                m_subscribeAlarmId;
    int                m_subscribeStatusId;
    osip_contact_t   * m_contact;

    Thread             m_eventThread;
private:
    static int         m_baseport;
};


/**
 * eXAutolock
 */
class eXAutolock
{
public:
    eXAutolock(eXosip_t *&eXosip);
    ~eXAutolock();

private:
    eXosip_t        *& m_eXcontext;
};

extern SipUA * sipUA;
#endif
