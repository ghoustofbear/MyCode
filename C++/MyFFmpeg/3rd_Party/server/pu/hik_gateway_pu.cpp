#include "hik_gateway_pu.h"
#include "json/json.h"
#include "tls_error.h"
#include "global_str_def.h"
void CALLBACK
login_CB(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser)
{
	HikGateWayPu *pu = (HikGateWayPu *)pUser;

	if (1 == result)
	{
		LOGI_MSV("Device: %s login success! user ID: %d", pu->ip().c_str(), userId);
		pu->m_login = true;
		pu->writeDevLog(STR_MSV_ON_LINE);
	}

	LOGI_MSV("login reslut: %d, %s", result, pu->ip().c_str());
}

HikGateWayPu::HikGateWayPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HIK;
    m_port = port;
    m_domainType = domainType;
	m_lUserID = -1;
	m_lastTimeStamp = 0;
}

HikGateWayPu::~HikGateWayPu()
{
    
}

int HikGateWayPu::start()
{
	if (Pu::start() < 0)
	{
		return -1;
	}
	NET_DVR_Init();
	NET_DVR_SetConnectTime(2000, 10);
	NET_DVR_SetReconnect(5000, true);
	m_init = TRUE;
	m_thread.setEntry(m_ThreadEntry, this, 0);
	return m_thread.start();
}

void HikGateWayPu::stop()
{
    if (!m_init)
        return;

    m_init = FALSE;

	m_thread.stop();

    logout();
    
    Pu::stop();
}

int HikGateWayPu::login()
{
	if (m_login)
	{
		return 0;
	}
	NET_DVR_USER_LOGIN_INFO loginInfo;//用户登录参数
	NET_DVR_DEVICEINFO_V40  deviceInfo;//设备信息
	memset(&loginInfo, 0, sizeof(loginInfo));
	memset(&deviceInfo, 0, sizeof(deviceInfo));

	loginInfo.wPort = m_port;
	loginInfo.bUseAsynLogin = TRUE;
	loginInfo.cbLoginResult = login_CB;
	loginInfo.pUser = this;

	strncpy(loginInfo.sDeviceAddress, m_ip.c_str(), sizeof(loginInfo.sDeviceAddress));
	strncpy(loginInfo.sUserName, m_user.c_str(), sizeof(loginInfo.sUserName));
	strncpy(loginInfo.sPassword, m_pswd.c_str(), sizeof(loginInfo.sPassword));

	m_lUserID = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
	if (m_lUserID < 0)
	{
		LOGE_MSV("NET_DVR_Login_V30 error, %d", NET_DVR_GetLastError());
		return -1;
	}
	int wait = 10000;
	while (wait > 0 && !m_login)
	{
		sys->sleep(100);
		wait -= 100;
	}

	if (wait <= 0)
	{
		if (-1 != m_lUserID)
		{
			/* It is recommended to use the NET_DVR_Logout for Hik SDK */
			NET_DVR_Logout(m_lUserID);
			m_login = false;
			m_lUserID = -1;
		}
		return -1;
	}
	return 0;
}

void HikGateWayPu::logout()
{

    if (!m_login)
        return;

	if (-1 != m_lUserID)
    {
		NET_DVR_Logout_V30(m_lUserID);
    }
	m_lUserID = -1;
    m_login = false;
}

/**
* @brief 远程门禁控制
* @param [in] gateWayIndex 门禁序号，从1开始，-1表示对所有门进行操作
* @param [in] cmd          命令值，0 关闭 ，1 打开，2 常开，3 常关，4 恢复
* @return 返回值为空
*/
int
HikGateWayPu::gateWayCtrl(int gateWayIndex, int cmd)
{
	if (-1 == m_lUserID)
	{
		printf("GateWayCtrl failed, reason as follows: user not login !");
		return -1;
	}
	BOOL ret = NET_DVR_ControlGateway(m_lUserID, gateWayIndex, cmd);
	if (FALSE == ret)
	{
		int errCode = NET_DVR_GetLastError();
		char strLog[512] = { 0 };
		sprintf(strLog, "GateWayCtrl failed,reason NET_DVR_ControlGateway failed,errCode:%d\n", errCode);
		printf(strLog);
		return -1;//测试使用

	}
	printf("GateWayCtrl success\n");
	return  0;
}

/**
* @brief  获取门禁状态
* @return 出开:2，进开:1，关闭:0，失败:-1
*/
int 
HikGateWayPu::gateWayStatus()
{
	if (-1 == m_lUserID)
	{
		printf("GateWayCtrl failed, reason as follows: user not login !");
		return -1;
	}
	NET_DVR_XML_CONFIG_INPUT  xmlConfigInput = { 0 };
	NET_DVR_XML_CONFIG_OUTPUT  xmlCongfigOutput = { 0 };
	xmlConfigInput.dwSize = sizeof(xmlConfigInput);
	xmlCongfigOutput.dwSize = sizeof(xmlCongfigOutput);

	char szUrl[256] = "";
	DWORD dwOutputLen = 1024 * 1024;
	char *pOutBuf = new char[dwOutputLen];

	BOOL bIsSupportFaceMatchBroken = FALSE;

	sprintf(szUrl, "GET /ISAPI/AccessControl/GateStatus");
	xmlConfigInput.lpRequestUrl = szUrl;
	xmlConfigInput.dwRequestUrlLen = strlen(szUrl);
	xmlConfigInput.dwRecvTimeOut = 5000;

	char szStatusBuff[1024] = { 0 };
	memset(pOutBuf, 0, dwOutputLen);
	xmlCongfigOutput.dwOutBufferSize = dwOutputLen;
	xmlCongfigOutput.lpOutBuffer = pOutBuf;
	xmlCongfigOutput.lpStatusBuffer = szStatusBuff;
	xmlCongfigOutput.dwStatusSize = sizeof(szStatusBuff);
	BOOL bRet = NET_DVR_STDXMLConfig(m_lUserID, &xmlConfigInput, &xmlCongfigOutput);//ISAPI协议命令透传
	string xxml((char*)(xmlCongfigOutput.lpOutBuffer), xmlCongfigOutput.dwReturnedXMLSize);
	delete[]pOutBuf;

	if (!bRet)
	{
		WORD errCode = NET_DVR_GetLastError();
		char strLog[512] = { 0 };
		sprintf(strLog, "GateWayCtrl failed,reason NET_DVR_ControlGateway failed,errCode:%d\n", errCode);
		printf(strLog);
		return -1;
	}

	CXmlBase xmlBase1;
	xmlBase1.Parse(xxml.c_str());

	if (xmlBase1.FindElem("GateStatus") && xmlBase1.IntoElem())
	{
		string ns = xmlBase1.GetNodeName();
		if (xmlBase1.FindElem("gateCurrentStatus") /*&& xmlBase1.IntoElem()*/)
		{
			string ns = xmlBase1.GetNodeName();
			string gateCurrentStatus = xmlBase1.GetData().c_str();
			if (gateCurrentStatus == "InDoorOpen")
			{
				return 1;
			}
			else if (gateCurrentStatus == "OutDoorOpen")
			{
				return 2;
			}
			xmlBase1.OutOfElem();
		}
		xmlBase1.OutOfElem();
	}
	return 0;
}

int                
HikGateWayPu::pushGatewayStatus()
{
	SHL_Json::FastWriter writer;
	SHL_Json::Value value;
	int status = gateWayStatus();
	if (-1 != status)
	{
		value["version"]    = "1.0";
		value["time"]       = sys->currentTime();
		value["deviceName"] = name();
		value["deviceHost"] = host();
		value["devicePort"] = port();
		value["addrcode"]   = addrcode();
		value["level"]      = 1;
		SHL_Json::Value item;
		item["type"]        = TriggerListener::TR_GATEWAY_STATUS;
		item["gateStatus"]= status;
		value["alarms"].append(item);
		string strResp = writer.write(value);
		notifyAlarm(strResp, NULL, 0);
		return 0;
	}
	return -1;
}

void*
HikGateWayPu::m_ThreadEntry(ThreadInfo *arg)
{
	HikGateWayPu *pu = (HikGateWayPu*)arg->data;
	pu->m_lastTimeStamp = sys->currentTimeMillis();
	while (pu->m_thread.active())
	{
		uint64_t currentTimeStamp = sys->currentTimeMillis();
		if (currentTimeStamp - pu->m_lastTimeStamp >= 1000 * 2)//to get gateway status every two seconds Here
		{
			pu->pushGatewayStatus();
			pu->m_lastTimeStamp = currentTimeStamp;
		}
		sys->sleep(100);
	}
	return NULL;
}

int
HikGateWayPu::onCustomCommand(string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	int ret = -1;

	if (!reader.parse(json, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return ret;
	}

	string jMethod = rvalue["command"].asString();
	if (!rvalue.isMember("command"))
	{
		ERRE_MSV("json error:no member \"Method\"");
		return ret;
	}

	if (jMethod == "gateCtrl")//控制摆闸开关状态
	{
		int index = rvalue["gatewayIndex"].asInt();
		int cmd   = rvalue["gateCmd"].asInt();
		ret       = gateWayCtrl(index, cmd);
		return ret;
	}
}

int
HikGateWayPu::onRequestConfig(string& jin, string& jout)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	string jMethod = rvalue["Method"].asString();

	if (!rvalue.isMember("Method"))
	{
		ERRE_MSV("json error:no member \"Method\"");
		return -1;
	}
	if (jMethod == "getGatewayStatus")//获取摆闸状态
	{
		SHL_Json::FastWriter writer;
		SHL_Json::Value value;
		int status = gateWayStatus();
		if (-1 != status)
		{
			value["version"] = "1.0";
			value["time"] = sys->currentTime();
			value["deviceName"] = name();
			value["deviceHost"] = host();
			value["devicePort"] = port();
			value["addrcode"] = addrcode();
			value["level"] = 1;
			SHL_Json::Value item;
			item["type"] = TriggerListener::TR_GATEWAY_STATUS;
			item["gateStatus"] = status;
			value["alarms"].append(item);
			jout = writer.write(value);
			return 0;
		}
	}
	return -1; 
}


