/**
 * @file      pu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-16 17:40:16
 *
 * @note
 *  Pu observer ChannelCfg
 */
#ifndef __PU_H__
#define __PU_H__
#include "observer.h"
#include "tlist.h"
#include "tvector.h"
#include "thread.h"

#include "log.h"
#include <map>
#include "tmap.h"

class Su;
class RealGu;
class RecordGu;
class StreamListener;
class TriggerListener;
class ThreadInfo;

const int MAX_UT_CHANNEL_NUM = 256;
const int MAX_UT_CANONLINE_ARRSIZE = 9;
#ifdef NANZI_ZHUZHAN
const std::string RTSP_FLAG = "#";
const char RTSP_FLAG_CAHR = '#';
#else
const std::string RTSP_FLAG = "@";
const char RTSP_FLAG_CAHR = '@';
#endif

const int UT_SUPERIOR_PR_NUM = 2;
const bool ENABLE_GB28181_PLAT = true;

typedef void(*QueryRecorCB)(int type, int protocol, string &jout, void *userdata);

class Pu : public tlist<TriggerListener *>, public Observer
{
public:
    enum
    {
        PROTO_TYPE_HIK,
        PROTO_TYPE_DAHUA,
        PROTO_TYPE_UT,
        PROTO_TYPE_TIANDY,
        PROTO_TYPE_GW_A,
        PROTO_TYPE_GW_B,          					       /* 国网标准 */
        PROTO_TYPE_GW_B_HS,       					       /* 和盛 */
        PROTO_TYPE_GW_B_HIK,      					       /* 海康 */
        PROTO_TYPE_GB28181,                                /* 公安GB28181 */
        PROTO_TYPE_EASY7,         					       /* 网络存储单元协议 */
        PROTO_TYPE_SONY,          					       /* 索尼 */
        PROTO_TYPE_EHOME,         					       /* 海康EHOME */
        PROTO_TYPE_HELMET,                                 /* 国信 */
        PROTO_TYPE_CDT,                                    /* CDT五防联动 */
        PROTO_TYPE_XUNWEI,                                 /* 讯维矩阵 */
        PROTO_TYPE_JIYUAN,                                 /* 安徽继远 */
        PROTO_TYPE_GW_B_UT,      					       /* 国网优特 */
        PROTO_TYPE_HUADIAN,                                /* 华电 */
		PROTO_TYPE_GQY,                                   /* GQY解码器 */
		PRTO_TYPE_ONVIF,								 /* ONVIF协议 */
    };


    enum
    {
        DEV_TYPE_DVR        = 352,
        DEV_TYPE_IPC        = 353,
        DEV_TYPE_DANBING    = 355,
        DEV_TYPE_NVR        = 360,
        DEV_TYPE_MATRIX     = 364,
        DEV_TYPE_DECODE     = 373,
        DEV_TYPE_DECODE_6901= 374,                           /**< 海康6901UD解码器 */
        DEV_TYPE_DECODE_B20 = 375,                           /**< 海康B2X解码器 */
        DEV_TYPE_MENKOU     = 381,                           /**< 海康门口机  */
        DEV_TYPE_FACEAUTH_RZ= 382,                           /**< 海康人脸认证，认证合一摄像机 */
        DEV_TYPE_FACEAUTH_RL = 383,                           /**< 海康人脸识别 */
        DEV_TYPE_HIK_TERMINAL = 384,                         /**< 海康车牌终端 */
        DEV_TYPE_CAR        = 348,                           /**< 轨道车 */
        DEV_TYPE_WFLINKAGE  = 347,
        DEV_TYPE_STREAMSERV = 100,
        DEV_TYPE_AI         = 490,                           /* 智能分析设备 */
        DEV_TYPE_VTALK      = 491,                           /* 视频会议 */
		DEV_TYPE_GATEWAY    = 492,                           /*人员通道摆闸*/
		DEV_TYPE_LAUNCH     = 493,                           /*朗驰平台*/
    };


    enum
    {
        DOMIAN_NONE,
        DOMAIN_HIK,
        DOMAIN_PUBLIC,
    };

    enum
    {
        PTZ_CMD_NONE,
        PTZ_CMD_UP       = 1,
        PTZ_CMD_DOWN,
        PTZ_CMD_LEFT,
        PTZ_CMD_RIGHT,
        PTZ_CMD_UPLEFT   = 5,
        PTZ_CMD_UPRIGHT,
        PTZ_CMD_DOWNLEFT,
        PTZ_CMD_DOWNRIGHT,
        PTZ_CMD_ZOOMIN,
        PTZ_CMD_ZOOMOUT  = 10,
        PTZ_CMD_FOCUSNEAR,
        PTZ_CMD_FOCUSFAR,
        PTZ_CMD_IRISOPEN,
        PTZ_CMD_IRISCLOSE,
        PTZ_CMD_AUTOSCAN = 15,
        PTZ_CMD_SETPRESET,
        PTZ_CMD_CLEARPRESET,
        PTZ_CMD_CALLPRESET,
        PTZ_CMD_RAIN_ON,                       //雨刷
        PTZ_CMD_RAIN_OFF = 20,
        PTZ_CMD_AUXLIGHT_ON,                   //辅助灯光   
        PTZ_CMD_AUXLIGHT_OFF,
        PTZ_CMD_3D_POSITION,
        PTZ_CMD_LOCKCTRL,
        PTZ_CMD_UNLOCKCTRL,
		PTZ_CMD_SETPOS,
        PTZ_CMD_END,
    };

    enum
    {
        RECORD_TYPE_ALL,
        RECORD_TYPE_MD,
        RECORD_TYPE_VCOVER,
        RECORD_TYPE_TIMER,
        RECORD_TYPE_LABEL,
    };

    enum
    {
        T_PU_LOGIN,
        T_UPDATE_CHNSTATUS
    };

public:
    Pu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host,int linktype=0);
    virtual ~Pu();

    bool               online();
    string             addrcode();
    string             name();
    string             host();
    string             ip();
    int                port();
    int                channels();
    int                protocol();
    int                linktype();
    int                type();
    string             userName();
    string             passWord();


    string             guname(int channelNo);
    string             gucode(int channelNo);

    int                guchannelFromName(string name);

    string             guname(string gucode);
    int                guchannel(string gucode);

    bool               isSame(string host, int port);

    int                addRealGu(int channelNo, string subHost, int subPort, int streamType, int streamPotoType, StreamListener *listener);
    int                addRealGu(string addrcode, int streamType, int streamPotoType, StreamListener *listener);

    void               delRealGu(int channelNo, string subHost, int subPort, int streamType, int streamPotoType, StreamListener *listener);
    void               delRealGu(string addrcode, int streamType, int streamPotoType, StreamListener *listener);
	void               delRealGu(string addrcode, int streamType, StreamListener *listener);

    int                addRecordGu(string filename, string subHost, int subPort, int streamType, StreamListener *listener, int reserve = 0, int mode = 0);
	int                addRecordGu(string filename, string addrcode, int streamType, StreamListener *listener, int reserve = 0, int mode = 0);
	int                addRecordGu(string fileurl, int streamType, StreamListener *listener, int reserve = 0, int mode = 0);

	int                addRecordGu(int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, StreamListener *listener, int reserve = 0, int mode = 0);
	int                addRecordGu(string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, StreamListener *listener, int reserve = 0, int mode = 0);
    void               delRecordGu(StreamListener *listener);

    Su*                addDualTalkGu(int channelNo, string subHost, int subPort);
    Su*                addDualTalkGu(string addrcode);
    void               delDualTalkGu(Su* su);
    void               delDualTalkGu(int channelNo, string subHost, int subPort);

    void               doAudioStream(int chn, void* data, int size);

    void               addTriggerListener(TriggerListener *listener);
    void               delTriggerListener(TriggerListener *listener);

    int                playRecord(StreamListener *listener, string scale, string range);
    int                pauseRecord(StreamListener *listener);
    int                continueRecord(StreamListener *listener);

    int                startRepos(StreamListener *listener);
    int                setPosRecord(int pos, StreamListener *listener);
    int                setTimeRecord(datetime_s &time, StreamListener *listener);
    int                insertReocrdLabel(string name, StreamListener *listener);
    int                playFrame(StreamListener* listener);
    int                playNormal(StreamListener* listener);
    int                playReverse(datetime_s &time, StreamListener* listener);
    int                playForward(datetime_s &time, StreamListener* listener);

    int                queryPreset(int channelNo, string &json);
    int                queryPreset(string addrcode, string &json);
    int                setPreset(int channelNo, string &jin);
    int                setPreset(string addrcode, string &jin);             

    //RealGu*            getRealGu(int channelNo);
    //RealGu*            getRealGu(string addrcode);
    Su*                getDualSu(int channelNo);
    Su*                getDualSu(string addrcode);

    int                CtrlPtzRight(string clientIp, string userName, string clientKey, uint32_t u32ClientOpt, uint32_t u32ChnNo, bool bStop, string& priorIp, string& priorUsername);

	int                needProcess(string key, time_t tt);
	static			   int GetChanNoByCode(string addrcode);

    void               setQueryRecordCB(QueryRecorCB cb, void *userdata);

public:
    virtual int        controlGate(int ctrlValue){ return 0; }
    virtual int        start();
    virtual void       stop();

    virtual bool       validate(int channelNo) = 0;
    virtual bool       validate(string gucode) = 0;

    virtual int        ptzCtrl(int cmd, int channelNo,   int speed, int preset, bool stop);//quxiaohui modified ,to push alarm
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) = 0;

    virtual int        queryRecord(int channelNo,   int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) = 0;
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) = 0;

    virtual int        queryAlarm(int channelNo, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, int &cnt);
    virtual int        queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        query_alarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode){return 0;}

    virtual int        queryResource(string addrcode, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        subscribeEvent(string addrcode, int mode, int alarmtype, int expires) { return 0; }
    
    virtual int        getChnStatus(int channelNo,    string &json);
    virtual int        getChnStatus(string addrcode,  string &json);

    virtual int        onCustomCommand(string& json) { return 0; }
    virtual int        onRequestConfig(string& jin, string& jout) { return 0; }

    virtual int        syncTime(datetime_s &dt, const string &tzone) = 0;

    virtual void       onTimer();

	virtual void       setCallListen(int flag) { m_callListen = flag; }

	/* >=0表示成功,<0表示失败 */
    virtual int		   getPicSize(int chanNo){ return -1;}

    virtual void       UpdateAliveTime(){;}

    void               closeAllRealGu();

protected:
    virtual int        login()  = 0;
    virtual void       logout() = 0;

    virtual void       update(const std::string &table, const std::string &json, int type);
public:
    void               notifyAlarm(string &json, void *data, uint32_t size);
    void               notifyYX(string &json, void *data, uint32_t size);
    void               notifyYC(string &json, void *data, uint32_t size);


    void               writeDevLog(string info);
    void               writeAlarmLog(const string& json);

    virtual int        updateChnStatus() { return 0;}
    void               uploadDeviceStatus();

    int                updateChannel();
    int                loginLoop();
    static void      * threadEntry(ThreadInfo *arg);
    static void      * threadEntryEx(ThreadInfo *arg);

protected:
    string             m_id;
    string             m_ip;

    string             m_host;
    int                m_port;
    int                m_protocol;

    int                m_type;
    string             m_addrcode;
    string             m_name;
    int                m_chnNum;
    string             m_user;
    string             m_pswd;

    int                m_domainType;
    int                m_callListen;
    int                m_linktype;

    volatile bool      m_login;
    volatile bool      m_init;
    volatile bool      m_first;
	volatile uint64_t  m_chnOnlineStatus[MAX_UT_CANONLINE_ARRSIZE];
	volatile uint64_t  m_chnOnlineStatusBak[MAX_UT_CANONLINE_ARRSIZE];

    Thread             m_threadLogin;
    Thread             m_threadUpdateStatus;

    QueryRecorCB       m_clienQueryRecord_cb;
    void             * m_clientUserdata;

public:
    tlist<RealGu *>    m_realGu;
    tlist<RecordGu *>  m_recordGu;
    tlist<Su*>         m_dualSu;
    
    typedef struct
    {
        std::string  strClientIp;
        std::string  strClientKey;
        std::string  strUserName;
        long         lClientOpt;
        long         lStartTime;
    }PTZ_CONTRL_PRIOR;

    Mutex m_mtxChnToPtzPrior;
    std::map<uint32_t, PTZ_CONTRL_PRIOR> m_mapChnToPtzPrior;

    tmap<std::string, time_t> m_mapAlarm;


    uint64_t           m_LastUpdateChnSt;

    typedef struct tagThreadParamEx
    {
        void * pthis;
        Thread *trd;
    }tagThreadParamEx;
};
#endif
