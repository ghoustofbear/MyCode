#include "jiyuan_iva_pu.h"
#include "json/json.h"
#include "utiva/typedef.h"
#include "putree.h"

#define CURL_EASY_SETOPT(A, B, C) \
	if ((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_setopt(%s, %s, %s) failed: %d\n", \
            #A, #B, #C, res);

#define CURL_EASY_PERFORM(A) \
	if ((res = curl_easy_perform((A))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_perform(%s) failed: %d\n", #A, res);

typedef struct tagThreadParam
{
    void * pthis;
    string json;
    Thread *trd;
}ThreadParam;


typedef struct tagSendDataParam
{
    char * pSendData;
    int    SendSize;
}SendDataParam;

JiYuanIvaPu::JiYuanIvaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_pLiveCurl = NULL;
	m_protocol = PROTO_TYPE_JIYUAN;
	m_type = DEV_TYPE_AI;
	m_port = port;
    m_tmPrev = sys->currentTimeMillis();
}

JiYuanIvaPu::~JiYuanIvaPu()
{

}

int JiYuanIvaPu::start()
{
	if (Pu::start() < 0)
	{
		return -1;
	}

	CURLcode res = CURLE_OK;
    m_pLiveCurl = curl_easy_init();
    if (NULL == m_pLiveCurl)
	{
		LOGI_MSV("get a easy handle failed.\n");
		return -1;
	}
	else
	{
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_VERBOSE, 0L);
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_NOPROGRESS, 1L);
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_TIMEOUT, 5L);
		if (res != CURLE_OK)
		{
            curl_easy_cleanup(m_pLiveCurl);
            m_pLiveCurl = NULL;
		}
	}

	m_init = true;

	return 0;
}


void *
JiYuanIvaPu::threadProc(ThreadInfo *info)
{
    ThreadParam * task = (ThreadParam *)(info->data);

    if (NULL == task)
    {
        return NULL;
    }
    JiYuanIvaPu * pPu = (JiYuanIvaPu*)task->pthis;
    if (NULL == pPu)
    {
        return NULL;
    }

    Thread *trd = (Thread*)task->trd;
    string json = task->json;

    pPu->sendcommand(json);

    delete trd;
    delete task;

    return NULL;
}

size_t readjson_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    SendDataParam * pSendData = (SendDataParam *)userp;

    if (size*nmemb < 1 || pSendData->SendSize <= 0)
        return 0;

    if (pSendData->SendSize <= (int)(size*nmemb))
    {
        int s32Ret = pSendData->SendSize;
        memcpy(ptr, pSendData->pSendData, pSendData->SendSize);
        pSendData->SendSize = 0;

        return s32Ret;
    }
    else
    {
        memcpy(ptr, pSendData->pSendData, size*nmemb);
        pSendData->pSendData += size*nmemb;
        pSendData->SendSize -= size*nmemb;

        return size*nmemb;
    }

    return 0;
}

int JiYuanIvaPu::sendcommand(string &json)
{
    CURL *pCmdCurl = NULL;
    do
    {
        pCmdCurl = curl_easy_init();
        if (NULL == pCmdCurl)
        {
            break;
        }
        CURLcode res = CURLE_OK;
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_VERBOSE, 0L);
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_NOPROGRESS, 1L);
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_TIMEOUT, 1L);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (!reader.parse(json, rvalue, false))
        {
            break;
        }

        int type = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseType"].asString().c_str());
        if ((type == SUPER_WORKING) || (type == SUPER_ACTIVITY) || (type == SUPER_CLASSIFY)
            || (type == SUPER_INVADE) || (type == SUPER_APPEARANCE) || (type == SUPER_FIRE))
        {
            SHL_Json::FastWriter      writer;
            SHL_Json::Value           value;
            value["Method"] = "Alg_Task_Dispatch";
            value["TaskId"] = rvalue["SIP_XML"]["Item"]["@TaskId"].asString();
            value["TaskName"] = rvalue["SIP_XML"]["Item"]["@TaskName"].asString();
            value["AnalyseType"] = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseType"].asString().c_str());
            value["AnalyseSubType"] = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseSubType"].asString().c_str());
            value["AnalyseExe"] = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseExe"].asString().c_str());

            if (!rvalue["SIP_XML"]["Item"]["@TaskPrj"].isNull())
                value["TaskPrj"] = atoi(rvalue["SIP_XML"]["Item"]["@TaskPrj"].asString().c_str());

            if (!rvalue["SIP_XML"]["Item"]["Area"].isArray())
            {
                if (!rvalue["SIP_XML"]["Item"]["Area"]["@PointNum"].isNull())
                {
                    value["AreaList"].append(rvalue["SIP_XML"]["Item"]["Area"]);
                }
            }
            else
            {
                for (int i = 0; i < int(rvalue["SIP_XML"]["Item"]["Area"].size()); i++)
                {
                    if (!rvalue["SIP_XML"]["Item"]["Area"][i]["@PointNum"].isNull())
                    {
                        value["AreaList"].append(rvalue["SIP_XML"]["Item"]["Area"][i]);
                    }
                }
            }

            string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
            Pu * pu = putree->findPu(addrcode);
            if (!pu)
            {
                break;
            }

            if (Pu::PROTO_TYPE_HIK == pu->protocol())
            {
                value["DevType"] = "Hik";
            }
            else  if (Pu::PROTO_TYPE_DAHUA == pu->protocol())
            {
                value["DevType"] = "DaHua";
            }
            else  if (Pu::PROTO_TYPE_TIANDY == pu->protocol())
            {
                value["DevType"] = "TianDy";
            }

            int  channelNo = pu->guchannel(addrcode);
            value["DevIp"] = pu->ip();
            value["DevPort"] = pu->port();
            value["DevChn"] = (channelNo + 1);
            if (!rvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
            {
                value["PresetNo"] = atoi(rvalue["SIP_XML"]["Item"]["@PrePoint"].asString().c_str());
            }
            else
            {
                value["PresetNo"] = 1;
            }
            value["UserName"] = pu->userName();
            value["Password"] = pu->passWord();

            SHL_Json::FastWriter write;
            string jsonStr = write.write(value);

            string utf8Json = sys->ascii2utf8(jsonStr.c_str());
            int s32Len = utf8Json.size();

            char url[512] = { 0 };
            sprintf(url, "http://%s:%d/TICP/control/getJsonCon/sendCommand", m_host.c_str(), m_port);

            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_URL, url);
            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_POST, 1L);

            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_HEADER, 1L);
            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_POST, 1L);

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Connection: Keep-Alive");
            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_HTTPHEADER, headers);

            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_POSTFIELDSIZE, s32Len);
            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_READFUNCTION, readjson_callback);

            SendDataParam * sendData = new SendDataParam;
            if (NULL == sendData)
            {
                break;
            }
            sendData->pSendData = (char *)utf8Json.c_str();
            sendData->SendSize = s32Len;

            CURL_EASY_SETOPT(pCmdCurl, CURLOPT_READDATA, sendData);

            CURL_EASY_PERFORM(pCmdCurl);

            delete sendData;
            curl_slist_free_all(headers);

            /*
            * curl_easy_perform为阻塞函数，所以这里可以不用做事件等待相关操作
            */
            if (CURLE_OK == res)
            {
                long lResponse = 0;
                curl_easy_getinfo(pCmdCurl, CURLINFO_RESPONSE_CODE, &lResponse);
                if (200 == lResponse)
                {
                    break;
                }
            }
        }
    } while (0);

    if (NULL != pCmdCurl)
    {
        curl_easy_cleanup(pCmdCurl);
    }
   
    return -1;
}

int JiYuanIvaPu::taskDispatch(const string &json, string &xml)
{
	/* 下发智能分析任务(启停) */
    Thread *trd = new Thread;
    if (NULL == trd)
    {
        return 0;
    }
    tagThreadParam * param = new tagThreadParam;
    if (NULL != param)
    {
        param->pthis = this;
        param->json  = json;
        param->trd   = trd;
        trd->setEntry(threadProc, param, 0);
        trd->start();
        trd->detach();
    }
    return 0;
}


int JiYuanIvaPu::cameraSnap(const string &json, string &xml)
{
	/* 启动摄像机抓拍 */

	return 0;
}

void JiYuanIvaPu::stop()
{
	if (!m_init)
		return;

	m_init = false;

	logout();

	Pu::stop();
}

int JiYuanIvaPu::login()
{
    //return 0;
	/* 发送固定心跳 */

	CURLcode res = CURLE_OK;

	SHL_Json::Value  rvalue;
	rvalue["Method"] = "keepalive";
	SHL_Json::FastWriter write;
	string jsonStr = write.write(rvalue);

    int s32Len = jsonStr.size();

	char url[512] = { 0 };
    sprintf(url, "http://%s:%d/TICP/control/getJsonCon/sendCommand", m_host.c_str(), m_port);
	
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_URL, url);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_POST, 1L);

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_HEADER, 1L);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_POST, 1L);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Connection: Keep-Alive");
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_HTTPHEADER, headers);

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_POSTFIELDSIZE, s32Len);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_READFUNCTION, readjson_callback);

    SendDataParam * sendData = new SendDataParam;
    if (NULL == sendData)
    {
        curl_slist_free_all(headers);
        return -1;
    }
    sendData->pSendData = (char *)jsonStr.c_str();
    sendData->SendSize = s32Len;

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_READDATA, sendData);

    CURL_EASY_PERFORM(m_pLiveCurl);

    delete sendData;
    curl_slist_free_all(headers);
	/*
	* curl_easy_perform为阻塞函数，所以这里可以不用做事件等待相关操作
	*/
	if (CURLE_OK == res)
	{
		long lResponse = 0;
        curl_easy_getinfo(m_pLiveCurl, CURLINFO_RESPONSE_CODE, &lResponse);
		if (200 == lResponse)
		{
            m_login = true;
			return 0;
		}
	}

	return -1;
}

void JiYuanIvaPu::logout()
{
    if (NULL != m_pLiveCurl)
	{
        curl_easy_cleanup(m_pLiveCurl);
        m_pLiveCurl = NULL;
	}
}

void JiYuanIvaPu::onTimer()
{
    if (!m_login)
    {
        return;
    }

    uint64_t tmNow = sys->currentTimeMillis();
    if (tmNow - m_tmPrev >= 60 * 1000)
    {
        m_tmPrev = tmNow;
        if (0 != login())
        {
            m_first = false;
            m_login = false;
        }
    }
}