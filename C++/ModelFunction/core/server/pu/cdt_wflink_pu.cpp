#include "cdt_wflink_pu.h"
#include "sys.h"
#include "json/json.h"
#include "triggerlistener.h"

unsigned char u8CrcTab[256] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
    0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
    0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
    0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
    0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
    0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
    0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
    0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
    0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
    0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
    0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

unsigned char u8SynWord[6] = { 0xeb, 0x90, 0xeb, 0x90, 0xeb, 0x90 };

unsigned char CrcCheckCode(unsigned char *pData)
{
    unsigned char u8Data;
    int j, k;
    unsigned char ch = 0x00;

    for (j = 0; j < 5; j++)
    {
        k = pData[j] ^ ch;
        ch = u8CrcTab[k];
    }
    u8Data = ch ^ 0xff;

    return u8Data;
}

CdtWfLinkPu::CdtWfLinkPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_sock = sys->createSocket();

    m_protocol = PROTO_TYPE_CDT;
    m_port = port;
    m_lastLiveTime = 0;
}

CdtWfLinkPu::~CdtWfLinkPu()
{
    if (NULL != m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
    if (NULL != m_trdRcvEvent)
    {
        m_trdRcvEvent->stop();
        delete m_trdRcvEvent;
        m_trdRcvEvent = NULL;
    }
}

int
CdtWfLinkPu::start()
{
    m_init = true;

    if (Pu::start() < 0)
    {
        return -1;
    }

    return 0;
}

int
CdtWfLinkPu::login()
{
    if (m_login)
    {
        return -1;
    }

    uint64_t tmNow = sys->currentTimeMillis();
    if (tmNow - m_lastLiveTime <= 5000)
    {
        return -1;
    }

    char ipaddr[128] = { 0 };
    sprintf(ipaddr, "%s:%d", m_host.c_str(), m_port);
    Host host(ipaddr, 0);

    if (m_sock->open(host) < 0 ||
        m_sock->connect() < 0)
    {
        m_lastLiveTime = tmNow;

        m_sock->close();
        m_login = false;
        return -1;
    }

    m_login = true;
    m_first = true;

    m_lastLiveTime = tmNow;

	m_trdRcvEvent = new Thread;
	m_trdRcvEvent->setEntry(threadProc, this, T_RCVEVENT);
	m_trdRcvEvent->start();

    return 0;
}

void
CdtWfLinkPu::logout()
{
	if (NULL != m_trdRcvEvent)
	{
		delete m_trdRcvEvent;
		m_trdRcvEvent = NULL;
	}

    if (NULL != m_sock)
    {
        m_sock->close();
        m_login = false;
        m_first = false;
    }
}

void*
CdtWfLinkPu::threadProc(ThreadInfo *info)
{
    CdtWfLinkPu * pu = (CdtWfLinkPu *)(info->data);

    switch (info->id)
    {
    case T_RCVEVENT:
        pu->recvEvent();
        break;
    }
    return NULL;
}

void
CdtWfLinkPu::recvEvent()
{
    uint32_t len = 25;
	int      timeout = 0;

    while (m_trdRcvEvent->active())
    {
        memset(m_s8RecvBuff, 0, sizeof(m_s8RecvBuff));

        int ret = m_sock->select(true, false, 5000);
        if (ret > 0)
        {
			timeout = 0;
            if (m_sock->read(m_s8RecvBuff, 12) > 0)
            {
                int s32DataLen = 0;
                unsigned char *pFrame = (unsigned char *)m_s8RecvBuff;
                if (0 == memcmp(pFrame, u8SynWord, 6) &&
                    0 != memcmp(&pFrame[6], u8SynWord, 6))
                {
                    if (pFrame[11] == CrcCheckCode(&pFrame[6]))
                    {
                        /* 获取信息字帧长，不包括同步字与控制字 */
                        s32DataLen = 6 * pFrame[8];
                    }
                    else
                    {
                        break;
                    }
                }

                if (m_sock->read(m_s8RecvBuff + 12, s32DataLen) > 0)
                {
                    switch (pFrame[7])
                    {
                    case 0xee:
                        ProcessOperBefore(pFrame, s32DataLen + 12);
                        break;
                    case 0xef:
                        ProcessOpering(pFrame, s32DataLen + 12);
                        break;
                    case 0xea:
                        ProcessOperEnd(pFrame, s32DataLen + 12);
                        break;
                    case 0x7a:
                        TimeSynchronize(pFrame, s32DataLen + 12);
                        break;
                    case 0xf4:
                        //if (m_bufYx+5000 < m_pYxWrite+s32DataLen)
                        //{
                        // 	  m_pYxWrite = m_bufYx;
                        // }
                        // memcpy(m_pYxWrite, pFrame, s32DataLen);
                        // m_pYxWrite += DataLength;
                        // 					
                        // m_bYxData = TRUE;
                        // m_dwLastYxTime = GetTickCount();
                        break;
                    case 0xa0://任务名
                        ProcessTaskName(pFrame, s32DataLen + 12);
                        break;
                    case 0xb0://带录像的
                        ProcessOperBefore(pFrame, s32DataLen + 12, LINK_TYPE_RECORD);
                        break;
                    case 0xc0:
                        ProcessOpering(pFrame, s32DataLen + 12, LINK_TYPE_RECORD);
                        break;
                    case 0xd0:
                        ProcessOperEnd(pFrame, s32DataLen + 12, LINK_TYPE_RECORD);
                        break;
                    }
                }
            }
			else
			{
				break;
			}
        }
		else
		{
			timeout++;
			if (timeout >= 2)
			{
                timeout = 0;
				break;
			}
		}

        sys->sleep(len);
    }

	logout();
}

void 
CdtWfLinkPu::ProcessTaskName(unsigned char* pFrame, uint32_t DataLength)
{
    int iInfoCount = *(pFrame + 8);//信息字数
    char buf[256] = { 0 };
    for (int i = 0; i < iInfoCount; i++)
    {
        buf[0 + i * 4] = *(pFrame + 12 + i * 6 + 1);
        buf[1 + i * 4] = *(pFrame + 12 + i * 6 + 2);
        buf[2 + i * 4] = *(pFrame + 12 + i * 6 + 3);
        buf[3 + i * 4] = *(pFrame + 12 + i * 6 + 4);
    }
    string strName(buf);

    //保存任务名 
    uint16_t wTaskId = SaveNewTask(strName);
    //回复任务ID
    unsigned char sendBuf[18] = { 0 };
    memcpy(sendBuf, pFrame, 12);
    sendBuf[7] = 0xa0;
    sendBuf[8] = 1;
    sendBuf[11] = CrcCheckCode(sendBuf + 6);
    sendBuf[12] = 0xa1;
    sendBuf[13] = (unsigned char)(wTaskId >> 8);
    sendBuf[14] = (unsigned char)wTaskId;
    sendBuf[17] = CrcCheckCode(sendBuf + 12);

    m_sock->write(sendBuf, 18);
}

uint16_t
CdtWfLinkPu::SaveNewTask(string strTaskName)
{
    uint16_t u16NowId = 1;
    list<TASK_INFO>::iterator iter;
    int iSize = m_TaskList.size();
    if (iSize > 0)
    {
        TASK_INFO &endItem = m_TaskList.back();
        u16NowId = endItem.u32Guid + 1;
    }
    TASK_INFO newItem;
    newItem.u32Guid = u16NowId;
    newItem.strName = strTaskName;

    //获取时间
    time_t ltime;
    struct tm *p_tm;
    ltime = time(NULL);
    p_tm = localtime(&ltime);

    newItem.taskTime.year = p_tm->tm_year + 1900;
    newItem.taskTime.month = p_tm->tm_mon + 1;
    newItem.taskTime.day = p_tm->tm_mday;
    newItem.taskTime.hour = p_tm->tm_hour;
    newItem.taskTime.minute = p_tm->tm_min;
    newItem.taskTime.second = p_tm->tm_sec;

    m_TaskList.push_back(newItem);
    return u16NowId;
}

void 
CdtWfLinkPu::TimeSynchronize(unsigned char* pFrame, uint32_t DataLength)
{
    uint16_t u16MSecond = 0;
    uint16_t u16Second = -1;
    uint16_t u16Minute = -1;
    uint16_t u16Hour = -1;
    uint16_t u16Day = -1;
    uint16_t u16Month = -1;
    uint16_t u16Year = -1;

    unsigned char* pInfo = pFrame + 12;
    while (pInfo < pFrame + DataLength)
    {
        switch (*(pInfo + 0))
        {
        case 0xee:
            u16Second = *(pInfo + 3);
            u16Minute = *(pInfo + 4);
            pInfo += 6;
            continue;
        case 0xef:
            u16Hour = *(pInfo + 1);
            u16Day = *(pInfo + 2);
            u16Month = *(pInfo + 3);
            u16Year = *(pInfo + 4);
            pInfo += 6;
            continue;
        default:
            pInfo += 6;//下一个信息字
            continue;
        }
#ifdef _WIN32
        SYSTEMTIME time;
        ZeroMemory(&time, sizeof(SYSTEMTIME));

        time.wYear = u16Year;
        time.wMonth = u16Month;
        time.wDay = u16Day;
        time.wHour = u16Hour;
        time.wMinute = u16Minute;
        time.wSecond = u16Second;

        //::SetLocalTime(&time);
#else
        struct tm tm_s;
        tm_s.tm_year = u16Year - 1900;
        tm_s.tm_mon = u16Month;
        tm_s.tm_mday = u16Day;
        tm_s.tm_hour = u16Hour;
        tm_s.tm_min = u16Minute;
        tm_s.tm_sec = u16Second;

        //time_t tSecond = mktime(&tm_s);		
        //struct timeval tv = { tSecond, 0 };	
        //settimeofday(&tv, NULL);
#endif
        
    }
}

void 
CdtWfLinkPu::ProcessOperBefore(unsigned char* pFrame, uint32_t DataLength, uint32_t iType /* =LINK_TYPE_NORECORD */)
{
    int iInfoCount = *(pFrame + 8);//信息字数
    int iDevCount = iInfoCount / 3;//每个设备有3个信息字
    unsigned char* pInfo = pFrame + 12;
    m_u16NowTaskId = 0;
    m_strNowTaskName = "";
    //处理录像功能的代码 liuwei 20130717
    if (iType == LINK_TYPE_RECORD)
    {
        if (*(pInfo + 0) == 0xff)
        {
            m_u16NowTaskId = pInfo[2] | (pInfo[1] << 8);
            bool bFind = false;
            if (m_u16NowTaskId != 0)//Id号从1开始，如果是0，则自动新生成一个事件
            {
                list<TASK_INFO>::iterator iter;
                for (iter = m_TaskList.begin(); iter != m_TaskList.end(); iter++)
                {
                    if (iter->u32Guid == m_u16NowTaskId)
                    {
                        m_strNowTaskName = iter->strName;
                        bFind = true;
                        break;
                    }
                }
            }
            if (!bFind)//如果没找到相应ID的任务，则新增一个
            {
                time_t ltime;
                struct tm *p_tm;

                ltime = time(NULL);
                p_tm = localtime(&ltime);

                char strTemp[1024] = { 0 };
                sprintf(strTemp, "%d年%02d月%02d日%02d时%02d分%02d秒", p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
                    p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
                m_strNowTaskName = strTemp;
                TASK_INFO newItem;
                newItem.u32Guid = m_u16NowTaskId;
                newItem.strName = m_strNowTaskName;
                newItem.taskTime.year = m_NowTaskTime.year = p_tm->tm_year + 1900;
                newItem.taskTime.month = m_NowTaskTime.month = p_tm->tm_mon + 1;
                newItem.taskTime.day = m_NowTaskTime.day = p_tm->tm_mday;
                newItem.taskTime.hour = m_NowTaskTime.hour = p_tm->tm_hour;
                newItem.taskTime.minute = m_NowTaskTime.minute = p_tm->tm_min;
                newItem.taskTime.second = m_NowTaskTime.second = p_tm->tm_sec;

                m_TaskList.push_back(newItem);
            }
            pInfo += 6;//下一个信息字
        }
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = name();
    value["deviceHost"] = host();
    value["devicePort"] = port();
    value["addrcode"]   = addrcode();
    value["level"]      = 1;

    value["operType"] = WFLINK_TASK_START;
    value["devNum"]   = iDevCount;
    value["taskId"]   = m_u16NowTaskId;
    value["taskName"] = m_strNowTaskName;

    uint8_t devIp[4] = { 0 };
    int  chnNo = 0;
    int  presetNo = 0;

    for (int i = 0; i < iDevCount; i++)
    {
        SHL_Json::Value item;;
        item["type"] = TriggerListener::TR_PUSH_LINKAGE;

        switch (*(pInfo + 0))
        {
        case 0xeb: //联动DVR的IP
            devIp[0] = pInfo[1];
            devIp[1] = pInfo[2];
            devIp[2] = pInfo[3];
            devIp[3] = pInfo[4];

            pInfo += 6;//下一个信息字
        case 0xec://得到摄像机通道号，预置点号
            if (pInfo[1])
                chnNo = pInfo[2];
            if (pInfo[3])
                presetNo = pInfo[4];
            pInfo += 6;//下一个信息字
        case 0xed://得到矩阵输出端口,无效，不处理
            pInfo += 6;//下一个信息字
        default:
            break;
        }

        char strip[128] = { 0 };
        sprintf(strip, "%d.%d.%d.%d", devIp[0], devIp[1], devIp[2], devIp[3]);
        item["devIp"] = strip;

        item["channelNo"] = chnNo;
        item["presetNo"]  = presetNo;
        item["status"]    = 0;

        value["alarms"].append(item);
    }

    string json = writer.write(value);
    notifyAlarm(json, NULL, 0);
    writeAlarmLog(json);
}

void 
CdtWfLinkPu::ProcessOpering(unsigned char* pFrame, uint32_t DataLength, uint32_t iType /* =LINK_TYPE_NORECORD */)
{
    unsigned char* pInfo = pFrame + 12;
    m_u16NowTaskId = 0;
    m_strNowTaskName = "";
    if (iType == LINK_TYPE_RECORD)
    {
        if (*(pInfo + 0) == 0xff)
        {
            m_u16NowTaskId = pInfo[2] | (pInfo[1] << 8);
            bool bFind = false;
            if (m_u16NowTaskId != 0)//Id号从1开始，如果是0，则自动新生成一个事件
            {
                list<TASK_INFO>::iterator iter;
                for (iter = m_TaskList.begin(); iter != m_TaskList.end(); iter++)
                {
                    if (iter->u32Guid == m_u16NowTaskId)
                    {
                        m_strNowTaskName = iter->strName;
                        bFind = true;
                        break;
                    }
                }
            }

            if (!bFind)//如果没找到相应ID的任务，则新增一个
            {
                time_t ltime;
                struct tm *p_tm;

                ltime = time(NULL);
                p_tm = localtime(&ltime);

                char strTemp[1024] = { 0 };
                sprintf(strTemp, "%d年%02d月%02d日%02d时%02d分%02d秒", p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
                    p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
                m_strNowTaskName = strTemp;
                TASK_INFO newItem;
                newItem.u32Guid = m_u16NowTaskId;
                newItem.strName = m_strNowTaskName;

                newItem.taskTime.year = m_NowTaskTime.year = p_tm->tm_year + 1900;
                newItem.taskTime.month = m_NowTaskTime.month = p_tm->tm_mon + 1;
                newItem.taskTime.day = m_NowTaskTime.day = p_tm->tm_mday;
                newItem.taskTime.hour = m_NowTaskTime.hour = p_tm->tm_hour;
                newItem.taskTime.minute = m_NowTaskTime.minute = p_tm->tm_min;
                newItem.taskTime.second = m_NowTaskTime.second = p_tm->tm_sec;
                m_TaskList.push_back(newItem);
            }
        }
        pInfo += 6;//下一个信息字
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = name();
    value["deviceHost"] = host();
    value["devicePort"] = port();
    value["addrcode"]   = addrcode();
    value["level"]      = 1;

    if (iType == LINK_TYPE_RECORD)
    {
        value["operType"] = WFLINK_TASK_OPERING;
    }
    else
    {
        value["operType"] = WFLINK_SIMU_OPERING;
    }
    
    value["devNum"]   = 1;
    value["taskId"]   = m_u16NowTaskId;
    value["taskName"] = m_strNowTaskName;

    uint8_t devIp[4] = { 0 };
    int  chnNo = 0;
    int  presetNo = 0;

    SHL_Json::Value item;;
    item["type"] = TriggerListener::TR_PUSH_LINKAGE;

    while (pInfo < pFrame + DataLength)
    {    
        //以下为正常处理过程
        switch (*(pInfo + 0))
        {
        case 0xeb: //通过IP得到服务器ID
            devIp[0] = pInfo[1];
            devIp[1] = pInfo[2];
            devIp[2] = pInfo[3];
            devIp[3] = pInfo[4];

            pInfo += 6;//下一个信息字
            continue;
        case 0xec://得到摄像头指针和预置点号
            if (pInfo[1])
                chnNo = pInfo[2];
            if (pInfo[3])
                presetNo = pInfo[4];

            pInfo += 6;//下一个信息字
            continue;
        case 0xed://得到矩阵输出端口			
            pInfo += 6;//下一个信息字	
            continue;
        case 0xea://模拟屏巡视
            value["operType"] = WFLINK_SIMU_SCOUT;
            pInfo += 6;
            continue;
        case  0xee://五防闭锁
            pInfo += 6;
            continue;
        default:
            pInfo += 6;//下一个信息字
            continue;
        }	
    }

    char strip[128] = { 0 };
    sprintf(strip, "%d.%d.%d.%d", devIp[0], devIp[1], devIp[2], devIp[3]);
    item["devIp"] = strip;
    item["channelNo"] = chnNo;
    item["presetNo"] = presetNo;
    item["status"] = 0;

    value["alarms"].append(item);

    string json = writer.write(value);
    notifyAlarm(json, NULL, 0);
    writeAlarmLog(json);
}

void 
CdtWfLinkPu::ProcessOperEnd(unsigned char* pFrame, uint32_t DataLength, uint32_t iType /* =LINK_TYPE_NORECORD */)
{
    unsigned char* pInfo = pFrame + 12;
    //处理录像功能的代码 liuwei 20130717
    if (iType == LINK_TYPE_RECORD)
    {
        if (*(pInfo + 0) == 0xff)
        {
            m_u16NowTaskId = pInfo[2] | (pInfo[1] << 8);
            list<TASK_INFO>::iterator iter;
            for (iter = m_TaskList.begin(); iter != m_TaskList.end(); iter++)
            {
                if (iter->u32Guid == m_u16NowTaskId)
                {
                    m_TaskList.erase(iter);
                    break;
                }
            }
        }
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"]    = "1.0";
    value["time"]       = sys->currentTime();
    value["deviceName"] = m_name;
    value["deviceHost"] = m_host;
    value["devicePort"] = m_port;
    value["addrcode"]   = m_addrcode;
    value["level"]      = 1;
    value["operType"]   = WFLINK_TASK_END;

    SHL_Json::Value item;;
    item["type"] = TriggerListener::TR_PUSH_LINKAGE;
    
    item["TaskId"]    = m_u16NowTaskId;
    item["TaskName"]  = m_strNowTaskName;
    item["DeviceNum"] = 1;
    value["alarms"].append(item);

    string json = writer.write(value);

    writeAlarmLog(json);
    notifyAlarm(json, NULL, 0);
    
    m_u16NowTaskId = 0;
    m_strNowTaskName = "";
}
