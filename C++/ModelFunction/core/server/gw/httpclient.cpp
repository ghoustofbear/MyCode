#include "httpclient.h"
#include "platform.h"
#include "j2x.h"
#include "platform.h"

#define  MAX_HTTP_REQ_LEN    (4*1024)

#define CURL_EASY_SETOPT(A, B, C) \
	if ((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_setopt(%s, %s, %s) failed: %d\n", \
            #A, #B, #C, res);

#define CURL_EASY_PERFORM(A) \
	if ((res = curl_easy_perform((A))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_perform(%s) failed: %d\n", #A, res);

HttpClient::HttpClient(const char *strUrl, void *pUser)
{
	m_s32ContentLen = 0;
	m_s32SendSize = 0;
	m_pUser  = NULL;
	m_strUrl = strUrl;
	m_pUser  = pUser;
	m_strXml = "";
}

HttpClient::~HttpClient()
{
	stop();
}

size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	HttpClient *pHttpClient = (HttpClient *)userp;

	if (size*nmemb < 1 || pHttpClient->m_s32SendSize <= 0)
		return 0;

	if (pHttpClient->m_s32SendSize <= (int)(size*nmemb))
	{
		int s32Ret = pHttpClient->m_s32SendSize;
		memcpy(ptr, pHttpClient->m_ps8ReadPtr, pHttpClient->m_s32SendSize);
		pHttpClient->m_s32SendSize = 0;

		return s32Ret;
	}
	else
	{
		memcpy(ptr, pHttpClient->m_ps8ReadPtr, size*nmemb);
		pHttpClient->m_ps8ReadPtr += size*nmemb;
		pHttpClient->m_s32SendSize -= size*nmemb;

		return size*nmemb;
	}

	return 0;
}

size_t post_resp_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
	HttpClient *pHttpClient = (HttpClient *)user;

	pHttpClient->m_strResponse += ptr;
	if (strncasecmp(ptr, "Content-Length", 14) == 0)
	{
		const char *p = strstr(ptr, ":");
		pHttpClient->m_s32ContentLen = atoi(p + 1);

		return size*nmemb;
	}

	if (pHttpClient->m_s32ContentLen != 0 &&
		strcmp("\r\n", ptr) != 0)
	{
		pHttpClient->m_s32RecvSize += size*nmemb;
	}

	if ((strcmp("\r\n\r\n", ptr) == 0 &&
		pHttpClient->m_s32ContentLen == 0) ||
		(pHttpClient->m_s32ContentLen != 0 &&
		pHttpClient->m_s32RecvSize == pHttpClient->m_s32ContentLen)
		)
	{
		stk::HttpMsg httpMsg(pHttpClient->m_strResponse.c_str(), pHttpClient->m_strResponse.size());
		pHttpClient->onPostRsp(httpMsg, pHttpClient->m_pUser);
	}

	return size*nmemb;
}

int HttpClient::makeXmlReq(HTTP_REQ_S req, string& xml)
{
	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

	if ("Request_Update_Status" == req.strType)
	{
		wvalue["HTTP_XML"]["@EventType"] = "Request_Update_Status";
		wvalue["HTTP_XML"]["Item"]["@From"] = platform->addrcode();
		wvalue["HTTP_XML"]["Item"]["@To"] = string((const char*)req.stUpdateReq.ps8AddrCode);
		wvalue["HTTP_XML"]["Item"]["@Status"] = std::to_string(req.stUpdateReq.u32Status);
		wvalue["HTTP_XML"]["Item"]["@HttpURL"] = string((const char*)req.stUpdateReq.ps8HttpURL);
		wvalue["HTTP_XML"]["Item"]["@SipURL"] = string((const char*)req.stUpdateReq.ps8SipURL);
	}
	else if ("KeepAlive" == req.strType)
	{
		wvalue["HTTP_XML"]["@EventType"] = "KeepAlive";
		wvalue["HTTP_XML"]["Item"]["@From"] = platform->addrcode();
		wvalue["HTTP_XML"]["Item"]["@To"] = string((const char*)req.stUpdateReq.ps8AddrCode);
	}
	else if ("Request_Resource" == req.strType)
	{
		wvalue["HTTP_XML"]["@EventType"] = "Request_Resource";
		wvalue["HTTP_XML"]["Item"]["@Code"] = string((const char*)req.stResouceReq.ps8AddrCode);
		wvalue["HTTP_XML"]["Item"]["@UserCode"] = string((const char*)req.stResouceReq.ps8UserAddrCode);
		wvalue["HTTP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stResouceReq.s32FromIndex);
		wvalue["HTTP_XML"]["Item"]["@ToIndex"] = std::to_string(req.stResouceReq.s32ToIndex);
	}
	else if ("Request_History_Video" == req.strType)
	{
		wvalue["HTTP_XML"]["@EventType"] = "Request_History_Video";
		wvalue["HTTP_XML"]["Item"]["@Code"] = string((const char*)req.stRecordReq.ps8ChnAddrCode);
		wvalue["HTTP_XML"]["Item"]["@UserCode"] = string((const char*)req.stRecordReq.ps8UserAddrCode);
		wvalue["HTTP_XML"]["Item"]["@Type"] = std::to_string(req.stRecordReq.u32RecType);
		wvalue["HTTP_XML"]["Item"]["@BeginTime"] = string((const char*)req.stRecordReq.ps8Begin);
		wvalue["HTTP_XML"]["Item"]["@EndTime"] = string((const char*)req.stRecordReq.ps8End);
		wvalue["HTTP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stRecordReq.s32FromIndex);
		wvalue["HTTP_XML"]["Item"]["@ToIndex"] = std::to_string(req.stRecordReq.s32ToIndex);
	}
	else if ("Request_History_Alarm" == req.strType)
	{
		wvalue["HTTP_XML"]["@EventType"] = "Request_History_Alarm";
		wvalue["HTTP_XML"]["Item"]["@Code"] = string((const char*)req.stAlarmLogReq.ps8ChnAddrCode);
		wvalue["HTTP_XML"]["Item"]["@UserCode"] = string((const char*)req.stAlarmLogReq.ps8UserAddrCode);
		wvalue["HTTP_XML"]["Item"]["@Type"] = std::to_string(req.stAlarmLogReq.u32AlarmType);
		wvalue["HTTP_XML"]["Item"]["@BeginTime"] = string((const char*)req.stAlarmLogReq.ps8Begin);
		wvalue["HTTP_XML"]["Item"]["@EndTime"] = string((const char*)req.stAlarmLogReq.ps8End);
		wvalue["HTTP_XML"]["Item"]["@Level"] = std::to_string(req.stAlarmLogReq.u32AlarmLevel);
		wvalue["HTTP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stAlarmLogReq.s32FromIndex);
		wvalue["HTTP_XML"]["Item"]["@ToIndex"] = std::to_string(req.stAlarmLogReq.s32ToIndex);
	}
	else
	{
		return -1;
	}

	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

	return 0;
}

bool HttpClient::start()
{
	CURLcode res = CURLE_OK;
	m_pCurl = curl_easy_init();
	if (NULL == m_pCurl)
	{
		LOGI_MSV("get a easy handle failed.\n");
		return false;
	}
	else
	{
		CURL_EASY_SETOPT(m_pCurl, CURLOPT_VERBOSE, 0L);
		CURL_EASY_SETOPT(m_pCurl, CURLOPT_NOPROGRESS, 1L);
		CURL_EASY_SETOPT(m_pCurl, CURLOPT_TIMEOUT, 5L);
		if (res != CURLE_OK)
		{
			curl_easy_cleanup(m_pCurl);
			m_pCurl = NULL;
		}
	}

	return true;
}

void HttpClient::stop()
{
	if (NULL != m_pCurl)
	{
		curl_easy_cleanup(m_pCurl);
		m_pCurl = NULL;
	}
}

void HttpClient::onPostRsp(stk::HttpMsg &httpMsg, void *pUser)
{
	string strMsg;
	httpMsg.to_str(strMsg);

	printf("%s", strMsg.c_str());

	if (strstr(strMsg.c_str(), "Response_Update_Status"))
	{
		onUpdateRsp(httpMsg, pUser);
	}
	else if (strstr(strMsg.c_str(), "KeepAlive"))
	{
		//OnHeartbeatRsp(httpMsg);
	}
	else if (strstr(strMsg.c_str(), "Response_Resource"))
	{
		onQueryResourceRsp(httpMsg, pUser);
	}
	else if (strstr(strMsg.c_str(), "Response_History_Video"))
	{
		onQueryRecordRsp(httpMsg, pUser);
	}
	else if (strstr(strMsg.c_str(), "Response_History_Alarm"))
	{
		onQueryAlarmLogRsp(httpMsg, pUser);
	}
}

int HttpClient::onUpdateRsp(stk::HttpMsg &httpMsg, void *pUser)
{
	m_strXml = httpMsg.GetBody();

	return 0;
}

int HttpClient::onQueryResourceRsp(stk::HttpMsg &httpMsg, void *pUser)
{
	m_strXml = httpMsg.GetBody();
	
	return 0;
}

int HttpClient::onQueryRecordRsp(stk::HttpMsg &httpMsg, void *pUser)
{
	m_strXml = httpMsg.GetBody();

	return 0;
}

int HttpClient::onQueryAlarmLogRsp(stk::HttpMsg &httpMsg, void *pUser)
{
	m_strXml = httpMsg.GetBody();

	return 0;
}

int HttpClient::doPost(const string eventType, const char *ps8Xml, int s32XmlLen)
{
	m_strResponse = "";
	m_s32ContentLen = 0;
	m_s32RecvSize = 0;

	CURLcode res = CURLE_OK;

	string strUrl = m_strUrl + eventType;

	CURL_EASY_SETOPT(m_pCurl, CURLOPT_URL, strUrl.c_str());

	CURL_EASY_SETOPT(m_pCurl, CURLOPT_HEADER, 1L);
	CURL_EASY_SETOPT(m_pCurl, CURLOPT_POST, 1L);  

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/xml");
	headers = curl_slist_append(headers, "Connection: Keep-Alive");
	CURL_EASY_SETOPT(m_pCurl, CURLOPT_HTTPHEADER, headers);

	CURL_EASY_SETOPT(m_pCurl, CURLOPT_POSTFIELDSIZE, s32XmlLen);
	CURL_EASY_SETOPT(m_pCurl, CURLOPT_READFUNCTION, read_callback);

	m_ps8ReadPtr = (char *)ps8Xml;
	m_s32SendSize = s32XmlLen;
	CURL_EASY_SETOPT(m_pCurl, CURLOPT_READDATA, this);

	CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, post_resp_callback);
	CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this);
	CURL_EASY_PERFORM(m_pCurl);

	curl_slist_free_all(headers);

	/*
	* curl_easy_perform为阻塞函数，所以这里可以不用做事件等待相关操作
	*/
	if (CURLE_OK == res)
	{
		long lResponse = 0;
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
		if (200 == lResponse)
		{
			return 0;
		}
	}

	return -1;
}

int HttpClient::update(const char *ps8AddrCode, int status)
{
	HTTP_REQ_S stReq;

	stReq.strType = "Request_Update_Status";
	stReq.stUpdateReq.ps8AddrCode = (char *)ps8AddrCode;
	stReq.stUpdateReq.ps8HttpURL  = "";
	stReq.stUpdateReq.ps8SipURL   = "";
	stReq.stUpdateReq.u32Status   = status;

	string strXml;
	makeXmlReq(stReq, strXml);

	return doPost("/Request_Update_Status", strXml.c_str(), strXml.size());
}

int HttpClient::heartbeat(const char *ps8AddrCode)
{
	HTTP_REQ_S stReq;

	stReq.strType = "KeepAlive";
	stReq.stUpdateReq.ps8AddrCode = (char *)ps8AddrCode;

	string strXml;
	makeXmlReq(stReq, strXml);

	return doPost("/KeepAlive", strXml.c_str(), strXml.size());
}

int HttpClient::query_resource(const char *ps8addrcode, int fromIndex, int toIndex, string &content, int jx)
{
	int iRet = 0;

	start();

	fromIndex = (fromIndex < 1) ? 1 : fromIndex;
	toIndex = (toIndex < fromIndex) ? (fromIndex + 1000) : toIndex;

	HTTP_REQ_S stReq;

	stReq.strType = "Request_Resource";
	stReq.stResouceReq.ps8AddrCode = (char *)ps8addrcode;
	stReq.stResouceReq.s32FromIndex = fromIndex;
	stReq.stResouceReq.s32ToIndex = toIndex;
    stReq.stResouceReq.ps8UserAddrCode = "";

    bool bFinished = false;
	while (!bFinished)
	{
		string strXml;
		makeXmlReq(stReq, strXml);
		if (doPost("/Request_Resource", strXml.c_str(), strXml.size()) < 0)
		{
			iRet = -1;
			break;
		}

		if (jx == 0)
		{
            xml2json(m_strXml.c_str(), content);
		}
		else
		{
            content = m_strXml;
		}

		bFinished = true;
	}

	stop();

	return iRet;
}

int HttpClient::query_record(const char *ps8addrcode, const char *ps8Begin, const char *ps8End, 
	                  uint32_t u32RecType, int fromIndex, int toIndex, string &content, int jx)
{
	int iRet = 0;

	start();

	fromIndex = (fromIndex < 1) ? 1 : fromIndex;
	toIndex = (toIndex < fromIndex) ? (fromIndex + 1000) : toIndex;

	HTTP_REQ_S stReq;

	stReq.strType = "Request_History_Video";
	stReq.stRecordReq.ps8ChnAddrCode = (char *)ps8addrcode;
	stReq.stRecordReq.ps8UserAddrCode = (char *)platform->addrcode().c_str();
	stReq.stRecordReq.ps8Begin = (char *)ps8Begin;
	stReq.stRecordReq.ps8End = (char *)ps8End;
	stReq.stRecordReq.u32RecType = 0xFFFFFFFF;
	stReq.stRecordReq.s32FromIndex = fromIndex;
	stReq.stRecordReq.s32ToIndex = toIndex;

    bool bFinished = false;
	while (!bFinished)
	{
		string strXml;
		makeXmlReq(stReq, strXml);
		if (doPost("/Request_History_Video", strXml.c_str(), strXml.size()) < 0)
		{
			iRet = -1;
			break;
		}

		if (jx == 0)
		{
            xml2json(m_strXml.c_str(), content);
		}
		else
		{
            content = m_strXml;
		}

		bFinished = true;
	}

	stop();

	return iRet;
}

int HttpClient::query_alarm(const char *ps8addrcode, const char *ps8Begin, const char *ps8End, uint32_t u32AlarmType, 
	               uint32_t u32AlarmLevel, int fromIndex, int toIndex, string &content, int jx)
{
	int iRet = 0;

	start();

	fromIndex = (fromIndex < 1) ? 1 : fromIndex;
	toIndex = (toIndex < fromIndex) ? (fromIndex + 1000) : toIndex;

	HTTP_REQ_S stReq;

	stReq.strType = "Request_History_Alarm";
	stReq.stAlarmLogReq.ps8ChnAddrCode = (char *)ps8addrcode;
	stReq.stAlarmLogReq.ps8UserAddrCode = (char *)platform->addrcode().c_str();
	stReq.stAlarmLogReq.ps8Begin = (char *)ps8Begin;
	stReq.stAlarmLogReq.ps8End = (char *)ps8End;
	stReq.stAlarmLogReq.u32AlarmType = 0xFFFFFFFF;
	stReq.stAlarmLogReq.u32AlarmLevel = u32AlarmLevel;
	stReq.stAlarmLogReq.s32FromIndex = fromIndex;
	stReq.stAlarmLogReq.s32ToIndex = toIndex;

    bool bFinished = false;
	while (!bFinished)
	{
		string strXml;
		makeXmlReq(stReq, strXml);
		if (doPost("/Request_History_Alarm", strXml.c_str(), strXml.size()) < 0)
		{
			iRet = -1;
			break;
		}

		if (jx == 0)
		{
            xml2json(m_strXml.c_str(), content);
		}
		else
		{
            content = m_strXml;
		}

		bFinished = true;
	}

	stop();

	return iRet;
}
