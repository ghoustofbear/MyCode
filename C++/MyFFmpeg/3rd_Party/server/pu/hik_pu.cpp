#include "hik_pu.h"
#include "tls_error.h"
#include "triggerlistener.h"
#include "bitoperation.h"
#include "global_str_def.h"
#include "system.h"
#include "j2x.h"
#include "stream.h"
#include "cfg_device.h"
#include "hik_terminal_pu.h"
#include "base64_ex.h"
#include "utility.h"
#include <fstream>
#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif
#ifdef _WIN32
#include <direct.h>
#include<io.h>
#else
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<cstddef>
#endif
#define SAFE_CLOSE(fp)	{fclose(fp);fp = NULL;}
//时间解析宏定义
#define GET_YEAR_HIKPU(_time_)      (((_time_)>>26) + 2000) 
#define GET_MONTH_HIKPU(_time_)     (((_time_)>>22) & 15)
#define GET_DAY_HIKPU(_time_)       (((_time_)>>17) & 31)
#define GET_HOUR_HIKPU(_time_)      (((_time_)>>12) & 31) 
#define GET_MINUTE_HIKPU(_time_)    (((_time_)>>6)  & 63)
#define GET_SECOND_HIKPU(_time_)    (((_time_)>>0)  & 63)

#define ISAPI_OUT_LEN	3 * 1024 * 1024
#define ISAPI_STATUS_LEN  8*1024


const string       STRVEHICLE_C = "setVehicleCallback_c";
const string       STRTEMP_C = "getTempratureCallback_c";
const string       STRTEMP_C1 = "getTempratureCallback_c1";

static string      strtemp;
static string resps = "";
static string resp = "";

WORD DecToHex(WORD wDec);
DWORD HexToDecMa1(DWORD wHex);

void CBK login_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser)
{
    HikPu *pu = (HikPu *)pUser;

    if (1 == result)
    {
        LOGI_MSV("Device: %s login success! user ID: %d", pu->ip().c_str(), userId);

        pu->m_login = true;
        pu->writeDevLog(STR_MSV_ON_LINE);
    }

    //LOGI_MSV("login reslut: %d, %s", result, pu->ip().c_str());
}
int HikPu::controlDoorGate(int ctrlValue){
	if (!m_login)
	{
		return -1;
	}
	NET_DVR_CONTROL_GATEWAY para = {0};
	para.dwSize = sizeof(NET_DVR_CONTROL_GATEWAY);
	para.wLockID = 1;
	para.byCommand = ctrlValue;//0：close,1:open,2:normally open,3:recover
	para.dwGatewayIndex = 1;
	para.byControlType = 1;
	strcpy((char*)para.byControlSrc, "11");
	bool flag = NET_DVR_RemoteControl(m_userId, NET_DVR_REMOTECONTROL_GATEWAY, &para, sizeof(para));
	return (flag) ? 0 : -1;
}

void CBK callData_cb(DWORD dateType, void* data, DWORD size, void* userData)
{
    if (!userData)
    {
        return;
    }

    HikPu* pu = (HikPu *)userData;

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    SHL_Json::Value item;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pu->name();
    value["deviceHost"] = pu->host();
    value["devicePort"] = pu->port();
    value["addrcode"] = pu->addrcode();
    value["level"] = 1;


    NET_DVR_VIDEO_CALL_PARAM callParam = { 0 };
    uint32_t status = 0;
    if (dateType == NET_SDK_CALLBACK_TYPE_STATUS)
    {
        status = *(uint32_t*)data;
        LOGE_MSV("NET_SDK_CALLBACK_TYPE_STATUS %d\n", status);
        if (NET_SDK_CALLBACK_STATUS_FAILED == status)
        {
            pu->m_callStatus = 2;
        }
    }
    else if (dateType == NET_SDK_CALLBACK_TYPE_DATA)
    {
        memcpy(&callParam, data, size);
        if (5 == callParam.dwCmdType)
        {
            pu->m_callStatus = 0;
        }

        printf("\n\n\n\t\tcall call call call call call  cmd:%d\n\n\n", callParam.dwCmdType);
        item["type"] = TriggerListener::TR_DEV_CALL;
        item["channelNo"] = 1;
        //item["cmd"] = (int)callParam.dwCmdType;
		if (0==callParam.dwCmdType)
		{
			item["cmd"] = 0;
		}
		else if (5==callParam.dwCmdType)
		{
			item["cmd"] = 1;
		}
        value["alarms"].append(item);
        string json = writer.write(value);
        pu->notifyAlarm(json, NULL, 0);
    }

}

void CBK
excep_cb(DWORD type, LONG userId, LONG handle, void *userData)
{
    HikPu *pu = NULL;
    {
        AutoLock locker(HikPu::m_userMap._mtx);
        pu = (HikPu *)HikPu::m_userMap[userId];
    }
    if (!pu)
    {
        return;
    }

    LOGI_MSV("exception occur type: 0x%x, ip: %s, userId: %d", type, pu->ip().c_str(), userId);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pu->name();
    value["deviceHost"] = pu->host();
    value["devicePort"] = pu->port();

    if (EXCEPTION_EXCHANGE == type)
    {
        /* Make sure write log once time */
        if (pu->m_login)
        {
            pu->m_login = false;

            pu->writeDevLog(STR_MSV_OFF_LINE);

            SHL_Json::Value item;
            item["type"] = TriggerListener::TR_DEV_STATUS;
            item["status"] = 0;
            item["name"] = pu->name();
            item["code"] = pu->addrcode();
            value["alarms"].append(item);
        }
    }
    else if ((RESUME_EXCHANGE == type) ||
        (RELOGIN_SUCCESS == type))
    {
        if (pu->m_login)
        {
            return;
        }
        pu->m_login = true;

        pu->writeDevLog(STR_MSV_RE_ON_LINE);

        SHL_Json::Value item;
        item["type"] = TriggerListener::TR_DEV_STATUS;
        item["status"] = 1;
        item["name"] = pu->name();
        item["code"] = pu->addrcode();
        value["alarms"].append(item);
    }
    else
    {
        //pu->writeDevLog(STR_MSV_OTHER_ERR);
        return;
    }

    string json = writer.write(value);
    pu->notifyAlarm(json, NULL, 0);
}

void CBK
alarm_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData)
{

    HikPu *pu = NULL;
    {
        AutoLock locker(HikPu::m_userMap._mtx);
        pu = (HikPu *)HikPu::m_userMap[alarmer->lUserID];
    }
    if (!pu)
    {
        return;
    }

    int channum = MAX_CHANNUM;
    int disnums = MAX_DISKNUM;
    int inputNo = 0;
    int alarmType = 0;
    int i = 0;


    int channels[MAX_CHANNUM_V30] = { 0 };
    int disNums[MAX_DISKNUM_V30] = { 0 };

    /* LOGW_MSV("alarm type: %0x", cmd); */

    if (cmd == COMM_ALARM)
    {
        NET_DVR_ALARMINFO info;
        memcpy(&info, alarmInfo, sizeof(NET_DVR_ALARMINFO));

        alarmType = info.dwAlarmType;
        inputNo = info.dwAlarmInputNumber;
        channum = MAX_CHANNUM;
        disnums = MAX_DISKNUM;

        for (i = 0; i < MAX_CHANNUM; i++)
        {
            channels[i] = info.dwChannel[i];
        }

        for (i = 0; i < MAX_DISKNUM; i++)
        {
            disNums[i] = info.dwDiskNumber[i];
        }
    }
    else if (cmd == COMM_ALARM_V30)
    {
        NET_DVR_ALARMINFO_V30 alarmInfoV30;
        memcpy(&alarmInfoV30, alarmInfo, sizeof(NET_DVR_ALARMINFO_V30));
        alarmType = alarmInfoV30.dwAlarmType;
        inputNo = alarmInfoV30.dwAlarmInputNumber;
        channum = MAX_CHANNUM_V30;
        disnums = MAX_DISKNUM_V30;

        for (i = 0; i < MAX_CHANNUM_V30; i++)
        {
            channels[i] = alarmInfoV30.byChannel[i];
        }

        for (i = 0; i < MAX_DISKNUM_V30; i++)
        {
            disNums[i] = alarmInfoV30.byDiskNumber[i];
        }
    }
    else if (cmd == COMM_UPLOAD_PLATE_RESULT)
    {
        alarmType = 0xFFFF;
    }
    else if (cmd == COMM_ITS_PLATE_RESULT)
    {
        alarmType = 0xFFFE;
    }
    else if (cmd == COMM_THERMOMETRY_ALARM)
    {
        alarmType = 0xFFFD;
    }
    else if (cmd == COMM_ALARM_RULE)
    {
        alarmType = 0xFFFC;    /**< ���ܷ��� */
    }
    else if (cmd == COMM_SNAP_MATCH_ALARM)
    {
        alarmType = 0xFFFB;    /**< ����ʶ�� */
    }
    else if (cmd == COMM_ALARM_PDC)
    {
        alarmType = 0xFFFA;    /**< 人数统计 */
    }
    else if (cmd == COMM_ITS_GATE_ALARMINFO)
    {
        alarmType = 0xFFF9;     /**< 出入口控制机数据（道闸 */
    }
    else if (cmd == COMM_VCA_ALARM)
    {
        /*
        Content-Type: multipart/form-data; boundary=MIME_boundary
        --MIME_boundary
        Content-Type: application/json
        Content-Length: 813

        {
	        "ipAddress":	"25.38.82.220",
	        "portNo":	8000,
	        "protocolType":	"HTTP",
	        "macAddress":	"98:df:82:35:c9:2e",
	        "dateTime":	"2021-08-12T16:28:30+08:00",
	        "channelID":	1,
	        "activePostCount":	1,
	        "eventType":	"safetyHelmetDetection",
	        "eventState":	"active",
	        "eventDescription":	"Safety Helmet Detection",
	        "channelName":	"Camera 01",
	        "Target":	[{
			        "recognitionType":	"safetyHelmet",
			        "TargetInfo":	{
				        "recognition":	"safetyHelmet",
				        "dataSource":	"trigger"
			        },
			        "rect":	[{
					        "height":	0.106361,
					        "width":	0.046912,
					        "x":	0.581704,
					        "y":	0.784150
				        }],
			        "contentID":	"safetyHelmet1_image",
			        "pId":	"2021081216283019100GVOfbpEXRDrIwMAQ2BCD8tiYCBl7fxr49QtE4q6urM2K"
		        }],
	        "contentID":	"background_image",
	        "pId":	"2021081216283019200K6V968kz0Sf3jzAdSMZjCA2j6xO6ApblniKcYZCNKNQw"
        }
        --MIME_boundary
        Content-Disposition: form-data; name="2021081216283019200K6V968kz0Sf3jzAdSMZjCA2j6xO6ApblniKcYZCNKNQw"; filename="background_image.jpg";
        Content-Type: image/jpeg
        Content-Length: 262461
        Content-ID: background_image      
        */

        alarmType = 0xFFF8;
    }
    else
    {
        return;
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pu->name();
    value["deviceHost"] = pu->host();
    value["devicePort"] = pu->port();
    value["addrcode"] = pu->addrcode();
    value["level"] = 1;

    bool bsave = false;

    switch (alarmType)
    {
    case 0:
    {
        char buf[100] = { 0 };
        sprintf(buf, "%d-%d", TriggerListener::TR_DEV_IO, inputNo);
        time_t tt = time(NULL);
        if (!pu->needProcess(buf, tt))
        {
            break;
        }

        SHL_Json::Value item;;
        item["type"] = TriggerListener::TR_DEV_IO;
        item["channelNo"] = inputNo;
        item["status"] = 1;
        item["gucode"] = pu->gucode(inputNo);
        item["name"] = pu->guname(inputNo);

        value["alarms"].append(item);
        bsave = true;
        break;
    }

    case 3:
        //for (i = 0; i < channum; i++)
        //{
        //    SHL_Json::Value item;;
        //    if (channels[i] == 1)
        //    {
        //        char buf[100] = { 0 };
        //        sprintf(buf, "%d-%d", TriggerListener::TR_DEV_MD, i);
        //        time_t tt = time(NULL);
        //        if (!pu->needProcess(buf, tt))
        //        {
        //            continue;
        //        }

        //        item["type"] = TriggerListener::TR_DEV_MD;
        //        item["channelNo"] = i;
        //        item["status"] = 1;
        //        item["gucode"] = pu->gucode(i);
        //        item["name"] = pu->guname(i);

        //        value["alarms"].append(item);
        //        bsave = true;
        //    }
        //}
        break;

    case 6:
        for (i = 0; i < channum; i++)
        {
            SHL_Json::Value item;;
            if (channels[i] == 1)
            {
                char buf[100] = { 0 };
                sprintf(buf, "%d-%d", TriggerListener::TR_DEV_VCOVER, i);
                time_t tt = time(NULL);
                if (!pu->needProcess(buf, tt))
                {
                    continue;
                }

                item["type"] = TriggerListener::TR_DEV_VCOVER;
                item["channelNo"] = i;
                item["status"] = 1;
                item["gucode"] = pu->gucode(i);
                item["name"] = pu->guname(i);

                value["alarms"].append(item);
                bsave = true;
            }
        }
        break;

    case 1:
    case 4:
    case 5:
    case 10:
        for (i = 0; i < disnums; i++)
        {
            SHL_Json::Value item;;
            if (disNums[i] == 1)
            {
                char buf[100] = { 0 };
                sprintf(buf, "%d-%d", TriggerListener::TR_DEV_HDD, i);
                time_t tt = time(NULL);
                if (!pu->needProcess(buf, tt))
                {
                    continue;
                }
                item["type"] = TriggerListener::TR_DEV_HDD;
                item["channelNo"] = i;
                item["status"] = 1;
                item["gucode"] = pu->gucode(i);
                item["name"] = pu->guname(i);

                value["alarms"].append(item);
                bsave = true;
            }
        }
        break;

    case 2:
    case 7:
    case 9:
        for (i = 0; i < channum; i++)
        {
            SHL_Json::Value item;;
            if (channels[i] == 1)
            {
                char buf[100] = { 0 };
                sprintf(buf, "%d-%d", TriggerListener::TR_DEV_VLOSS, i);
                time_t tt = time(NULL);
                if (!pu->needProcess(buf, tt))
                {
                    continue;
                }

                item["type"] = TriggerListener::TR_DEV_VLOSS;
                item["channelNo"] = i;
                item["status"] = 1;
                item["gucode"] = pu->gucode(i);
                item["name"] = pu->guname(i);

                value["alarms"].append(item);
                bsave = true;
            }
        }
        break;

    case 0xFFFE:
    {
        NET_ITS_PLATE_RESULT alarmPlateInfo;
        memcpy(&alarmPlateInfo, alarmInfo, sizeof(alarmPlateInfo));


        SHL_Json::Value item;;
        item["type"] = TriggerListener::TR_AI_PLATE;
        item["channelNo"] = inputNo;
        item["status"] = 1;
        item["gucode"] = pu->gucode(inputNo);
        item["name"] = pu->guname(inputNo);
        item["preset"] = 1;

        int yyyy = ((alarmPlateInfo.struPicInfo[0].byAbsTime[0] - '0') * 1000 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[1] - '0') * 100 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[2] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[3] - '0'));

        int MM = ((alarmPlateInfo.struPicInfo[0].byAbsTime[4] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[5] - '0'));

        int dd = ((alarmPlateInfo.struPicInfo[0].byAbsTime[6] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[7] - '0'));
        int hh = ((alarmPlateInfo.struPicInfo[0].byAbsTime[8] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[9] - '0'));
        int mm = ((alarmPlateInfo.struPicInfo[0].byAbsTime[10] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[11] - '0'));
        int ss = ((alarmPlateInfo.struPicInfo[0].byAbsTime[12] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[13] - '0'));

        int ms = ((alarmPlateInfo.struPicInfo[0].byAbsTime[14] - '0') * 100 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[15] - '0') * 10 + \
            (alarmPlateInfo.struPicInfo[0].byAbsTime[16] - '0'));

        if (TriggerListener::PLATE_HIK_NORMAL == pu->m_manuPlateType)
        {
            yyyy += 1900;
            MM += 1;
        }

        char        szTime[40];
        snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
            yyyy, MM, dd, hh, mm, ss, ms);
        string timeFmt = szTime;
        value["time"] = timeFmt;

        if (alarmPlateInfo.struPlateInfo.byColor == VCA_BLUE_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BLUE;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_YELLOW_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_YELLOW;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_WHITE_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_WHITE;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_BLACK_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BLACK;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_GREEN_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_GREEN;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_BKAIR_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BKAIR;
        }
        else
        {
            item["plateColor"] = TriggerListener::AI_COLOR_UNKNOWN;
        }

        item["plateType"] = alarmPlateInfo.struPlateInfo.byPlateType;



        if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BLUE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BLUE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_YELLOW)
        {
            item["carColor"] = TriggerListener::AI_COLOR_YELLOW;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_WHITE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_WHITE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BLACK)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BLACK;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_GREEN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_GREEN;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_RED)
        {
            item["carColor"] = TriggerListener::AI_COLOR_RED;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_GRAY)
        {
            item["carColor"] = TriggerListener::AI_COLOR_GRAY;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_DARKBLUE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_DARKBLUE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BROWN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BROWN;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_PINK)
        {
            item["carColor"] = TriggerListener::AI_COLOR_PINK;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_PURPLE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_PURPLE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_DARKGRAY)
        {
            item["carColor"] = TriggerListener::AI_COLOR_DARKGRAY;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_CYAN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_CYAN;
        }
        else
        {
            item["carColor"] = TriggerListener::AI_COLOR_UNKNOWN;
        }

        uint8_t carType = 0;
        if (VTR_RESULT_CAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SMALLCAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_MICROCAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SUV_MPV == alarmPlateInfo.byVehicleType)
        {
            carType = 1;
        }
        else if (VTR_RESULT_TRUCK == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SMALLTRUCK == alarmPlateInfo.byVehicleType)
        {
            carType = 2;
        }
        else if (VTR_RESULT_MINIBUS == alarmPlateInfo.byVehicleType)
        {
            carType = 3;
        }
        item["carType"] = carType;
        item["carDirect"] = alarmPlateInfo.byCarDirectionType;
        item["plateLicense"] = sys->ascii2utf8(alarmPlateInfo.struPlateInfo.sLicense);
        item["manuPlateType"] = pu->m_manuPlateType;

        if (TriggerListener::PLATE_HIK_STARNDARD == pu->m_manuPlateType && systm->platePath() != "")
        {

            char s8PlateInfo[512] = { 0 };
            snprintf(s8PlateInfo, 512, "%04d%02d%02d%02d%02d%02d%03d_%s.jpg",
                yyyy, MM, dd, hh, mm, ss, ms, alarmPlateInfo.struPlateInfo.sLicense);

            string tmpplatePath = systm->platePath();
#ifdef WIN32
            std::string   filename = sys->utf2ascii(tmpplatePath.c_str()) + "/" + s8PlateInfo;
#else
            std::string   filename = tmpplatePath + "/" + s8PlateInfo;
#endif
            FileStream         fileWrite;
            fileWrite.openWriteReplace(sys->ascii2utf8(filename.c_str()).c_str());
            if (fileWrite.isopen())
            {
                fileWrite.write(alarmPlateInfo.struPicInfo[0].pBuffer, alarmPlateInfo.struPicInfo[0].dwDataLen);
                fileWrite.close();
            }
            item["picName"] = sys->ascii2utf8(filename);
        }

        value["alarms"].append(item);

        bsave = true;

    }
        break;
    case 0xFFFF:
    {
        NET_DVR_PLATE_RESULT alarmPlateInfo;
        memcpy(&alarmPlateInfo, alarmInfo, sizeof(alarmPlateInfo));


        SHL_Json::Value item;;
        item["type"] = TriggerListener::TR_AI_PLATE;
        item["channelNo"] = inputNo;
        item["status"] = 1;
        item["gucode"] = pu->gucode(inputNo);
        item["name"] = pu->guname(inputNo);
        item["preset"] = 1;

        int yyyy = ((alarmPlateInfo.byAbsTime[0] - '0') * 1000 + \
            (alarmPlateInfo.byAbsTime[1] - '0') * 100 + \
            (alarmPlateInfo.byAbsTime[2] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[3] - '0'));

        int MM = ((alarmPlateInfo.byAbsTime[4] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[5] - '0'));

        int dd = ((alarmPlateInfo.byAbsTime[6] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[7] - '0'));
        int hh = ((alarmPlateInfo.byAbsTime[8] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[9] - '0'));
        int mm = ((alarmPlateInfo.byAbsTime[10] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[11] - '0'));
        int ss = ((alarmPlateInfo.byAbsTime[12] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[13] - '0'));

        int ms = ((alarmPlateInfo.byAbsTime[14] - '0') * 100 + \
            (alarmPlateInfo.byAbsTime[15] - '0') * 10 + \
            (alarmPlateInfo.byAbsTime[16] - '0'));


        if (TriggerListener::PLATE_HIK_NORMAL == pu->m_manuPlateType)
        {
            yyyy += 1900;
            MM += 1;
        }

        char        szTime[40];
        snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
            yyyy,MM,dd,hh,mm,ss,ms);
        string timeFmt = szTime;
        value["time"] = timeFmt;

        if (alarmPlateInfo.struPlateInfo.byColor == VCA_BLUE_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BLUE;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_YELLOW_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_YELLOW;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_WHITE_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_WHITE;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_BLACK_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BLACK;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_GREEN_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_GREEN;
        }
        else if (alarmPlateInfo.struPlateInfo.byColor == VCA_BKAIR_PLATE)
        {
            item["plateColor"] = TriggerListener::AI_COLOR_BKAIR;
        }
        else
        {
            item["plateColor"] = TriggerListener::AI_COLOR_UNKNOWN;
        }

        item["plateType"] = alarmPlateInfo.struPlateInfo.byPlateType;


        if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_WHITE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_WHITE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_SILVER)
        {
            item["carColor"] = TriggerListener::AI_COLOR_SILVER;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_GRAY)
        {
            item["carColor"] = TriggerListener::AI_COLOR_GRAY;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BLACK)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BLACK;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_RED)
        {
            item["carColor"] = TriggerListener::AI_COLOR_RED;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_DARKBLUE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_DARKBLUE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BLUE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BLUE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_YELLOW)
        {
            item["carColor"] = TriggerListener::AI_COLOR_YELLOW;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_GREEN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_GREEN;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_BROWN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_BROWN;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_PINK)
        {
            item["carColor"] = TriggerListener::AI_COLOR_PINK;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_PURPLE)
        {
            item["carColor"] = TriggerListener::AI_COLOR_PURPLE;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_DARKGRAY)
        {
            item["carColor"] = TriggerListener::AI_COLOR_DARKGRAY;
        }
        else if (alarmPlateInfo.struVehicleInfo.byColor == VCR_CLR_CYAN)
        {
            item["carColor"] = TriggerListener::AI_COLOR_CYAN;
        }
        else
        {
            item["carColor"] = TriggerListener::AI_COLOR_UNKNOWN;
        }

        uint8_t carType = 0;
        if (VTR_RESULT_CAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SMALLCAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_MICROCAR == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SUV_MPV == alarmPlateInfo.byVehicleType)
        {
            carType = 1;
        }
        else if (VTR_RESULT_TRUCK == alarmPlateInfo.byVehicleType ||
            VTR_RESULT_SMALLTRUCK == alarmPlateInfo.byVehicleType)
        {
            carType = 2;
        }
        else if (VTR_RESULT_MINIBUS == alarmPlateInfo.byVehicleType)
        {
            carType = 3;
        }

        item["carType"] = carType;
        item["carDirect"] = alarmPlateInfo.byCarDirectionType;
        item["plateLicense"] = sys->ascii2utf8(alarmPlateInfo.struPlateInfo.sLicense);
        item["manuPlateType"] = pu->m_manuPlateType;

		if (TriggerListener::PLATE_HIK_STARNDARD == pu->m_manuPlateType && systm->platePath() != "")
		{

			char s8PlateInfo[512] = { 0 };
			snprintf(s8PlateInfo, 512, "%04d%02d%02d%02d%02d%02d%03d_%s.jpg",
				yyyy, MM, dd, hh, mm, ss, ms, alarmPlateInfo.struPlateInfo.sLicense);

			std::string   filename = systm->platePath() + "/" + s8PlateInfo;
			FileStream         fileWrite;
            fileWrite.openWriteReplace(sys->ascii2utf8(filename.c_str()).c_str());
			if (fileWrite.isopen())
			{
				fileWrite.write(alarmPlateInfo.pBuffer4, alarmPlateInfo.dwCarPicLen);
				fileWrite.close();
			}
			item["picName"] = filename;
		}


        value["alarms"].append(item);

        bsave = true;


    }
        break;
    case 0xFFFD://温度报警
    {
		NET_DVR_THERMOMETRY_ALARM alarmThermometryInfo;
		memcpy(&alarmThermometryInfo, alarmInfo, sizeof(NET_DVR_THERMOMETRY_ALARM));
		int chanNo   = alarmThermometryInfo.dwChannel;//通道号
		int presetNo = alarmThermometryInfo.wPresetNo;//预置点号
		int roiId    = alarmThermometryInfo.byRuleID - 1;//max 6
		if (roiId > 5)
		{
			roiId = 0;
		}
		SHL_Json::Value itemAlarm;
		itemAlarm["type"] = TriggerListener::TR_THERMOMETRY_ALARM;;
		itemAlarm["channelNo"] = chanNo - 1;
		itemAlarm["status"] = "1";//告警为1
		itemAlarm["gucode"] = "";
		itemAlarm["name"] = "";
		itemAlarm["preset"] = presetNo;
		itemAlarm["alarmdesc"] = "";
		itemAlarm["roiId"] = roiId;
		itemAlarm["picName"] = "";
		if (systm->platePath() != "")
		{
			char s8PlateInfo[512] = { 0 };
			snprintf(s8PlateInfo, 512, "%lld_TemperatureAlarm.jpg",sys->currentTimeMillis());

			string tmpplatePath = systm->platePath();
#ifdef _WIN32
			if (0 != access(tmpplatePath.c_str(), 0))
			{
				mkdir(tmpplatePath.c_str());
			}
#else
			if (NULL == opendir(tmpplatePath.c_str()))
			{
				mkdir(tmpplatePath.c_str(), 0775);
			}
#endif
#ifdef WIN32
			std::string   filename = sys->utf2ascii(tmpplatePath.c_str()) + "/" + s8PlateInfo;
#else
			std::string   filename = tmpplatePath + "/" + s8PlateInfo;
#endif
			/*FileStream         fileWrite;
			fileWrite.openWriteReplace(sys->ascii2utf8(filename.c_str()).c_str());
			if (fileWrite.isopen())
			{
				if (NULL != alarmThermometryInfo.pThermalPicBuff && 0 != alarmThermometryInfo.dwThermalPicLen)
				{
					fileWrite.write(alarmThermometryInfo.pThermalPicBuff, alarmThermometryInfo.dwThermalPicLen);
				}
				else if (NULL != alarmThermometryInfo.pPicBuff && 0 != alarmThermometryInfo.dwPicLen)
				{
					fileWrite.write(alarmThermometryInfo.pPicBuff, alarmThermometryInfo.dwPicLen);
				}
				fileWrite.close();
			}*/
			itemAlarm["picName"] = sys->ascii2utf8(filename);
		}
		if (pu)
		{
			itemAlarm["gucode"] = pu->gucode(chanNo-1);
			itemAlarm["name"] = pu->guname(chanNo-1);
		}
		value["alarms"].append(itemAlarm);
		bsave = true;
    }
        break;
	case 0xFFFC://行为分析报警
	{
		//NET_VCA_RULE_ALARM alarmVcaRuleInfo;
		//memcpy(&alarmVcaRuleInfo, alarmInfo, sizeof(NET_VCA_RULE_ALARM));
		//int chanNo = alarmVcaRuleInfo.struDevInfo.byIvmsChannel;//通道号
		//SHL_Json::Value itemAlarm;
		//itemAlarm["type"] = TriggerListener::TR_AI_TRIPWIRE;;
		//itemAlarm["channelNo"] = chanNo - 1;
		//itemAlarm["status"] = "1";//告警为1
		//itemAlarm["gucode"] = "";
		//itemAlarm["name"] = "";
		//itemAlarm["preset"] = 1;
		//itemAlarm["alarmdesc"] = "";
		//itemAlarm["roiId"] = 0;
		//itemAlarm["picName"] = "";
		//if (pu)
		//{
		//	itemAlarm["gucode"] = pu->gucode(chanNo - 1);
		//	itemAlarm["name"] = pu->guname(chanNo - 1);
		//}
		//value["alarms"].append(itemAlarm);
		//bsave = true;
		//bsave = false;
	}
		break;
    case 0xFFF9://出入口控制机报警
    {
        NET_DVR_GATE_ALARMINFO alarmGateInfo;
        memcpy(&alarmGateInfo, alarmInfo, sizeof(NET_DVR_GATE_ALARMINFO));
        int externalDevType = alarmGateInfo.byExternalDevType;
        int externalDevStatus = alarmGateInfo.byExternalDevStatus;
        if (externalDevType != DEVICES_TYPE_DZ_DEV)
        {
            break;
        }
        SHL_Json::Value itemAlarm;
        /* 0 - 道闸正在开;1 - 道闸正在关;2 - 道闸开到位;3 - 道闸关到位;4 - 道闸无信号;5 - 锁闸 */
        string status = "6";
        string desc = "Unknown state";
        switch (externalDevStatus)
        {
        case DEVICES_STATUS_OPENING:
            status = "0";
            desc = "gate opening";
            break;
        case DEVICES_STATUS_CLOSING:
            status = "1";
            desc = "gate closing";
            break;
        case DEVICES_STATUS_OPENED:
            status = "2";
            desc = "gate opened";
            break;
        case DEVICES_STATUS_CLOSED:
            status = "3";
            desc = "gate closed";
            break;
        case DEVICES_STATUS_NOSIGNAL:
            status = "4";
            desc = "gate nosignal";
            break;
        case DEVICES_STATUS_LOCK:
            status = "5";
            desc = "gate lock";
            break;
        default:
            break;
        }
        itemAlarm["type"] = TriggerListener::TR_GATE_STATUS;;
        //itemAlarm["type"] = TriggerListener::TR_AI_TRIPWIRE;
        itemAlarm["channelNo"] = 0;
        itemAlarm["status"] = status;
        itemAlarm["gucode"] = "";
        itemAlarm["name"] = "";
        itemAlarm["preset"] = 1;
        itemAlarm["alarmdesc"] = desc;
        itemAlarm["roiId"] = 0;
        itemAlarm["picName"] = "";

        if (pu)
        {
            itemAlarm["gucode"] = pu->gucode(inputNo);
            itemAlarm["name"] = pu->guname(inputNo);
        }


        value["alarms"].append(itemAlarm);
        bsave = true;
    }
        break;
    case 0xFFFA:
    {
        NET_DVR_PDC_ALRAM_INFO alarmPDCInfo;
        memcpy(&alarmPDCInfo, alarmInfo, sizeof(NET_DVR_PDC_ALRAM_INFO));
        if (0 == alarmPDCInfo.byMode) //0:实时统计
        {
            int inperson = alarmPDCInfo.dwEnterNum;
            int outperson = alarmPDCInfo.dwLeaveNum;
            int chanNo = alarmPDCInfo.byChannel;

            SHL_Json::Value itemAlarm;
            itemAlarm["type"] = TriggerListener::TR_AI_NUMPEOPLE;;
            itemAlarm["channelNo"] = chanNo;
            itemAlarm["status"] = "0";
            itemAlarm["gucode"] = "";
            itemAlarm["name"] = "";
            itemAlarm["preset"] = 1;
            itemAlarm["alarmdesc"] = "";
            itemAlarm["roiId"] = 0;
            itemAlarm["picName"] = "";
            itemAlarm["inperson"] = inperson;
            itemAlarm["outperson"] = outperson;

            if (pu)
            {
                itemAlarm["gucode"] = pu->gucode(inputNo);
                itemAlarm["name"] = pu->guname(inputNo);
            }


            value["alarms"].append(itemAlarm);
            bsave = true;
        }
    }
        break;
    case 0xFFF8:
    {
        char *jsonstart = strchr(alarmInfo, '{');
        if (jsonstart == NULL)
            return;
        char *jsonend = strrchr(jsonstart, '}');
        if (jsonend == NULL)
            return;
        int jsonsize = jsonend - jsonstart + 1;
        char* pCBBuf = new char[jsonsize + 1];
        memset(pCBBuf, 0, jsonsize + 1);
        memcpy(pCBBuf, jsonstart, jsonsize);

        LOGI_MSV("helmet json:%d\n%s\n", jsonsize, pCBBuf);

        SHL_Json::Reader jsreader;
        SHL_Json::Value  jsrvalue;        

        if (!jsreader.parse(pCBBuf, jsrvalue, false))
        {
            delete pCBBuf;
            return;
        }

        LOGI_MSV("helmet json success");

        if (!jsrvalue["eventType"].isNull())
        {
            SHL_Json::Value  jsonArray = jsrvalue["Target"];
            if (jsonArray.isArray())
            {
                for (int iCount = 0; iCount <int(jsonArray.size()); ++iCount)
                {
                    string rt = jsonArray[iCount]["recognitionType"].asString();
                    if (rt == "safetyHelmet")
                    {                        
                        int cchn = jsrvalue["channelID"].asInt();
                        SHL_Json::Value itemAlarm;
                        itemAlarm["type"] = TriggerListener::TR_AI_HELMET;;
                        itemAlarm["channelNo"] = cchn;
                        itemAlarm["status"] = 1;
                        itemAlarm["preset"] = 0;
                        itemAlarm["alarmdesc"] = "";
                        itemAlarm["picName"] = "";
                                               
                        if (pu)
                        {
                            itemAlarm["gucode"] = pu->gucode(cchn);
                            itemAlarm["name"] = pu->guname(cchn);
                        }

                        value["alarms"].append(itemAlarm);
                        bsave = true;

                        string testsssjson = writer.write(value);
                        LOGI_MSV("helmet alarm:\n%s", testsssjson.c_str());
                    }
                }
            }
        }
        else
        {

        }
        delete pCBBuf;
    }
    break;
    default:
        break;
    }

    if (bsave)
    {
        string json = writer.write(value);
        pu->notifyAlarm(json, NULL, 0);
        pu->writeAlarmLog(json);
    }
}


void CBK thermInfo_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
    HikPu *pu = (HikPu*)pUserData;
    if (!pu)
    {
        return;
    }
	string resp = "";
	LPNET_DVR_THERMOMETRY_UPLOAD lpThermometry = new NET_DVR_THERMOMETRY_UPLOAD;
	pu->m_meatemptype = dwType;
    if (dwType == NET_SDK_CALLBACK_TYPE_DATA)
    {
        memcpy(lpThermometry, lpBuffer, sizeof(*lpThermometry));
		memset(pu->m_lpThermometry, 0, sizeof(*lpThermometry));
		memcpy(pu->m_lpThermometry, lpBuffer, sizeof(*lpThermometry));

        if (lpThermometry->byRuleCalibType != 0&&
            lpThermometry->byRuleCalibType != 1&&
            lpThermometry->byRuleCalibType != 2)
        {
			//pu->m_evtGetTemp.notify(strtemp, resp);
            ERRE_MSV("NET_DVR_GET_MANUALTHERM_INFO failed,lpThermometry->byRuleCalibType:%d", lpThermometry->byRuleCalibType);
        }

		SHL_Json::FastWriter writer;
		SHL_Json::Value      value;
		SHL_Json::Value      array;

		value["version"] = "1.0";
		value["time"] = sys->currentTime();
		value["deviceName"] = pu->name();
		value["deviceHost"] = pu->host();
		value["devicePort"] = pu->port();
		value["addrcode"] = pu->addrcode();
		value["level"] = 1;


		SHL_Json::Value itemAlarm;
		itemAlarm["type"] = TriggerListener::TR_AI_THERMOMETRY;
		//itemAlarm["channelNo"] = chanNo;
		itemAlarm["status"] = "0";
		itemAlarm["gucode"] = "";
		itemAlarm["name"] = "";
		itemAlarm["alarmdesc"] = "";
		itemAlarm["roiId"] = 0;
		itemAlarm["picName"] = "";

		itemAlarm["ruleName"] = lpThermometry->szRuleName;
		itemAlarm["ruleID"] = lpThermometry->byRuleID;
		itemAlarm["ruleCalibType"] = lpThermometry->byRuleCalibType;
		itemAlarm["presetNo"] = lpThermometry->wPresetNo;

		if (lpThermometry->byRuleCalibType == 0) //点测温
		{
			itemAlarm["pointTemp"] = lpThermometry->struPointThermCfg.fTemperature;
			itemAlarm["Maxtemp"] = lpThermometry->struPointThermCfg.fTemperature;
		}

		if ((lpThermometry->byRuleCalibType == 1) || (lpThermometry->byRuleCalibType == 2)) //框/线测温
		{
			itemAlarm["polygonMaxTemp"] = lpThermometry->struLinePolygonThermCfg.fMaxTemperature;
			itemAlarm["polygonMinTemp"] = lpThermometry->struLinePolygonThermCfg.fMinTemperature;
			itemAlarm["polygonAverageTemp"] = lpThermometry->struLinePolygonThermCfg.fAverageTemperature;
			itemAlarm["polygonDiffTemp"] = lpThermometry->struLinePolygonThermCfg.fTemperatureDiff;
			itemAlarm["Maxtemp"] = lpThermometry->struLinePolygonThermCfg.fMaxTemperature;
		}

		value["alarms"].append(itemAlarm);
		resp = writer.write(value);
//		pu->m_evtGetTemp.notify(strtemp, resp);
		
    }
    else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
    {
        DWORD dwStatus = *(DWORD*)lpBuffer;
		pu->m_meatempstatus = dwStatus;
        if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
        {
            LOGI_MSV("dwStatus:NET_SDK_CALLBACK_STATUS_SUCCESS");
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
        {
            DWORD dwErrCode = *(DWORD*)((char *)lpBuffer + 4);
            ERRE_MSV("NET_DVR_GET_MANUALTHERM_INFO failed, Error code %d", dwErrCode);
		//	pu->m_evtGetTemp.notify(strtemp, resp);
        }
    }
	pu->notifyAlarm(resp, NULL, 0);
	//pu->writeAlarmLog(resp);
	if (lpThermometry != NULL)
	{
		delete lpThermometry;
		lpThermometry = NULL;
	}
}



UserMap HikPu::m_userMap;
HikPu::HikPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HIK;
    m_manuPlateType = TriggerListener::PLATE_HIK_NORMAL;
    m_port = port;
    m_domainType = domainType;
    m_userId = -1;
    m_callStatus = 0;
    m_remoteCtrlId = -1;
	m_lpThermometry = new NET_DVR_THERMOMETRY_UPLOAD;
	memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
	memset((void*)m_chnOnlineStatusBak, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
}

HikPu::~HikPu()
{
	if (m_lpThermometry)
	{
		delete m_lpThermometry;
		m_lpThermometry = NULL;
	}
}

long
HikPu::getUserID()
{
    return m_userId;
}

HIK_DVR_CFG
HikPu::getDvrCfg()
{
    return m_dvrCfg;
}

int
HikPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    /* Initialize Hik SDK */
    NET_DVR_Init();

    NET_DVR_LOCAL_CHECK_DEV timeKeep = { 0 };
    timeKeep.dwCheckOnlineTimeout = 30;
    timeKeep.dwCheckOnlineNetFailMax = 3;
    NET_DVR_SetSDKLocalCfg(NET_SDK_LOCAL_CFG_TYPE_CHECK_DEV, &timeKeep);

    /* set login timeout */
    NET_DVR_SetConnectTime(6000, 1);

    uint32_t version = NET_DVR_GetSDKBuildVersion();

    char strVer[128] = { 0 };
    sprintf(strVer, "HCNetSDK V%d.%d.%d.%d",
        (0xff000000 & version) >> 24,
        (0x00ff0000 & version) >> 16,
        (0x0000ff00 & version) >> 8,
        (0x000000ff & version));

    LOGI_MSV("%s", strVer);

    NET_DVR_SetLogToFile(3);

    m_init = true;

    return 0;
}

void
HikPu::stop()
{
    if (!m_init)
        return;
    m_init = false;

    logout();

    Pu::stop();
}

int
HikPu::login()
{
    if (m_login)
    {
        return 0;
    }

    NET_DVR_SetExceptionCallBack_V30(0, NULL, excep_cb, this);
    NET_DVR_SetReconnect(5000);

    char  dvrIp[MAX_IPV4_STR_LEN] = { 0 };
    if (DOMAIN_HIK == m_domainType)
    {
        m_privDomain = m_ip;
        BOOL res = NET_DVR_GetDVRIPByResolveSvr_EX((char *)m_privDomain.c_str(), 7071, (BYTE *)m_name.c_str(), (WORD)m_name.size(), NULL, 0, dvrIp, (DWORD *)&m_port);
        if (!res)
        {
            LOGE_MSV("NET_DVR_GetDVRIPByResolveSvr_EX error, %d", NET_DVR_GetLastError());
            return -1;
        }

        m_ip = dvrIp;
    }
    else if (DOMAIN_PUBLIC == m_domainType)
    {
    }

    NET_DVR_USER_LOGIN_INFO loginInfo;
    NET_DVR_DEVICEINFO_V40  deviceInfo;
    memset(&loginInfo, 0, sizeof(loginInfo));
    memset(&deviceInfo, 0, sizeof(deviceInfo));

    loginInfo.wPort = m_port;
    loginInfo.bUseAsynLogin = true;
    loginInfo.cbLoginResult = login_cb;
    loginInfo.pUser = this;

    strncpy(loginInfo.sDeviceAddress, m_ip.c_str(), sizeof(loginInfo.sDeviceAddress));
    strncpy(loginInfo.sUserName, m_user.c_str(), sizeof(loginInfo.sUserName));
    strncpy(loginInfo.sPassword, m_pswd.c_str(), sizeof(loginInfo.sPassword));

    m_userId = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
    if (m_userId < 0)
    {
        LOGE_MSV("NET_DVR_Login_V40 error, %d", NET_DVR_GetLastError());

        return false;
    }

    int wait = 10000;
    while (wait > 0 && !m_login)
    {
        sys->sleep(100);
        wait -= 100;
    }

    if (wait <= 0)
    {
        if (-1 != m_userId)
        {
            /* It is recommended to use the NET_DVR_Logout for Hik SDK */
            NET_DVR_Logout(m_userId);
            m_userId = -1;
            m_login = false;
        }

        return -1;
    }

    DWORD res;
    NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_DEVICECFG_V40, 0xFFFFFFFF, &m_dvrCfg, sizeof(NET_DVR_DEVICECFG_V40), &res);
    LOGI_MSV("HIKVISION DVR(ip: %s) type is 0x%x", m_ip.c_str(), m_dvrCfg.byDVRType);

    {
        AutoLock locker(m_userMap._mtx);
        m_userMap[m_userId] = this;
    }

    //NET_DVR_SetDVRMessageCallBack_V31(alarm_cb, this);

    BOOL re =NET_DVR_SetDVRMessageCallBack_V50(0, alarm_cb, NULL);
    if(re == FALSE)
    {
        int ecode = NET_DVR_GetLastError();
       LOGI_MSV("NET_DVR_SetDVRMessageCallBack_V50 failed,error code：%d\n",NET_DVR_GetLastError());
       NET_DVR_Logout(m_userId);
       m_login = false; 
       m_userId = -1;
       return -1;
    }
    NET_DVR_SETUPALARM_PARAM setAlarm;
    memset(&setAlarm, 0, sizeof(setAlarm));

    setAlarm.dwSize = sizeof(NET_DVR_SETUPALARM_PARAM);
    setAlarm.byAlarmInfoType = 1;
    setAlarm.byAlarmTypeURL = 1 << 1;
    setAlarm.byLevel = 1;


    re = NET_DVR_SetupAlarmChan_V41(m_userId, &setAlarm);
    if(re == -1)
    {
         int ecode = NET_DVR_GetLastError();
         LOGI_MSV("NET_DVR_SetupAlarmChan_V41 failed,error code：%d\n",NET_DVR_GetLastError());
         //NET_DVR_Logout(m_userId);
         //m_userId = -1;
         //m_login = false;
         //return -1;
    }

    getAbilityPlate();

    if (1 == m_callListen)
    {
        startListenCall();
    }

    return 0;
}

void
HikPu::logout()
{
    do
    {
        if (-1 == m_userId)
        {
            break;
        }

        /* It is recommended to use the NET_DVR_Logout for Hik SDK */
        /* NET_DVR_Logout_V30(m_userId); */
		NET_DVR_CloseAlarmChan_V30(m_userId);

        NET_DVR_Logout(m_userId);

        {
            AutoLock locker(m_userMap._mtx);
            map<long, Pu *>::iterator i = m_userMap.find(m_userId);

            if (i != m_userMap.end())
            {
                m_userMap.erase(i);
            }
        }

        m_userId = -1;
    } while (0);

    if (m_login)
    {
        m_login = false;

        writeDevLog(STR_MSV_OFF_LINE);
    }
}


int HikPu::updateChnStatus()
{
    if (!m_login)
    {
        memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
		memset((void*)m_chnOnlineStatusBak, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
        return -1;
    }
#if 0
    DWORD res = -1;
    NET_DVR_IPPARACFG_V40 ipCfg = { 0 };
    if (!NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_IPPARACFG_V40, 0, &ipCfg, sizeof(NET_DVR_IPPARACFG_V40), &res))
    {
        LOGD_MSV("\n\n HikPu::updateChnStatus().NET_DVR_GetDVRConfig failed\n\n");
	}

	int chnNum = channels() > MAX_UT_CHANNEL_NUM ? MAX_UT_CHANNEL_NUM : channels();
	for (int chnIdx = 0; chnIdx < MAX_UT_CHANNEL_NUM; chnIdx++)
    {
        uint32_t hikChnNo = chnIdx + 1;

        if (hikChnNo < (uint32_t)(m_dvrCfg.byStartChan + m_dvrCfg.byChanNum))
        {
            //set
            m_chnOnlineStatus |= (uint64_t)0x1 << chnIdx;
            continue;
        }

        if (hikChnNo < ipCfg.dwStartDChan)
        {
            //clear
            m_chnOnlineStatus &= ~((uint64_t)0x1 << chnIdx);
            continue;
        }

        uint32_t index = hikChnNo - ipCfg.dwStartDChan;
        if (!ipCfg.struStreamMode[index].uGetStream.struChanInfo.byEnable)
        {
            //clear
            m_chnOnlineStatus &= ~((uint64_t)0x1 << chnIdx);
        }
        else
        {
            //set
            m_chnOnlineStatus |= (uint64_t)0x1 << chnIdx;
        }
	}
#elif 1
    if (type() == DEV_TYPE_DVR || type() == DEV_TYPE_NVR)
    {
        NET_DVR_GETWORKSTATE_COND struWorkStateCond = { 0 };
        struWorkStateCond.dwSize = sizeof(NET_DVR_GETWORKSTATE_COND);
        struWorkStateCond.byFindChanByCond = 0;
        struWorkStateCond.byFindHardByCond = 0;
        DWORD dwList = 0;
        NET_DVR_WORKSTATE_V40 ipCfg = { 0 };
        if (FALSE == NET_DVR_GetDeviceConfig(m_userId, NET_DVR_GET_WORK_STATUS, 1, &struWorkStateCond, sizeof(NET_DVR_GETWORKSTATE_COND), &dwList, &ipCfg, sizeof(NET_DVR_WORKSTATE_V40)) || (dwList != 0))
        {
           
            WORD errCode = NET_DVR_GetLastError();
            int intErrcode = errCode;
            LOGD_MSV("\n\n HikPu::updateChnStatus().NET_DVR_GetDeviceConfig failed code:%d \n\n", errCode);
        }
        memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
        if (type() == DEV_TYPE_DVR)
        {
            for (int chnIdx = 0; chnIdx < 16; chnIdx++)
            {
                m_chnOnlineStatus[0] |= (uint64_t)0x1 << chnIdx;
            }
        }
        for (int i = 0; i < MAX_CHANNUM_V40; ++i)
        {
            DWORD chanNo = ipCfg.struChanStatic[i].dwChannelNo;
            int idx = (chanNo - 1) / 64;

			if (chanNo == 0xffffffff)
			{
				continue;
			}
			if (chanNo < 1 || chanNo >MAX_UT_CHANNEL_NUM)
			{
				continue;
			}

            //shl adpter CVR
            if (i >= 64)
            {   
                idx = i / 64;
                m_chnOnlineStatus[idx] |= (uint64_t)0x1 << (i % 64);
            }

            //printf("chnID:%d  st:%d\n", i, ipCfg.struChanStatic[i].bySignalStatic);
            
            if (!(ipCfg.struChanStatic[i].bySignalStatic))
            {
                //set                
                m_chnOnlineStatus[idx] |= (uint64_t)0x1 << ((chanNo - 1) % 64);
                //m_chnOnlineStatus[idx] &= ~((uint64_t)0x1 << ((chanNo -1)%64));
            }
        }
    }
    else
    {
        memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
        m_chnOnlineStatus[0] |= (uint64_t)0x1;
    }
#elif 0
    if (type() == DEV_TYPE_DVR || type() == DEV_TYPE_NVR)
    {
        memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
        if (type() == DEV_TYPE_DVR)
        {
            for (int chnIdx = 0; chnIdx < 16; chnIdx++)
            {
                m_chnOnlineStatus[0] |= (uint64_t)0x1 << chnIdx;
            }
        }

        NET_DVR_XML_CONFIG_INPUT  xmlConfigInput = { 0 };
        NET_DVR_XML_CONFIG_OUTPUT  xmlCongfigOutput = { 0 };
        xmlConfigInput.dwSize = sizeof(xmlConfigInput);
        xmlCongfigOutput.dwSize = sizeof(xmlCongfigOutput);

        char szUrl[256] = "";
        DWORD dwOutputLen = 1024 * 1024;
        char *pOutBuf = new char[dwOutputLen];

        BOOL bIsSupportFaceMatchBroken = FALSE;

        sprintf(szUrl, "GET /ISAPI/ContentMgmt/InputProxy/channels/status");
        xmlConfigInput.lpRequestUrl = szUrl;
        xmlConfigInput.dwRequestUrlLen = strlen(szUrl);
        xmlConfigInput.dwRecvTimeOut = 5000;

        char szStatusBuff[1024] = { 0 };
        memset(pOutBuf, 0, dwOutputLen);
        xmlCongfigOutput.dwOutBufferSize = dwOutputLen;
        xmlCongfigOutput.lpOutBuffer = pOutBuf;
        xmlCongfigOutput.lpStatusBuffer = szStatusBuff;
        xmlCongfigOutput.dwStatusSize = sizeof(szStatusBuff);
        BOOL bRet = NET_DVR_STDXMLConfig(m_userId, &xmlConfigInput, &xmlCongfigOutput);
        string xxml((char*)(xmlCongfigOutput.lpOutBuffer), xmlCongfigOutput.dwReturnedXMLSize);
        delete []pOutBuf;
        if (!bRet)
        {
            WORD errCode = NET_DVR_GetLastError();
            return -1;
        }
        string json;
        xml2json(xxml.c_str(), json);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        int ret = -1;

        if (!reader.parse(json, rvalue, false))
        {
            ERRE_MSV("SHL_Json parse error");
            return ret;
        }
        SHL_Json::Value  jsonArray = rvalue["InputProxyChannelStatusList"]["InputProxyChannelStatus"];
        if (jsonArray.isArray())        
        {
            for (int i = 0; i < jsonArray.size(); ++i)
            {
                if (!jsonArray[i]["online"].isNull() && !jsonArray[i]["id"].isNull() && jsonArray[i]["online"].asString() == "true")
                {
                    string strChanNo = jsonArray[i]["id"].asString();
                    int chanNo = 32 + atoi(strChanNo.c_str());
                    m_chnOnlineStatus[(chanNo - 1) / 64] |= (uint64_t)0x1 << ((chanNo - 1) % 64);
                }
            }
        }
    }
    else
    {
        memset((void*)m_chnOnlineStatus, 0, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);
        m_chnOnlineStatus[0] |= (uint64_t)0x1;
    }
#endif
	/*状态的判断以及告警实现wxr*/
	bool arraysEqual = true;
	int	count = 0;
	vector<int> differ;
	vector<int> channelNo;
	while (count < MAX_UT_CANONLINE_ARRSIZE)
	{
		if (m_chnOnlineStatusBak[count] != m_chnOnlineStatus[count])
		{
			arraysEqual = false;
			differ.push_back(count);
		}
		count++;
	}
	if (arraysEqual == false)
	{
		int a[64] = { 0 };
		int b[64] = { 0 };
		for (int j = 0; j < differ.size();j++)
		{
			int numner = differ[j];
			uint64_t chnOnlineStatustmp = m_chnOnlineStatus[numner];
			for (int i = 0; i < 64; i++)
			{
				a[i] = chnOnlineStatustmp % 2;	//取出x二进制中的最低位，存到数组a中
				chnOnlineStatustmp /= 2;		//去掉x二进制中的最低位
			}
			chnOnlineStatustmp = m_chnOnlineStatusBak[numner];
			for (int i = 0; i < 64; i++)
			{
				b[i] = chnOnlineStatustmp % 2;
				chnOnlineStatustmp /= 2;
			}
			for (int i = 0; i<64; i++)
			{
				if (a[i] ^ b[i] == 1)//如果对应位不同，则count加1
				{
					int chennel = i + j * 64;
					channelNo.push_back(chennel);
				}
			}

		}
		SHL_Json::Value value;
		value["version"] = "1.0";
		value["time"] = sys->currentTime();
		value["deviceName"] = this->name();
		value["deviceHost"] = this->host();
		value["devicePort"] = this->port();
		value["addrcode"]   = this->addrcode();
		value["count"]      = channelNo.size();
		value["level"] = 1;
		for (int k = 0; k < channelNo.size(); k++)
		{
			SHL_Json::Value itemAlarm;

			itemAlarm["status"] = validate(channelNo[k]) ? 1 : 0;
			itemAlarm["type"] = TriggerListener::TR_CHANNEL_STATUS;
			itemAlarm["channelNo"] = channelNo[k]+1;
			itemAlarm["name"] = this->guname(channelNo[k]);
			value["alarms"].append(itemAlarm);
		}
		SHL_Json::FastWriter writer;;
		string json = writer.write(value);
		this->notifyAlarm(json, NULL, 0);
		this->writeAlarmLog(json);
		memcpy((char*)m_chnOnlineStatusBak, (char*)m_chnOnlineStatus, sizeof(uint64_t) * MAX_UT_CANONLINE_ARRSIZE);

	}

    return 0;
}

bool
HikPu::validate(int channelNo)
{
	if (channelNo < 0 || channelNo > MAX_UT_CHANNEL_NUM)
    {
        return false;
    }
	int idx = channelNo / 64;

    
    return (m_chnOnlineStatus[idx] & ((uint64_t)0x1 << (channelNo%64))) ? true : false;

#if 0
    uint32_t hikChnNo = channelNo + 1;

    if (hikChnNo < (uint32_t)(m_dvrCfg.byStartChan + m_dvrCfg.byChanNum))
    {
        return true;
    }

    DWORD res   = -1;
    NET_DVR_IPPARACFG_V40 ipCfg;

    if (!NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_IPPARACFG_V40, 0, &ipCfg, sizeof(NET_DVR_IPPARACFG_V40), &res))
    {
        ERRE_MSV("hik device has no ip channel: %d", hikChnNo);

        return false;
    }

    bool result = false;

    do
    {
        if (hikChnNo < ipCfg.dwStartDChan)
        {
            ERRE_MSV("invalidate channel: %d", hikChnNo);
            break;
        }

        uint32_t index = hikChnNo - ipCfg.dwStartDChan;

        if (!ipCfg.struStreamMode[index].uGetStream.struChanInfo.byEnable)
        {
            ERRE_MSV("ip channel %d is not online.", hikChnNo);

            break;
        }

        result = true;
    } while (0);

    return result;
#endif
}

bool
HikPu::validate(string gucode)
{
	int channelNo = GetChanNoByCode(gucode);

    return validate(channelNo);
}

int
HikPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    BOOL res = true;

    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
    }

    int ptzCmd = -1;
    switch (cmd)
    {
    case PTZ_CMD_UP:
        ptzCmd = TILT_UP;
        break;
    case PTZ_CMD_DOWN:
        ptzCmd = TILT_DOWN;
        break;
    case PTZ_CMD_LEFT:
        ptzCmd = PAN_LEFT;
        break;
    case PTZ_CMD_RIGHT:
        ptzCmd = PAN_RIGHT;
        break;
    case PTZ_CMD_UPLEFT:
        ptzCmd = UP_LEFT;
        break;
    case PTZ_CMD_UPRIGHT:
        ptzCmd = UP_RIGHT;
        break;
    case PTZ_CMD_DOWNLEFT:
        ptzCmd = DOWN_LEFT;
        break;
    case PTZ_CMD_DOWNRIGHT:
        ptzCmd = DOWN_RIGHT;
        break;
    case PTZ_CMD_ZOOMIN:
        ptzCmd = ZOOM_IN;
        break;
    case PTZ_CMD_ZOOMOUT:
        ptzCmd = ZOOM_OUT;
        break;
    case PTZ_CMD_FOCUSNEAR:
        ptzCmd = FOCUS_NEAR;
        break;
    case PTZ_CMD_FOCUSFAR:
        ptzCmd = FOCUS_FAR;
        break;
    case PTZ_CMD_IRISOPEN:
        ptzCmd = IRIS_OPEN;
        break;
    case PTZ_CMD_IRISCLOSE:
        ptzCmd = IRIS_CLOSE;
        break;
    case PTZ_CMD_AUTOSCAN:
        ptzCmd = PAN_AUTO;
        break;

    case PTZ_CMD_SETPRESET:
        ptzCmd = SET_PRESET;
        res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, SET_PRESET, preset);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_CLEARPRESET:
        res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, CLE_PRESET, preset);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_CALLPRESET:
        res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, GOTO_PRESET, preset);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_RAIN_ON:
    case PTZ_CMD_RAIN_OFF:
        ptzCmd = WIPER_PWRON;
        break;
    case PTZ_CMD_AUXLIGHT_ON:
    case PTZ_CMD_AUXLIGHT_OFF:
        ptzCmd = LIGHT_PWRON;
        break;
    case PTZ_CMD_LOCKCTRL:
    {
        NET_DVR_PTZ_LOCKCFG lock = { 0 };
        lock.dwSize = sizeof(lock);
        lock.byWorkMode = 1;
        res = NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PTZLOCKCFG, channelNo + 1, &lock, sizeof(lock));
        return (res == TRUE ? 0 : -1);
    }

    case PTZ_CMD_UNLOCKCTRL:
    {
        NET_DVR_PTZ_LOCKCFG lock = { 0 };
        lock.dwSize = sizeof(lock);
        lock.byWorkMode = 0;
        res = NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PTZLOCKCFG, channelNo + 1, &lock, sizeof(lock));
        return (res == TRUE ? 0 : -1);
    }

	case PTZ_CMD_SETPOS:
	{
		 NET_DVR_PTZPOS ptzPos;
		 ptzPos.wAction = 1;
		 ptzPos.wPanPos = DecToHex(speed * 10);
		 ptzPos.wTiltPos = DecToHex(preset * 10);
		 ptzPos.wZoomPos = DecToHex(((stop)? (2): 1) * 10);
		 BOOL bRet = NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PTZPOS, channelNo + 1, &ptzPos, sizeof(NET_DVR_PTZPOS));
		 if (!bRet)
		 {
		   int err = NET_DVR_GetLastError();
		   return -1;
		 }
		 return 0;
	}
    }

    float rate = 7 / (float)10;
    int actualSpeed = (int)(speed * rate + 0.50);

    if (actualSpeed < 0 || actualSpeed > 7)
    {
        actualSpeed = 4;
    }

    res = NET_DVR_PTZControlWithSpeed_Other(m_userId, channelNo + 1, ptzCmd, stop, actualSpeed);
    int errCode = 0;
    if (res != TRUE)
    {
        errCode = NET_DVR_GetLastError();
    }
    LOGI_MSV("channelNo: %d, cmd: %d, actualSpeed: %d, preset: %d, stop: %d, result: %d,errCode: %d", channelNo + 1, ptzCmd, actualSpeed, preset, stop, res, errCode);
    return (res == TRUE ? 0 : -1);
}

int
HikPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
    //addrcode --- 18 bit char , start pointer add 16 
	int channelNo = GetChanNoByCode(addrcode);

    return ptzCtrl(cmd, channelNo, speed, preset, stop);
}

int
HikPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    SHL_Json::Value value;
    SHL_Json::Value array;

    if (RECORD_TYPE_LABEL == type)
    {
        NET_DVR_FIND_LABEL param = { 0 };
        param.dwSize = sizeof(NET_DVR_FIND_LABEL);
        param.lChannel = channelNo + 1;
        param.struStartTime.dwYear = start.year;
        param.struStartTime.dwMonth = start.month;
        param.struStartTime.dwDay = start.day;
        param.struStartTime.dwHour = start.hour;
        param.struStartTime.dwMinute = start.minute;
        param.struStartTime.dwSecond = start.second;

        param.struStopTime.dwYear = end.year;
        param.struStopTime.dwMonth = end.month;
        param.struStopTime.dwDay = end.day;
        param.struStopTime.dwHour = end.hour;
        param.struStopTime.dwMinute = end.minute;
        param.struStopTime.dwSecond = end.second;


        int handle = NET_DVR_FindRecordLabel(m_userId, &param);
        if (-1 == handle)
        {
            ERRE_MSV("NET_DVR_FindFile_V40 err: %d", NET_DVR_GetLastError());

            return -1;
        }
        int index = 0;
        while (1)
        {
            NET_DVR_FINDLABEL_DATA hikData;

            memset(&hikData, 0, sizeof(NET_DVR_FINDLABEL_DATA));

            LONG res = NET_DVR_FindNextLabel(handle, &hikData);
            if (res == NET_DVR_FILE_SUCCESS)
            {
                index++;
                if (index < fromIndex || index > toIndex)
                {
                    continue;
                }

                SHL_Json::Value record;
                record["labelName"] = (char*)hikData.sLabelName;
                record["labelIdentify"] = (char*)hikData.struLabelIdentify.sLabelIdentify;
                record["index"] = index;

                datetime_s start;
                start.year = hikData.struTimeLabel.dwYear;
                start.month = hikData.struTimeLabel.dwMonth;
                start.day = hikData.struTimeLabel.dwDay;
                start.hour = hikData.struTimeLabel.dwHour;
                start.minute = hikData.struTimeLabel.dwMinute;
                start.second = hikData.struTimeLabel.dwSecond;

                record["labeltime"] = sys->strftime(start);

                array.append(record);
            }
            else if (res == NET_DVR_ISFINDING)
            {
                sys->sleep(1);
                continue;
            }
            else if ((res == NET_DVR_NOMOREFILE) || (res == NET_DVR_FILE_NOFIND))
            {
                NET_DVR_FindClose_V30(handle);
                break;
            }
            else
            {

                ERRE_MSV("NET_DVR_FindNextLabel err");
                NET_DVR_FindClose_V30(handle);
                return -1;
            }
        }

        value["records"] = array;
        value["realNum"] = index;
        value["fromIndex"] = fromIndex;
        value["toIndex"] = fromIndex + array.size();

        SHL_Json::StreamWriterBuilder writer;
        json = SHL_Json::writeString(writer, value);
        return 0;

    }


    NET_DVR_FILECOND_V50 findCond;
    memset(&findCond, 0, sizeof(findCond));



    int recordtype = RECORD_TYPE_ALL;
    if (RECORD_TYPE_ALL == type)
    {
        recordtype = 0xFF;
    }
    else if (RECORD_TYPE_MD == type)
    {
        recordtype = 1;
    }
    else if (RECORD_TYPE_VCOVER == type)
    {
        recordtype = 24;
    }
    else if (RECORD_TYPE_TIMER == type)
    {
        recordtype = 0;
    }


    findCond.struStreamID.dwChannel = channelNo + 1;
    findCond.dwFileType = recordtype;
    //findCond.dwIsLocked = 0xff;
    findCond.struStartTime.wYear = start.year;
    findCond.struStartTime.byMonth = start.month;
    findCond.struStartTime.byDay = start.day;
    findCond.struStartTime.byHour = start.hour;
    findCond.struStartTime.byMinute = start.minute;
    findCond.struStartTime.bySecond = start.second;

    findCond.struStopTime.wYear = end.year;
    findCond.struStopTime.byMonth = end.month;
    findCond.struStopTime.byDay = end.day;
    findCond.struStopTime.byHour = end.hour;
    findCond.struStopTime.byMinute = end.minute;
    findCond.struStopTime.bySecond = end.second;

    LONG handle = NET_DVR_FindFile_V50(m_userId, &findCond);
    if (0 > handle)
    {
        ERRE_MSV("NET_DVR_FindFile_V50 err: %d", NET_DVR_GetLastError());
        return -1;
    }

    int index = 0;
    while (1)
    {
        NET_DVR_FINDDATA_V50 hikData;

        memset(&hikData, 0, sizeof(NET_DVR_FINDDATA_V50));

        LONG res = NET_DVR_FindNextFile_V50(handle, &hikData);
        if (res == NET_DVR_FILE_SUCCESS)
        {
            index++;
            if (index < fromIndex || index > toIndex)
            {
                continue;
            }

            SHL_Json::Value record;
            record["filename"] = hikData.sFileName;
            record["fileurl"] = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(hikData.sFileName);
            record["channelNo"] = (int)channelNo;
            record["size"] = (int)hikData.dwFileSize;
            record["index"] = index;
            record["locked"] = hikData.byLocked;

            if (0 == hikData.byFileType)
            {
                //record["type"] = 1;
                record["type"] = 0x100000;
            }
            else if (6 == hikData.byFileType)
            {
                //record["type"] = 2;
                record["type"] = 0x200000;
            }
            else if (13 == hikData.byFileType)
            {
                //record["type"] = 3;
                record["type"] = 0x2;
            }
            else if (24 == hikData.byFileType)
            {
                record["type"] = 0x2;
            }
            else if (1 == hikData.byFileType)
            {
                record["type"] = 0x2;
            }

            datetime_s start;
            datetime_s end;
            char startStr[32] = { 0 };
            char endStr[32] = { 0 };

            start.year = hikData.struStartTime.wYear;
            start.month = hikData.struStartTime.byMonth;
            start.day = hikData.struStartTime.byDay;
            start.hour = hikData.struStartTime.byHour;
            start.minute = hikData.struStartTime.byMinute;
            start.second = hikData.struStartTime.bySecond;

            sprintf(startStr, "%d-%02d-%02dT%02d:%02d:%02dZ",
                    hikData.struStartTime.wYear,
                    hikData.struStartTime.byMonth,
                    hikData.struStartTime.byDay,
                    hikData.struStartTime.byHour,
                    hikData.struStartTime.byMinute,
                    hikData.struStartTime.bySecond);
            record["start"] = startStr;


            end.year = hikData.struStopTime.wYear;
            end.month = hikData.struStopTime.byMonth;
            end.day = hikData.struStopTime.byDay;
            end.hour = hikData.struStopTime.byHour;
            end.minute = hikData.struStopTime.byMinute;
            end.second = hikData.struStopTime.bySecond;
            sprintf(endStr, "%d-%02d-%02dT%02d:%02d:%02dZ",
                    hikData.struStopTime.wYear,
                    hikData.struStopTime.byMonth,
                    hikData.struStopTime.byDay,
                    hikData.struStopTime.byHour,
                    hikData.struStopTime.byMinute,
                    hikData.struStopTime.bySecond);
            record["end"] = endStr;

            array.append(record);
        }
        else if (res == NET_DVR_ISFINDING)
        {
            sys->sleep(1);
            continue;
        }
        else if ((res == NET_DVR_NOMOREFILE) || (res == NET_DVR_FILE_NOFIND))
        {
            NET_DVR_FindClose_V30(handle);
            break;
        }
        else
        {
            ERRE_MSV("NET_DVR_FindNextLabel err");
            NET_DVR_FindClose_V30(handle);
            return -1;
        }
    }

    value["records"] = array;
    value["realNum"] = index;
    value["fromIndex"] = fromIndex;
    value["toIndex"] = fromIndex + array.size();

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);

    return 0;
}

int
HikPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	int channelNo = GetChanNoByCode(addrcode);

    int ret = queryRecord(channelNo, type, start, end, fromIndex, toIndex, json); 
    if (m_clienQueryRecord_cb)
    {
		m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }
    return ret;
}

int
HikPu::syncTime(datetime_s &dt, const string &tzone)
{
    if (!m_login)
    {
        return -1;
    }

    NET_DVR_TIME stDvrTime =
    {
        dt.year,
        dt.month,
        dt.day,
        dt.hour,
        dt.minute,
        dt.second
    };

    if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_TIMECFG, -1, &stDvrTime, sizeof(NET_DVR_TIME)))
    {
        return -1;
    }
    //write log


    return 0;
}

int
HikPu::onCustomCommand(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;
    int ret = -1;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return ret;
    }

    string jMethod = rvalue["Method"].asString();
    if (!rvalue.isMember("Method"))
    {
        ERRE_MSV("json error:no member \"Method\"");
        return ret;
    }
    if (jMethod == "OnCallAnswer")
    {
        int    chnNo = rvalue["chnNo"].asInt();
        int    cmd = rvalue["Command"].asInt();
        ret = doCallAnswer(cmd);
        return ret;
    }
    else if (jMethod == "setPrivacyMask")
    {
        int    chnNo = rvalue["chnNo"].asInt();
        int    shelterCnt = rvalue["shelterCnt"].asInt();
        int    enable = rvalue["enable"].asInt();
        //NET_DVR_SHELTER *shelter = new NET_DVR_SHELTER[shelterCnt];

        float x = rvalue["shelter"][0]["X"].asFloat();
        float y = rvalue["shelter"][0]["Y"].asFloat();
        float w = rvalue["shelter"][0]["w"].asFloat();
        float h = rvalue["shelter"][0]["h"].asFloat();

        NET_VCA_POINT  struPos[VCA_MAX_POLYGON_POINT_NUM] = { 0 }; //?����?D?��???��?,��??����???

        struPos[0].fX = x;
        struPos[0].fY = y;
        struPos[1].fX = x;
        struPos[1].fY = y + h;
        struPos[2].fX = x + w;
        struPos[2].fY = y + h;
        struPos[3].fX = x + w;
        struPos[3].fY = y;

        ret = setPrivacyMask(chnNo, enable, struPos, shelterCnt);
        return ret;
    }
    else if (jMethod == "setShowString")
    {
        int    chnNo = rvalue["chnNo"].asInt();
        NET_DVR_SHOWSTRING_V30* showstring = new NET_DVR_SHOWSTRING_V30;
        memset(showstring, 0, sizeof(NET_DVR_SHOWSTRING_V30));
        showstring->dwSize = sizeof(NET_DVR_SHOWSTRING_V30);

        int  Cnt = rvalue["ItemCount"].asInt();
        for (int i = 0; i < Cnt; ++i)
        {
            if (i >= MAX_STRINGNUM_V30)
                break;

            showstring->struStringInfo[i].wShowStringTopLeftX = rvalue["items"][i]["X"].asInt();
            showstring->struStringInfo[i].wShowStringTopLeftY = rvalue["items"][i]["Y"].asInt();
            showstring->struStringInfo[i].wShowString = rvalue["items"][i]["bShowString"].asInt();
            showstring->struStringInfo[i].wStringSize = rvalue["items"][i]["stringLength"].asInt();
            string c = rvalue["items"][i]["content"].asString();
			string gc = sys->utf2ascii(c.c_str());
            int len = strlen(gc.c_str());
			if (len > 44)  //显示字符串不能超过44
			{
				len = 44;
			}
			
			memcpy(showstring->struStringInfo[i].sString, gc.c_str(), len);
        }



        if (setShowString(chnNo, showstring) != 0)
        {
            delete showstring;
            return -1;
        }

        delete showstring;
        return 0;        
    }
    else if (jMethod == "setShowStringV1")
    {
        AutoLock lock(m_setStringMtx);

        int    chnNo = rvalue["chnNo"].asInt();
        NET_DVR_SHOWSTRING_V30* showstring = new NET_DVR_SHOWSTRING_V30;
        memset(showstring, 0, sizeof(NET_DVR_SHOWSTRING_V30));
        showstring->dwSize = sizeof(NET_DVR_SHOWSTRING_V30);

        if (getShowString(chnNo, showstring) != 0)
        {
            delete showstring;
            return -1;
        }

        int  Cnt = rvalue["ItemCount"].asInt();
		int indexstr = rvalue["items"][0]["index"].asInt();
		for (int i = 0; i < MAX_STRINGNUM_V30; ++i)
        {
            
            if (i >= MAX_STRINGNUM_V30 || indexstr >= MAX_STRINGNUM_V30)
                break;
			if (i == indexstr)
			{
				showstring->struStringInfo[indexstr].wShowStringTopLeftX = rvalue["items"][0]["X"].asInt();
				showstring->struStringInfo[indexstr].wShowStringTopLeftY = rvalue["items"][0]["Y"].asInt();
				showstring->struStringInfo[indexstr].wShowString = rvalue["items"][0]["bShowString"].asInt();
				showstring->struStringInfo[indexstr].wStringSize = rvalue["items"][0]["stringLength"].asInt();
				string c = rvalue["items"][0]["content"].asString();

				string gc = sys->utf2ascii(c.c_str());
				int len = strlen(gc.c_str());
				memset(showstring->struStringInfo[indexstr].sString, 0, sizeof(showstring->struStringInfo[indexstr].sString));
				memcpy(showstring->struStringInfo[indexstr].sString, gc.c_str(), len);
			}
            //memset(showstring->struStringInfo[indexstr].sString, 0, sizeof(showstring->struStringInfo[indexstr].sString));
            
        }


        if (setShowString(chnNo, showstring) != 0)
        {
            delete showstring;
            return -1;
        }

        delete showstring;
        return 0;
    }
    else if (jMethod == "ptzPostion3D")
    {
        NET_DVR_POINT_FRAME *dvrPoint = new NET_DVR_POINT_FRAME;
        int    chnNo = rvalue["chnNo"].asInt();

        dvrPoint->bCounter = 0;
        dvrPoint->xTop = rvalue["X1"].asInt();
        dvrPoint->yTop = rvalue["Y1"].asInt();
        dvrPoint->xBottom = rvalue["X2"].asInt();
        dvrPoint->yBottom = rvalue["Y2"].asInt();

        int ret = 0;
        if (ptzPostion3D(chnNo, dvrPoint) != 0)
        {
            ret = -1;
        }

        delete dvrPoint;
        return ret;
    }
    else if (jMethod == "setPtzCruise")
    {
        int    chnNo = rvalue["chnNo"].asInt();
        int    cmd = rvalue["cmd"].asInt();
        int    seqCnt = rvalue["seqCnt"].asInt();
        int    curiseRoute = rvalue["curiseRoute"].asInt();
        for (int i = 0; i < seqCnt; ++i)
        {
            int seq = rvalue["seqItems"][i]["seq"].asInt();
            int dwell = rvalue["seqItems"][i]["dwell"].asInt();
            int speed = rvalue["seqItems"][i]["speed"].asInt();
            int preset = rvalue["seqItems"][i]["preset"].asInt();

            if (cmd == 0)  // add ptzCurise
            {
                addPtzCruise(chnNo, curiseRoute, seq, preset, dwell, speed);
            }
            else if (cmd == 1) //del ptzCurise
            {
                delPtzCruise(chnNo, curiseRoute, seq, preset);
            }
            else if (cmd == 2) //start ptzCurise
            {
                enablePtzCruise(chnNo, curiseRoute, true);
            }
            else if (cmd == 3) //stop ptzCurise
            {
                enablePtzCruise(chnNo, curiseRoute, false);
            }
            else if (cmd == 4) //del curise Line
            {
                delPtzCruiseRoute(chnNo, curiseRoute);
            }
        }
        return 0;
    }
    else if (jMethod == "SetVideoEffect")
    {
        int    chnNo = rvalue["nChn"].asInt();
        int    brightValue = rvalue["bright"].asInt();
        int    contrastValue = rvalue["contrast"].asInt();
        int    saturationValue = rvalue["saturation"].asInt();
        int    hueValue = rvalue["hue"].asInt();

        return setVideoEffect(chnNo, brightValue, contrastValue, saturationValue, hueValue);
    }
    else if (jMethod == "LockRecordFile")
    {
        int lock = rvalue["lock"].asInt();
        string filename = rvalue["fileName"].asString();

        return lockRecordFile(filename, lock == 1 ? true : false);
    }
    else if (jMethod == "ptzPatternCtrl")
    {
        int chnNo = rvalue["DevChn"].asInt();
        int patternCmd = rvalue["PatternCmd"].asInt();
        int patternId = rvalue["PatternId"].asInt();

        return ptzPatternCtrl(chnNo, patternCmd, patternId);
    }
    else if (jMethod == "ptzParkaction")
    {
        int chnNo = rvalue["chnNo"].asInt();
        int enable = rvalue["enablE"].asInt();
        int parkTime = rvalue["parkTime"].asInt();
        int actionType = rvalue["actionType"].asInt();
        int id = rvalue["ID"].asInt();

        return setPtzParkaction(chnNo, enable, parkTime, actionType, id);
    }
    else if (jMethod == "gateCtrl")
    {
        int gateCmd = rvalue["gateCmd"].asInt();
        return gateCtrl(gateCmd);
    }
    else if (jMethod == "vehicleCtrl")
    {
        return vehicleCtrl(json);
    }
    else if (jMethod == "resetCounter")
    {
        return resetCounter(json);
    }
    else if ("GateCtrl" == jMethod)
    {
        int CtrlVal = rvalue["CtrlVal"].asInt();
        return controlDoorGate(CtrlVal);
    }
	else if ("RecordCtrl" == jMethod)
	{
		int DevChn = rvalue["DevChn"].asInt();
		int CtrlVal = rvalue["CtrlVal"].asInt();
		if (0 == CtrlVal)
		{
			return  NET_DVR_StartDVRRecord(m_userId, DevChn, 0);
		}
		else if (1 == CtrlVal)
		{
			return NET_DVR_StopDVRRecord(m_userId,DevChn);
		}
		else if (2 == CtrlVal)
		{
			return NET_DVR_StopDVRRecord(m_userId, 0xffff);//close all channels
		}
	}
    //else if ("CapturePicture" == jMethod)
    //{
   //     return capturePic(json);
    //}
    else if ("LockFileByTime" == jMethod)
    {
        int    chnNo     = rvalue["chnNo"].asInt();
        string startTime = rvalue["startTime"].asString();
        string stopTime  = rvalue["stopTime"].asString();
        int    duration = rvalue["duration"].asInt();

        return lockRecordFileByTime(chnNo, startTime, stopTime, duration);
    }
    else if ("setPTZPos" == jMethod)
    {
        return onSetPTZPos(json);
    }
    else if ("setThermometryPresetInfo" == jMethod)
    {
        return onSetThermometryPresetInfo(json);
    }
	else if ("setOSDInfo" == jMethod)//add wxr set the OSD type
	{
		int    chnNo = rvalue["chnNo"].asInt();
		NET_DVR_PICCFG_V30*  osdInfo = new NET_DVR_PICCFG_V30;
		memset(osdInfo, 0, sizeof(NET_DVR_PICCFG_V30));
		osdInfo->dwSize = sizeof(NET_DVR_PICCFG_V30);
		string c = rvalue["sChanName"].asString();
		string gc = sys->utf2ascii(c.c_str());
		int len = strlen(gc.c_str());
		memcpy(osdInfo->sChanName, gc.c_str(), len);
		osdInfo->struOsdColor.byRed = rvalue["byRed"].asInt();
		osdInfo->struOsdColor.byGreen = rvalue["byGreen"].asInt();
		osdInfo->struOsdColor.byBlue = rvalue["byBlue"].asInt();
		osdInfo->wShowNameTopLeftX = rvalue["name_x"].asInt();
		osdInfo->wShowNameTopLeftY = rvalue["name_y"].asInt();
		osdInfo->wOSDTopLeftX = rvalue["osd_x"].asInt();
		osdInfo->wOSDTopLeftY = rvalue["osd_y"].asInt();
		osdInfo->byFontSize = rvalue["fonttype"].asInt();
		osdInfo->byOSDAttrib = rvalue["attribtype"].asInt();
		osdInfo->dwShowChanName = rvalue["ShowChanName"].asUInt();
		osdInfo->dwShowOsd = rvalue["ShowOsd"].asUInt();
		osdInfo->byOSDColorType = 1;
		if (setOSDInfo(chnNo, osdInfo) != 0)
		{
			delete osdInfo;
			return -1;
		}

		delete osdInfo;
		return 0;
	}
	else if ("stopmeasuretemp" == jMethod)
	{
		return StopMeaTemperature(json);
	}

    ERRE_MSV("method name error");
    return -1;
}

int 
HikPu::lockRecordFileByTime(int channelNo, string &startTime, string &stopTime, int duration)
{
    if (-1 == m_userId)
    {
        return -1;
    }

    int startYear, startMonth, startDay, startHour, startMinute, startSecond;
    int stopYear, stopMonth, stopDay, stopHour, stopMinute, stopSecond;

    sscanf(startTime.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &startYear, &startMonth, &startDay, &startHour, &startMinute, &startSecond);
    sscanf(stopTime.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &stopYear, &stopMonth, &stopDay, &stopHour, &stopMinute, &stopSecond);

    NET_DVR_STREAM_TIME_LOCK streamTimeLock = { 0 };
    streamTimeLock.dwSize = sizeof(streamTimeLock);

    streamTimeLock.strBeginTime.dwYear    = startYear;
    streamTimeLock.strBeginTime.dwMonth   = startMonth;
    streamTimeLock.strBeginTime.dwDay     = startDay;
    streamTimeLock.strBeginTime.dwHour    = startHour;
    streamTimeLock.strBeginTime.dwMinute  = startMinute;
    streamTimeLock.strBeginTime.dwSecond  = startSecond;

    streamTimeLock.strEndTime.dwYear      = stopYear;
    streamTimeLock.strEndTime.dwMonth     = stopMonth;
    streamTimeLock.strEndTime.dwDay       = stopDay;
    streamTimeLock.strEndTime.dwHour      = stopHour;
    streamTimeLock.strEndTime.dwMinute    = stopMinute;
    streamTimeLock.strEndTime.dwSecond    = stopSecond;

    streamTimeLock.dwLockDuration = duration;

    streamTimeLock.struStreamInfo.dwSize = sizeof(streamTimeLock.struStreamInfo);
    streamTimeLock.struStreamInfo.dwChannel = channelNo;

    NET_DVR_LOCK_RETURN lockReturn = { 0 };
    lockReturn.dwSize = sizeof(lockReturn);

    BOOL bRes = NET_DVR_LockStreamFileByTime(m_userId, &streamTimeLock, &lockReturn);

    if (FALSE == bRes)
    {
        int err = NET_DVR_GetLastError();
        ERRE_MSV("NET_DVR_LockStreamFileByTime failed, errno =%d",err);
    }

    return (bRes) ? 0 : -1;
}

int
HikPu::onRequestConfig(string& json, string& jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    string jMethod = rvalue["Method"].asString();

    if (jMethod == "getPrivacyMask")
    {
        NET_DVR_PICCFG_V40 *piccfg_v40 = new NET_DVR_PICCFG_V40;
        int    chnNo = rvalue["chnNo"].asInt();
        if (0 != getPrivacyMask(chnNo, piccfg_v40))
        {
            delete piccfg_v40;
            return -1;
        }


        SHL_Json::Value shelter;
        SHL_Json::Value array;

        for (int i = 0; i < MAX_SHELTERNUM; ++i)
        {
            SHL_Json::Value value;
            value["X"] = piccfg_v40->struShelter[i].wHideAreaTopLeftX;
            value["Y"] = piccfg_v40->struShelter[i].wHideAreaTopLeftY;
            value["w"] = piccfg_v40->struShelter[i].wHideAreaWidth;
            value["h"] = piccfg_v40->struShelter[i].wHideAreaHeight;

            array.append(value);
        }

        shelter["enable"] = (int)piccfg_v40->dwEnableHide;
        shelter["shelter"] = array;

        SHL_Json::StreamWriterBuilder writer;
        jout = SHL_Json::writeString(writer, shelter);
        delete piccfg_v40;

        return 0;
    }
    else if (jMethod == "getShowString")
    {
        NET_DVR_SHOWSTRING_V30* showstring = new NET_DVR_SHOWSTRING_V30;
        int    chnNo = rvalue["chnNo"].asInt();
        if (getShowString(chnNo, showstring) != 0)
        {
            delete showstring;
            return -1;
        }

        SHL_Json::Value shelter;
        SHL_Json::Value array;

        for (int i = 0; i < MAX_STRINGNUM_V30; ++i)
        {
            SHL_Json::Value value;
            value["bShowString"] = showstring->struStringInfo[i].wShowString;
            value["stringLength"] = showstring->struStringInfo[i].wStringSize;
            value["X"] = showstring->struStringInfo[i].wShowStringTopLeftX;
            value["Y"] = showstring->struStringInfo[i].wShowStringTopLeftY;

            string c = sys->ascii2utf8(showstring->struStringInfo[i].sString);
            value["content"] = c;
            array.append(value);
        }
        shelter["items"] = array;
        shelter["ItemCount"] = MAX_STRINGNUM_V30;

        SHL_Json::StreamWriterBuilder writer;
        jout = SHL_Json::writeString(writer, shelter);

        delete showstring;
        return 0;
    }
    else if (jMethod == "getVideoEffect")
    {
        DWORD bright = 0;
        DWORD contrast = 0;
        DWORD saturation = 0;
        DWORD hue = 0;
        int   chnNo = rvalue["nChn"].asInt();
        int ret = getVideoEffect(chnNo, &bright, &contrast, &saturation, &hue);
        if (ret == 0)
        {
            SHL_Json::Value value;
            value["bright"] = (int)bright;
            value["contrast"] = (int)contrast;
            value["saturation"] = (int)saturation;
            value["hue"] = (int)hue;

            SHL_Json::StreamWriterBuilder writer;
            jout = SHL_Json::writeString(writer, value);
        }
        return ret;
    }
    else if (jMethod == "getPtzCurise")
    {
        int   chnNo = rvalue["chnNo"].asInt();
        int   curiseRoute = rvalue["curiseRoute"].asInt();

        NET_DVR_CRUISEPOINT_V50 *struCruisePointV50 = new NET_DVR_CRUISEPOINT_V50;
        memset(struCruisePointV50, 0, sizeof(NET_DVR_CRUISEPOINT_V50));
        int ret = -1;
        if (0 != getPtzCruise(chnNo, curiseRoute, struCruisePointV50))
        {
            ret = -1;
        }
        else
        {
            SHL_Json::Value value;
            SHL_Json::Value array;

            for (int i = 0; i < MAX_CRUISEPOINT_NUM_V50; ++i)
            {
                SHL_Json::Value cruisePoint;
                if (struCruisePointV50->struCruisePoint[i].wPresetNo == 0)
                {
                    continue;
                }

                cruisePoint["PresetNo"] = struCruisePointV50->struCruisePoint[i].wPresetNo;
                cruisePoint["Dwell"] = struCruisePointV50->struCruisePoint[i].wDwell;
                cruisePoint["bySpeed"] = struCruisePointV50->struCruisePoint[i].bySpeed;
                array.append(cruisePoint);
            }
            value["cruisePoints"] = array;
            value["ItemCount"] = array.size();

            SHL_Json::StreamWriterBuilder writer;
            jout = SHL_Json::writeString(writer, value);

            ret = 0;
        }

        delete struCruisePointV50;
        return ret;
    }
    else if (jMethod == "getRecordLabel")
    {
        int   chnNo = rvalue["nChn"].asInt();
        datetime_s st;
        st.year = rvalue["year_s"].asInt();
        st.month = rvalue["month_s"].asInt();
        st.day = rvalue["day_s"].asInt();
        st.hour = rvalue["hour_s"].asInt();
        st.minute = rvalue["minute_s"].asInt();
        st.second = rvalue["second_s"].asInt();
        datetime_s et;
        et.year = rvalue["year_e"].asInt();
        et.month = rvalue["month_e"].asInt();
        et.day = rvalue["day_e"].asInt();
        et.hour = rvalue["hour_e"].asInt();
        et.minute = rvalue["minute_e"].asInt();
        et.second = rvalue["second_e"].asInt();
        int ret = getRecordLabel(chnNo, st, et, jout);

        return ret;
    }
    else if (jMethod == "getRelationDev")
    {
        int   chnNo = rvalue["chnNo"].asInt();
        string guaddrcode = gucode(chnNo);

        std::string devjson = "";
        char cond[128] = { 0 };
        snprintf(cond, 128, "addrcode = '%s'", guaddrcode.c_str());
        std::string con(cond);
        deviceCfg->query(devjson, con);


        SHL_Json::Value  devRvalue;
        SHL_Json::StreamWriterBuilder writer;

        //string gbkDevJson = sys->utf2ascii(devjson);

        if (!(reader.parse(devjson, devRvalue, false) && !devRvalue.isNull()))
        {
            return -1;
        }

        string reTrackCode = devRvalue[0]["reTrackCode"].asString();
        if (reTrackCode == "")
        {
            jout = "";
        }
        else
        {
            memset(cond, 0, 128);
            snprintf(cond, 128, "addrcode = '%s'", reTrackCode.c_str());
            string tmpTrackJson;
            deviceCfg->query(tmpTrackJson, cond);
            SHL_Json::Value value;
            SHL_Json::Value trackValue;
            if (!(reader.parse(tmpTrackJson, trackValue, false) && !trackValue.isNull()))
            {
                return -1;
            }




            value["addrcode"] = trackValue[0]["addrcode"].asString();
            value["name"] = trackValue[0]["name"].asString();
            value["host"] = trackValue[0]["host"].asString();
            value["port"] = trackValue[0]["port"].asInt();
            value["chnNo"] = trackValue[0]["chnNo"].asInt();


            jout = SHL_Json::writeString(writer, value);
        }

        return 0;

    }
    else if (jMethod == "getPtzParkaction")
    {
        int chnNo = rvalue["DevChn"].asInt();

        int enable = 0;
        int parkTime = 0;
        int actionType = 0;
        int id = 0;


        if (0 == getPtzParkaction(chnNo, enable, parkTime, actionType, id))
        {
            SHL_Json::Value value;
            value["enable"] = enable;
            value["parkTime"] = parkTime;
            value["actionType"] = actionType;
            value["id"] = id;

            SHL_Json::StreamWriterBuilder writer;
            jout = SHL_Json::writeString(writer, value);

            return 0;
        }
    }
	else if ("getRecordWorkStatus" == jMethod)
	{
		NET_DVR_GETWORKSTATE_COND cond = { 0 };
		cond.dwSize = sizeof(cond);

		NET_DVR_WORKSTATE_V40 workStatus = { 0 };
        char *status = new char[10 * 1024];
        if (NULL == status)
        {
            return -1;
        }
        BOOL bReturn = NET_DVR_GetDeviceConfig(m_userId, NET_DVR_GET_WORK_STATUS, 0, &cond, sizeof(cond), status, &workStatus, sizeof(workStatus));
		if (!bReturn)
		{
			return -1;
		}
        delete[] status;

		SHL_Json::Value array;
		array["Method"]  = "getRecordWorkStatus";
		array["DevIp"]   = m_ip;
		array["DevPort"] = m_port;

		for (int i = 0; i < MAX_CHANNUM_V40; i++)
		{
			if (0xffffffff == workStatus.struChanStatic[i].dwChannelNo)//current and subsequent values are invalid
			{
				break;
			}
            int chanNo = workStatus.struChanStatic[i].dwChannelNo;
			SHL_Json::Value ele;
            ele["chnNo"] = chanNo;   // not consider dvr
			ele["recordStatus"] = workStatus.struChanStatic[i].byRecordStatic;
			array["item"].append(ele);
		}
		if (0==array.size())
		{
			jout = "";
			return 0;
		}
		SHL_Json::StreamWriterBuilder writer;
		jout = SHL_Json::writeString(writer,array);
		return 0;
		
	}
    else if (jMethod == "GetChannelStatus")
    {
        if (0 == onGetChannelStatus(json, jout))
        {
            return 0;
        }
    }
	else if (jMethod == "GetChannelStatusAll")
	{
		if (0 == onGetChannelStatus_all(json, jout))
		{
			return 0;
		}
	}
    else if ("getPTZPos" == jMethod)
    {
        return onGetPTZPos(json, jout);
    }
    else if ("getThermometryResult" == jMethod)
    {
        return onGetThermometryResult(json, jout);
    }

	else if (jMethod == "CaptureThermalFile")
	{
		int ret = captureThermalFile(json, jout);
//		int ret =  remoteCapture(json, jout);
		return ret;
	}
	else if (jMethod == "getOSDInfo") 
	{
		NET_DVR_PICCFG_V40 * getOSDIfo = new NET_DVR_PICCFG_V40;
		int    chnNo = rvalue["chnNo"].asInt();
		if (getOSDIfoCor(chnNo, getOSDIfo) != 0)
		{
			delete getOSDIfo;
			return -1;
		}
		SHL_Json::Value shelter;
		SHL_Json::Value array;


		SHL_Json::Value value;
		value["OSD_X"]			= getOSDIfo->wOSDTopLeftX;
		value["OSD_Y"]			= getOSDIfo->wOSDTopLeftY;
		value["name_x"]			= getOSDIfo->wShowNameTopLeftX;
		value["name_y"]			= getOSDIfo->wShowNameTopLeftY;
		value["FontSize"]		= getOSDIfo->byFontSize;
		value["byOSDAttrib"]	= getOSDIfo->byOSDAttrib;
		value["dwShowChanName"] = (SHL_Json::UInt64)getOSDIfo->dwShowChanName;
		value["dwShowOsd"]	    = (SHL_Json::UInt64)getOSDIfo->dwShowOsd;

		//array.append(value);
		//shelter["items"] = array;

		SHL_Json::StreamWriterBuilder writer;
		jout = SHL_Json::writeString(writer, value);
		delete getOSDIfo;
		return 0;
	}
	 else if ("CapturePicture" == jMethod)
    {
        return capturePic(json,jout);
    }
	else if("getPtzStatus" == jMethod)
	{

		SHL_Json::Reader     reader;

		if (!reader.parse(json, rvalue, false))
		{
			return -1;
		}
		int  channelNo = rvalue["chnNo"].asInt();

		return getPtzStatus(channelNo, jout);
	}
	else if ("startmeasuretemp" == jMethod)
	{
		return StartMeaTemperature(json, jout);
	}
	else if ("getmeasuretemp" == jMethod)
	{
		return getMeaTemperature(json, jout);
	}
	else if ("getrealpicture" == jMethod)
	{
		return remoteCapture(json, jout);

	}
    return -1;
}

int
HikPu::getOSDIfoCor(int chnNo, NET_DVR_PICCFG_V40 *showString)
{
	DWORD dwReturned = 0;
	memset(showString, 0, sizeof(NET_DVR_PICCFG_V40));
	if (!NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_PICCFG_V40, chnNo, showString, sizeof(NET_DVR_PICCFG_V40), &dwReturned))
	{
		int error = NET_DVR_GetLastError();
		ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_SHOWSTRING_V30 failed, errno:%d", NET_DVR_GetLastError());
		return -1;
	}
	return 0;
}

int
HikPu::getAbilityPlate()
{
    /* ������������DVR���û�ȡ�豸������ֱ�Ӳ�����! */

    if (Pu::DEV_TYPE_IPC == m_type)
    {
        /**< ͨ���ж�ITDeviceAbility������(���ܽ�ͨ��Ʒ�������ܽ�ͨ��Ʒ)*/
        char ITCAbility[50 * 1024] = { 0 };
        char sendBuf[128] = { 0 };
        strcpy(sendBuf, "<ITDeviceAbility version=\"2.0\"><channelNO>1</channelNO></ITDeviceAbility>");
        NET_DVR_GetDeviceAbility(m_userId, DEVICE_ABILITY_INFO, sendBuf, 128, ITCAbility, 50 * 1024);

        if (strncmp(ITCAbility, "", 50 * 1024) != 0)
        {
            string tmpJson = "";
            xml2json(ITCAbility, tmpJson);
            SHL_Json::Reader reader;
            SHL_Json::Value  rvalue;

            if (!reader.parse(tmpJson, rvalue, false))
            {
                return -1;
            }
            if (rvalue["ITDeviceAbility"]["ITCAbility"]["ITCFTP"].isNull())
            {
                return 0;
            }
            else
            {
                m_manuPlateType = TriggerListener::PLATE_HIK_STARNDARD;
            }
        }
    }
    return 0;
}

int
HikPu::startListenCall()
{
    char s8ACSAbility[50 * 1024] = { 0 };
    char s8SendBuf[128] = { 0 };

    strcpy(s8SendBuf, "<AcsAbility version=\"2.0\" />");
    BOOL bReturn = NET_DVR_GetDeviceAbility(m_userId, ACS_ABILITY, s8SendBuf, 128, s8ACSAbility, 50 * 1024);
    if (!bReturn)
    {
        return 0;
    }

    if (-1 != m_remoteCtrlId)
    {
        NET_DVR_StopRemoteConfig(m_remoteCtrlId);
        m_remoteCtrlId = -1;
    }

    if (-1 == m_remoteCtrlId)
    {
        NET_DVR_VIDEO_CALL_COND  callCond = { 0 };
        memset(&callCond, 0, sizeof(callCond));
        callCond.dwSize = sizeof(callCond);

        m_remoteCtrlId = NET_DVR_StartRemoteConfig(m_userId, NET_DVR_VIDEO_CALL_SIGNAL_PROCESS, &callCond, sizeof(callCond), callData_cb, this);
        if (m_remoteCtrlId < 0)
        {
            DWORD dwRet = NET_DVR_GetLastError();
            m_callStatus = 2;
            return -1;
        }

        m_callStatus = 0;
    }

    return 0;
}

int
HikPu::doCallAnswer(uint32_t u32Cmd)
{

    if (-1 == m_remoteCtrlId)
    {
        return -1;
    }

    if ((u32Cmd != 2) &&
        (u32Cmd != 3) &&
        (u32Cmd != 5))
    {
        return  -1;
    }

    NET_DVR_VIDEO_CALL_PARAM callParam = { 0 };
    callParam.dwSize = sizeof(callParam);
    callParam.dwCmdType = u32Cmd;
    int sssss = NET_DVR_SendRemoteConfig(m_remoteCtrlId, 0, (char*)&callParam, sizeof(callParam));
    LOGE_MSV("NET_DVR_SendRemoteConfig : %d", sssss);

    if (2 == u32Cmd)
    {
        m_callStatus = 1;
    }
    else
    {
        m_callStatus = 0;
    }
    return 0;
}



int
HikPu::getPrivacyMask(int chnNo, NET_DVR_PICCFG_V40* piccfg)
{
    DWORD returnLen = 0;

    memset(piccfg, 0, sizeof(NET_DVR_PICCFG_V40));
    piccfg->dwSize = sizeof(NET_DVR_PICCFG_V40);

    if (!NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_PICCFG_V40, chnNo, piccfg, sizeof(NET_DVR_PICCFG_V40), &returnLen))
    {
        ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_PICCFG_V40 failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;

}

int
HikPu::setPrivacyMask(int chnNo, int enable, NET_VCA_POINT* shelter, int sheltercnt)
{
#if 0
    int ret = -1;
    DWORD returnLen = 0;

    NET_DVR_PICCFG_V40 *piccfg_v40 = new NET_DVR_PICCFG_V40;
    if (0 != getPrivacyMask(chnNo, piccfg_v40))
    {
        delete piccfg_v40;
        return -1;
    }


    piccfg_v40->dwEnableHide = enable;
    for (int i = 0; i < sheltercnt; ++i)
    {
        if (i >= 4)
        {
            break;
        }
        memcpy(&piccfg_v40->struShelter[i], shelter + i, sizeof(NET_DVR_SHELTER));
    }


    if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PICCFG_V40, chnNo, piccfg_v40, sizeof(NET_DVR_PICCFG_V40)))
    {
        ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_PICCFG_V40 failed, errno:%d", NET_DVR_GetLastError());
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    delete piccfg_v40;
    return ret;
#endif

    NET_DVR_PRIVACY_MASKS_COND privacyMaskCond = { 0 };
    memset(&privacyMaskCond, 0, sizeof(NET_DVR_PRIVACY_MASKS_COND));

    privacyMaskCond.dwSize = sizeof(privacyMaskCond);
    privacyMaskCond.byRegionalID = 1;
    privacyMaskCond.dwChan = chnNo + 1;


    NET_DVR_PRIVACY_MASKS_CFG privacyMaskCfg = { 0 };
    memset(&privacyMaskCfg, 0, sizeof(NET_DVR_PRIVACY_MASKS_CFG));

    privacyMaskCfg.dwSize = sizeof(privacyMaskCfg);
    privacyMaskCfg.byEnable = 1;
    privacyMaskCfg.byActiveZoomRatio = 1;
    privacyMaskCfg.byCurrentRegionEnable = 1;
    privacyMaskCfg.byColorType = 1;

    privacyMaskCfg.struRegion.dwPointNum = 4;
    memcpy(privacyMaskCfg.struRegion.struPos, shelter, sizeof(privacyMaskCfg.struRegion.struPos));
    int ret = 0;
    DWORD *pStatus = new DWORD;
    if (!NET_DVR_SetDeviceConfig(m_userId, NET_DVR_SET_PRIVACY_MASKSCFG, 1, &privacyMaskCond, sizeof(NET_DVR_PRIVACY_MASKS_COND),
        pStatus, &privacyMaskCfg, sizeof(NET_DVR_PRIVACY_MASKS_CFG)))
    {
        ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_PRIVACY_MASKSCFG failed, errno:%d", NET_DVR_GetLastError());
        ret = NET_DVR_GetLastError();
    }

    NET_DVR_PRIVACY_MASKS_ENABLECFG param = { 0 };
    param.dwSize = sizeof(param);
    param.byEnable = enable;

    if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PRIVACY_MASKS_ENABLECFG, chnNo + 1,
        &param, sizeof(param)))
    {
        ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_PRIVACY_MASKS_ENABLECFG failed, errno:%d", NET_DVR_GetLastError());
        ret = NET_DVR_GetLastError();
    }

    return ret;
}

int
HikPu::getShowString(int chnNo, NET_DVR_SHOWSTRING_V30 *showString)
{
    DWORD dwReturned = 0;
    memset(showString, 0, sizeof(NET_DVR_SHOWSTRING_V30));
	BOOL ret = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_SHOWSTRING_V30, chnNo, showString, sizeof(NET_DVR_SHOWSTRING_V30), &dwReturned);
    if (!ret)
    {
		int error = NET_DVR_GetLastError();
        ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_SHOWSTRING_V30 failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;
}

int
HikPu::setShowString(int chnNo, NET_DVR_SHOWSTRING_V30 *showString)
{
    if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_SHOWSTRING_V30, chnNo, showString, sizeof(NET_DVR_SHOWSTRING_V30)))
    {
        ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_SHOWSTRING_V30 failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;
}

int
HikPu::setOSDInfo(int chnNo, NET_DVR_PICCFG_V30 *showString)
{
	if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PICCFG_V30, chnNo, showString, sizeof(NET_DVR_PICCFG_V40)))
	{
		int error = NET_DVR_GetLastError();
		ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_SHOWSTRING_V30 failed, errno:%d", NET_DVR_GetLastError());
		return -1;
	}
	return 0;
}

int
HikPu::ptzPostion3D(int chnNo, NET_DVR_POINT_FRAME* netDvrPtFrame)
{

    if (netDvrPtFrame->xTop < netDvrPtFrame->xBottom)
    {
        if (netDvrPtFrame->yTop < netDvrPtFrame->yBottom)
        {
            netDvrPtFrame->bCounter = 4;
        }
        else
        {
            netDvrPtFrame->bCounter = 2;
        }
    }
    else
    {
        if (netDvrPtFrame->yTop < netDvrPtFrame->yBottom)
        {
            netDvrPtFrame->bCounter = 3;
        }
        else
        {
            netDvrPtFrame->bCounter = 1;
        }
    }

    BOOL bRet = NET_DVR_PTZSelZoomIn_EX(m_userId, chnNo + 1, netDvrPtFrame);
    return (bRet == TRUE ? 0 : -1);
}

int
HikPu::getPtzCruise(int chnNo, int cruiseRoute, NET_DVR_CRUISEPOINT_V50* struCruisePointV50)
{
    int iStatus = 0;
    NET_DVR_CRUISEPOINT_COND struCruisePointCond = { 0 };
    memset(&struCruisePointCond, 0, sizeof(NET_DVR_CRUISEPOINT_COND));
    struCruisePointCond.dwSize = sizeof(NET_DVR_CRUISEPOINT_COND);
    struCruisePointCond.dwChan = chnNo;
    struCruisePointCond.wRouteNo = cruiseRoute;

    BOOL bRet = NET_DVR_GetDeviceConfig(m_userId, NET_GET_CRUISEPOINT_V50, 1, &struCruisePointCond,
        sizeof(struCruisePointCond), &iStatus, struCruisePointV50, sizeof(NET_DVR_CRUISEPOINT_V50));
    if (bRet && iStatus == 0)
    {
        return 0;
    }
    else
    {
        ERRE_MSV("NET_DVR_GetDeviceConfig NET_GET_CRUISEPOINT_V50 failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
}

int
HikPu::addPtzCruise(int chnNo, int cruiseRoute, int seq, int presetNum, int dwell, int speed)
{
    BOOL bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, FILL_PRE_SEQ, cruiseRoute, seq, presetNum);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other FILL_PRE_SEQ failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }

    bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, SET_SEQ_DWELL, cruiseRoute, seq, dwell);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other SET_SEQ_DWELL failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }

    bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, SET_SEQ_SPEED, cruiseRoute, seq, speed);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other SET_SEQ_SPEED failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }

    return 0;
}

int
HikPu::delPtzCruise(int chnNo, int cruiseRoute, int seq, int presetNum)
{
    BOOL bRet;
    bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, CLE_PRE_SEQ, cruiseRoute, seq, presetNum);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other CLE_PRE_SEQ failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;
}

int
HikPu::enablePtzCruise(int chnNo, int cruiseRoute, bool bStart)
{
    DWORD cmd = 0;
    if (bStart)
        cmd = RUN_SEQ;
    else
        cmd = STOP_SEQ;

    BOOL bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, cmd, cruiseRoute, 0, 0);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other RUN_SEQ failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;
}

int
HikPu::delPtzCruiseRoute(int chnNo, int curiseID)
{
    BOOL bRet = NET_DVR_PTZCruise_Other(m_userId, chnNo, DEL_SEQ, curiseID, 0, 0);
    if (!bRet)
    {
        ERRE_MSV("NET_DVR_PTZCruise_Other DEL_SEQ failed, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
    return 0;
}

int HikPu::getVideoEffect(int channelNo, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue)
{
    BOOL res = true;

    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
    }

    res = NET_DVR_GetVideoEffect(m_userId, channelNo, pBrightValue, pContrastValue, pSaturationValue, pHueValue);

    return res ? 0 : -1;

}

int HikPu::setVideoEffect(int channelNo, DWORD brightValue, DWORD contrastValue, DWORD saturationValue, DWORD hueValue)
{
    BOOL res = true;

    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
    }

    res = NET_DVR_SetVideoEffect(m_userId, channelNo, brightValue, contrastValue, saturationValue, hueValue);

    return res ? 0 : -1;
}

int HikPu::getVideoEffect(string addrcode, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue)
{
    //addrcode --- 18 bit char , start pointer add 16 
	int channelNo = GetChanNoByCode(addrcode);

    return getVideoEffect(channelNo, pBrightValue, pContrastValue, pSaturationValue, pHueValue);
}

int HikPu::setVideoEffect(string addrcode, DWORD brightValue, DWORD contrastValue, DWORD saturationValue, DWORD hueValue)
{
    //addrcode --- 18 bit char , start pointer add 16 
	int channelNo = GetChanNoByCode(addrcode);

    return setVideoEffect(channelNo, brightValue, contrastValue, saturationValue, hueValue);
}

int HikPu::lockRecordFile(string fileName, bool lock)
{
    BOOL res = true;

    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
    }
    if (lock)
    {
        res = NET_DVR_LockFileByName(m_userId, (char*)fileName.c_str());
    }
    else
    {
        res = NET_DVR_UnlockFileByName(m_userId, (char*)fileName.c_str());
    }

    return res ? 0 : -1;
}

int HikPu::getRecordLabel(int channelNo, datetime_s &start, datetime_s &end, string &json)
{

    SHL_Json::Value value;
    SHL_Json::Value array;

    NET_DVR_FIND_LABEL struFindLabel = { 0 };
    struFindLabel.lChannel = channelNo;

    struFindLabel.struStartTime.dwYear = start.year;
    struFindLabel.struStartTime.dwMonth = start.month;
    struFindLabel.struStartTime.dwDay = start.day;
    struFindLabel.struStartTime.dwHour = start.hour;
    struFindLabel.struStartTime.dwMinute = start.minute;
    struFindLabel.struStartTime.dwSecond = start.second;

    struFindLabel.struStopTime.dwYear = end.year;
    struFindLabel.struStopTime.dwMonth = end.month;
    struFindLabel.struStopTime.dwDay = end.day;
    struFindLabel.struStopTime.dwHour = end.hour;
    struFindLabel.struStopTime.dwMinute = end.minute;
    struFindLabel.struStopTime.dwSecond = end.second;
    struFindLabel.byDrawFrame = 0;

    LONG lFindId = NET_DVR_FindRecordLabel(m_userId, &struFindLabel);
    if (-1 == lFindId)
    {
        ERRE_MSV("NET_DVR_FindRecordLabel err: %d", NET_DVR_GetLastError());

        return -1;
    }

    int index = 0;
    while (1)
    {
        NET_DVR_FINDLABEL_DATA struFindData = { 0 };

        LONG res = NET_DVR_FindNextLabel(lFindId, &struFindData);
        if (res == NET_DVR_FILE_SUCCESS)
        {
            index++;
            if (index < 1 || index > 100)
            {
                continue;
            }

            SHL_Json::Value record;
            record["name"] = struFindData.sLabelName;
            record["year"] = (int)struFindData.struTimeLabel.dwYear;
            record["month"] = (int)struFindData.struTimeLabel.dwMonth;
            record["day"] = (int)struFindData.struTimeLabel.dwDay;
            record["hour"] = (int)struFindData.struTimeLabel.dwHour;
            record["minute"] = (int)struFindData.struTimeLabel.dwMinute;
            record["second"] = (int)struFindData.struTimeLabel.dwSecond;
            record["channelNo"] = (int)channelNo;
            record["labelid"] = struFindData.struLabelIdentify.sLabelIdentify;

            array.append(record);
        }
        else if (res == NET_DVR_ISFINDING)
        {
            sys->sleep(1);
            continue;
        }
        else if ((res == NET_DVR_NOMOREFILE) || (res == NET_DVR_FILE_NOFIND))
        {
            NET_DVR_StopFindLabel(lFindId);
            break;
        }
        else
        {
            NET_DVR_StopFindLabel(lFindId);
            return -1;
        }
    }

    value["records"] = array;
    value["realNum"] = index;

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);

    return 0;
}

int
HikPu::getEntance(NET_DVR_ENTRANCE_CFG *pStruEntrance)
{
    NET_DVR_BARRIERGATE_COND struGateCond = { 0 };  //出入口控制参数配置条件
    struGateCond.byLaneNo = 1;               //闸机号
    //int dwStatus =0;

    DWORD *pStatus = new DWORD[1];
    *pStatus = 0;
    BOOL ret = NET_DVR_GetDeviceConfig(m_userId, NET_DVR_GET_ENTRANCE_PARAMCFG, 1, &struGateCond, sizeof(NET_DVR_BARRIERGATE_COND), pStatus, pStruEntrance, sizeof(NET_DVR_ENTRANCE_CFG));
    DWORD dwStatus = *pStatus;
    delete[] pStatus;
    if (ret)
    {
        if (dwStatus != 0)
        {
            ERRD_MSV("NET_DVR_GetDeviceConfig NET_DVR_GET_ENTRANCE_PARAMCFG(getEntance dwStatus = %d) failed:%d", dwStatus,NET_DVR_GetLastError());
            return FALSE;
        }
        //AfxMessageBox("获取出入口参数成功!");
    }
    else
    {
        ERRD_MSV("NET_DVR_GetDeviceConfig NET_DVR_GET_ENTRANCE_PARAMCFG(getEntance ) failed:%d",  NET_DVR_GetLastError());
        //ErrorNum.Format("获取出入口参数失败, 错误号: %d\n", NET_DVR_GetLastError());
        //AfxMessageBox(ErrorNum);
        return FALSE;
    }
    return TRUE;
}
int
HikPu::setEntance(NET_DVR_ENTRANCE_CFG *pStruEntrance, BYTE ctrlMode)
{
    pStruEntrance->byEnable = 0;// 使能：0- 关闭，1- 打开
    pStruEntrance->byBarrierGateCtrlMode = ctrlMode;// 道闸控制模式：0- 相机自动控制，1- 平台外部控制
    NET_DVR_BARRIERGATE_COND struGateCond = { 0 };  // 出入口控制参数配置条件
    struGateCond.byLaneNo = 1;                // 闸机号

    DWORD *pStatus = new DWORD[1];
    *pStatus = 0;
    int ret = NET_DVR_SetDeviceConfig(m_userId, NET_DVR_SET_ENTRANCE_PARAMCFG, 1, &struGateCond, sizeof(NET_DVR_BARRIERGATE_COND), pStatus, pStruEntrance, sizeof(NET_DVR_ENTRANCE_CFG));
    DWORD dwStatus = *pStatus;
    delete[] pStatus;
    if (!ret)
    {
        ERRD_MSV("NET_DVR_SetDeviceConfig NET_DVR_SET_ENTRANCE_PARAMCFG(setEntance ctrlMode=%d) failed:%d", ctrlMode, NET_DVR_GetLastError());
        return FALSE;
    }
    else
    {
        if (dwStatus != 0)
        {
            ERRD_MSV("NET_DVR_SetDeviceConfig NET_DVR_SET_ENTRANCE_PARAMCFG(setEntance ctrlMode=%d,dwStatus =%d) failed:%d", ctrlMode, dwStatus, NET_DVR_GetLastError());
            return FALSE;
        }
    }
    return TRUE;
}
void CALLBACK vehicle_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
    char szLan[128] = { 0 };
    HikPu * pu = (HikPu *)pUserData;
    int  resp = 0;

    if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
    {
        DWORD dwStatus = *(DWORD *)lpBuffer;
        if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
        {
            //AfxMessageBox("设备发送信息成功");
            LOGE_MSV("set vrhicle balck and white list success");
            resp = TRUE;
            pu->m_evtSetVihicle.notify(STRVEHICLE_C, resp);
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING)
        {
            LOGE_MSV("set vrhicle balck and white list doing");
            //AfxMessageBox("设备发送信息处理中");
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
        {
            LOGE_MSV("set vrhicle balck and white list failed");
            resp = FALSE;
            pu->m_evtSetVihicle.notify(STRVEHICLE_C, resp);
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION)
        {
            LOGE_MSV("set vrhicle balck and white list exception");
            //AfxMessageBox("设备返回异常");
            resp = FALSE;
            pu->m_evtSetVihicle.notify(STRVEHICLE_C, resp);
        }
    }
    return;
}

void HikPu::getResolution(int index,int& height,int& weight)
{

	if (index == 19)
	{
		height = 720;
		weight = 1280;
	}
	else if (index == 3)
	{
		height = 576;
		weight = 704;
	}
	else
	{
		height = 1080;
		weight = 1920;
	}


}

int                
HikPu::capturePic(string json,string& jout)
{
    if (m_userId < 0)
    {
        return FALSE;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    
    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return FALSE;
    }

    int chanNo = rvalue["chnNo"].asInt();
    int devPort = rvalue["DevPort"].asInt();
    string devIp = "";
    if (!rvalue["DevIp"].isNull())
    {
        devIp = rvalue["DevIp"].asString();
    }

    int width = 1;
    int height = 1;
    int picSize = getPicSize(chanNo+1);
    /*if (picSize < 0)
    {
        ERRE_MSV("HikPu::capturePic failed! getPicSize() failed:addrcode = %s", addrcode().c_str());
        return -1;
    }
	getResolution(picSize, height, width);*/
    NET_DVR_JPEGPARA struJpegPara;
    memset(&struJpegPara, 0, sizeof(NET_DVR_JPEGPARA));
    struJpegPara.wPicQuality = 0;
	struJpegPara.wPicSize = 0xff;
    int iChanShowNum = chanNo+1; //iChanIndex + g_struDeviceInfo[iDeviceIndex].iStartChan;
	
#ifdef WIN32
     string tmpplatePath = sys->utf2ascii(systm->facePath());
#else
    string tmpplatePath = systm->facePath();
#endif

    string strCurTime = sys->currentTime();
    int pos = strCurTime.find(' ');
    if (pos >= 0)
    {
        strCurTime.replace(pos, 1, "_");
    }

    for (int i = 0; i < 2; i++)
    {
        pos = strCurTime.find(':');
        if (pos >= 0)
        {
            strCurTime.replace(pos, 1, "-");
        }
    }

    char cFilename[256] = { 0 };
    sprintf(cFilename, "%s/JPEG_%s.jpg", \
        tmpplatePath.c_str(),   strCurTime.c_str());

    BOOL ret = NET_DVR_CaptureJPEGPicture(m_userId, iChanShowNum, &struJpegPara, cFilename);
    if (TRUE == ret)
    {
        SHL_Json::Value      value;
		SHL_Json::StreamWriterBuilder writer;

        value["version"] = "1.0";
        value["time"] = sys->currentTime();
        value["deviceName"] = name();
        value["deviceHost"] = host();
        value["devicePort"] = port();
        value["addrcode"] = addrcode();
		value["picpath"]  = cFilename;
		value["channelNo"] = chanNo;
		value["picSize"]  = struJpegPara.wPicSize;
		//读取图片文件
		ifstream is(cFilename, ifstream::in | ios::binary);
		is.seekg(0, is.end);
		int length = is.tellg();
		is.seekg(0,is.beg);
		char *buffer = new char[length];
		is.read(buffer, length);

		GetSize((unsigned char *)buffer, length, height, width);
		value["width"] = width;
		value["height"] = height;
		string picData = base64_encode((const unsigned char *)buffer, length);
		value["picData"] = picData;
		if (buffer)
			delete[] buffer;
		jout = SHL_Json::writeString(writer, value);
        return 0;
    }
    else
    {
		int error = NET_DVR_GetLastError();
        ERRE_MSV("NET_DVR_CaptureJPEGPicture failed,errorCode:%d", NET_DVR_GetLastError());
        return -1;
    }
    return -1;
}
int 
HikPu::getPtzStatus(int channelNo, string& jout)
{
	SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value  rvalue;
	int status;
	int channel = channelNo - 1;
	map<uint32_t, PTZ_CONTRL_PRIOR>::iterator pos = m_mapChnToPtzPrior.find(channel);
	if (pos == m_mapChnToPtzPrior.end())
	{
		status = 0;//0表示没有被控制
	}
	else
	{
		status = 1;//1表示已经被控制
	}
	rvalue["ptzStatus"] = status;

	jout = SHL_Json::writeString(writer, rvalue);
	
	return 0;
}

int
HikPu::onSetPTZPos(const string jin)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    int    chnNo = rvalue["chnNo"].asInt();

    float iPara1 = rvalue["P"].asFloat();
    float iPara2 = rvalue["T"].asFloat();
    float iPara3 = rvalue["Z"].asFloat();

    NET_DVR_PTZPOS ptzPos;
    ptzPos.wAction = rvalue["action"].asInt();
    ptzPos.wPanPos = DecToHex(iPara1 * 10);
    ptzPos.wTiltPos = DecToHex(iPara2 * 10);
    ptzPos.wZoomPos = DecToHex(iPara3 * 10);
    BOOL bRet = NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PTZPOS, chnNo, &ptzPos, sizeof(NET_DVR_PTZPOS));
    if (!bRet)
    {
        int err = NET_DVR_GetLastError();
        ERRE_MSV("onSetPTZPos failed, errno =%d", err);
        return -1;
    }
    return 0;
}

int
HikPu::onGetPTZPos(const string jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    int    chnNo = rvalue["chnNo"].asInt();

    NET_DVR_PTZPOS ptzPos = { 0 };
	NET_DVR_PTZSCOPE ptzPoco = { 0 };
    DWORD dwReturned;
	BOOL bRet;
	bRet = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_PTZSCOPE, chnNo, &ptzPoco, sizeof(NET_DVR_PTZSCOPE), &dwReturned);
	if (!bRet)
	{
		int err = NET_DVR_GetLastError();
		ERRE_MSV("onGetPTZPos failed, errno =%d", err);
		return -1;
	}
    bRet = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_PTZPOS, chnNo, &ptzPos, sizeof(NET_DVR_PTZPOS), &dwReturned);
    if (!bRet)
    {
        int err = NET_DVR_GetLastError();
        ERRE_MSV("onGetPTZPos failed, errno =%d", err);
        return -1;
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value wvalue;

    int iPara1 = HexToDecMa1(ptzPos.wPanPos);
    int iPara2 = HexToDecMa1(ptzPos.wTiltPos);
    int iPara3 = HexToDecMa1(ptzPos.wZoomPos);
	int PiParamin = HexToDecMa1(ptzPoco.wPanPosMin);
	int PiParamax = HexToDecMa1(ptzPoco.wPanPosMax);
	int WiParamin = HexToDecMa1(ptzPoco.wTiltPosMin);
	int WiParamax = HexToDecMa1(ptzPoco.wTiltPosMax);
	int ZiParamin = HexToDecMa1(ptzPoco.wZoomPosMin);
	int ZiParamax = HexToDecMa1(ptzPoco.wZoomPosMax);

    wvalue["P"] = (float)((float)iPara1 / 10);
    wvalue["T"] = (float)((float)iPara2 / 10);
    wvalue["Z"] = (float)((float)iPara3 / 10);
	wvalue["Pmin"] = (float)((float)PiParamin / 10);
	wvalue["Pmax"] = (float)((float)PiParamax / 10);
	wvalue["Tmin"] = (float)((float)WiParamin / 10);
	wvalue["Tmax"] = (float)((float)WiParamax / 10);
	wvalue["Zmin"] = (float)((float)ZiParamin / 10);
	wvalue["Zmax"] = (float)((float)ZiParamax / 10);
    jout = writer.write(wvalue);
    return 0;
}

int
HikPu::onGetThermometryResult(const string jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    int chnNo    = rvalue["chnNo"].asInt();
    int ruleId   = rvalue["ruleId"].asInt();
	int presetNo = rvalue["presetNo"].asInt();

	if (!NET_DVR_PTZPreset_Other(m_userId, chnNo+1, GOTO_PRESET, presetNo))						/*转到预置点*/
	{
		ERRE_MSV("NET_DVR_SET_THERMOMETRY_BASICPARAM failed, error code: %d\n", NET_DVR_GetLastError());
		return -1;
	}

    //启动实时温度检测
    NET_DVR_REALTIME_THERMOMETRY_COND struThermCond = { 0 };
    struThermCond.dwSize = sizeof(struThermCond);
    struThermCond.byRuleID = ruleId;       //规则ID，0代表获取全部规则，具体规则ID从1开始
    struThermCond.dwChan = chnNo+1; //从1开始，0xffffffff代表获取全部通道

    LONG lHandle = NET_DVR_StartRemoteConfig(m_userId, NET_DVR_GET_REALTIME_THERMOMETRY, &struThermCond, sizeof(struThermCond), thermInfo_cb, this);
    if (lHandle < 0)
    {
        ERRE_MSV("NET_DVR_GET_REALTIME_THERMOMETRY failed, error code: %d", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        LOGI_MSV("NET_DVR_GET_REALTIME_THERMOMETRY is successful!");
    }

    
    m_evtGetTemp.init(STRTEMP_C, resp);
	strtemp = STRTEMP_C;
    if (!m_evtGetTemp.wait(6000) || resp == "")
    {
        NET_DVR_StopRemoteConfig(lHandle);
        ERRD_MSV("get temprature no result."); 
        return -1;
    }
    NET_DVR_StopRemoteConfig(lHandle);
    this->writeAlarmLog(resp);

    //sendto c2
    this->notifyAlarm(resp, NULL, 0);

    jout = resp;
    return 0;
}

int 
HikPu::StartMeaTemperature(const string jin, string &jout)
{

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	int chnNo = rvalue["chnNo"].asInt();
	int ruleId = rvalue["ruleId"].asInt();
	int presetNo = rvalue["presetNo"].asInt();

	if (!NET_DVR_PTZPreset_Other(m_userId, chnNo+1, GOTO_PRESET, presetNo))						/*转到预置点*/
	{
		ERRE_MSV("NET_DVR_SET_THERMOMETRY_BASICPARAM failed, error code: %d\n", NET_DVR_GetLastError());
		return -1;
	}

	//启动实时温度检测
	NET_DVR_REALTIME_THERMOMETRY_COND struThermCond = { 0 };
	struThermCond.dwSize = sizeof(struThermCond);
	struThermCond.byRuleID = ruleId;       //规则ID，0代表获取全部规则，具体规则ID从1开始
	struThermCond.dwChan = chnNo+1;			//从1开始，0xffffffff代表获取全部通道
	int Handle = NET_DVR_StartRemoteConfig(m_userId, NET_DVR_GET_REALTIME_THERMOMETRY, &struThermCond, sizeof(struThermCond), thermInfo_cb, this);
	if (Handle < 0)
	{
		ERRE_MSV("NET_DVR_GET_REALTIME_THERMOMETRY failed, error code: %d", NET_DVR_GetLastError());
		return -1;
	}
	else
	{
		LOGI_MSV("NET_DVR_GET_REALTIME_THERMOMETRY is successful!");
	}
	memset(m_lpThermometry, 0, sizeof(LPNET_DVR_THERMOMETRY_UPLOAD)); /*清除获取温度的参数值*/
	m_meatemptype = 0;
	SHL_Json::FastWriter writer;
	SHL_Json::Value wvalue;
	wvalue["deviceHost"] = this->host();
	wvalue["devicePort"] = this->port();
	wvalue["handle"] = int64_t(Handle);
	jout = writer.write(wvalue);
	return 0;
}

int
HikPu::getMeaTemperature(const string jin, string &jout)
{
	SHL_Json::FastWriter writer;
	SHL_Json::Value      value;
	if (m_meatemptype == NET_SDK_CALLBACK_TYPE_DATA)
	{
		value["deviceHost"] = this->host();
		value["devicePort"] = this->port();
		value["ruleName"]	= m_lpThermometry->szRuleName;
		value["ruleID"]		= m_lpThermometry->byRuleID;
		value["ruleCalibType"] = m_lpThermometry->byRuleCalibType;
		value["presetNo"] = m_lpThermometry->wPresetNo;
		if (m_lpThermometry->byRuleCalibType == 0) //点测温
		{
			value["pointTemp"] = m_lpThermometry->struPointThermCfg.fTemperature;
		}

		if ((m_lpThermometry->byRuleCalibType == 1) || (m_lpThermometry->byRuleCalibType == 2)) //框/线测温
		{
			value["polygonMaxTemp"] = m_lpThermometry->struLinePolygonThermCfg.fMaxTemperature;
			value["polygonMinTemp"] = m_lpThermometry->struLinePolygonThermCfg.fMinTemperature;
			value["polygonAverageTemp"] = m_lpThermometry->struLinePolygonThermCfg.fAverageTemperature;
			value["polygonDiffTemp"] = m_lpThermometry->struLinePolygonThermCfg.fTemperatureDiff;
		}
		jout = writer.write(value);
	}
	else if (m_meatemptype == NET_SDK_CALLBACK_TYPE_STATUS)
	{
		if (m_meatempstatus == NET_SDK_CALLBACK_STATUS_FAILED)
		{
			ERRE_MSV("NET_DVR_GET_MANUALTHERM_INFO failed\n");
	     	return -1;
		}
		
	}
	memset(m_lpThermometry, 0, sizeof(LPNET_DVR_THERMOMETRY_UPLOAD)); /*清除获取温度的参数值*/
	m_meatemptype = 0;
	return 0;
}

int	
HikPu::StopMeaTemperature(const string jin)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}
	long handle = rvalue["handle"].asInt64();
	bool res = NET_DVR_StopRemoteConfig(handle);
	if (res == false)
	{
		ERRE_MSV("NET_DVR_GET_REALTIME_THERMOMETRY failed, error code: %d", NET_DVR_GetLastError());
		return -1;
	}
	return 0;
}

int
HikPu::onSetThermometryPresetInfo(const string jin)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    int presetNo = rvalue["presetNo"].asInt();
    int chnNo = rvalue["chnNo"].asInt();
    int ruleId = rvalue["ruleId"].asInt();
    int enable = rvalue["enable"].asInt();//0-no,1-yes
    string ruleName = rvalue["ruleName"].asString();//max len 32
    float emissivity = rvalue["emissivity"].asFloat();//[0.01, 1.00]
    float reflex = rvalue["reflex"].asFloat();//****.*
    int distance = rvalue["distance"].asInt();//[0,10000]
    int ruleCalibType = rvalue["ruleCalibType"].asInt(); //0-point,1-polygon,2-line
	int tempMode = 1;  /*测温模式 0 -普通模式 1- 专家模式*/

    //get all rules info of preset
    NET_DVR_THERMOMETRY_COND        paramCond = { 0 };
    NET_DVR_STD_CONFIG              paramCfg = { 0 };
    NET_DVR_THERMOMETRY_PRESETINFO  paramPreset = { 0 };
    char s8StatusBuf[16 * 1024] = { 0 };

	//测温模式的选择
	int channelno = chnNo + 1;
	memset(&paramCfg, 0, sizeof(NET_DVR_STD_CONFIG));
	NET_DVR_THERMOMETRY_MODE temperatureMode;
	temperatureMode.byMode = 1;															/*测温模式：0- 普通模式，1- 专家模式*/
	temperatureMode.byThermometryROIEnabled = 1;
	temperatureMode.dwSize = sizeof(NET_DVR_THERMOMETRY_MODE);

	paramCfg.lpCondBuffer = &channelno;
	paramCfg.dwCondSize = sizeof(int);
	paramCfg.lpInBuffer = (LPVOID)&(temperatureMode);
	paramCfg.dwInSize = sizeof(NET_DVR_THERMOMETRY_MODE);

	paramCfg.lpStatusBuffer = NULL;
	paramCfg.dwStatusSize = 0;
	paramCfg.lpOutBuffer = NULL;
	paramCfg.dwOutSize = 0;

	if (!NET_DVR_SetSTDConfig(m_userId, NET_DVR_SET_THERMOMETRY_MODE, &paramCfg)) /*设置测温模式*/
	{
		int error = NET_DVR_GetLastError();
		ERRE_MSV("NET_DVR_SET_THERMOMETRY_BASICPARAM failed, error code: %d\n", NET_DVR_GetLastError());
		return -1;
	}

	/*获取测温点的关联信息*/
	memset(&paramCfg, 0, sizeof(NET_DVR_STD_CONFIG));
    paramCond.dwSize = sizeof(NET_DVR_THERMOMETRY_COND);
    paramCond.dwChannel = chnNo+1;
    paramCond.wPresetNo = presetNo;

    paramCfg.lpCondBuffer = &paramCond;
    paramCfg.dwCondSize = sizeof(NET_DVR_THERMOMETRY_COND);
    paramCfg.lpOutBuffer = &paramPreset;
    paramCfg.dwOutSize = sizeof(NET_DVR_THERMOMETRY_PRESETINFO);
    paramCfg.lpStatusBuffer = s8StatusBuf;
    paramCfg.dwStatusSize = 16 * 1024;

    if (!NET_DVR_GetSTDConfig(m_userId, NET_DVR_GET_THERMOMETRY_PRESETINFO, &paramCfg))
    {
		ERRE_MSV("NET_DVR_SET_THERMOMETRY_BASICPARAM failed, error code: %d\n", NET_DVR_GetLastError());
        return -1;
    }

	NET_DVR_THERMOMETRY_PRESETINFO pararout = paramPreset;

    paramPreset.dwSize = sizeof(NET_DVR_THERMOMETRY_PRESETINFO);
    paramPreset.wPresetNo = presetNo;

    int s32Index = ruleId - 1;

    paramPreset.struPresetInfo[s32Index].byEnabled = enable;
    paramPreset.struPresetInfo[s32Index].byReflectiveEnabled = 1;

    strncpy(paramPreset.struPresetInfo[s32Index].szRuleName, ruleName.c_str(), 31);
    paramPreset.struPresetInfo[s32Index].byRuleID = ruleId;
    paramPreset.struPresetInfo[s32Index].fEmissivity = emissivity;
    paramPreset.struPresetInfo[s32Index].fReflectiveTemperature = reflex;
    paramPreset.struPresetInfo[s32Index].wDistance = distance;

    int pointCount = rvalue["pointList"].size();
    if (pointCount <= 0)
    {
        LOGE_MSV("pointList empty!!json:%s", jin.c_str());
        return -1;
    }
    else if (1 == pointCount)
    {
        paramPreset.struPresetInfo[s32Index].byRuleCalibType = 0;
        paramPreset.struPresetInfo[s32Index].struPoint.fX = rvalue["pointList"][0]["x"].asFloat();
        paramPreset.struPresetInfo[s32Index].struPoint.fY = rvalue["pointList"][0]["y"].asFloat();
    }
    else
    {
        if (2 == pointCount)
        {
            paramPreset.struPresetInfo[s32Index].byRuleCalibType = 2;
        }
        else
            paramPreset.struPresetInfo[s32Index].byRuleCalibType = 1;
        paramPreset.struPresetInfo[s32Index].struRegion.dwPointNum = pointCount;
        for (int i = 0; i < pointCount; ++i)
        {
            paramPreset.struPresetInfo[s32Index].struRegion.struPos[i].fX = rvalue["pointList"][i]["x"].asFloat();
            paramPreset.struPresetInfo[s32Index].struRegion.struPos[i].fY = rvalue["pointList"][i]["y"].asFloat();
        }
    }

    memset(&paramCfg, 0, sizeof(NET_DVR_STD_CONFIG));
    paramCfg.lpCondBuffer = &paramCond;
    paramCfg.dwCondSize = sizeof(NET_DVR_THERMOMETRY_COND);
    paramCfg.lpInBuffer = &paramPreset;
    paramCfg.dwInSize = sizeof(NET_DVR_THERMOMETRY_PRESETINFO);
    paramCfg.lpStatusBuffer = s8StatusBuf;
    paramCfg.dwStatusSize = 16 * 1024;


    BOOL bReturn = NET_DVR_SetSTDConfig(m_userId, NET_DVR_SET_THERMOMETRY_PRESETINFO, &paramCfg);
    if (FALSE == bReturn)
    {
		ERRE_MSV("NET_DVR_SET_THERMOMETRY_BASICPARAM failed, error code: %d\n", NET_DVR_GetLastError());
        return -1;
    }

    return 0;
}

int HikPu::remoteCapture(const string json, string &jout)
{
	if (m_userId < 0)
	{
		return -1;
	}

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	int chanNo = rvalue["chnNo"].asInt();
	int devPort = rvalue["DevPort"].asInt();
	string devIp = "";
	if (!rvalue["DevIp"].isNull())
	{
		devIp = rvalue["DevIp"].asString();
	}

	int width = 1;
	int height = 1;
	int picSize = getPicSize(chanNo + 1);
	if (picSize < 0)
	{
		ERRE_MSV("HikPu::remoteCapture failed! getPicSize() failed:addrcode = %s", addrcode().c_str());
		return -1;
	}

	NET_DVR_JPEGPARA struJpegPara;//JPEG图像参数
	memset(&struJpegPara, 0, sizeof(NET_DVR_JPEGPARA));
	struJpegPara.wPicQuality = 0;
	struJpegPara.wPicSize = picSize;
	int iChanShowNum = chanNo + 1;
	char *buffer = new char[1024*1024];
	int bufferSize = 1024*1024;
	int realSize   = 0;
	int ret = -1;
	//单帧数据捕获，并存放到指定的内存空间
	BOOL res = NET_DVR_CaptureJPEGPicture_NEW(m_userId, iChanShowNum, &struJpegPara, buffer, bufferSize, (LPDWORD)&realSize);
	if (TRUE == res)
	{
		SHL_Json::FastWriter writer;
		SHL_Json::Value      value;

		value["version"]    = "1.0";
		value["time"]       = sys->currentTime();
		value["deviceName"] = name();
		value["deviceHost"] = host();
		value["devicePort"] = port();
		value["channelNo"]  = chanNo;
		string picData      = base64_encode((const unsigned char *)buffer, realSize);
		value["picData"]    = picData;
		value["picSize"]    = picData.size();
		value["guname"]     = guname(chanNo);

		jout = writer.write(value);
		ret = 0;
	}
	else
	{
		int ERROR = NET_DVR_GetLastError();
		ERRE_MSV("NET_DVR_CaptureJPEGPicture_NEW failed,errorCode:%d", ERROR);
	}
	if (NULL != buffer)
	{
		delete[]buffer;
		buffer = NULL;
	}
	return ret;
}

int HikPu::AnalysisHotPicData(char* P2PDataBuff, char*buff, int Height, int Width, char* filePath, bool b_save_file, float* p_maxTemp, float* p_minTemp)
{
	int iIndex = 0;
	char temp[10] = { 0 };
	FILE* fp = NULL;

	float minTemp = 10000.0;
	float maxTemp = -10000.0;

	if (b_save_file)
	{
		fp = fopen(filePath, "wb");
		if (NULL == fp)
		{
			return -1;
		}
	}

	//根据宽高遍历所有数据
	for (int iWriteHeight = 0; iWriteHeight < Height; ++iWriteHeight)
	{
		for (int iWriteWidth = 0; iWriteWidth < Width; ++iWriteWidth)
		{
			//解析数据
			float fTemp = *((float*)(P2PDataBuff + iIndex));

			//判断fTemp是否是一个正常值，不是则赋值最大或最小，防止设备崩溃
			fTemp = (9999 < fTemp) ? 9999 : ((-9999 > fTemp) ? -9999 : fTemp);

			minTemp = (minTemp > fTemp) ? fTemp : minTemp;
			maxTemp = (maxTemp > fTemp) ? maxTemp : fTemp;

			memset(temp, 0, sizeof(temp));
			sprintf(temp, "%.2f,", fTemp);

			short s = fTemp * 10;
			int   index = iWriteHeight * Width + iWriteWidth;
			memcpy(buff + index * 2, &s, 2);

			if (b_save_file)
			{
				if (1 != fwrite(temp, sizeof(temp), 1, fp))
				{
					SAFE_CLOSE(fp);
					return -1;
				}
			}

			iIndex += 4;
		}

		if (b_save_file)
		{
			if (1 != fwrite("\n", sizeof("\n"), 1, fp))
			{
				SAFE_CLOSE(fp);
				return -1;
			}
		}
	}

	*p_maxTemp = maxTemp;
	*p_minTemp = minTemp;

	if (b_save_file)
	{
		SAFE_CLOSE(fp);
	}
	return 0;
}



int HikPu::captureThermalFile(const string jin, string &jout)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	int chanNo = rvalue["chnNo"].asInt();

	char  header[64]   = { 0 }; //定义64个字节的头部信息
	float transmission = 0.5;   //光学穿透率
	float radiance     = 0.5;   //比辐射率
	float distance     = 1;     //距离
	float temperature  = 30;    //环境温度
	float humidity     = 30;    //环境湿度
	unsigned long width = 0;     //图像宽度
	unsigned long height = 0;     //图像高度
	memset(header, 0, 64);
	strcpy(header + 4 * 0,"FIR\0");
	memcpy(header + 4 * 1, &transmission, 4);
	memcpy(header + 4 * 2, &radiance, 4);
	memcpy(header + 4 * 3, &distance, 4);
	memcpy(header + 4 * 4, &temperature, 4);
	memcpy(header + 4 * 5, &humidity, 4);

	int ret = -1;
	char AnalysisDataPath[128] = { 0 };
	sprintf_s(AnalysisDataPath, "ThermalFile_%s.csv",sys->currentTime().c_str());
	float minTemp = 10000.0 , maxTemp = -10000.0;
	NET_DVR_JPEGPICTURE_WITH_APPENDDATA struJpegWithAppendData = { 0 };

	const int ciPictureBufSize    = 2 * 1024 * 1024;//2M
	const int ciVisPictureBufSize = 4 * 1024 * 1024;//2M
	char* ucJpegBuf       = new char[ciPictureBufSize];
	char* ucAppendDataBuf = new char[ciPictureBufSize];
	char* ucvisJpegBuf    = new char[ciVisPictureBufSize];

	memset(ucJpegBuf, 0, ciPictureBufSize);
	memset(ucAppendDataBuf, 0, ciPictureBufSize);
	memset(ucvisJpegBuf, 0, ciVisPictureBufSize);
	struJpegWithAppendData.pJpegPicBuff = ucJpegBuf;
	struJpegWithAppendData.pP2PDataBuff = ucAppendDataBuf;
	struJpegWithAppendData.pVisiblePicBuff = ucvisJpegBuf;
	/*获取热图的SDK接口*/
	if (TRUE == NET_DVR_CaptureJPEGPicture_WithAppendData(m_userId, chanNo, &struJpegWithAppendData))
	{
		width  = struJpegWithAppendData.dwJpegPicWidth;
		height = struJpegWithAppendData.dwJpegPicHeight;
		char * buffer = new char[64 + width * height * 2];
		memcpy(header + 4 * 6, &width, 2);
		memcpy(header + 4 * 6 + 2, &height, 2);
		memcpy(buffer, header, 64);
		ret= AnalysisHotPicData(struJpegWithAppendData.pP2PDataBuff, buffer + 64,struJpegWithAppendData.dwJpegPicHeight, struJpegWithAppendData.dwJpegPicWidth,AnalysisDataPath,FALSE, &maxTemp, &minTemp);
		if (0 == ret)
		{
			SHL_Json::FastWriter writer;
			SHL_Json::Value      wvalue;

			wvalue["version"]    = "1.0";
			wvalue["time"]       = sys->currentTime();
			wvalue["deviceName"] = name();
			wvalue["deviceHost"] = host();
			wvalue["devicePort"] = port();
			wvalue["channelNo"]  = chanNo;
			string picData       = base64_encode((const unsigned char *)buffer, 64 + width * height *2);
			wvalue["picData"]    = picData;
			wvalue["picSize"]    = picData.size();
			wvalue["guname"]     = guname(chanNo);

			jout = writer.write(wvalue);
		}
		else
		{
			ERRE_MSV("AnalysisHotPicData Failed!");
		}
		delete buffer;
	}
	else
	{
		ERRE_MSV("NET_DVR_CaptureJPEGPicture_WithAppendData Failed , reason:%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
	}
	delete ucvisJpegBuf;
	delete ucJpegBuf;
	delete ucAppendDataBuf;
	return ret;
}

int                
HikPu::resetCounter(string json)
{
    if (m_userId < 0)
    {
        return FALSE;
    }

    NET_DVR_RESET_COUNTER_CFG resetCfg = { 0 };
    resetCfg.dwSize = sizeof(NET_DVR_RESET_COUNTER_CFG);
    resetCfg.byEnable = 1;
    resetCfg.byMode = 1;
    BOOL ret = NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_RESET_COUNTER, 1, &resetCfg, sizeof(NET_DVR_RESET_COUNTER_CFG));
    if (TRUE == ret)
    {
        SHL_Json::FastWriter writer;
        SHL_Json::Value      value;
        SHL_Json::Value      array;

        value["version"] = "1.0";
        value["time"] = sys->currentTime();
        value["deviceName"] = name();
        value["deviceHost"] = host();
        value["devicePort"] = port();
        value["addrcode"] = addrcode();
        value["level"] = 1;


        SHL_Json::Value itemAlarm;
        itemAlarm["type"] = TriggerListener::TR_AI_NUMPEOPLE;;
        itemAlarm["channelNo"] = 0;
        itemAlarm["status"] = "0";
        itemAlarm["gucode"] = "";
        itemAlarm["name"] = "";
        itemAlarm["preset"] = 1;
        itemAlarm["alarmdesc"] = "";
        itemAlarm["roiId"] = 0;
        itemAlarm["picName"] = "";
        itemAlarm["inperson"] = 0;
        itemAlarm["outperson"] = 0;

        itemAlarm["gucode"] = gucode(0);
        itemAlarm["name"] = guname(0);


        value["alarms"].append(itemAlarm);
        string json = writer.write(value);
        notifyAlarm(json, NULL, 0);
        //writeAlarmLog(json);
        return 0;
    }
    else
    {
        ERRE_MSV("NET_DVR_SetDVRConfig  NET_DVR_SET_RESET_COUNTER failed,errorCode:%d", NET_DVR_GetLastError());
        return -1;
    }
    return -1;
}

int
HikPu::vehicleCtrl(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    int ret = TRUE;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return FALSE;
    }


    int operateType = rvalue["operateType"].asInt();


    if (operateType == 0)
    {
        //建立连接
        LONG lSyncVehHandle = NET_DVR_StartRemoteConfig(m_userId, NET_DVR_VEHICLELIST_CTRL_START/*NET_DVR_VEHICLE_CONTROL_LIST_START*/, NULL, 0, vehicle_cb, this);

        if (lSyncVehHandle < 0)
        {
            ERRD_MSV("NET_DVR_StartRemoteConfig NET_DVR_VEHICLE_CONTROL_LIST_INFO send failed:%d", NET_DVR_GetLastError());
            return FALSE;
        }

        string plateNum = rvalue["plateNum"].asString();
        int listType = rvalue["listType"].asInt();
        int plateType = 0;//rvalue["plateType"].asInt();
        int plateColor = 0;//rvalue["plateColor"].asInt();
        string cardNum = "";//rvalue["plateNum"].asString();
        int enableTime = 1;//rvalue["enableTime"].asInt();
        string startTime = "";
        string endTime = "";
        //if (enableTime)
        //{
        startTime = rvalue["startTime"].asString();
        endTime = rvalue["endTime"].asString();
        //}
        NET_DVR_VEHICLE_CONTROL_LIST_INFO struVehInfo;
        memset(&struVehInfo, 0, sizeof(NET_DVR_VEHICLE_CONTROL_LIST_INFO));
        struVehInfo.dwSize = sizeof(NET_DVR_VEHICLE_CONTROL_LIST_INFO);
        struVehInfo.dwChannel = 1;
        struVehInfo.dwDataIndex = 0;
        memset(struVehInfo.sOperateIndex, 0, sizeof(MAX_OPERATE_INDEX_LEN));
        memcpy(struVehInfo.sLicense, plateNum.c_str(), MAX_LICENSE_LEN);
        struVehInfo.byListType = listType;
        struVehInfo.byPlateType = plateType;
        if (5 == plateColor)
        {
            struVehInfo.byPlateColor = 0xff;
        }
        else
        {
            struVehInfo.byPlateColor = plateColor;
        }
        memcpy(struVehInfo.sCardNo, cardNum.c_str(), MAX_CARDNO_LEN);
        if (enableTime)
        {
            sscanf(startTime.c_str(), "%u-%u-%u %u:%u:%u",
                &struVehInfo.struStartTime.wYear, &struVehInfo.struStartTime.byMonth, &struVehInfo.struStartTime.byDay,
                &struVehInfo.struStartTime.byHour, &struVehInfo.struStartTime.byMinute, &struVehInfo.struStartTime.bySecond);

            sscanf(endTime.c_str(), "%u-%u-%u %u:%u:%u",
                &struVehInfo.struStopTime.wYear, &struVehInfo.struStopTime.byMonth, &struVehInfo.struStopTime.byDay,
                &struVehInfo.struStopTime.byHour, &struVehInfo.struStopTime.byMinute, &struVehInfo.struStopTime.bySecond);
        }
        else
        {
            memset(&struVehInfo.struStartTime, 0, sizeof(NET_DVR_TIME_V30));
            memset(&struVehInfo.struStopTime, 0, sizeof(NET_DVR_TIME_V30));
        }

        //发送配置数据
        if (!NET_DVR_SendRemoteConfig(lSyncVehHandle, 0, (char*)&struVehInfo, sizeof(struVehInfo)/**MAX_MUN*/))
        {
            ERRD_MSV("NET_DVR_SendRemoteConfig NET_DVR_VEHICLE_CONTROL_LIST_INFO send failed:%d", NET_DVR_GetLastError());
            ret = FALSE;
        }
        else
        {
            int resp;
            m_evtSetVihicle.init(STRVEHICLE_C, resp);
            if (!m_evtSetVihicle.wait(4000) || resp == FALSE)
            {
                ERRD_MSV("NET_DVR_SendRemoteConfig NET_DVR_VEHICLE_CONTROL_LIST_INFO send failed");
                ret = -1;
            }
        }


        //关闭黑白名单远程配置链接
        if (!NET_DVR_StopRemoteConfig(lSyncVehHandle))
        {
            ERRD_MSV("NET_DVR_StopRemoteConfig NET_DVR_VEHICLE_CONTROL_LIST_INFO failed:%d", NET_DVR_GetLastError());
        }
        return ret;
    }
    //删除黑白名单
    else if (operateType == 1)
    {
        string plateNum = rvalue["plateNum"].asString();
        string cardNum = "";//rvalue["plateNum"].asString();
        int delType = 0;//rvalue["delType"].asInt();
        int plateColor = 0;//rvalue["plateColor"].asInt();
        int listType = 0;//rvalue["listType"].asInt();
        int plateType = 0;//rvalue["plateType"].asInt();
        
        char szLan[128] = { 0 };
        NET_DVR_VEHICLE_CONTROL_DELINFO struDelVehicleInfo;
        struDelVehicleInfo.dwSize = sizeof(NET_DVR_VEHICLE_CONTROL_DELINFO);
        memcpy(struDelVehicleInfo.sLicense, plateNum.c_str(), sizeof(struDelVehicleInfo.sLicense));
        memcpy(struDelVehicleInfo.sCardNo, cardNum.c_str(), sizeof(struDelVehicleInfo.sCardNo));
        if (0 == delType)
        {
            struDelVehicleInfo.byOperateType = 0;
        }
        else if (1 == delType)
        {
            struDelVehicleInfo.byOperateType = 0xff;
        }

        if (5 == plateColor)
        {
            struDelVehicleInfo.byPlateColor = 0xff;
        }
        else
        {
            struDelVehicleInfo.byPlateColor = plateColor;
        }
        struDelVehicleInfo.byPlateType = plateType;
        struDelVehicleInfo.byListType = listType;

        if (!NET_DVR_RemoteControl(m_userId, NET_DVR_VEHICLE_DELINFO_CTRL, &struDelVehicleInfo, sizeof(NET_DVR_VEHICLE_CONTROL_DELINFO)))
        {
            ERRD_MSV("NET_DVR_RemoteControl NET_DVR_VEHICLE_DELINFO_CTRL failed:%d", NET_DVR_GetLastError());
            return FALSE;
        }
    }
    return TRUE;
}

int
HikPu::onGetChannelStatus(const string jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }
    
    SHL_Json::Value rspvalue;

    for (int i = 0; i < MAX_UT_CANONLINE_ARRSIZE;++i)
    {
        uint32_t h = m_chnOnlineStatus[i] >> 32;
        uint32_t l = m_chnOnlineStatus[i];
        string key = "u64status" + to_string(i)+"_h";
        rspvalue[key] = h;
        key = "u64status" + to_string(i) + "_l";
        rspvalue[key] = l;
    }

    SHL_Json::StreamWriterBuilder writer;
    jout = SHL_Json::writeString(writer, rspvalue);

    return 0;
}

int
HikPu::onGetChannelStatus_all(const string jin, string &jout)
{
	SHL_Json::Value rspvalue;
	SHL_Json::Value array;
	rspvalue["version"] = "v1.0";
	rspvalue["deviceip"] = ip();
	rspvalue["devonline"] = online() ? 1 : 0;
	for (int i = 0; i < m_chnNum; ++i)
	{
		SHL_Json::Value item;
		item["channelno"] = i+1;
		item["status"] = validate(i) ? 1 : 0;
		array.append(item);
	}
	rspvalue["channellist"] = array;
	SHL_Json::StreamWriterBuilder writer;
	jout = SHL_Json::writeString(writer, rspvalue);

	return 0;
}

int
HikPu::getWhiteBlackList(string & jin, string& jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  element;
    int ret = -1;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return ret;
    }
    SHL_Json::Value queryValue;
    SHL_Json::Value queryArray;

    char szLan[128] = { 0 };
    NET_DVR_VEHICLE_CONTROL_COND struVehCond;
    memcpy(struVehCond.sLicense, "", sizeof(""));
    memcpy(struVehCond.sCardNo, "", sizeof(""));
    struVehCond.byListType = 0xff;
    int queryType = 2; rvalue["queryType"].asInt();
    switch (queryType)
    {
    case 0:
    {
        //车牌号码

        string plateNum = rvalue["plateNum"].asString();
        struVehCond.dwOperateType = VCA_LICENSE_TYPE;
        memcpy(struVehCond.sLicense, plateNum.c_str(), sizeof(struVehCond.sLicense));

    }
        break;
    case 1:
    {
        //卡号

        string cardNum = rvalue["cardNum"].asString();
        struVehCond.dwOperateType = VCA_CARDNO_TYPE;
        memcpy(struVehCond.sCardNo, cardNum.c_str(), sizeof(struVehCond.sCardNo));
    }
        break;
    case 2:
    {
        //名单属性

        int listType = 2;//rvalue["listType"].asInt();
        struVehCond.dwOperateType = VCA_LISTTYPE_TYPE;
        if (2 == listType)
        {
            struVehCond.byListType = 0xff;
        }
        else
        {
            struVehCond.byListType = listType;
        }
    }
        break;
    default:
        break;
    }

    LONG lHandle = NET_DVR_StartRemoteConfig(m_userId, NET_DVR_GET_ALL_VEHICLE_CONTROL_LIST, &struVehCond, sizeof(struVehCond), NULL, NULL);
    if (lHandle >= 0)
    {
        int iRet = 0;
        char szLan[128] = { 0 };
        char szInfoBuf[128] = { 0 };
        int index = 0;
        NET_DVR_VEHICLE_CONTROL_LIST_INFO pStruVehInfo;
        while (1)
        {
            iRet = NET_DVR_GetNextRemoteConfig(lHandle, &pStruVehInfo, sizeof(NET_DVR_VEHICLE_CONTROL_LIST_INFO));
            if (iRet == NET_SDK_GET_NEXT_STATUS_SUCCESS)
            {
                ++index;
                char szStr[128] = { 0 };
                SHL_Json::Value qvalue;
                qvalue["plateNum"] = pStruVehInfo.sLicense;
                qvalue["listType"] = pStruVehInfo.byListType;
                qvalue["plateType"] = pStruVehInfo.byPlateType;
                qvalue["plateColor"] = pStruVehInfo.byPlateColor;
                qvalue["cardNum"] = pStruVehInfo.sCardNo;
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%d-%d-%d %d:%d:%d",
                    pStruVehInfo.struStartTime.wYear, pStruVehInfo.struStartTime.byMonth, pStruVehInfo.struStartTime.byDay,
                    pStruVehInfo.struStartTime.byHour, pStruVehInfo.struStartTime.byMinute, pStruVehInfo.struStartTime.bySecond);
                if (0 == strcmp(szStr, "0-0-0 0:0:0"))
                {
                    qvalue["startTime"] = "";
                }
                else
                {
                    qvalue["startTime"] = szStr;
                }

                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%d-%d-%d %d:%d:%d",
                    pStruVehInfo.struStopTime.wYear, pStruVehInfo.struStopTime.byMonth, pStruVehInfo.struStopTime.byDay,
                    pStruVehInfo.struStopTime.byHour, pStruVehInfo.struStopTime.byMinute, pStruVehInfo.struStopTime.bySecond);
                if (0 == strcmp(szStr, "0-0-0 0:0:0"))
                {
                    qvalue["endTime"] = "";
                }
                else
                {
                    qvalue["endTime"] = szStr;
                }

                queryArray.append(qvalue);
            }
            else
            {
                if (iRet == NET_SDK_GET_NETX_STATUS_NEED_WAIT)
                {
                    sys->sleep(5);
                    continue;
                }
                if (iRet == NET_SDK_GET_NEXT_STATUS_FINISH)
                {
                    // 				CString str;
                    // 				str.Format(_T("%d"),pThis->m_lstVehicle.GetItemCount());
                    // 				pThis->GetDlgItem(IDC_EDIT_VEHICLE_INF0_COUNT)->GetWindowText(str);
                    //g_StringLanType(szLan, "长连接获取黑白名单数据结束!", "Get Black and White List Finish");
                    //sprintf(szInfoBuf, "%s[Info Count:%d]", szLan, pThis->m_lstVehicle.GetItemCount());
                    //AfxMessageBox(szInfoBuf);
                    break;
                }
                else if (iRet == NET_SDK_GET_NEXT_STATUS_FAILED)
                {
                    //g_StringLanType(szLan, "长连接获取黑白名单数据失败", "Get Black and White List failed");
                    //AfxMessageBox(szLan);
                    break;
                }
                else
                {
                    //g_StringLanType(szLan, "未知状态", "Unknown status");
                    //AfxMessageBox(szLan);
                    break;
                }
            }
        }
        queryValue["hwlist"] = queryArray;
        queryValue["arraySize"] = index;

        SHL_Json::StreamWriterBuilder writer;
        jout = SHL_Json::writeString(writer, queryValue);
    }
    else
    {
        //g_StringLanType(szLan, "请求长连接获取黑白名单数据失败", "Ask Get Black and White List failed");
        //DWORD ret = NET_DVR_GetLastError();
        //sprintf(szLan, "%s 错误码：%d", szLan, ret);
        //AfxMessageBox(szLan);
        return -1;
    }
    return 0;
}

void               
HikPu::getGateStatus()
{
    if (m_userId >= 0)
    {
        NET_DVR_ALARMHOST_MAIN_STATUS_V40 output = { 0 };
        DWORD dwReturn = 0;
        BOOL bRet = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_ALARMHOST_MAIN_STATUS_V40, 0xFFFFFFFF, &output, sizeof(output), &dwReturn);
        //output.byAlarmOutStatus
        int status = 4;
        for (int i = 0; i < 6; ++i)
        {
            if (output.byAlarmOutStatus[i])
            {
                status = i;
                break;
            }
        }


        SHL_Json::FastWriter writer;
        SHL_Json::Value  alarmValue;

        alarmValue["version"] = "1.0";
        alarmValue["time"] = sys->currentTime();
        alarmValue["deviceName"] = this->name();
        alarmValue["deviceHost"] = this->host();
        alarmValue["devicePort"] = this->port();
        alarmValue["addrcode"] = this->addrcode();
        alarmValue["level"] = 1;

        SHL_Json::Value itemAlarm;
        //0 - 道闸正在开;1 - 道闸正在关;2 - 道闸开到位;3 - 道闸关到位;4 - 道闸无信号;5 - 锁闸
        itemAlarm["type"] = TriggerListener::TR_GATE_STATUS;
        itemAlarm["gateStatus"] = status;
        itemAlarm["roiId"] = 0;


        alarmValue["alarms"].append(itemAlarm);
        string jsonAlarm = writer.write(alarmValue);


        this->writeAlarmLog(jsonAlarm);

        //sendto c2
        this->notifyAlarm(jsonAlarm, NULL, 0);
    }
}

int
HikPu::gateCtrl(int cmd)//zhangshenyu 20200509
{
    /* 获取当前出入口控制参数参数 */
    NET_DVR_ENTRANCE_CFG struEntrance = { 0 };      // 出入口控制参数
    if (!getEntance(&struEntrance))
    {
        return FALSE;
    }

    /* 修改为平台控制模式 */
    if (!setEntance(&struEntrance, 1))
    {
        return FALSE;
    }

    /* 对道闸进行开关控制 */
    NET_DVR_BARRIERGATE_CFG struBarrierGateCtrl = { 0 };
    struBarrierGateCtrl.dwSize = sizeof(NET_DVR_BARRIERGATE_CFG);
    struBarrierGateCtrl.dwChannel = 1; // 通道号
    struBarrierGateCtrl.byLaneNo = 1;// 道闸号: 0- 无效值，1- 道闸 
    struBarrierGateCtrl.byBarrierGateCtrl = cmd;// 控制参数: 0- 关闭道闸，1- 开启道闸，2- 停止道闸，3- 锁定道闸
    struBarrierGateCtrl.byEntranceNo = 1;
    int ret = NET_DVR_RemoteControl(m_userId, NET_DVR_BARRIERGATE_CTRL, &struBarrierGateCtrl, sizeof(NET_DVR_BARRIERGATE_CFG));
    if (FALSE == ret)
    {
        ERRD_MSV("NET_DVR_RemoteControl NET_DVR_VEHICLE_DELINFO_CTRL failed:%d", NET_DVR_GetLastError());
    }
    /* 控完后，修改为相机控制模式 */
    if (!setEntance(&struEntrance, 0))
    {
        return FALSE;
    }
    return ret ? 0 : -1;
}

int HikPu::ptzPatternCtrl(int channelNo, int patternCmd, int patternId)
{
    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
    }
    NET_DVR_PTZ_PATTERN struPtzPattern = { 0 };
    struPtzPattern.dwSize = sizeof(struPtzPattern);
    struPtzPattern.dwChannel = channelNo;
    switch (patternCmd)
    {
    case 1:
        struPtzPattern.dwPatternCmd = STA_MEM_CRUISE;
        break;
    case 2:
        struPtzPattern.dwPatternCmd = STO_MEM_CRUISE;
        break;
    case 3:
        struPtzPattern.dwPatternCmd = RUN_CRUISE;
        break;
    case 4:
        struPtzPattern.dwPatternCmd = STOP_CRUISE;
        break;
    case 5:
        struPtzPattern.dwPatternCmd = DELETE_CRUISE;
    default:
        ;

    }
    struPtzPattern.dwPatternID = patternId;
    bool ret = NET_DVR_RemoteControl(m_userId, NET_DVR_CONTROL_PTZ_PATTERN, &struPtzPattern, sizeof(struPtzPattern));
    return ret ? 0 : -1;
}

int
HikPu::setPtzParkaction(int chn, int enable, int parkTime, int actionType, int id)
{
    NET_DVR_PTZ_PARKACTION_CFG param = { 0 };
    param.dwSize = sizeof(param);
    param.byEnable = enable;
    param.dwParkTime = parkTime;
    param.wActionType = actionType;
    param.wID = id;
    param.byOneTouchSwitch = 0;

    if (!NET_DVR_SetDVRConfig(m_userId, NET_DVR_SET_PTZ_PARKACTION_CFG, chn, &param, sizeof(param)))
    {
        ERRE_MSV("NET_DVR_SetDVRConfig, NET_DVR_SET_PTZ_PARKACTION_CFG failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }

    return 0;
}

int
HikPu::getPtzParkaction(int chn, int &enable, int &parkTime, int &actionType, int &id)
{
    NET_DVR_PTZ_PARKACTION_CFG param = { 0 };
    DWORD dwReturned = 0;

    if (!NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_PTZ_PARKACTION_CFG, chn, &param, sizeof(param), &dwReturned))
    {
        ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_PTZ_PARKACTION_CFG failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }

    enable = param.byEnable;
    parkTime = param.dwParkTime;
    actionType = param.wActionType;
    id = param.wID;

    return 0;
}



void
HikPu::onTimer()
{
    /* TODO:  <22-06-17, yourname> */

    //if ((VOICE_OK == m_voiceState) && (1 == m_callStatus)
    //    && (time(NULL) - m_lLastSendVoiceTime >= 60))
    //{
    //    m_VoiceState = VOICE_NORMAL;
    //    DoCallAnswer(5);
    //}
    static long long count = 0;
    ++count;
    static time_t staLastReCallListen = time(NULL);
    if ((2 == m_callStatus) && (time(NULL) - staLastReCallListen) > 1)
    {
        startListenCall();
        staLastReCallListen = time(NULL);
    }
    

    ///* 模拟发送道闸状态告警（道闸开*/
    //if (Pu::DEV_TYPE_IPC == this->type() && count % 51 == 0)
    //{
    //    NET_DVR_GATE_ALARMINFO info;
    //    info.byExternalDevType = DEVICES_TYPE_DZ_DEV;
    //    info.byExternalDevStatus = DEVICES_STATUS_OPENING;
    //    NET_DVR_ALARMER talarmer;
    //    talarmer.lUserID = this->getUserID();
    //    alarm_cb(COMM_ITS_GATE_ALARMINFO, &talarmer, (char*)&info, sizeof(NET_DVR_GATE_ALARMINFO), this);
    //}

    ///* 模拟发送道闸状态告警（道闸关 */
    //if (Pu::DEV_TYPE_IPC == this->type() && count % 100 == 0)
    //{
    //    NET_DVR_GATE_ALARMINFO info;
    //    info.byExternalDevType = DEVICES_TYPE_DZ_DEV;
    //    info.byExternalDevStatus = DEVICES_STATUS_CLOSING;
    //    NET_DVR_ALARMER talarmer;
    //    talarmer.lUserID = this->getUserID();
    //    alarm_cb(COMM_ITS_GATE_ALARMINFO, &talarmer, (char*)&info, sizeof(NET_DVR_GATE_ALARMINFO), this);
    //    count = 0;
    //}
    Pu::onTimer();
}


int HikPu::getPicSize(int chanNo)
{
    if (m_userId < 0)
        return -1;
    NET_DVR_COMPRESSIONCFG_V30 output = { 0 };

    DWORD dwReturn = 0;
    BOOL ret = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_COMPRESSCFG_V30, chanNo, &output, sizeof(output), &dwReturn);
    if (ret == FALSE)
    {
        ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_COMPRESSCFG_V30 failed, errno:%d", NET_DVR_GetLastError());
        return -1;
    }
    return output.struNormHighRecordPara.byResolution;
}

WORD DecToHex(WORD wDec)
{
    return (wDec / 1000) * 4096 + ((wDec % 1000) / 100) * 256 + ((wDec % 100) / 10) * 16 + (wDec % 10);
}

DWORD HexToDecMa1(DWORD wHex)
{
    return (wHex / 4096) * 1000 + ((wHex % 4096) / 256) * 100 + ((wHex % 256) / 16) * 10 + (wHex % 16);
}

int
HikPu::GetSize(unsigned char* buffer, long length, int &height, int &weidth)
{	
	for (int i = 0; i < length; i++)
	{
		if (buffer[i] == 0xff && buffer[i + 1] == 0xc0)
		{
			weidth = (buffer[i + 7] << 8) | buffer[i + 8];
			height = (buffer[i + 5] << 8) | buffer[i + 6];
			break;
		}
	}
	return 0;
}

