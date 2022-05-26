#include "launch_pu.h"
#include "json/json.h"
#include "tls_error.h"
#include "global_str_def.h"
#include "platform.h"
#include "cfg_device.h"
#include "putree.h"
#include <fstream>
#define CURL_EASY_SETOPT(A, B, C) \
if ((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) \
	fprintf(stderr, "curl_easy_setopt(%s, %s, %s) failed: %d\n", \
#A, #B, #C, res);

#define CURL_EASY_PERFORM(A) \
if ((res = curl_easy_perform((A))) != CURLE_OK) \
	fprintf(stderr, "curl_easy_perform(%s) failed: %d\n", #A, res);
typedef struct tagSendDataParam
{
	const char * pSendData;
	int    SendSize;
}SendDataParam;
LaunchPu *launchPu = NULL;
LaunchPu::LaunchPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
	m_protocol = PROTO_TYPE_UT;
	m_type = DEV_TYPE_LAUNCH;
	m_port = port;
	m_domainType = domainType;
	m_lastTimeStamp = 0;
	m_token = "";
	m_expiretime = 720000;
	launchPu = this;
	m_proxyHost = "";
	m_proxyPort = 0;
}

LaunchPu::~LaunchPu()
{
	delete launchSVR;
	launchSVR = NULL;

	launchPu = NULL;
}

/*
*上传数据回调，返值表示上传字节数，返回0表示数据结束，非0数据未结束将继续调用该函数直至返回0
*/
size_t LaunchPu::read_data(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	SendDataParam * pSendData = (SendDataParam *)userdata;

	if (size*nmemb < 1 || pSendData->SendSize <= 0)
		return 0;

	if (pSendData->SendSize <= (int)(size*nmemb))
	{
		int s32Ret = pSendData->SendSize;
		memcpy(buffer, pSendData->pSendData, pSendData->SendSize);
		pSendData->SendSize = 0;

		return s32Ret;
	}
	else
	{
		memcpy(buffer, pSendData->pSendData, size*nmemb);
		pSendData->pSendData += size*nmemb;
		pSendData->SendSize -= size*nmemb;

		return size*nmemb;
	}
}
/*
*buffer为接收到的数据，size＊nmemb为接收的数据大小，userdata返回的数据指定的输出流
*/
size_t  LaunchPu::write_data(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	string* str = dynamic_cast<string*>((string *)userdata);

	if (NULL == str || NULL == buffer || size*nmemb < 1)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return size *nmemb;
}
/*
*返回值：成功0，失败-1
*/
int LaunchPu::curlPost(string jin, string url, string &jout)
{
	int ret = 0;
	string resp_data = "";
	CURL *pCmdCurl = NULL;
	SendDataParam sendData;
	string utf8Json = "";
	do
	{
		pCmdCurl = curl_easy_init();
		if (NULL == pCmdCurl)
		{
			ret = -1;
			break;
		}
		CURLcode res = CURLE_OK;
		if (m_proxyHost != "" && m_proxyPort != 0)
		{
			char proxy[128];
			snprintf(proxy, 128, "%s:%d", m_proxyHost.c_str(), m_proxyPort);
			CURL_EASY_SETOPT(pCmdCurl, CURLOPT_PROXY, proxy);//设置代理
		}
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_VERBOSE, 0L);//关闭打印
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_NOPROGRESS, 1L);//不显示进度条
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_TIMEOUT, 1L);//超时时间

		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Accept:application/json");
		headers = curl_slist_append(headers, "Connection: Keep-Alive");
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_HTTPHEADER, headers);
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_URL, url.c_str());
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_POST, 1);
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_WRITEFUNCTION, write_data);
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_WRITEDATA, (void*)&resp_data);
		if (!jin.empty())
		{
			sendData.pSendData = jin.c_str();
			sendData.SendSize = (long)strlen(sendData.pSendData);

			CURL_EASY_SETOPT(pCmdCurl, CURLOPT_READFUNCTION, read_data);
			CURL_EASY_SETOPT(pCmdCurl, CURLOPT_READDATA, &sendData);
			CURL_EASY_SETOPT(pCmdCurl, CURLOPT_POSTFIELDSIZE, sendData.SendSize);
		}
		res = curl_easy_perform(pCmdCurl);
		curl_slist_free_all(headers);
		if (res != CURLE_OK)
		{
			ret = -1;
		}

	} while (0);

	jout = resp_data;

	if (NULL != pCmdCurl)
	{
		curl_easy_cleanup(pCmdCurl);
	}
	return ret;
}

/*
*返回值：成功0，失败-1
*/
int LaunchPu::curlGet(string &url, string &jout)
{
	int ret = 0;
	CURL *pCmdCurl = NULL;
	string resp_data = "";
	do
	{
		pCmdCurl = curl_easy_init();
		if (NULL == pCmdCurl)
		{
			ret = -1;
			break;
		}
		CURLcode res = CURLE_OK;
		if (m_proxyHost != "" && m_proxyPort != 0)
		{
			char proxy[128];
			snprintf(proxy, 128, "%s:%d", m_proxyHost.c_str(), m_proxyPort);
			CURL_EASY_SETOPT(pCmdCurl, CURLOPT_PROXY, proxy);//设置代理
		}
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_VERBOSE, 0L);//关闭打印
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_NOPROGRESS, 1L);//不显示进度条
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_TIMEOUT, 1L);//超时时间

		//写入请求头部
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");//接收数据类型
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_HTTPHEADER, headers);//curl_slist结构体类型，该选项自定义请求头信息
		CURL_EASY_SETOPT(pCmdCurl, CURLOPT_URL, url.c_str());

		//写入接收回调函数
		curl_easy_setopt(pCmdCurl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(pCmdCurl, CURLOPT_WRITEDATA, (void*)&resp_data);

		// 执行HTTP GET操作
		CURL_EASY_PERFORM(pCmdCurl); //这个函数在初始化CURL类型的指针 以及curl_easy_setopt完成后调用..让设置的option 运作起来。

		curl_slist_free_all(headers);
		if (res != CURLE_OK) {
			ret = -1;
		}
	} while (0);

	jout = resp_data;

	if (NULL != pCmdCurl)
	{
		curl_easy_cleanup(pCmdCurl);
	}
	return ret;
}


int LaunchPu::start()
{
	int res = 0;
	SHL_Json::Reader reader;
	SHL_Json::Value rvalue;
	ifstream in("lmt.json", ios::binary);
	if (!in.is_open())
	{
		ERRE_MSV("open lmt.json failed!");
		res = -1;
	}
	if (!reader.parse(in, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		res = -1;
	}
	//ASSERT(0 == res);
	if (0 == res)
	{
		m_launchHost     = rvalue["External_IP"].asString();
		m_launchPort     = rvalue["Launch_PORT"].asInt();
		m_statusinterval = rvalue["Launch_Interval"].asInt();
		m_proxyHost      = rvalue["Proxy_Host"].asString();
		m_proxyPort      = rvalue["Proxy_Port"].asInt();
	}

	if (Pu::start() < 0)
	{
		return -1;
	}
	m_init = true;
	launchSVR = new LaunchServer("", m_launchPort);
    launchSVR->start();
	launchSVR->setPu(this);

	//to start heartbeat thread
	m_thread.setEntry(heartbeat, this, 0);
	m_thread.start();


	//to start status thread
	m_statusThread.setEntry(statusEntry, this, 0);
	m_statusThread.start();
	return 0;
}


int LaunchPu::queryDeviceStatus(string device_id,DeviceStatus&status)
{
	SHL_Json::Reader reader;
	SHL_Json::FastWriter writer;
	SHL_Json::Value rvalue;
	SHL_Json::Value wvalue;
	int res = -1;
	string body = "";
	string response = "";
	char url[512] = { 0 };
	sprintf(url, "http://%s:%d/api/v1/device/status?token=%s",m_host.c_str(), m_port, m_token.c_str());
	wvalue["id_device"] = device_id;
	body = writer.write(wvalue);
	res = curlPost(body, string(url), response);
	if (0 == res)
	{
		if (!reader.parse(response, rvalue, false))
		{
			ERRE_MSV("SHL_Json parse error");
			return -1;
		}
		int code   = rvalue["code"].asInt();
		string msg = rvalue["msg"].asString();
		status.device_id = device_id;
		if (1000 == code)//success
		{
			status.online_status = rvalue["data"]["e_online_status"].asInt();
			status.fault_status  = rvalue["data"]["e_fault_status"].asInt();
			status.ctrl_mode     = rvalue["data"]["e_ctrl_mode"].asInt();
			status.ctrl_priority = rvalue["data"]["e_ctrl_priority"].asInt();
			status.run_status    = rvalue["data"]["e_run_status"].asInt();
			return 0;
		}
		else//failed
		{
			ERRE_MSV("to query device status failed,reason:%s", msg.c_str());
			return -1;
		}
	}
	return -1;
}


int LaunchPu::subscribeDeviceStatus(int msg_type[],int msg_num,string dest_url)
{
	SHL_Json::Reader reader;
	SHL_Json::FastWriter writer;
	SHL_Json::Value rvalue;
	SHL_Json::Value wvalue;
	int res = -1;
	string body = "";
	string response = "";
	char url[512] = { 0 };
	sprintf(url, "http://%s:%d/api/v1/message/subscription?token=%s", m_host.c_str(), m_port, m_token.c_str());
	for (int i= 0; i < msg_num; i++)
	{
		wvalue["msg_types"].append(msg_type[i]);
	}
	wvalue["dest_url"] = dest_url;
	body = writer.write(wvalue);
	res = curlPost(body, string(url), response);
	if (0 == res)
	{
		if (!reader.parse(response, rvalue, false))
		{
			ERRE_MSV("SHL_Json parse error");
			return -1;
		}
		int code = rvalue["code"].asInt();
		string msg = rvalue["msg"].asString();
		if (1000 == code)//success
		{
			return 0;
		}
		else//failed
		{
			ERRE_MSV("to subscribe device status failed,reason:%s", msg.c_str());
			return -1;
		}
	}
	return -1;
}


int LaunchPu::unsubscribeDeviceStatus(int msg_type[], int msg_num)
{
	SHL_Json::Reader reader;
	SHL_Json::FastWriter writer;
	SHL_Json::Value rvalue;
	SHL_Json::Value wvalue;
	int res = -1;
	string body = "";
	string response = "";
	char url[512] = { 0 };
	sprintf(url, "http://%s:%d/api/v1/message/unsubscription?token=%s", m_host.c_str(), m_port, m_token.c_str());
	for (int i = 0; i < msg_num; i++)
	{
		wvalue["msg_types"].append(msg_type[i]);
	}
	body = writer.write(wvalue);
	res = curlPost(body, string(url), response);
	if (0 == res)
	{
		if (!reader.parse(response, rvalue, false))
		{
			ERRE_MSV("SHL_Json parse error");
			return -1;
		}
		int code = rvalue["code"].asInt();
		string msg = rvalue["msg"].asString();
		if (1000 == code)//success
		{
			return 0;
		}
		else//failed
		{
			ERRE_MSV("to unsubscribe device status failed,reason:%s", msg.c_str());
			return -1;
		}
	}
	return -1;
}

void LaunchPu::stop()
{
    if (!m_init)
        return;

    m_init = false;

	m_thread.stop();

    logout();
    
    Pu::stop();

	launchSVR->stop();

	int msg_type[] = { 201 };

	unsubscribeDeviceStatus(msg_type, 1);
}

int LaunchPu::login()
{
	if (m_login)
	{
		return 0;
	}
	int res = -1;
	string ret = "";
	char url[512] = { 0 };
	sprintf(url, "http://%s:%d/api/v1/token?access_id=%s&access_secret=%s",
		    m_host.c_str(), m_port,m_user.c_str(),m_pswd.c_str());
	string u = string(url);
	res = curlGet(u, ret);//to get token
	if (0 == res)
	{
		SHL_Json::Reader reader;
		SHL_Json::Value  value;
		if (!reader.parse(ret, value, false))
		{
			ERRE_MSV("SHL_Json parse error");
			return -1;
		}
		int code        = value["code"].asInt();
		string msg      = value["msg"].asString();
		if (1000 == code)//success
		{
			m_token = value["data"]["token"].asString();
			m_expiretime = value["data"]["expires_in"].asInt();
			m_login = true;

			//subscribe device real-time status
			char url[512] = { 0 };
			sprintf(url, "http://%s:%d/launch", m_launchHost.c_str(), m_launchPort);
			int msg_type[] = { 201 };
			res = subscribeDeviceStatus(msg_type, 1, string(url));
			return 0;
		}
		else//failed
		{
			ERRE_MSV("to get token failed,reason:%s",msg.c_str());
		}
	}

	return -1;
}

void LaunchPu::logout()
{
    if (!m_login)
        return;
    m_login = false;
}
void*
LaunchPu::heartbeat(ThreadInfo *arg)
{
	LaunchPu *pu = (LaunchPu*)arg->data;
	pu->m_lastTimeStamp = sys->currentTimeMillis();
	while (pu->m_thread.active())
	{
		uint64_t currentTimeStamp = sys->currentTimeMillis();
		if (currentTimeStamp - pu->m_lastTimeStamp >= pu->m_expiretime/12)//every minute
		{
 			pu->m_lastTimeStamp = currentTimeStamp;
			//TODO:refresh token
			int res = -1;
			string ret = "";
			char url[512] = { 0 };
			sprintf(url, "http://%s:%d/api/v1/refresh_token?token=%s",
				    pu->m_host.c_str(), pu->m_port, pu->m_token.c_str());
			string u = string(url);
			res = pu->curlGet(u, ret);
			if (0 == res)
			{
				SHL_Json::Reader reader;
				SHL_Json::Value  value;
				if (!reader.parse(ret, value, false))
				{
					ERRE_MSV("SHL_Json parse error");
				}
				int code = value["code"].asInt();
				string msg = value["msg"].asString();
				if (1000 == code)//success
				{
					pu->m_login = true;
					ERRI_MSV("to refresh token success\n");
				}
				else//failed
				{
					ERRE_MSV("to refresh token failed,reason:%s", msg.c_str());
					pu->m_login = false;
					pu->m_first = false;
				}
			}
		}
		sys->sleep(1000);
	}
	return NULL;
}

void * LaunchPu::statusEntry(ThreadInfo *arg)
{
	LaunchPu *pu = (LaunchPu*)arg->data;
	pu->m_lastStatusTimeStamp = 0;
	while (pu->m_statusThread.active())
	{
		uint64_t currentTimeStamp = sys->currentTimeMillis();
		if (currentTimeStamp - pu->m_lastStatusTimeStamp >= pu->m_statusinterval)//every minute
		{
			pu->m_lastStatusTimeStamp = currentTimeStamp;
			//TODO:query device status
			SHL_Json::Reader reader;
			SHL_Json::Value  rvalue;
			string devJson;
			deviceCfg->query(devJson, "type = 301");
			if (reader.parse(devJson, rvalue, false) && rvalue.isArray() && rvalue.size() >= 1)
			{
				//AutoLock locker(pu->m_deviceStatus._mtx);
				for (int i = 0; i < int(rvalue.size()); i++)
				{
					string deviceID = rvalue[i]["comment"].asString();
					if (deviceID != "")
					{
						DeviceStatus status;
						int ret = pu->queryDeviceStatus(deviceID, status);
						if (0 == ret)//查询成功
						{
							if (1 == status.online_status && 0 == status.run_status && 0 == status.ctrl_mode)//可用
							{
								pu->m_deviceStatus[deviceID] = 1;
							}
							else
							{
								pu->m_deviceStatus[deviceID] = 0;
							}
						}
						else//查询失败
						{
							pu->m_deviceStatus[deviceID] = 1;//可用
						}
					}
				}
			}
		}
		sys->sleep(1000);
	}
	return NULL;
}

int
LaunchPu::onCustomCommand(string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	int ret = -1;

	if (!reader.parse(json, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return ret;
	}
	if (!rvalue.isMember("command"))
	{
		ERRE_MSV("json error:no member \"Method\"");
		return ret;
	}
	string jMethod = rvalue["command"].asString();
	return ret;
}
int cont = 0;
int
LaunchPu::onRequestConfig(string& jin, string& jout)
{
	SHL_Json::Reader reader;
	SHL_Json::FastWriter writer;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  wvalue;
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
	if ("queryLaunchDeviceStatus" == jMethod)
	{
		int    channelNo  = rvalue["chnNo"].asInt();
		string deviceHost = rvalue["DevIp"].asString();
		int    devicePort = rvalue["DevPort"].asInt();
		string addrcode   = putree->findCode(deviceHost, devicePort, channelNo);
		std::string devjson = "";
		char cond[128] = { 0 };
		snprintf(cond, 128, "addrcode = '%s'", addrcode.c_str());
		deviceCfg->query(devjson, cond);
		SHL_Json::Value  value;
		string device_id;
		if (reader.parse(devjson,value,false) && value.size() >= 1)
		{
			wvalue["DevIp"]   = deviceHost;
			wvalue["DevPort"] = devicePort;
			wvalue["chnNo"]   = channelNo;
			device_id = value[0]["comment"].asString();
			wvalue["status"] = m_deviceStatus[device_id];
			jout = writer.write(wvalue);
			return 0;
		}
	}
	else if ("queryAllLaunchDeviceStatus" == jMethod)
	{
		if (rvalue["chnList"].isArray() && rvalue["chnList"].size() >= 1)
		{
			for (int i = 0; i < int(rvalue["chnList"].size());i++)
			{
				int    channelNo  = rvalue["chnList"][i]["chnNo"].asInt();
				string deviceHost = rvalue["chnList"][i]["DevIp"].asString();
				int    devicePort = rvalue["chnList"][i]["DevPort"].asInt();
				string addrcode = putree->findCode(deviceHost, devicePort, channelNo);
				std::string devjson = "";
				char cond[128] = { 0 };
				snprintf(cond, 128, "addrcode = '%s'", addrcode.c_str());
				deviceCfg->query(devjson, cond);
				SHL_Json::Value  value;
				SHL_Json::Value item;
				string device_id;
				if (reader.parse(devjson, value, false) && value.size() >= 1)
				{
					item["DevIp"] = deviceHost;
					item["DevPort"] = devicePort;
					item["chnNo"] = channelNo;
					device_id = value[0]["comment"].asString();
					item["status"] = m_deviceStatus[device_id];
					wvalue["chnList"].append(item);
				}
				jout = writer.write(wvalue);
				return 0;
			}
		}
	}
	return -1; 
}