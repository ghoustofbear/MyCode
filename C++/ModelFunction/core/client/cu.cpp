#include "cu.h"
#include "socket.h"
#include "json/json.h"
#include "preview.h"
#include "localavstream.h"
#include "backrecorder.h"
#include "tls_error.h"
#include "utility.h"
#include "http_playback.h"
#include "http_download.h"

#include "atalksession.h"
#include "streamgr.h"


Cu::Cu(string host)
{
    m_isLogin     = false;
    m_isLogout    = false;
    m_sock        = NULL;
    m_loginCB     = NULL;
    m_loginData   = NULL;
    m_alarmCB     = NULL;
    m_alarmData   = NULL;
    m_dualtalk    = NULL;
    m_talkSession = NULL;

    m_serverStatusCB =NULL;
    m_serverStatus = NULL;

    m_alarmBitmap = 0;
    m_lastHeartBeat = sys->currentTimeMillis();

    AutoLock locker(m_vhost._mtx);
    Host h(host, 0);
    m_vhost.push_back(h);
    m_host = h;
}

Cu::~Cu()
{
    {
        AutoLock locker(m_viewList._mtx);
        for (list<Preview *>::const_iterator i = m_viewList.begin(); i != m_viewList.end(); ++i)
        {
            delete *i;
        }
        m_viewList.erase(m_viewList.begin(), m_viewList.end());
    }

    {
        AutoLock locker(m_rstreamList._mtx);
        for (list<RecordStream *>::const_iterator i = m_rstreamList.begin(); i != m_rstreamList.end(); ++i)
        {
            delete *i;
        }
        m_rstreamList.clear();
    }

    {
        AutoLock locker(m_backRECList._mtx);
        for (list<BackRecorder *>::const_iterator i = m_backRECList.begin(); i != m_backRECList.end(); ++i)
        {
            delete *i;
        }
        m_backRECList.clear();
    }

    if (m_dualtalk)
    {
        m_dualtalk->stop();
        delete m_dualtalk;
        m_dualtalk = NULL;
    }

    if (m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
}

int
Cu::init()
{
    return -1;
}

void
Cu::uninit()
{

}

int
Cu::start(string json, c2_login_cb loginCB, void *loginData)
{
    int result  = 0;
    m_isLogout = false;
    m_loginJson = json;
    m_loginCB   = loginCB;
    m_loginData = loginData;

    m_sock = sys->createSocket();
    m_sock->setWriteTimeout(2000);

    m_mainThread.setEntry(threadProc,  this, T_MIAN);
    m_timerThread.setEntry(threadProc, this, T_TIMER);
    m_heartbeatThread.setEntry(threadProc, this, T_HEARTBEAT);

    result += m_mainThread.start();
    result += m_timerThread.start();
    result += m_heartbeatThread.start();

    return result;
}

void
Cu::stop()
{
    m_isLogout = true;
    {
        AutoLock locker(m_viewList._mtx);
        for (list<Preview *>::const_iterator i = m_viewList.begin(); i != m_viewList.end(); ++i)
        {
            (*i)->stop();
        }
    }

    {
        AutoLock locker(m_rstreamList._mtx);
        for (list<RecordStream *>::const_iterator i = m_rstreamList.begin(); i != m_rstreamList.end(); ++i)
        {
            (*i)->stop();
        }
    }


    {
        AutoLock locker(m_backRECList._mtx);
        for (list<BackRecorder *>::const_iterator i = m_backRECList.begin(); i != m_backRECList.end(); ++i)
        {
            (*i)->stop();
        }
    }

    if (m_sock)
    {
        m_sock->close();
    }

    m_heartbeatThread.stop();
    m_mainThread.stop();
    m_timerThread.stop();
}

bool
Cu::isLogin()
{
    return m_isLogin;
}

int
Cu::addStandby(const char *ip, int port)
{
    AutoLock locker(m_vhost._mtx);
    Host h(ip, port);

    auto i = find(m_vhost.begin(), m_vhost.end(), h);
    if (i != m_vhost.end())
    {
        ERRW_SDK("host: %s,%d is exist.", ip, port);
        return -1;
    }

    m_vhost.push_back(h);

    return 0;
}

int
Cu::delStandby(const char *ip, int port)
{
    AutoLock locker(m_vhost._mtx);
    Host h(ip, port);

    auto i = find(m_vhost.begin(), m_vhost.end(), h);
    if (i == m_vhost.end())
    {
        ERRW_SDK("host: %s,%d is not exist.", ip, port);
        return -1;
    }

    m_vhost.erase(i);

    return 0;
}

int
Cu::onPush(Socket* s, string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  pushVal;

    if (!reader.parse(json, pushVal, false))
    {
        printf("Cu::onPush error !!!!!!\n");
        return -1;
    }

    SHL_Json::FastWriter writer;
    string cbjson = writer.write(pushVal);
    /* cbjson = sys->utf2ascii(cbjson); */
    int type = pushVal["type"].asInt();

    if (type == ALARM_SYSTEM_STATUS)
    {
        if (m_serverStatusCB)
        {
            m_serverStatusCB(cbjson.c_str(), cbjson.size(), m_serverStatus);
        }
    }
    else
    {
        if (m_alarmCB)
        {
            m_alarmCB(cbjson.c_str(), cbjson.size(), m_alarmData);
        }
		
#ifdef UNIX_DRAW_RECT
		// iva draw rect
        int roiShow = 0;
        {
            AutoLock lock(g_sdkParamMtx);
            roiShow = g_roiShow;
        }
        if (0 == roiShow)
            drawRoi(json);
#endif
    }
    return 0;
}

#ifdef UNIX_DRAW_RECT
void 
Cu::drawRoi(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  pushVal;

    if (!reader.parse(json, pushVal, false))
    {
        printf("Cu::onPush error !!!!!!\n");
        return ;
    }

	string devIp = pushVal["deviceHost"].asString();
	int devPort = pushVal["devicePort"].asInt();
    //draw iva roi
    if (pushVal["alarms"].isArray())
    {
        for (int i = 0; i < pushVal["alarms"].size(); ++i)
        {
            int type = pushVal["alarms"][i]["type"].asInt();
            int chanNo = pushVal["alarms"][i]["channelNo"].asInt();
            string status = pushVal["alarms"][i]["status"].asString();

            SHL_Json::FastWriter writer;
            string alarmStr = writer.write(pushVal["alarms"][i]);

#if 0
            AutoLock lock(m_viewList._mtx);
            tlist<Preview*>::iterator it = m_viewList.begin();
            for (; it != m_viewList.end(); ++it)
            {
				if ((chanNo == (*it)->getChannelNo()) && 
					(devIp == (*it)->getDeviceIp()) &&
					(devPort == (*it)->getDevicePort()))
                {
                    if (ALARM_STARTED_IVA == type)
                        (*it)->drawROI(alarmStr);

                    (*it)->onIvaResult(alarmStr);

                    //break;
                }
            }
#else
			extern StreamMgr *streamMgr;
			streamMgr->drawRoi(devIp, devPort, chanNo, alarmStr, type);
#endif
        }
    }
}
#endif

void
Cu::mainLoop()
{
    while (m_mainThread.active())
    {
        /* clear to avoid recursive writing error */
        setError("");

        m_isLogin = false;
        if (m_sock->open(m_host) < 0 ||
                m_sock->connect() < 0)
        {
            m_sock->close();

            ERRE_SDK("connect failed.");
            LOGIN_STATUS(CODE_LOGIN_CONNFAIL);

            switchHost();
            sys->sleep(200);

            continue;
        }

        int code = login();
        if (0 != code)
        {
            m_sock->close();
            ERRE_SDK("login failed.");

            if (code < 0)
            {
                //ERRE_SDK("socket err.");
                LOGIN_STATUS(CODE_LOGIN_CONNFAIL);
                switchHost();
            }
            else
            {
                //ERRE_SDK("login failed.");
                LOGIN_STATUS(CODE_LOGIN_FAIL);
            }

            sys->sleep(1000);
            continue;
        }

        subscribe(m_alarmBitmap, m_alarmCB, m_alarmData);

        LOGIN_STATUS(CODE_LOGIN_SUCCESS);

        string json   = "";
        string method = "";
        int    type   = MSG_NONE;
        int    status = -1;
        int    cnt    = 0;
        

        while (m_mainThread.active())
        {
            bool timeout = false;
            int  wait    = 2000;
            int  tout    = 200;
            int  result  = -1;
            time_t begin = time(NULL);

            do
            {
                if (!m_mainThread.active())
                {
                    break;
                }

                result = m_sock->select(true, false, tout);
                if (result > 0)
                {
                    break;
                }
                else if (result < 0)
                {
                    break;
                }
                else
                {
                    wait -= tout;
                    continue;
                }
            }
            while (wait > 0);

            if (wait <= 0)
            {
                timeout = true;
            }

            if (result > 0)
            {
                type = MSG_NONE;
                if (0 == recvMsg(method, type, status, json))
                {
                    cnt = 0;
                    switch (type)
                    {
                    case MSG_HEARTBEAT:
                        break;
                    case MSG_PUSH:
                        onPush(m_sock, json);
                        break;
                    default:
                        continue;
                    }

                    time_t end = time(NULL);
                    if (MSG_PUSH == type && abs(end - begin) >= 5)
                    {
                        ERRE_SDK("alarm cb process faild.");
                        timeout = true;
                        begin   = end;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else if (result < 0)
            {
                m_sock->close();
                ERRE_SDK("socket error.");
                break;
            }

            if (timeout)
            {
                if (++cnt < 3 /*&& (0 == heartbeat())*/)
                {
                    //LOGT_SDK("client send heartbeat!");
                    continue;
                }
            }

            ERRE_SDK("heart beat time out.");
            m_sock->close();
            break;
        }

        switchHost();
    }

    m_isLogin = false;
}

void Cu::heartbeatLoop()
{
    uint64_t curtim = 0;
    while (m_heartbeatThread.active())
    {
        curtim = sys->currentTimeMillis();
        if (curtim - m_lastHeartBeat > 2000 && m_isLogin)
        {
            heartbeat();
            m_lastHeartBeat = curtim;
        }
        sys->sleep(200);
    }
}

int
Cu::subscribe(uint64_t typeBitmap, c2_alarm_cb alarmCB, void* alarmData)
{
    m_alarmBitmap = typeBitmap;
    m_alarmCB     = alarmCB;
    m_alarmData   = alarmData;

    if (0 != m_alarmBitmap)
    {

        SHL_Json::FastWriter writer;
        SHL_Json::Value      value;

        value["typeBitmap"] = typeBitmap;
        string json = writer.write(value);
        /* json = sys->ascii2utf8(json); */


        string jout;

        return remoteCall(R_SUBSCRIBE, json, jout);
    }

    return -1;
}

long
Cu::queryDevice(string jin, c2_devinfo_cb devinfoCB, void *userData)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_DEVICE, jin, jout) < 0)
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  value;

    if (!reader.parse(jout, value, false))
    {
        ERRE_SDK("parse fail. jout: %s", jout.c_str());
        return -1;
    }

    SHL_Json::Value &devices = value["devices"];

    c2_devinfo_s info;
    for (uint32_t i = 0; i < devices.size(); i++)
    {
        memset(&info, 0, sizeof(c2_devinfo_s));

        if (!devices[i]["code"].isNull())
        {
            strncpy_s(info.addrcode, devices[i]["code"].asCString(), sizeof(info.addrcode));
        }

        if (!devices[i]["parent"].isNull())
        {
            strncpy_s(info.parent, devices[i]["parent"].asCString(), sizeof(info.parent));
        }

        if (!devices[i]["name"].isNull())
        {
            strncpy_s(info.name, devices[i]["name"].asCString(), sizeof(info.name));
        }

        if (!devices[i]["host"].isNull())
        {
            strncpy_s(info.host, devices[i]["host"].asCString(), sizeof(info.host));
        }

        info.port       = devices[i]["port"].asUInt();
        info.type       = devices[i]["type"].asInt();
        info.channelNo  = devices[i]["channel"].asInt();
        info.channelNum = devices[i]["subnum"].asInt();
        info.protoType  = devices[i]["protoType"].asInt();
        info.streamType = devices[i]["streamType"].asInt();
        info.status     = devices[i]["status"].asBool();
		info.devtype    = devices[i]["devtype"].asInt();
		info.manutype   = devices[i]["manutype"].asInt();

        if (devinfoCB)
        {
            devinfoCB(&info, userData);
        }
    }

    return 0;
}

long
Cu::queryDevice(string jin, char *json, int *size)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_DEVICE, jin, jout) < 0)
    {
        return -1;
    }

    strncpy(json, jout.c_str(), *size);
    *size = (*size < (int)jout.size()) ? *size : jout.size();
    
    return 0;
}

long Cu::queryChannel(string jin, char *json, int *size)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_CHANNEL, jin, jout) < 0)
    {
        return -1;
    }

    strncpy(json, jout.c_str(), *size);
    *size = (*size < (int)jout.size()) ? *size : jout.size();

    return 0;
}

long
Cu::queryUser(c2_userinfo_cb userInfoCb, void *userData)
{
	string jout;
	SHL_Json::Value  val;
	val["version"] = "v1.00";
	SHL_Json::FastWriter writer;
	string cbjson = writer.write(val);
	if (remoteCall(R_QUERY_USER, cbjson, jout) < 0)
	{
		return -1;
	}

	SHL_Json::Reader reader;
	SHL_Json::Value  value;

	if (!reader.parse(jout, value, false))
	{
		ERRE_SDK("parse fail. jout: %s", jout.c_str());
		return -1;
	}

	SHL_Json::Value &users = value["users"];
	SHL_Json::Value &groups = value["groups"];
	SHL_Json::Value &userGrs = value["userGrs"];
	c2_user_info_s info;
	for (uint32_t i = 0; i < users.size(); i++)
	{
		memset(&info, 0, sizeof(c2_user_info_s));

		if (!users[i]["name"].isNull())
		{
			strcpy(info.user, users[i]["name"].asString().c_str());
		}

		if (!users[i]["pswd"].isNull())
		{
			strcpy(info.password, users[i]["pswd"].asString().c_str());
		}

		//20200622 zhangshenyu add
		//имик?иоио??бьиибз?T
		string userId = "";
		string userGruId = "";
		string userGrgId = "";
		uint64_t rightBit = 0;
		if (!users[i]["id"].isNull())
		{
			userId = users[i]["id"].asString();
			for (int j = 0; j <int( userGrs.size()); ++j)
			{
				if (!userGrs[j]["userId"].isNull() && !userGrs[j]["groupId"].isNull() && userGrs[j]["userId"].asString() == userId)
				{
					userGruId = userGrs[j]["userId"].asString();
					userGrgId = userGrs[j]["groupId"].asString();
					for (int k = 0; k <int( groups.size()); ++k)
					{
						if (!groups[k]["id"].isNull() && groups[k]["id"].asString() == userGrgId)
						{
							info.rightBit |= groups[k]["rightBit"].asInt64();
							info.type |= groups[k]["type"].asInt();
							//info.previewBit |= groups[k]["previewBit"].asInt();
							//info.playbackBit |= groups[k]["playbackBit"].asInt();
							//info.ctrlBit |= groups[k]["ctrlBit"].asInt();
							info.level |= groups[k]["level"].asInt();
						}
					}
				}
			}
		}

		if (userInfoCb)
		{
			userInfoCb(&info, userData);
		}
	}

	return 0;
}

HANDLE
Cu::startPreview(string json, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    //LOGT_SDK(json.c_str());

    AutoLock locker(m_viewList._mtx);

    Preview *view = new Preview(this);
    if (view)
    {
        if (view->start(json, wnd, streamCB, streamData, statusCB, statusData) < 0)
        {
            LOGI_SDK("%s", getError());
            delete view;
            return NULL;
        }
    }
	LOGI_SDK("view = %p\n", view);
    m_viewList.push_back(view);

    return view;
}

void
Cu::stopPreview(HANDLE view)
{
    AutoLock locker(m_viewList._mtx);
    list<Preview *>::iterator i = find(m_viewList.begin(), m_viewList.end(), view);
    if (i != m_viewList.end())
    {
		LOGI_SDK("view = %p\n", view);
        (*i)->stop();
        delete *i;
        m_viewList.erase(i);
    }
}


HANDLE
Cu::startBackREC(string json, string path)
{

    LOGT_SDK(json.c_str());

    AutoLock locker(m_backRECList._mtx);

    BackRecorder *view = new BackRecorder(this);
    if (view)
    {
        if (view->start(json, path) < 0)
        {
            LOGI_SDK("%s", getError());
            delete view;
            return NULL;
        }
    }

    m_backRECList.push_back(view);

    return view;
}

void
Cu::stopBackREC(HANDLE prec)
{
    AutoLock locker(m_backRECList._mtx);
    list<BackRecorder *>::iterator i = find(m_backRECList.begin(), m_backRECList.end(), prec);
    if (i != m_backRECList.end())
    {
        (*i)->stop();
        delete *i;
        m_backRECList.erase(i);
    }
}

long
Cu::ptzCtrl(string json)
{
    LOGI_SDK(json.c_str());

    string jout;
    if (remoteCall(R_PTZ_CTRL, json, jout) < 0)
    {
        return -1;
    }

    return 0;
}

long
Cu::queryPreset(string jin, c2_preset_cb presetCB, void *userData)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_PRESET, jin, jout) < 0)
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  value;
    if (!reader.parse(jout, value, false))
    {
        return -1;
    }

    SHL_Json::Value &presetList = value["presetList"];
    c2_preset_s info;
    char tmp[128];
    for (uint32_t i = 0; i < presetList.size(); i++)
    {
        memset(tmp, 0, 128);

        if (!presetList[i]["name"].isNull())
        {
            strncpy_s(tmp, presetList[i]["name"].asCString(), 128);
        }
        info.name = tmp;
        info.type = presetList[i]["type"].asInt();
        info.idx = presetList[i]["idx"].asInt();
        info.isIVS = presetList[i]["isIVS"].asInt();

        if (presetCB)
        {
            presetCB(&info, userData);
        }
    }

    return 0;
}

long               
Cu::queryPreset(string jin, char* json, int *size)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_PRESET, jin, jout) < 0)
    {
        return -1;
    }

    strncpy(json, jout.c_str(), *size);
    *size = (*size < (int)jout.size()) ? *size : jout.size();
    return 0;
}

long               
Cu::setPreset(string jin)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_SET_PRESET, jin, jout) < 0)
    {
        return -1;
    }

    return 0;
}

int 
Cu::sendCommand(int cmdType, string jin)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(cmdType, jin, jout) < 0)
    {
        return -1;
    }
    return 0;
}

int
Cu::getConfig(string jin, char* json, int *size)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_GET_CONFIG, jin, jout) < 0)
    {
        return -1;
    }

    strncpy(json, jout.c_str(), *size);
    *size = (*size < (int)jout.size()) ? *size : jout.size();
    return 0;
}

long
Cu::queryRecord(string jin, c2_record_cb recordCB, void *userData)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_RECORD, jin, jout) < 0)
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  value;
    
    if (!reader.parse(jout, value, false))
    {
        return -1;
    }

    int realNum   = value["realNum"].asInt();
    int fromIndex = value["fromIndex"].asInt();
    int toIndex   = value["toIndex"].asInt();
    string NVR = value["NVR_Type"].asString();
    string data1 = "";
    int len = value["records"].size();
    SHL_Json::Value& value1 = value["records"];
    if (NVR == "dahua")
    {
        c2_record_s info1;
        for (int i = 0; i < len; i++)
        {
            string Token = value1[i]["Record_Token"].asString();
            string rtspurl = value1[i]["fileurl"].asString();
			if (!value1[i]["fileurl"].isNull())
			{
				strncpy_s(info1.filename, value1[i]["fileurl"].asCString(), sizeof(info1.filename));
			}
            //info1.rtspUrl = value1[i]["fileurl"].asCString();
            //info1.token= value1[i]["Record_Token"].asCString();
			if (recordCB)
			{
				recordCB(&info1, realNum, fromIndex, toIndex, userData);
			}
        }

    }
    else
    {
		SHL_Json::Value& records = value["data"];

		c2_record_s info;

		for (uint32_t i = 0; i < records.size(); i++)
		{
			memset(&info, 0, sizeof(c2_record_s));

			if (!records[i]["filename"].isNull())
			{
				strncpy_s(info.filename, records[i]["filename"].asCString(), sizeof(info.filename));
			}

			info.size = records[i]["size"].asUInt();
			info.channelNo = records[i]["channelNo"].asInt();
			info.type = records[i]["type"].asInt();
			info.index = records[i]["index"].asInt();

			if (!records[i]["start"].isNull())
			{
				string start = records[i]["start"].asString();
				sys->strptime(start, info.start);
			}

			if (!records[i]["end"].isNull())
			{
				string end = records[i]["end"].asString();
				sys->strptime(end, info.end);
			}

			if (recordCB)
			{
				recordCB(&info, realNum, fromIndex, toIndex, userData);
			}
		}

    }
    //SHL_Json::Value &records = value["data"];

    //c2_record_s info;
   
    //for (uint32_t i = 0; i < records.size(); i++)
    //{
    //    memset(&info, 0, sizeof(c2_record_s));

    //    if (!records[i]["filename"].isNull())
    //    {
    //        strncpy_s(info.filename, records[i]["filename"].asCString(), sizeof(info.filename));
    //    }

    //    info.size      = records[i]["size"].asUInt();
    //    info.channelNo = records[i]["channelNo"].asInt();
    //    info.type      = records[i]["type"].asInt();
    //    info.index     = records[i]["index"].asInt();

    //    if (!records[i]["start"].isNull())
    //    {
    //        string start = records[i]["start"].asString();
    //        sys->strptime(start, info.start);
    //    }

    //    if (!records[i]["end"].isNull())
    //    {
    //        string end = records[i]["end"].asString();
    //        sys->strptime(end, info.end);
    //    }

    //    if (recordCB)
    //    {
    //        recordCB(&info, realNum, fromIndex, toIndex, userData);
    //    }
    //}

    return 0;
}

long
Cu::queryRecord(string jin, char *json, int *size)
{
    LOGT_SDK(jin.c_str());

    string jout;
    if (remoteCall(R_QUERY_RECORD, jin, jout) < 0)
    {
        return -1;
    }

    strncpy(json, jout.c_str(), *size);
    *size = (*size < (int)jout.size()) ? *size : jout.size();

    return 0;
}

long
Cu::syncTime(string json)
{
    LOGI_SDK(json.c_str());

    string jout;
    if (remoteCall(R_SYN_TIME, json, jout) < 0)
    {
        return -1;
    }

    return 0;
}

int
Cu::matrixSwitch(string jin)
{
    LOGT_SDK(jin.c_str());
    string jout;
    if (remoteCall(R_MATRIX_SWITCH, jin, jout) < 0)
    {
        return -1;
    }

    return 0;
}


HANDLE
Cu::startDualtalk(string json)
{
    LOGT_SDK(json.c_str());

    LocalAVStream* lav = new LocalAVStream();


    //if (!m_dualtalk)
    //{
    //    m_dualtalk = new LocalAVStream();
    //    m_dualtalk->startAudio();
    //}


    //localAVSteamMgr->addAudioTalk(this);

    //ATalkSession *talk = new ATalkSession(this);

    //talk->start("");

    //m_dualtalk->addAudioListener(talk);    

    
    //HANDLE asdf = localAVSteamMgr->addAudioTalk(this);
    //localAVSteamMgr->startAudio();

    return lav;
}

void
Cu::stopDualtalk()
{
    //localAVSteamMgr->stopAudio();
    //return localAVSteamMgr->delAudioTalk(this);

    LocalAVStream* lav = new LocalAVStream();

}

int
Cu::protocol()
{
    return m_protocol;
}

Host&
Cu::host()
{
    return m_host;
}

string
Cu::userKey()
{
    if (!m_isLogin)
    {
        return "";
    }

    return m_userKey;
}

HANDLE
Cu::playbackByName(string json, string filename, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    LOGT_SDK(json.c_str());

    AutoLock locker(m_rstreamList._mtx);

    RecordStream *stream = NULL;
    if (P_HTTP == m_protocol)
    {
        stream = new Httplayback(filename, this);
        ((Httplayback *)stream)->init(wnd, streamCB, streamData, statusCB, statusData);
    }

    if (stream)
    {
        if (stream->start(json) < 0)
        {
            ERRE_SDK("Httplayback().start() failed");
            delete stream;
            return NULL;
        }
    }

    m_rstreamList.push_back(stream);

    return stream;
}

HANDLE
Cu::playbackByTime(string json, datetime_s &start, datetime_s &end, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    LOGT_SDK(json.c_str());

    AutoLock locker(m_rstreamList._mtx);

    RecordStream *stream = NULL;
    if (P_HTTP == m_protocol)
    {
        stream = new Httplayback(start, end, this);
        ((Httplayback *)stream)->init(wnd, streamCB, streamData, statusCB, statusData);
    }

    if (stream)
    {
        if (stream->start(json) < 0)
        {
            ERRE_SDK("Httplayback().start() failed");
            delete stream;
            return NULL;
        }
    }

    m_rstreamList.push_back(stream);

    return stream;
}

void
Cu::stopPlayback(HANDLE playback)
{
    if (!playback)
    {
        return;
    }

    AutoLock locker(m_rstreamList._mtx);
    list<RecordStream *>::iterator i = find(m_rstreamList.begin(), m_rstreamList.end(), playback);
    if (i != m_rstreamList.end())
    {
        (*i)->stop();
        delete *i;
        m_rstreamList.erase(i);
    }
}

HANDLE
Cu::startDownload(string json, string savepath, bool standard, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    LOGT_SDK(json.c_str());

    AutoLock locker(m_rstreamList._mtx);

    RecordStream *stream = NULL;
    if (P_HTTP == m_protocol)
    {
        stream = new HttpDownload(this, savepath, standard);
        ((HttpDownload *)stream)->init(streamCB, streamData, statusCB, statusData);
    }

    if (stream)
    {
        if (stream->start(json) < 0)
        {
            ERRE_SDK("HttpDownload().start() failed");
            delete stream;
            return NULL;
        }
    }

    m_rstreamList.push_back(stream);

    return stream;
}

void
Cu::stopDownload(HANDLE download)
{
    if (!download)
    {
        return;
    }

    AutoLock locker(m_rstreamList._mtx);
    list<RecordStream *>::iterator i = find(m_rstreamList.begin(), m_rstreamList.end(), download);
    if (i != m_rstreamList.end())
    {
        (*i)->stop();
        delete *i;
        m_rstreamList.erase(i);
    }
}

void
Cu::switchHost()
{
    if (m_isLogout)
    {
        return;
    }

    AutoLock locker(m_vhost._mtx);
    if (1 == m_vhost.size())
    {
        return;
    }

    auto i = find(m_vhost.begin(), m_vhost.end(), m_host);
    i++;
    if (i != m_vhost.end())
    {
        m_host = *i;
    }
    else
    {
        m_host = m_vhost[0];
    }

    ERRI_SDK("switch host: %s, %d", m_host.getIPStr().c_str(), m_host.getPort());
    LOGIN_STATUS(CODE_LOGIN_SWITCHOST);
}

void
Cu::timerdriver()
{
    while (m_timerThread.active())
    {
        {
            AutoLock locker(m_viewList._mtx);

            for (list<Preview *>::const_iterator i = m_viewList.begin(); i != m_viewList.end(); ++i)
            {
                (*i)->timerdriver();
            }
        }

        {
            AutoLock locker(m_rstreamList._mtx);

            for (list<RecordStream *>::const_iterator i = m_rstreamList.begin(); i != m_rstreamList.end(); ++i)
            {
                (*i)->timerdriver();
            }
        }
        sys->sleep(50);
    }
}

void *
Cu::threadProc(ThreadInfo *info)
{
    Cu *cuPtr = (Cu *)(info->data);

    switch (info->id)
    {
    case T_MIAN:
        cuPtr->mainLoop();
        break;

    case T_TIMER:
        cuPtr->timerdriver();
        break;

    case T_HEARTBEAT:
        cuPtr->heartbeatLoop();
        break;

    default:
        break;
    }

    return NULL;
}

int 
Cu::lockRecordFile(string json)
{
    string jout;
    return remoteCall(R_LOCK_FILE, json, jout);
}

int 
Cu::ptzPatternCtrl(string json)
{
    string jout;
    return remoteCall(R_PTZ_PATTERN_CTRL, json, jout);
}

int
Cu::setServerStatusCb(c2_alarm_cb statucb, void *alarmData)
{
    int enable;
    if (statucb == NULL)
    {
        enable = 0;
    }
    else
    {
        enable = 1;
    }

    string jout;
    char buf[64] = {0};
    sprintf(buf, "{enable:%d, version:v1.0}", enable);

    m_serverStatusCB = statucb;
    m_serverStatus = alarmData;

    return remoteCall(R_SUBSCRIBE_SERVER_STATUS, buf, jout);
}
