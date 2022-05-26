#ifndef __GB28181_SIPUA_H__
#define __GB28181_SIPUA_H__

#include "sipua.h"
#include <map>

class Pu;


/**
* ???��????
*/
enum {
    GB28181_PTZ_CMD_IRISCLOSE       = 0x48,   /**< 1a��|1?  */
    GB28181_PTZ_CMD_IRISOPEN        = 0x44,   /**< 1a��|?a  */

    GB28181_PTZ_CMD_FOCUSNEAR       = 0x42,   /**< ?��???1  */
    GB28181_PTZ_CMD_FOCUSFAR        = 0x41,   /**< ?????1  */

    GB28181_PTZ_CMD_ALLSTOP_LENS    = 0x40,   /**< ����?1?�̨���?����D?������  */

    GB28181_PTZ_CMD_ZOOMIN          = 0x10,   /**< ��?�䨮  */
    GB28181_PTZ_CMD_ZOOMOUT         = 0x20,   /**< ??D?  */

    GB28181_PTZ_CMD_UP              = 0x08,   /**< ?����?  */
    GB28181_PTZ_CMD_DOWN            = 0x04,   /**< ?��??  */

    GB28181_PTZ_CMD_RIGHT           = 0x01,   /**< ������a  */
    GB28181_PTZ_CMD_LEFT            = 0x02,   /**< ������a  */

    GB28181_PTZ_CMD_UPLEFT          = 0x0A,   /**< ������?��??��???��  */
    GB28181_PTZ_CMD_DOWNLEFT        = 0x06,   /**< ����??��??��???��  */

    GB28181_PTZ_CMD_UPRIGHT         = 0x09,   /**< ������?��??��???��  */
    GB28181_PTZ_CMD_DOWNRIGHT       = 0x05,   /**< ����??��??��???��  */

    GB28181_PTZ_CMD_ALLSTOP_PTZ     = 0x00,   /**< ����?1?����D??����2������  */

    GB28181_PTZ_CMD_SETPRESET       = 0x81,   /**< ?��????�����?  */
    GB28181_PTZ_CMD_CALLPRESET      = 0x82,   /**< ?��????�̡¨�?  */
    GB28181_PTZ_CMD_CLEARPRESET     = 0x83,   /**< ?��????��?3y  */


    GB28181_PTZ_CMD_RAIN_ON         = 0x8C,   /**< ?��?��?a1??������:?a  */
    GB28181_PTZ_CMD_RAIN_OFF        = 0x8D,   /**< ?��?��?a1??�����¡�o1?  */
};

/* ��������????��y?Y??�̡� */
typedef void(*pfnRecordCallback)(char *data, int size, void *pUser);


typedef struct
{
    int sumNum;                         /**< ?����?������y */
    int revNum;                         /**< ��?��?��??����?��y */
    uint64_t generateTime;              /**< ��y?Y�㨹���?������??��?��?����3?����??���� */
    list<string> *listJson;             /**< ��?��?��?��y?Y */
}GB28181_Query_Context_T;

typedef struct
{
    uint32_t  u32SN;                    /**< ?����?D����Do? */
    int8_t * ps8ChnAddrCode;            /**< ??����������?��??������?? */
    uint32_t u32SubType;                /**< ????����D��:0-��??t��?1-???? */
    int8_t * ps8StartTime;              /**< ����?��?e��?����?? */
    int8_t * ps8EndTime;                /**< ����?��?����?����?? */
}GB28181_SIP_SUBSCRIBE_S;

typedef struct
{
    uint32_t  u32SN;                    /**< ?����?D����Do? */
    int8_t  * ps8ChnAddrCode;           /**< ??����������?��??������?? */
}GB28181_SIP_QUERY_CATALOG_S;

typedef struct
{
    uint32_t  u32SN;                    /**< ?����?D����Do? */
    int8_t  * ps8ChnAddrCode;           /**< ??����������?��??������?? */
    int8_t  * ps8PTZCmd;                /**< ??����?????����?*/
}GB28181_SIP_CONTROL_DEVCTRL_S;

typedef struct
{
    uint32_t  u32SN;                    /**< ?����?D����Do? */
    int8_t  * ps8ChnAddrCode;           /**< ??����������?��??������?? */
    bool      zoomin;                   /**< ��?��?��?�䨮*/
    int32_t     winW;                   /**< 2����?���?��3��?��?????��*/
    int32_t     winH;                   /**< 2����?���?��?��?��?????��*/
    int32_t     midPointX;              /**< ��-?��?DD?��?o��?����?����?????��*/
    int32_t     midPointY;              /**< ��-?��?DD?��?��Y?����?����?????��*/
    int32_t     lengthX;                /**< ��-?��3��?��?????��*/
    int32_t     lengthY;                /**< ��-?��?��?��?????��*/
}GB28181_SIP_CONTROL_DRAGZOOM_S;

typedef struct
{
    uint32_t  u32SN;                /**< ?����?D����Do? */
    int8_t  * ps8ChnAddrCode;          /**< ??����������?��??������?? */
    int32_t start;
    int32_t end;
    int32_t type;

    pfnRecordCallback cb;
    void    *userdata;
}GB28181_SIP_QUERY_RECORD_S;

typedef struct
{
    string   strType;
    GB28181_Query_Context_T context;
    union
    {
        GB28181_SIP_QUERY_CATALOG_S  stResouceReq;
        GB28181_SIP_CONTROL_DEVCTRL_S  stDevctrlReq;
        GB28181_SIP_CONTROL_DRAGZOOM_S  stDragzoomReq;
        GB28181_SIP_SUBSCRIBE_S stSubscribeReq;
        GB28181_SIP_QUERY_RECORD_S stRecordReq;
    };
}GB28181_SIP_REQ_S;


class GB28181_SipUA : public SipUA
{
    enum
    {
        T_EVENTLOOP,
        T_DATAPROLOOP,
    };
public:
    GB28181_SipUA(int transport, string bindIp, int port);
    virtual ~GB28181_SipUA();

    friend void send_message_rsp_cb(string response, int code, int tid, void *userdata);
public:
    virtual int         start();
    virtual void        stop();
    virtual int        regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first);

    virtual int        message(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &xml, string &xresp);

    virtual int        notify(const string &event, int did, const string &xml, string &jresp, string contact);

public:
    int query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        string &content, int jx = 0, int fromidx = 1, int toidx = 1000);

    int ctrl_camera(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        int cmd, int param1 = 0, int param2 = 0, int param3 = 0);

    int query_record(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        int start, int end, int type, string &content);

    int subscribe_envent(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, int subType, datetime_s begin, datetime_s end, int expire);


public:
    virtual int        push_alarm(const string &time, string &jalarms, int did, Superior *super);
    virtual int        push_status(const string &code, string &jstatus, int did, Superior *super);
    virtual int        push_catalog(const string &time, string &xml, int did, Superior *super);
    virtual int        push_mediaStatus(const string& gucode, const string& strType, Superior* super);
    

protected:
    virtual int        on_notify_response(osip_message_t *sip) { return 0; }
    virtual int        on_invite_response(osip_message_t *sip);
    virtual int        on_message_response(osip_message_t *sip);
    virtual int        on_subscribe_response(osip_message_t *sip){ return 0; }


protected:
    virtual int        on_register(osip_message_t *sip);
    virtual int        on_notify(osip_message_t *sip) { return 0; }
    virtual int        on_subscribe(osip_message_t *sip);
    virtual int        on_message(osip_message_t *sip);
    virtual int        on_info(osip_message_t *sip);
    virtual int        on_invite(osip_message_t *sip);
    virtual int        on_ack(osip_message_t *sip);
    virtual int        on_bye(osip_message_t *sip);

protected:
    int                on_parse_requestEx(osip_message_t *sip, string &xml, Superior *super = NULL, string fromh = "", int fromp = 0,Pu *pu=NULL);
    virtual int        on_parse_request(osip_message_t *sip, string &xml, Superior *super = NULL){return 0;}
    virtual int        on_parse_response(osip_message_t *sip, string &json, Superior *super = NULL) { return 0; }
    virtual int        on_parse_subscribe(osip_message_t *sip, string &xml, Superior *super = NULL);

protected:
    int                on_resource(const string &json, string &xml, Superior *super = NULL);
    int                on_deviceCtrl(const string &json, string &xml, Superior *super = NULL);
	int                on_RecordInfo(const string &json, string &xml, Superior *super = NULL);
    int                on_history_alarm(const string &json, string &xml, Superior *super = NULL);
    int                on_subscribe(const string &json, string &xml, Superior *super = NULL, int did = -1, int expires = 0);
    int                on_dragZoom(const string &json, string &xml, Superior *super = NULL) const;
    int                on_catalogRsp(const string &json, string &xml, Superior *super = NULL);
    int                on_recordRsp(const string &json, string &xml, Superior *super = NULL);

protected:
    void              processCatalog(GB28181_SIP_REQ_S *req);
    void              processRecord(GB28181_SIP_REQ_S *req);

public:
    uint32_t            GenerateSN();
    int                makeXmlReq(const GB28181_SIP_REQ_S *req, string& xml);
    void               dataProloop();
    static void      * GB28181_threadProc(ThreadInfo *arg);

protected:
    uint32_t           m_SN;
    Thread             m_dataProThread;
    Mutex              m_mtxSN;
    bool               m_bFirstRegister;
    bool               m_bRegisted;
    map<uint32_t, GB28181_SIP_REQ_S*> m_mapSN;

    int                m_subsribeAlarmId;
    int                m_subsribeCatalogId;
};




#endif // __GB28181_SIPUA_H__