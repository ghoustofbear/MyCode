/**
 * @file      cu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     client unit defines
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:33:30
 *
 * @note
 *  cu.h defines
 */

#ifndef __CU_H__
#define __CU_H__

#include "def.h"
#include "thread.h"
#include "c2sdk.h"
#include "socket.h"
#include "tlist.h"
#include "tvector.h"

class Socket;
class RealStream;
class RecordStream;
class Preview;
class BackRecorder;

class ATalkSession;
class LocalAVStream;

typedef RecordStream RStream;

class Cu
{

#define LOGIN_STATUS(code) \
    do \
    { \
        if (m_loginCB) \
        { \
            m_loginCB(code, m_loginData); \
        } \
    } while (0);

    enum
    {
        T_NONE,
        T_MIAN,
        T_TIMER,
        T_HEARTBEAT
    } TREAD_E;

public:
    enum
    {
        MSG_NONE,
        MSG_LOGIN,
        MSG_HEARTBEAT,
        MSG_PUSH,

    } MESSAGE_E;

    enum
    {
        R_NONE,
        R_SUBSCRIBE,
        R_QUERY_DEVICE,
        R_QUERY_PRESET,
        R_SET_PRESET,
        R_QUERY_RECORD,
        R_PTZ_CTRL,
        R_SYN_TIME,
        R_MATRIX_SWITCH,
        R_CALL_DECODER,
        R_SET_FACE,
        R_CUSTOM_COMMAND,
        R_GET_CONFIG,
        R_LOCK_FILE,
        R_PTZ_PATTERN_CTRL,
        R_SUBSCRIBE_SERVER_STATUS,
        R_QUERY_USER,
        R_QUERY_CHANNEL,
    } REMOTE_CALL_E;

    enum
    {
        P_HTTP,
        P_PRIVATE,
    } PROTOCOL_E;

    Cu(string host);
    virtual ~Cu();

    int                start(string json, c2_login_cb loginCB, void *loginData);
    void               stop();

    virtual int        init();
    virtual void       uninit();

    virtual int        login() = 0;
    virtual void       logout(long &) = 0;
    virtual long       getServerInfo(c2_servinfo_s *info) = 0;

    bool               isLogin();

    int                addStandby(const char *ip, int port);
    int                delStandby(const char *ip, int port);

    long               queryDevice(string jin, c2_devinfo_cb devinfoCB, void *userData);
    long               queryDevice(string jin, char *json, int *size);
    long               queryChannel(string jin, char *json, int *size);
    long               ptzCtrl(string json);
    long               queryPreset(string jin, c2_preset_cb presetCB, void *userData);
    long               queryPreset(string jin, char* json, int *size);
    long               setPreset(string jin);
	long               queryUser(c2_userinfo_cb userInfoCb, void *userData);

    long               queryRecord(string jin, c2_record_cb recordCB, void *userData);
    long               queryRecord(string jin, char *json, int *size);

    int                subscribe(uint64_t typeBitmap, c2_alarm_cb alarmCB, void* alarmData);
    HANDLE             startPreview(string json, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);
    void               stopPreview(HANDLE view);

    HANDLE             startBackREC(string json, string path);
    void               stopBackREC(HANDLE prec);

    HANDLE             playbackByName(string json, string filename, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);
    HANDLE             playbackByTime(string json, datetime_s &start, datetime_s &end, HWND wnd,
                                      c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);
    void               stopPlayback(HANDLE playback);

    HANDLE             startDownload(string json, string savepath, bool standard, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);
    void               stopDownload(HANDLE download);

    long               syncTime(string json);

    HANDLE             startDualtalk(string json);
    void               stopDualtalk();

    int                lockRecordFile(string json);
    int                ptzPatternCtrl(string json);

    int                setServerStatusCb(c2_alarm_cb alarmCB, void* alarmData);

    int                matrixSwitch(string jin);
    int                sendCommand(int cmdType, string jin);
    int                getConfig(string jin, char* json, int *size);

    int                protocol();
    Host             & host();
    string             userKey();
    

    virtual int        heartbeat() = 0;
    virtual int        remoteCall(uint32_t call, string jin, string &jout) = 0;

protected:
    virtual int        onPush(Socket* s, string json);
    virtual int        recvMsg(string &method, int &type, int &status, string &json) = 0;

    void               timerdriver();
    static void      * threadProc(ThreadInfo *);
protected:
    void               switchHost();
    void               mainLoop();
    void               heartbeatLoop();
private:
#ifdef UNIX_DRAW_RECT
    void drawRoi(string json);
#endif

protected:
    volatile bool      m_isLogin;

    int                m_isLogout; //是否主动注销
    int                m_protocol;

    Host               m_host;
    tvector<Host>      m_vhost;

    Socket           * m_sock;

    string             m_loginJson;
    string             m_rspJson;
    string             m_userKey;

    Thread             m_mainThread;
    Thread             m_timerThread;
    Thread             m_heartbeatThread;


    c2_login_cb        m_loginCB;
    void             * m_loginData;

    uint64_t           m_alarmBitmap;
    c2_alarm_cb        m_alarmCB;
    void             * m_alarmData;

    c2_alarm_cb        m_serverStatusCB;
    void             * m_serverStatus;

    uint64_t           m_lastHeartBeat;

private:
    tlist<Preview *>   m_viewList;
    tlist<RStream *>   m_rstreamList;
    tlist<BackRecorder*> m_backRECList;

    LocalAVStream      * m_dualtalk;
    ATalkSession       * m_talkSession;


};

#endif
