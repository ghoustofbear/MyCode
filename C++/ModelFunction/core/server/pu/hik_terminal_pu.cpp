#include "hik_terminal_pu.h"
#include "json/json.h"
#include "utiva/typedef.h"
#include "putree.h"
#include "md5.h"
#include <algorithm>

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
    const char * pSendData;
    int    SendSize;
}SendDataParam;



HikTerminalPu::HikTerminalPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_mux.lock();
    m_pLiveCurl = NULL;
    m_mux.unlock();
    m_protocol = PROTO_TYPE_HIK;
    m_port = port;
    m_tmPrev = sys->currentTimeMillis();
}

HikTerminalPu::~HikTerminalPu()
{

}

int HikTerminalPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }
    CURLcode res = CURLE_OK;
    //���ڷ���������
    m_mux.lock();
    m_pLiveCurl = curl_easy_init();
    if (NULL == m_pLiveCurl)
    {
        m_mux.unlock();
        LOGI_MSV("HikTerminalPu::start() get a easy handle failed.\n");
        return -1;
    }
    else
    {
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_VERBOSE, 0L);//�򿪻��ϵ��ϸģʽ����������Ϊ1��ʹ����ʾ�йش˾���ϲ����Ĵ�����ϸ��Ϣ
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_NOPROGRESS, 1L);//���ø�ֵΪ����ֵ�ر�CRUL������ʾ�Ľ�������Ϊ0��ʾ�򿪡�
        CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_TIMEOUT, 5L);//���ú���ִ�е��ʱ�䣬ʱ�䵥λΪs 
        if (res != CURLE_OK)
        {
            curl_easy_cleanup(m_pLiveCurl);
            m_pLiveCurl = NULL;
        }
    }
    m_mux.unlock();

    m_init = true;
	return 0;
}




/*
*�ϴ����ݻص�����ֵ��ʾ�ϴ��ֽ���������0��ʾ���ݽ�������0����δ�������������øú���ֱ������0
*/
size_t read_data(void* buffer, size_t size, size_t nmemb, void* userdata)
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
*bufferΪ���յ������ݣ�size��nmembΪ���յ����ݴ�С��userdata���ص�����ָ���������
*/
size_t write_data(void* buffer, size_t size, size_t nmemb, void* userdata)
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
*����ֵ���ɹ�0��ʧ��-1
*/
int                
HikTerminalPu::curlPost(string jin, string url, string &jout)
{
    int ret = 0;
    string resp_data="";
    CURL *pCmdCurl = NULL;
    SendDataParam sendData;
    string utf8Json= "";
    do 
    {
        pCmdCurl = curl_easy_init();
        if (NULL == pCmdCurl)
        {
            ret = -1;
            break;
        }
        CURLcode res = CURLE_OK;
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_VERBOSE, 0L);//�رմ�ӡ
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_NOPROGRESS, 1L);//����ʾ������
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_TIMEOUT, 1L);//��ʱʱ��

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
*����ֵ���ɹ�0��ʧ��-1
*/
int                
HikTerminalPu::curlGet(string &url, string &jout)
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
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_VERBOSE, 0L);//�رմ�ӡ
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_NOPROGRESS, 1L);//����ʾ������
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_TIMEOUT, 1L);//��ʱʱ��

        //д������ͷ��
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");//������������
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_HTTPHEADER, headers);//curl_slist�ṹ�����ͣ���ѡ���Զ�������ͷ��Ϣ
        CURL_EASY_SETOPT(pCmdCurl, CURLOPT_URL, url.c_str());

        //д����ջص�����
        curl_easy_setopt(pCmdCurl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(pCmdCurl, CURLOPT_WRITEDATA, (void*)&resp_data);

        // ִ��HTTP GET����
        CURL_EASY_PERFORM(pCmdCurl); //��������ڳ�ʼ��CURL���͵�ָ�� �Լ�curl_easy_setopt��ɺ����..�����õ�option ����������

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

//��ȡ������Ϣ
int               
HikTerminalPu::sendReqLane(string jin, string &jout)
{
    char url[512] = { 0 };
    sprintf(url, "http://%s:%d/getRoadWayPage", m_host.c_str(), m_port);
    return curlPost(string(""), string(url), jout);
}

//�ڰ�����
int
HikTerminalPu::sendReqPlate(string jin, string jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }
    int operateType = rvalue["operateType"].asInt();
    switch (operateType)
    {
    case 0:
    {
        //������
        string plateNum = rvalue["plateNum"].asString();
        int listType = rvalue["listType"].asInt();
        string startTime = rvalue["startTime"].asString();
        string endTime = rvalue["endTime"].asString();
        string createTime = rvalue["createTime"].asString();
        string ownerPhone = rvalue["ownerPhone"].asString();
        int enableflag = 1;

        SHL_Json::Value reqValue;
        SHL_Json::FastWriter reqWrite;
        char url[512] = { 0 };
        //���İ�����
        if (listType == 0)
        {
            sprintf(url, "http://%s:%d/addInnerVehicle", m_host.c_str(), m_port);
            reqValue["createTime"] = createTime;
            reqValue["ownerPhone"] = ownerPhone;
        }
        //���ĺ�����
        else
        {
            sprintf(url, "http://%s:%d/addAlarmVehicle ", m_host.c_str(), m_port);
            reqValue["enableflag"] = enableflag;
            reqValue["contactnumber"] = ownerPhone;
        }

        reqValue["plateNo"] = plateNum;
        reqValue["startTime"] = startTime;
        reqValue["endTime"] = endTime;

        //����
        char md5[33] = { 0 };
        string pswd_md5 = "";
        MD5((uint8_t*)passWord().c_str(), (uint8_t *)md5, sizeof(md5));
        pswd_md5 = md5;
        transform(pswd_md5.begin(), pswd_md5.end(), pswd_md5.begin(), ::toupper);
        pswd_md5 = plateNum + pswd_md5;
        MD5((uint8_t*)pswd_md5.c_str(), (uint8_t *)md5, sizeof(md5));
        pswd_md5 = md5;
        transform(pswd_md5.begin(), pswd_md5.end(), pswd_md5.begin(), ::toupper);
        reqValue["sign"] = pswd_md5;

        string reqJson = reqWrite.write(reqValue);

        //��������
        string ansJson = "";
        int ret = curlPost(reqJson, string(url), ansJson);
        if (ret)
        {
            LOGI_MSV("HikTerminalPu::sendReqPlate() addAlarmVehicle curlPost failed.\n");
            return -1;
        }

        SHL_Json::Reader ansReader;
        SHL_Json::Value  ansValue;
        if (!ansReader.parse(ansJson, ansValue, false))
        {
            ERRE_MSV("SHL_Json parse error");
            return -1;
        }
        //���ش������, 0 - �ɹ����� 0 - �������
        int code = ansValue["code"].asInt();
        if (code != 0)
        {
            string errDesc = ansValue["errDesc"].asString();
            LOGI_MSV("HikTerminalPu::sendReqPlate() addAlarmVehicle failed,reason��%s\n",errDesc.c_str());
            return -1;
        }
        return 0;            
        
    }
        break;
    case 1:
    {
        //ɾ
        string plateNum = rvalue["plateNum"].asString();        
        SHL_Json::Value reqValue;
        SHL_Json::FastWriter reqWrite;
        reqValue["plateNo"] = plateNum;
        //����
        char md5[33] = { 0 };
        string pswd_md5 = "";
        MD5((uint8_t*)passWord().c_str(), (uint8_t *)md5, sizeof(md5));
        pswd_md5 = md5;
        transform(pswd_md5.begin(), pswd_md5.end(), pswd_md5.begin(), ::toupper);
        pswd_md5 = plateNum + pswd_md5;
        MD5((uint8_t*)pswd_md5.c_str(), (uint8_t *)md5, sizeof(md5));
        pswd_md5 = md5;
        transform(pswd_md5.begin(), pswd_md5.end(), pswd_md5.begin(), ::toupper);
        reqValue["sign"] = pswd_md5;

        string reqJson = reqWrite.write(reqValue);

        //����ɾ������
        char url[512] = { 0 };
        //�����������ͣ�Ĭ��ɾ��������
        if (rvalue["listType"].isNull())
        {
            sprintf(url, "http://%s:%d/delInnerVehicle", m_host.c_str(), m_port);
        }
        else
        {
            int listtype = rvalue["listType"].asInt();
            if (listtype == 0)
            {
                sprintf(url, "http://%s:%d/delInnerVehicle", m_host.c_str(), m_port);
            }
            else
            {
                sprintf(url, "http://%s:%d/delAlarmVehicle ", m_host.c_str(), m_port);
            }
        }
        string ansJson = "";
        int ret = curlPost(reqJson, string(url), ansJson);
        if (ret)
        {
            LOGI_MSV("HikTerminalPu::sendReqPlate() delInnerVehicle curlPost failed\n");
            return -1;
        }

        SHL_Json::Reader ansReader;
        SHL_Json::Value  ansValue;
        if (!ansReader.parse(ansJson, ansValue, false))
        {
            ERRE_MSV("SHL_Json parse error");
            return -1;
        }
        //���ش������, 0 - �ɹ����� 0 - �������
        int code = ansValue["code"].asInt();
        if (code != 0)
        {
            string errDesc = ansValue["errDesc"].asString();
            LOGI_MSV("HikTerminalPu::sendReqPlate() delInnerVehicle failed,reason��%s\n", errDesc.c_str());
            return -1;
        }
        return 0;
    }
        break;
    default:
        break;
    }
    return -1;
}

//��բ����
int
HikTerminalPu::sendReqGate(string jin, string jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    
    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }
    string laneName = rvalue["laneName"].asString();
    int gateCmd = rvalue["gateCmd"].asInt();

    //��ȡ���ó�����Ϣ
    string lanejson = "";
    if (sendReqLane(string(""), lanejson) != 0)
    {
        LOGI_MSV("HikTerminalPu::sendReqPlate() sendReqLane failed\n");
        return -1;
    }

    SHL_Json::Reader laneReader;
    SHL_Json::Value  laneValue;
    if (!laneReader.parse(lanejson, laneValue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }
    int code = laneValue["code"].asInt();
    if (code == 0)
    {
        //�Աȳ������ƣ���ȡ�������
        string laneCode = "";
        int total = laneValue["total"].asInt();
        if (total>0)
        {
            SHL_Json::Value roadWay = laneValue["roadWay"];
            for (int i = 0; i < total;++i)
            {
                string tname = roadWay[i]["name"].asString();
                string tlaneCode = roadWay[i]["laneCode"].asString();
                if (tname == laneName)
                {
                    laneCode = tlaneCode;
                    break;
                }
            }
        }
        if (laneCode.empty())
        {
            return -1;
        }

        SHL_Json::Value reqValue;
        SHL_Json::FastWriter reqWrite;
        reqValue["laneCode"] = laneCode;
        reqValue["operType"] = gateCmd;
        string reqJson = reqWrite.write(reqValue);
        //���͵�բ��������
        char url[512] = { 0 };
        sprintf(url, "http://%s:%d/operateBarrier", m_host.c_str(), m_port);
        
        string ansJson = "";
        int ret = curlPost(reqJson, string(url), ansJson);
        if (ret)
        {
            LOGI_MSV("HikTerminalPu::sendReqPlate() operateBarrier curlPost failed\n");
            return -1;
        }
        SHL_Json::Reader ansReader;
        SHL_Json::Value  ansValue;
        if (!ansReader.parse(ansJson, ansValue, false))
        {
            ERRE_MSV("SHL_Json parse error");
            return -1;
        }
        //���ش������, 0 - �ɹ����� 0 - �������
        int code = ansValue["code"].asInt();
        if (code != 0)
        {
            string errDesc = ansValue["errDesc"].asString();
            LOGI_MSV("HikTerminalPu::sendReqPlate() operateBarrier failed,reason��%s\n", errDesc.c_str());
            return -1;
        }
        return 0;
    }
    else
    {
        string errDesc = laneValue["errDesc"].asString();
        LOGI_MSV("HikTerminalPu::sendReqPlate() sendReqLane failed,reason��%s\n", errDesc.c_str());
        return -1;
    }
    return 0;
}

void HikTerminalPu::stop()
{
    if (!m_init)
        return;

    m_init = false;

    logout();

    Pu::stop();
}


int HikTerminalPu::login()
{
    //return 0;
    /* ���͹̶����� */
    m_mux.lock();
    if (NULL == m_pLiveCurl)
    {
        m_mux.unlock();
        return -1;
    }
    CURLcode res = CURLE_OK;

    SHL_Json::Value  rvalue;
    rvalue["Method"] = "keepalive";
    SHL_Json::FastWriter write;
    string jsonStr = write.write(rvalue);

    int s32Len = jsonStr.size();

    char url[512] = { 0 };
    sprintf(url, "http://%s:%d/getRoadWayPage", m_host.c_str(), m_port);

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_URL, url);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_POST, 1L);

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_HEADER, 1L);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Connection: Keep-Alive");
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_HTTPHEADER, headers);

    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_POSTFIELDSIZE, s32Len);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_READFUNCTION, read_data);

    string resp_data = "";
   
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_WRITEFUNCTION, write_data);
    CURL_EASY_SETOPT(m_pLiveCurl, CURLOPT_WRITEDATA, (void*)&resp_data);

    SendDataParam * sendData = new SendDataParam;
    if (NULL == sendData)
    {
        m_mux.unlock();
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
    * curl_easy_performΪ��������������������Բ������¼��ȴ���ز���
    */
    if (CURLE_OK == res)
    {
        long lResponse = 0;
        curl_easy_getinfo(m_pLiveCurl, CURLINFO_RESPONSE_CODE, &lResponse);//��ȡӦ����
        if (200 == lResponse) 
        {
            m_mux.unlock();
            m_login = true;
            return 0;
        }
    }
    m_mux.unlock();

    return -1;
}

void HikTerminalPu::logout()
{
    m_mux.lock();
    if (NULL != m_pLiveCurl)
    {
        curl_easy_cleanup(m_pLiveCurl);
        m_pLiveCurl = NULL;
    }
    m_mux.unlock();
}

void HikTerminalPu::onTimer()
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

int        
HikTerminalPu::onCustomCommand(string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }
    string Method = rvalue["Method"].asString();
    if (Method == "gateCtrl")
    {
        return sendReqGate(json, string(""));
    }
    else if (Method == "vehicleCtrl")
    {
        return sendReqPlate(json, string(""));
    }
    return 0;
}

