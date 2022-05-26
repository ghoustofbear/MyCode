#ifndef __GB28181_SIPUA_H__
#define __GB28181_SIPUA_H__

#include "eXosip2/eXosip.h"
#include <string>
#include "thread.h"
#include "tevqueue.h"
#include "tmap.h"
#include "json.h"
#include "gb28181sdk.h"

struct eXosip_t;
struct eXosip_event;
struct osip_uri;

typedef struct osip_message osip_message_t;
typedef tevqueue<std::string, SHL_Json::Value> CallQueue;

/**
* �ƾ�����
*/
enum {
    GB28181_PTZ_CMD_IRISCLOSE       = 0x48,   /**< ��Ȧ��  */
    GB28181_PTZ_CMD_IRISOPEN        = 0x44,   /**< ��Ȧ��  */

    GB28181_PTZ_CMD_FOCUSNEAR       = 0x42,   /**< ���۽�  */
    GB28181_PTZ_CMD_FOCUSFAR        = 0x41,   /**< Զ�۽�  */

    GB28181_PTZ_CMD_ALLSTOP_LENS    = 0x40,   /**< ֹͣ��ͷ���ж���  */

    GB28181_PTZ_CMD_ZOOMOUT         = 0x20,   /**< ��С  */
    GB28181_PTZ_CMD_ZOOMIN          = 0x10,   /**< �Ŵ�  */

    GB28181_PTZ_CMD_UP              = 0x08,   /**< ����  */
    GB28181_PTZ_CMD_DOWN            = 0x04,   /**< ����  */

    GB28181_PTZ_CMD_RIGHT           = 0x01,   /**< ��ת  */
    GB28181_PTZ_CMD_LEFT            = 0x02,   /**< ��ת  */

    GB28181_PTZ_CMD_UPLEFT          = 0x0A,   /**< ���Ϸ����˶�  */
    GB28181_PTZ_CMD_DOWNLEFT        = 0x06,   /**< ���·����˶�  */

    GB28181_PTZ_CMD_UPRIGHT         = 0x09,   /**< ���Ϸ����˶�  */
    GB28181_PTZ_CMD_DOWNRIGHT       = 0x05,   /**< ���·����˶�  */

    GB28181_PTZ_CMD_ALLSTOP_PTZ     = 0x00,   /**< ֹͣ������̨����  */

    GB28181_PTZ_CMD_SETPRESET       = 0x81,   /**< Ԥ��λ����  */
    GB28181_PTZ_CMD_CALLPRESET      = 0x82,   /**< Ԥ��λ����  */
    GB28181_PTZ_CMD_CLEARPRESET     = 0x83,   /**< Ԥ��λɾ��  */


    GB28181_PTZ_CMD_RAIN_ON         = 0x8C,   /**< �������ض���:��  */
    GB28181_PTZ_CMD_RAIN_OFF        = 0x8D,   /**< �������ض�������  */
};

typedef struct
{
    int sumNum;                         /**< �ڵ����� */
    int revNum;                         /**< ���յ��ڵ��� */
    uint64_t generateTime;              /**< ���ݰ�����ʱ�䣬���ڳ�ʱ���� */
    pfnQueryResourceCallback cb;        /**< �û��ص� */
    void    *userdata;                  /**< �û����� */
    list<string> *listJson;             /**< ���յ����� */
}Query_Context_T;

typedef struct
{
    uint32_t  u32SN;                    /**< �������к� */
    int8_t * ps8ChnAddrCode;            /**< Ŀ���豸��ַ���� */
    uint32_t u32SubType;                /**< ��������:0-�¼���1-Ŀ¼ */
    int8_t * ps8StartTime;              /**< ������ʼʱ�� */
    int8_t * ps8EndTime;                /**< ��������ʱ�� */
}SIP_SUBSCRIBE_S;

typedef struct
{
    uint32_t  u32SN;                    /**< �������к� */    
    int8_t  * ps8ChnAddrCode;           /**< Ŀ���豸��ַ���� */
}SIP_QUERY_CATALOG_S;

typedef struct
{
    uint32_t  u32SN;                    /**< �������к� */
    int8_t  * ps8ChnAddrCode;           /**< Ŀ���豸��ַ���� */
    int8_t  * ps8PTZCmd;                /**< ��̨��������*/
}SIP_CONTROL_DEVCTRL_S;

typedef struct
{
    uint32_t  u32SN;                    /**< �������к� */
    int8_t  * ps8ChnAddrCode;           /**< Ŀ���豸��ַ���� */
    bool      zoomin;                   /**< �Ƿ�Ŵ�*/
    int32_t     winW;                   /**< ���Ŵ��ڳ�������ֵ*/
    int32_t     winH;                   /**< ���Ŵ��ڿ������ֵ*/
    int32_t     midPointX;              /**< �������ĵĺ�����������ֵ*/
    int32_t     midPointY;              /**< �������ĵ�������������ֵ*/
    int32_t     lengthX;                /**< ���򳤶�����ֵ*/
    int32_t     lengthY;                /**< ����������ֵ*/
}SIP_CONTROL_DRAGZOOM_S;

typedef struct
{
	uint32_t  u32SN;                /**< �������к� */
	int8_t  * ps8ChnAddrCode;          /**< Ŀ���豸��ַ���� */
	int32_t start;
	int32_t end;
	int32_t type;

	pfnRecordCallback cb;
	void    *userdata;
}SIP_QUERY_RECORD_S;

typedef struct
{
    string   strType;
    Query_Context_T context;
    union
    {
        SIP_QUERY_CATALOG_S  stResouceReq;
        SIP_CONTROL_DEVCTRL_S  stDevctrlReq;
        SIP_CONTROL_DRAGZOOM_S  stDragzoomReq;
        SIP_SUBSCRIBE_S stSubscribeReq;
		SIP_QUERY_RECORD_S stRecordReq;
    };
}SIP_REQ_S;

class GB28181_SipUA 
{
    enum
    {
        T_EVENTLOOP,
        T_DATAPROLOOP,
        T_CLEANHPLOOP,
    };
    struct HostPort
    {
        string ip;
        int port;
        uint64_t freshTime;
    };
public:
    GB28181_SipUA(std::string bindIp, int port, char *addrcode);
    virtual ~GB28181_SipUA();
    
    friend void send_message_rsp_cb(std::string response, int code, int tid, void *userdata);
public:

    virtual int        start();
    virtual void       stop();

    virtual int        message(const string &ton, const string &toh, int top,const string &xml, string &xresp);

    virtual int        invite(const string &ton, const string &toh, int top,const string &sdp, string &jresp);

    virtual int        info(const string &ton, const string &toh, int top, int did, const string &rtsp, string &jresp);

    virtual int        subscribe(const string &ton, const string &toh, int top, const string &event, int expires, const string &xml, string &jresp, int subType);

    virtual int        refreshsubscribe(const string &ton, const string &toh, int top, const string &event, int expires, const string &xml, string &jresp, int subType);
    
    virtual int        ack();

    virtual void       bye(int32_t s32CallId, int32_t s32DialogId);

    virtual int        doAnswer(int code);
    string             ip() { return m_bindIp; }
    int                port() { return m_listenPort; }
    string              addrcode(){ return m_addrcode; }

    typedef void(*pfnsubscribecb)(const char *data, int size, void *user);
    void               setsubsribecb(pfnsubscribecb fncb, void *user);
    void               setStatusCB(pfnsubscribecb fncb, void *user);

public:
    int                query_resource(const string &ton, const string &toh, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser);

	int                query_record(const string &ton, const string &toh, int top, const string &code, int start, int end, int type, pfnRecordCallback fnCb, void *pUser);

    int                ctrl_camera(const string &ton, const string &toh, int top, const string &code,int cmd, int param1 = 0, int param2 = 0, int param3 = 0);

    int                subscribe_envent(const string &ton, const string &toh, int top, int subType, datetime_s begin, datetime_s end, int expire);

    int                drag_zoom(const string &ton, const string &toh, int top, const string &code, bool zoomIn, int winW, int winH, int midPointX, int midPointY,int lenX,int lenY);

protected:
    virtual int        on_register_response(osip_message_t *sip);
    virtual int        on_notify_response(osip_message_t *sip){ return 0; }
    virtual int        on_invite_response(osip_message_t *sip);
    virtual int        on_info_response(osip_message_t *sip);
    virtual int        on_message_response(osip_message_t *sip);
    virtual int        on_subscribe_response(osip_message_t *sip);
    virtual int        on_bye_response(osip_message_t *sip){ return 0; }



protected:
    virtual int        on_register(osip_message_t *sip);
    virtual int        on_notify(osip_message_t *sip);
    virtual int        on_message(osip_message_t *sip);

    virtual int        on_parse_request(osip_message_t *sip, string &xml);
    virtual int        on_parse_notify(osip_message_t *sip, string &xml);

protected:
    int                on_request(osip_message_t *sip);
    int                on_response(osip_message_t *sip);




protected:
    std::string           & osip2str(osip_message_t *sip, std::string &str);
    int                makeXmlReq(const SIP_REQ_S *req, string& xml);
    uint32_t           GetSN();
    int                 parseXml(const string &toCode, const string &xml, SHL_Json::Value &json);
    bool                hostPortConvert(const string addrcode,string &host,int &port);

    void               cleanHostPort();
    void               dataProloop();
    void               eventloop();
    static void      * threadProc(ThreadInfo *arg);
protected:
    eXosip_t         * m_eXcontext;
    eXosip_event     * m_eXevent;
    osip_uri         * m_sipUrl;                           /**< SIP request URL path */

    CallQueue          m_cqueue;                           /* call queue */
   
    std::string             m_addrcode;
    std::string             m_bindIp;
    int                m_listenPort;
    void              *m_user;
    int                m_subsribeAlarmId;
    int                m_subsribeCatalogId;
    pfnsubscribecb     m_pfnsubscribecb;
    osip_contact_t   * m_contact;

    pfnsubscribecb     m_pfnStatusCb;
    void*              m_pfnStatusData;

private:
    Thread             m_eventThread;
    Thread             m_dataProThread;
    Thread             m_cleanHpThread;
private:
    bool               m_bFirstRegister;
    bool               m_bRegisted;
    uint32_t           m_SN;
    Mutex              m_mtxSN;
    Mutex              m_mtxHostPort;
    map<uint32_t, SIP_REQ_S*> m_mapSN;
    map<string, HostPort> m_mapHostPort;
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

#endif // __GB28181_SIPUA_H__