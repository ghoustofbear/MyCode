// c2platform_conn.cpp: implementation of the c2platform_conn class.
//
//////////////////////////////////////////////////////////////////////

#include "c2platform_conn.h"
#include "c2platform_preview.h"
#include "c2platform_record.h"
#include "c2platform_download.h"
#include "c2Platform_DownloadByTime.h"

#include "proto_def.h" 
#include "json/json.h"  
#include "defc1.h"  
#include <STRING>
//#include <STDLIB.H>
#include <stdlib.h>
#include <IOSTREAM>
#include "commonc1.h"
#include <map>
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//************************************************************************
//�������ƣ�loadC2platform(const char* jvm2nativePath)
//����˵������ʼ��
//����������
//    (in) jvm2nativePath jvm2nativePath jvm�����·��ӳ�䵽native·��������SDK����jvm�е��ļ�ϵͳ, eg:"/data/data/com.utc2/app_libs/"
//����ֵ���ɹ�����0��ʧ�ܷ���-1
//************************************************************************/
long loadC2platform(const char* jvm2nativePath)
{
	long lRet = -1;
	if(NULL != jvm2nativePath)
	{
		lRet = c2_init(jvm2nativePath);
	}
	else
	{
		lRet = c2_init();
	}
	return lRet;
}

//************************************************************************
//�������ƣ�loadC2platform(const char* jvm2nativePath)
//����˵��������ƽ̨
//������������
//����ֵ����
//************************************************************************/
void destroyC2platform()
{
	c2_uninit();
}

C2PlatformConn::C2PlatformConn():BasePlatformConn()
{
	m_pfnAlarmCallback = NULL;
	m_pfnConnCallback = NULL;
	m_pfnDeviceListCallback = NULL;
	m_pfnQueryDetailCallback = NULL;
	m_pfnQueryCalendarCallback = NULL;
	m_pfnPresetListCallback = NULL;
	m_pfnPresetListCallbackEx = NULL;
	m_pfnUserListCallback = NULL;	
	m_pfnWFLinkageCallback= NULL;
	m_pAlarmUserData = NULL;
	
	memset(m_ip, 0, sizeof(m_ip));	
	memset(m_subIp, 0, sizeof(m_subIp));
	memset(m_addr, 0, sizeof(m_addr));
	memset(m_subName, 0, sizeof(m_subName));
	memset(m_password, 0, sizeof(m_password));
	m_port = 0;
	m_subPort = 0;
	
	m_devInfo = NULL;
	m_hLogin = NULL;  
	
	m_pUserPreset = NULL;
	m_pUserDevice = NULL;
	m_pUserLogin = NULL; 
	m_pUserRecord = NULL; 
	m_pUserData = NULL;
	m_pUserWF = NULL; 
	m_presetList = "";
	m_pDualTalkEx = NULL;
}

C2PlatformConn::~C2PlatformConn()
{
	if (NULL != m_devInfo)
	{
		delete [] m_devInfo;
		m_devInfo = NULL;
	}
}

//************************************************************************
//�������ƣ�C2LoginCallback(int code, void* userdata)
//����˵������¼״̬�ص�
//����������
//    (in) code ״̬��
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
VOID C2LoginCallback(int code, void* userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;
	int iCode = -1;
	
	if(NULL == pConn->m_pfnConnCallback)
	{
		return;
	}
	
	switch(code)
	{
	case CODE_LOGIN_SUCCESS: //��¼�ɹ�
		{
			iCode = MSG_CONN_SUCCESS;			
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
			
			iCode = MSG_LOGIN_SUCCESS;
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);			
		}	
		break;
	case CODE_LOGIN_FAIL: //��¼ʧ��
		{
			iCode = MSG_LOGIN_FAIL;
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_CONNFAIL://����ʧ��
		{
			iCode = MSG_CONN_FAIL;	
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_LOGINED: //���û��Ѿ���¼�Ҳ����ظ���¼
		{
			iCode = MSG_LOGIN_USERLOGINED;	
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_NOUSER: //�û���������
		{
			iCode = MSG_LOGIN_NOUSERNAME;
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_PSWDERR://�������
		{
			iCode = MSG_LOGIN_PASSWORDERROR;
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_NORIGHT: //û�е�¼Ȩ��
		{
			iCode = MSG_LOGIN_NORIGHT;
			pConn->m_pfnConnCallback(iCode, pConn->m_pUserLogin);
		}	
		break;
	case CODE_LOGIN_SWITCHOST: //�л�������
		{
			
		}	
		break;
	}	
}

//************************************************************************
//�������ƣ�C2DeviceListCallback(c2_devinfo_s *info, void *userdata)
//����˵�����豸��Ϣ�ص�����
//����������
//    (in) info     �����豸��Ϣ�ṹ
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
VOID C2DeviceListCallback(c2_devinfo_s *info, void *userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;
	DEVINFO_RSP_S pRsp = {0};
	
	string devName = utf2asc(info->name);
	strcpy(pRsp.s8DevName, devName.c_str());
	strcpy(pRsp.s8AddrCode, info->addrcode);
	strcpy(pRsp.s8PareAddrCode, info->parent); 
	strcpy(pRsp.s8DevDesc, "");
	
	pRsp.s32ChnNo = info->channelNo;
	pRsp.s32ChnNum = info->channelNum;
	
	INET_PTON(info->host, pRsp.u8IPAddr);
	
	pRsp.u16Port = info->port;
	pRsp.u16DevType = info->devtype;
	
	pRsp.s8ProtoType = info->protoType;
	pRsp.s8StreamType = info->streamType;
	pRsp.s8ManuType =  info->manutype;
	
	pRsp.u32StSize = sizeof(pRsp);
	
	pConn->m_pfnDeviceListCallback(&pRsp, pConn->m_pUserDevice);
}

//************************************************************************
//�������ƣ�C2GetDeviceDataCallback(c2_devinfo_s *info, void *userdata)
//����˵�����豸��Ϣ�ص�����
//����������
//    (in) info     �����豸��Ϣ�ṹ
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
VOID C2GetDeviceDataCallback(c2_devinfo_s *info, void *userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;
	
	c2_devinfo_s devInfo = *info;
	
	//preset->nameΪutf-8��ʽ����ת��ΪUnicode����
	int len = MultiByteToWideChar(CP_UTF8, 0, devInfo.name, -1, NULL, 0);
	wchar_t *strName = new wchar_t[len + 1];
	wmemset(strName, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, devInfo.name, -1, strName, len);
	
	//�ٰ�Unicode����ת��ΪGBK����
	len = WideCharToMultiByte(CP_ACP, 0, strName, -1, NULL, 0, NULL, NULL);
	char* devName = new char[len];
	WideCharToMultiByte(CP_ACP, 0, strName, -1, devName, len, NULL, NULL);
	
	strcpy(devInfo.name, devName);	
	pConn->m_vcDeviceInfo.push_back(devInfo);
	delete []strName;
	delete []devName;
}

//************************************************************************
//�������ƣ�C2UserInfoCallback(c2_user_info_s *info, void* userdata)
//����˵�����û��б����ݻص�
//����������
//    (in) info     �û���Ϣ
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
void C2UserInfoCallback(c2_user_info_s *info, void* userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;
	USERINFO_RSP_S userInfo = {0};
	
	strcpy(userInfo.s8Username, info->user);
	strcpy(userInfo.s8Password, info->password);
	
	userInfo.stUserRight.u32OptRightBit |= (info->rightBit & 0x2)  > 0 ?1<<0:0; 
	userInfo.stUserRight.u32OptRightBit |= (info->rightBit & 0x4)  > 0 ?1<<1:0; 
	userInfo.stUserRight.u32OptRightBit |= (info->rightBit & 0x8)  > 0 ?1<<2:0; 
	userInfo.stUserRight.u32OptRightBit |= (info->rightBit & 0x10) > 0 ?1<<3:0; 
	userInfo.stUserRight.u32OptRightBit |= (info->rightBit & 0x1)  > 0 ?1<<4:0; 
	
	
	userInfo.u32StSize = sizeof(userInfo);
	
	pConn->m_pfnUserListCallback(&userInfo, pConn->m_pUserData);
}

//************************************************************************
//�������ƣ�C2AlarmCallback(const char *json, int size, void *userdata)
//����˵�����������ݻص�
//����������
//    (in) json  ��������
//    (in) size  �������ݳ���
//    (in) size  �û�����
//����ֵ�� 
//************************************************************************/
int C2AlarmCallback(const char *json, int size, void *userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;

	if( !pConn || !pConn->m_pfnAlarmCallback)
	{
		return 0;
	}

	int iType = 0;
	// 	{"alarms":[{"code":"362110000103810004","name":"192.168.66.167","status":0,"type":1}],
	// 		"deviceHost":"192.168.66.167",
	// 		"deviceName":"192.168.66.167",
	// 		"devicePort":8000,
	// 		"time":"2019-12-09 18:07:16",
	// 		"version":"1.0"}
	
	//preset->nameΪutf-8��ʽ����ת��ΪUnicode����
	int len = MultiByteToWideChar(CP_UTF8, 0, json, -1, NULL, 0);
	wchar_t *strJson = new wchar_t[len + 1];
	if (NULL == strJson)
	{
	   return 0;
	}
	wmemset(strJson, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, json, -1, strJson, len);
	
	//�ٰ�Unicode����ת��ΪGBK����
	len = WideCharToMultiByte(CP_ACP, 0, strJson, -1, NULL, 0, NULL, NULL);
	char* sJson = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strJson, -1, sJson, len, NULL, NULL);
	if (NULL == sJson)
	{
	   delete []strJson;
	   strJson = NULL;
	   return 0;
	}

	if (NULL != strJson)
	{
	    delete [] strJson;
		strJson = NULL;
	}
	
	if(NULL == pConn || NULL == json)
	{
		if (NULL != sJson)
		{
		   delete [] sJson;
		   sJson = NULL;
		}
		return -1;
	}
	
	SHL_Json::Value rootNode;
	SHL_Json::Reader reader;
	if(!reader.parse(json, rootNode))
	{
		if (NULL != sJson)
		{
		   delete [] sJson;
		   sJson = NULL;
		}
		return -1;
	}
	
	if (NULL != sJson)
	{
		delete [] sJson;
		sJson = NULL;
	}

	//��ַ��
	string addrCode = "";	 
	if( !rootNode["addrcode"].isNull() )
	{
		addrCode = rootNode["addrcode"].asString(); 
	}
	
	//�豸IP
	string ip = "0.0.0.0";
	char cIp[MAX_ADDR_LEN] = {0};
	if( !rootNode["deviceHost"].isNull())
	{
		ip = rootNode["deviceHost"].asString();
		strcpy(cIp, ip.c_str());
	}
	
	//�豸��  
	string stDeviceName = "";
	char deviceName[MAX_NAME_LEN_C2] = {0};
	if( !rootNode["deviceName"].isNull() )
	{
		stDeviceName = utf2asc(rootNode["deviceName"].asString().c_str());
		strcpy(deviceName, stDeviceName.c_str());
	}  
	
	//�豸�˿� 
	int devicePort =0;
	if( !rootNode["devicePort"].isNull() )
	{
		if(rootNode["devicePort"].isInt())
		{
			devicePort = rootNode["devicePort"].asInt();
		}
	}
	
	//�澯ʱ��
	string time = "0000-00-00 00:00:00";	
	char cTime[MAX_NAME_LEN_C2] = {0};
	if( !rootNode["time"].isNull() )
	{
		time = rootNode["time"].asString();
		strcpy(cTime, time.c_str());
	}

	//����
	int level = 0;
	if( !rootNode["level"].isNull() )
	{
		if(rootNode["level"].isInt())
		{
			level = rootNode["level"].asInt(); 
		}		
	}
	
	//�汾
	string strVersion = "";  
	if( !rootNode["version"].isNull() )
	{
		strVersion = rootNode["version"].asString(); 
	}
  	
	//�澯��
	SHL_Json::Value alarms = rootNode["alarms"]; 
	for(int index = 0; index < alarms.size(); index++)
	{
		SHL_Json::Value ele = alarms[index];		
		
		ALARMER_S pAlarm = {0};	

		strcpy(pAlarm.s8DeviceName, deviceName); //�豸��
        pAlarm.u16DevicePort = devicePort;		//�豸�˿�
		INET_PTON(cIp, pAlarm.u8DeviceIp);		//�豸IP

		//�澯���� 
		int type = -1;
		if( !ele["type"].isNull() )
		{
			if(ele["type"].isInt())
			{
				type = ele["type"].asInt(); 
			}		
		}
 
        //��������ʱ��
        DATETIME_S aiTime; 
        memset(&aiTime,0,sizeof(DATETIME_S));

        sscanf(cTime,"%u-%u-%u %u:%u:%u",
            &aiTime.u32Year,&aiTime.u32Month,&aiTime.u32Day,
            &aiTime.u32Hour,&aiTime.u32Minute,&aiTime.u32Second );

        //ͨ����
        int	aiChnNo = 0;
		if( !ele["channelNo"].isNull() )
		{
			if(ele["channelNo"].isInt())
			{
				aiChnNo = ele["channelNo"].asInt(); 
			}		
		} 

        //ͨ��Ԥ��λ
        int aiPreset = 0;
		if( !ele["preset"].isNull() )
		{
			if(ele["preset"].isInt())
			{
				aiPreset = ele["preset"].asInt(); 
			}		
		}  

        //roiId (�����±�)
        int aiId = 0;
		if( !ele["roiId"].isNull() )
		{
			if(ele["roiId"].isInt())
			{
				aiId = ele["roiId"].asInt(); 
			}		
		}  
	 
        //ͼƬ����·��
        string aiPicName = "";  
		if( !ele["picName"].isNull() )
		{
			aiPicName = utf2asc(ele["picName"].asString().c_str());
		}

		//��ȡ�澯״̬ 
        int aiStatus = 0; 
		if( !ele["status"].isNull() )
		{					
			if(ele["status"].isInt())
			{
				aiStatus = ele["status"].asInt(); 
			}
			else if(ele["status"].isString())
			{
				string strStatus = ele["status"].asString();
				aiStatus = atoi(strStatus.c_str());
			}
		}

        //�豸��ַ��
		string guCode = "";
		char cGuCode[MAX_NAME_LEN_C2] = {0};
		if( !ele["gucode"].isNull() )
		{
			guCode = ele["gucode"].asString();
			strcpy(cGuCode, guCode.c_str());
		}
	 
        //ͨ������
        string name = "";
		char cName[MAX_NAME_LEN_C2] = {0};
		if( !ele["name"].isNull() )
		{
			name = utf2asc(ele["name"].asString().c_str());
			strcpy(cName, name.c_str());
		}
  
		switch(type)
		{
			
		case ALARM_DEV_STATUS:  
			{				
				ALARM_DEVSTATE_RPT_S devState = {0};
				
				devState.u8DeviceIp[0] = pAlarm.u8DeviceIp[0];
				devState.u8DeviceIp[1] = pAlarm.u8DeviceIp[1];
				devState.u8DeviceIp[2] = pAlarm.u8DeviceIp[2];
				devState.u8DeviceIp[3] = pAlarm.u8DeviceIp[3];
				
				devState.u16DevicePort = devicePort;
				devState.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;	
				
				devState.stTime = aiTime;//pConn->String2DateTime(cTime);  
				
				int size = sizeof(devState);				
				iType = REPORT_DEV_STAT;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_DEVSTATE_RPT_S*)&devState, size, pConn->m_pAlarmUserData);  //aaaaaaaaaa
			}
			break;
		case ALARM_DEV_IO: /**< �豸I/O̽ͷ���� */
			{							
				ALARM_IO_RPT_S io = {0};
							 
				io.stTime = aiTime;//pConn->String2DateTime(cTime);
				io.u8ChnNo = 0;
				io.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;	
				
				int size = sizeof(io);				
				iType = ALARM_IO;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_IO_RPT_S*)&io, size, pConn->m_pAlarmUserData);  //aaaaaaaaaa
			}
			break;
		case ALARM_DEV_HDD: /**< �豸Ӳ�̱��� */
			{					
				ALARM_HDD_RPT_S hdd = {0};
				
				hdd.stTime = aiTime;  //aaaaaaaaa
				
				hdd.u8HddNo = 0;
				hdd.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;	
				
				int size = sizeof(hdd);				
				iType = ALARM_HDD;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_IO_RPT_S*)&hdd, size, pConn->m_pAlarmUserData);  //aaaaaaaaaa
			}
			break;
		case ALARM_DEV_VLOSS: /**< �豸��Ƶ��ʧ���� */
			{				
				ALARM_VLOSS_RPT_S vLoss = {0};
				
				vLoss.stTime = aiTime;  //aaaaaaaaa
				
				vLoss.u8ChnNo = 0;
				vLoss.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;	
				
				int size = sizeof(vLoss);				
				iType = ALARM_VLOSS;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_VLOSS_RPT_S*)&vLoss, size, pConn->m_pAlarmUserData);  //aaaaaaaaaa
			}
			break;
		case ALARM_DEV_MD: /**< �豸�ƶ���ⱨ�� */
			{				
				ALARM_MD_RPT_S md = {0};
				
				md.stTime = aiTime;  //aaaaaaaaa
				
				md.u8ChnNo = 0;
				md.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;	
				
				int size = sizeof(md);				
				iType = ALARM_MD;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_MD_RPT_S*)&md, size, pConn->m_pAlarmUserData);  //aaaaaaaaaa
			}
			break;
		case ALARM_DEV_VCOVER: /**< �豸��Ƶ�ڵ����� */
			{
				iType = ALARM_VCOVER;
			}
			break;
		case ALARM_DEV_INVADE: /**< �豸�Ƿ����ֱ��� */
			{
				
			}
			break;
		case ALARM_DEV_MANUAL: /**< �ֳ��豸�ֶ����� */
			{				
				ALARM_MANUAL_RPT_S manual = {0};
				
				manual.u8ChnNo = 0; //aaaaaaaaa
				manual.stTime = aiTime; //aaaaa
				manual.u8Status = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
				
				int size = sizeof(ALARM_MANUAL_RPT_S);	
				iType = ALARM_MANUAL;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_MANUAL_RPT_S*)&manual, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_DEV_GPS: /**< �ֳ��豸GPS���� */
			{				
				ALARM_GPS_RPT_S gps = {0};
				
				gps.stTime = aiTime; //aaaaa
				gps.s8Division[0] = 'E';//aaaaaaaa
				gps.s8Division[1] = 'W';//aaaaaaaa
				gps.u8Satelites = 0;//aaaaaaaaa
				gps.u8Precision = 100;//aaaaaaaa
				gps.u32Longitude = 0;
				gps.u32Latitude = 0;
				gps.u32Direction = 0;//
				gps.u32Speed = 0;
				gps.u32Height = 0;
				
				int size = sizeof(gps);	
				iType = ALARM_GPS;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_GPS_RPT_S*)&gps, size, pConn->m_pAlarmUserData); 				
			}
			break;
		case ALARM_DEV_CALL: /**< �ſڻ����б����� */
			{ 				
				ALARM_CALL_RPT_S call = {0};
				
				call.u8DevIp[0] = pAlarm.u8DeviceIp[0];
				call.u8DevIp[1] = pAlarm.u8DeviceIp[1];
				call.u8DevIp[2] = pAlarm.u8DeviceIp[2];
				call.u8DevIp[3] = pAlarm.u8DeviceIp[3];
				
				call.u16DevPort = devicePort;
				call.u16Chn     = aiChnNo;	 //aaaaaaaaaa

				int  cmdCall =  0;
				if( !ele["cmd"].isNull() )
				{
                    cmdCall = ele["cmd"].asInt(); 
				}
				
				call.u8Cmd      = cmdCall;	 //aaaaaaaaaa
				
				int size = sizeof(call);	
				iType = ALARM_CALL_STATUS;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_CALL_RPT_S*)&call, size, pConn->m_pAlarmUserData); 				
				
			}
			break;
		case ALARM_DEV_END: /**< �ָ���ʶ: �¶����豸������������֮ǰ��� */
			{
				
			}
			break;
		case ALARM_AI_TRIPWIRE: /**< ���� */
			{ 				                
                ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_TRIPWIRE;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiTripwite,0,MAX_AIOBJ_NUM*sizeof(AI_TRIPWIRE_S));
				ai.AiTripwite[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiTripwite[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_PERIMETER:                                    /**< �ܽ� */
			{ 							                
                ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_PERIMETER;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiPerimeter,0,MAX_AIOBJ_NUM*sizeof(AI_PERIMETER_S));
				ai.AiPerimeter[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiPerimeter[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_ITEMLEFT: /**< ��Ʒ������ʧ */
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_ITEMLEFT;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiItemleft,0,MAX_AIOBJ_NUM*sizeof(AI_ITEMLEFT_S));
				ai.AiItemleft[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiItemleft[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_PLATE: /**< ���Ƽ�� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_PLATE;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = aiStatus;

                memset(&ai.AiPlate,0,sizeof(AI_PLATE_S));
				
				//���ƺ�
				string plateLicense = "";
				char cPlateLicense[12] = {0};
				if( !ele["plateLicense"].isNull() )
				{
					plateLicense = utf2asc(ele["plateLicense"].asString().c_str());
					strcpy(cPlateLicense, plateLicense.c_str());
				}
				strcpy(ai.AiPlate.s8Plate, cPlateLicense);

				//��������
				int plateType = 0; 
				if( !ele["plateType"].isNull() )
				{
					if(ele["plateType"].isInt())
					{
						plateType = ele["plateType"].asInt();						
					}
				}
				ai.AiPlate.u8PlateType = plateType;

				//������ɫ
				int plateColor = 0; 
				if( !ele["plateColor"].isNull() )
				{
					if(ele["plateColor"].isInt())
					{
						plateColor = ele["plateColor"].asInt();						
					}
				}
				ai.AiPlate.u8PlateColor = plateColor;

				//������ɫ
				int carColor = 0; 
				if( !ele["carColor"].isNull() )
				{
					if(ele["carColor"].isInt())
					{
						carColor = ele["carColor"].asInt();						
					}
				}
				ai.AiPlate.u8CarColor = carColor;

				//��������
				int carType = 0; 
				if( !ele["carType"].isNull() )
				{
					if(ele["carType"].isInt())
					{
						carType = ele["carType"].asInt();						
					}
				}
				ai.AiPlate.u8CarType = carType;

				//��������
				int carDirect = 0; 
				if( !ele["carDirect"].isNull() )
				{
					if(ele["carDirect"].isInt())
					{
						carDirect = ele["carDirect"].asInt();						
					}
				}
				ai.AiPlate.u8Direct = carDirect;

				//DVR�豸����
				int devType = 0; 
				if( !ele["manuPlateType"].isNull() )
				{
					if(ele["manuPlateType"].isInt())
					{
						devType = ele["manuPlateType"].asInt();						
					}
				}				
				ai.AiPlate.u8DevType = devType;

				//��ȡ����
				U16 millSec = 0;
				DATETIME_S aiTime1; 
				memset(&aiTime1,0,sizeof(DATETIME_S));				
				sscanf(cTime,"%u-%u-%u %u:%u:%u:%u",
					&aiTime1.u32Year,&aiTime1.u32Month,&aiTime1.u32Day,
            &aiTime1.u32Hour,&aiTime1.u32Minute,&aiTime1.u32Second, &millSec );
				ai.AiPlate.u16Millsecond = millSec;
			
				//��ȡͼƬ��
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_HOVER: /**< �ǻ���� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_HOVER;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiHover,0,MAX_AIOBJ_NUM*sizeof(AI_HOVER_S));
				ai.AiHover[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiHover[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_ANTITHEFT: /**< ������� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_ANTITHEFT;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiAntitheft,0,MAX_AIOBJ_NUM*sizeof(AI_ANTITHEFT_S));
				ai.AiAntitheft[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiAntitheft[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_OFFJOB: /**< �Ѹڼ�� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_OFFJOB;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiOffjob,0,MAX_AIOBJ_NUM*sizeof(AI_OFFJOB_S));
				ai.AiOffjob[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiOffjob[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_MAINTENAREA: /**< ���������� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_MAINTENAREA;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiMaintenarea,0,MAX_AIOBJ_NUM*sizeof(AI_MAINTENAREA_S));
				ai.AiMaintenarea[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiMaintenarea[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_PRESSPLATE: /**< ѹ��Ͷ��״̬��� */
			{	
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_PRESSPLATE;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(&ai.AiPressplate,0,MAX_AIOBJ_NUM*sizeof(AI_PRESSPLATE_S));
// 				ai.AiPressplate.u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiPressplate.u8Enable[0] = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_INDICATOR: /**< �ֺ�ָʾ�Ƽ��*/
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_INDICATOR;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiIndicator,0,MAX_AIOBJ_NUM*sizeof(AI_INDICATOR_S));
				ai.AiIndicator[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiIndicator[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_DISCONNECTOR:
			{            
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_DISCONNECTOR;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiDisconnector,0,MAX_AIOBJ_NUM*sizeof(AI_DISCONNECTOR_S));
				ai.AiDisconnector[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiDisconnector[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_LIGHT:
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_LIGHT;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiLight,0,MAX_AIOBJ_NUM*sizeof(AI_LIGHT_S));
// 				ai.AiLight[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiLight[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_HELMET: 
			{                 
                ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_HELMET;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiHelmet,0,MAX_AIOBJ_NUM*20*sizeof(AI_HELMET_S));
				ai.AiHelmet[aiId][0].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiHelmet[aiId][0].u8Enable = 1;

				
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_YX_END: /**< �ָ���ʶ: ��ң����������֮ǰ��� */
			{
				return -1;
			}
			break;
		case ALARM_AI_LEDDIGIT:                            /**< ��ƶ������(�߶������) */
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_LEDDIGIT;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiLeddigit,0,MAX_AIOBJ_NUM*sizeof(AI_LEDDIGIT_S));
				ai.AiLeddigit[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiLeddigit[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_METER:                                        /**< ��ƶ������(�̶���) */
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_METER;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiMeter,0,MAX_AIOBJ_NUM*sizeof(AI_METER_S));
// 				ai.AiMeter[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiMeter[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_NUMPEOPLE: /**< ����ͳ�� */
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_NUMPEOPLE;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiNumPeople,0,MAX_AIOBJ_NUM*sizeof(AI_NUMPEOPLE_S));
// 				ai.AiNumPeople[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiNumPeople[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_THERMOMETRY:                                  /**< ������ */
			{ 
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_THERMOMETRY;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiThermometry,0,MAX_AIOBJ_NUM*sizeof(AI_THERMOMETRY_S));
				//ai.AiThermometry[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiThermometry[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_AI_YC_END: /**< �ָ���ʶ: ��ң����������֮ǰ��� */
			{
				return -1;
			}
			break;				
		case ALARM_PUSH_SYSTEMINFO: /**< ϵͳ���в��� */		 
			{
				//״̬ 
				int status = rootNode["status"].asInt(); 
				
				//����
				string name = rootNode["name"].asString(); 
				char cName[MAX_NAME_LEN_C2] = {0};
				strcpy(cName, name.c_str());
				
				string code = rootNode["code"].asString(); //��ַ��
				char cCode[MAX_NAME_LEN_C2] = {0};
				strcpy(cCode, code.c_str());
				
				strcpy(pAlarm.s8DeviceName, cName);
				pAlarm.u16DevicePort = devicePort;				

				INET_PTON(cIp, pAlarm.u8DeviceIp);
				
				ALARM_SYS_RPT_S sys = {0};
				
				sys.stStartTime = 0;
				sys.stRunTime = 0;
				sys.u32ClientNo = 0;
				sys.u32TranTaskNo = 0;
				sys.u32CpuUsed = 0;
				sys.u32TranCpuUsed = 0;
				sys.u32PhMemTotal = 0;
				sys.u32PhMemRemain = 0;
				sys.u32VirMemRemain = 0;
				sys.u32VirMemTotal = 0;
				sys.fRecvNetworkRemain = 0;
				sys.fRecvNetworkTotal = 0;
				sys.fSendNetworkRemain = 0;
				sys.fSendNetworkTotal = 0;
				
				int size = sizeof(sys);	
				iType = SYSTEM_INFO_UPLOAD;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_SYS_RPT_S*)&sys, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_PUSH_LINKAGE: /**< ��������ϴ� */
			{
				//״̬ 
				int status = rootNode["status"].asInt(); 
				
				//����
				string name = rootNode["name"].asString(); 
				char cName[MAX_NAME_LEN_C2] = {0};
				strcpy(cName, name.c_str());
				
				string code = rootNode["code"].asString(); //��ַ��
				char cCode[MAX_NAME_LEN_C2] = {0};
				strcpy(cCode, code.c_str());
				
				strcpy(pAlarm.s8DeviceName, cName);
				pAlarm.u16DevicePort = devicePort;

				INET_PTON(cIp, pAlarm.u8DeviceIp);
				
				LINKAGE_RPT_S linkage = {0};
				strcpy(linkage.s8TaskName, cName);
				linkage.u8ChnNO = 0; //aaaaaaaaa
				linkage.u8PresetNo = 0;//aaaaaaaaaa
				linkage.u16TaskId = 12;//aaaaaaaaaaa
				
				linkage.u8DeviceIp[0] = pAlarm.u8DeviceIp[0];
				linkage.u8DeviceIp[1] = pAlarm.u8DeviceIp[1];
				linkage.u8DeviceIp[2] = pAlarm.u8DeviceIp[2];
				linkage.u8DeviceIp[3] = pAlarm.u8DeviceIp[3];
				
				int size = sizeof(linkage);	
				iType = LINKAGE_PUSH;
				pConn->m_pfnWFLinkageCallback(&pAlarm, iType, (LINKAGE_RPT_S*)&linkage, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_PUSH_LINKAGE_TRANSMIT: /**< �ͻ�������ת�� */
			{
				//״̬ 
				int status = rootNode["status"].asInt(); 
				
				//����
				string name = rootNode["name"].asString(); 
				char cName[MAX_NAME_LEN_C2] = {0};
				strcpy(cName, name.c_str());
				
				string code = rootNode["code"].asString(); //��ַ��
				char cCode[MAX_NAME_LEN_C2] = {0};
				strcpy(cCode, code.c_str());
				
				strcpy(pAlarm.s8DeviceName, cName);
				pAlarm.u16DevicePort = devicePort;
					
				INET_PTON(cIp, pAlarm.u8DeviceIp);
				
				LINKAGE_TRANS_RPT_S linkageTrans = {0};
				strcpy(linkageTrans.s8SrcName, cName);
				strcpy(linkageTrans.s8Desc, "");//aaaaaaaa
				linkageTrans.u32DeviceNum = 0; //aaaaaaaaa
				linkageTrans.u32CameraId[0] = 0;//aaaaaaaaaa
				linkageTrans.u8PresetNo[0] = 12;//aaaaaaaaaaa
				
				linkageTrans.u8SrcIp[0] = pAlarm.u8DeviceIp[0];
				linkageTrans.u8SrcIp[1] = pAlarm.u8DeviceIp[1];
				linkageTrans.u8SrcIp[2] = pAlarm.u8DeviceIp[2];
				linkageTrans.u8SrcIp[3] = pAlarm.u8DeviceIp[3];
				
				int size = sizeof(linkageTrans);	
				iType = LINKAGE_TRANSMIT;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (LINKAGE_TRANS_RPT_S*)&linkageTrans, size, pConn->m_pAlarmUserData); 
			}
			break;
		case ALARM_PUSH_VI: /**< ���ܷ���Ѳ�����������ʼ */
			{
				iType = ALARM_VI;
			}
			break;
			
		case ALARM_PUSH_END: /**< �ָ���ʶ: �¶���������������֮ǰ��� */
			{
				return -1;
			}
			break;
		case ALARM_AI_SIGNAL_LOST:                             /**< ��Ƶ�źŶ�ʧ */
		case ALARM_AI_SIGNAL_CONTRASTLOW:                                 /**< �Աȶȵ� */
		case ALARM_AI_SIGNAL_FUZZY:                                 /**< ͼ��ģ�� */
		case ALARM_AI_SIGNAL_OVERORUNDERBRIGHT:                            /**< ͼ�����\���� */
		case ALARM_AI_SIGNAL_COLORCAST:                             /**< ͼ��ƫɫ */
		case ALARM_AI_SIGNAL_NOISEJAMMING:                          /**< �������� */
		case ALARM_AI_SIGNAL_STRIPENOISE:                           /**< ���Ƹ��� */
		case ALARM_AI_SIGNAL_BLACKWHITEIMAGE:                       /**< �ڰ�ͼ�� */
		case ALARM_AI_SIGNAL_VIDEOOCCLUSION:                        /**< ��Ƶ�ڵ� */
		case ALARM_AI_SIGNAL_IMAGEFREEZE:                           /**< ���涳�� */
		case ALARM_AI_SIGNAL_VIDEOUPHEAVAL:                         /**< ��Ƶ��� */
		case ALARM_AI_SIGNAL_VIDEOSHAKE:                            /**< ��Ƶ���� */
		case ALARM_AI_SIGNAL_SCENECHANGE:                           /**< ������� */
		case ALARM_AI_SIGNAL_PTZ_OUTOFCONTROL:                      /**< ��̨ʧ�� */
		case ALARM_AI_VIDEO_QUALITY_END:                            /**< �ָ���ʶ */
			
		case ALARM_PB_EXCEEDHEIGHT:                                 /**< �����޸� */
		case ALARM_PERSION_TOGETHER:                                /**< ��Ա�ۼ� */
		case ALARM_PERMETER_PERSON:                                 /**< �ܽ���-�� */
		case ALARM_PERMETER_CAR:                                    /**< �ܽ���-�� */
		case ALARM_PERMETER_NOCAR:                                  /**< �ܽ���-�ǻ����� */
		case ALARM_PERMETER_OBJECT:                                 /**< �ܽ���-�� */
		case ALARM_CAR_NOIN:                                        /**< �����ܿ�-�������� */
		case ALARM_CAR_NOSTOP:                                      /**< �����ܿ�-��ͣ���� */
		case ALARM_PB_FALLDOWN:                                     /**< ���� */
		case ALARM_PB_DAJIA:                                        /**< ��� */
		case ALARM_PB_SMOKE:                                        /**< ���� */
		case ALARM_FIRE_CONTROL:                                    /**< �̻� */
		case ALARM_TEMP_AREA:                                       /**< ������� */
		case ALARM_TEMP_DIFF:                                       /**< �²��� */
			
		case ALARM_TEMP_BODONG:                                     /**< �²�� */
		case ALARM_WAIGUAN_MINPAI:
			break;                                                  /**< ��ۼ��-���� */
		case ALARM_WAIGUAN_JUEYUAN:                                 /**< ��ۼ��-��Ե�� */
			{				
				printf("ALARM_WAIGUAN_JUEYUAN\n");
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_JUEYUAN;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;
				
                memset(ai.AiJueYuan,0,MAX_AIOBJ_NUM*sizeof(AI_JUEYUAN_S));
				ai.AiJueYuan[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiJueYuan[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				
                printf("ALARM_WAIGUAN_JUEYUAN callback\n");
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 				
			}
			break;
		case ALARM_WAIGUAN_TAOGUAN:                                 /**< ��ۼ��-�׹� */
		case ALARM_WAIGUAN_BIANYABT:                                /**< ��ۼ��-��ѹ������ */
		case ALARM_WAIGUAN_BILEIZHENG:                              /**< ��ۼ��-������ */
		case ALARM_WAIGUAN_PATI:                                    /**< ��ۼ��-���� */
		case ALARM_WAIGUAN_QIANGTI:break;                           /**< ��ۼ��-ǽ�� */
		case ALARM_WAIGUAN_GANTANG:                                 /**< ��ۼ��-���� */			
			{				
                printf("ALARM_WAIGUAN_GANTANG\n");
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_GANTA;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;
				
                memset(ai.AiGanTa,0,MAX_AIOBJ_NUM*sizeof(AI_GANTA_S));
				ai.AiGanTa[aiId].u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
                ai.AiGanTa[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				
                printf("ALARM_WAIGUAN_GANTANG callback\n");
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 				
			}
			break;
		case ALARM_FACE_DETECTTION:                                 /**< ������� */
			{			
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_SNAPMATCH;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(&ai.AiSnapMatch,0,sizeof(AI_SNAPMATCH_S));
// 				ai.AiSnapMatch.u8Alarm = (aiStatus!=0) ? (aiStatus!=-1?1:128) : 0;
//                 ai.AiSnapMatch.u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 
				
			 
			}
			break;
		case ALARM_FACE_IDCARD_DETECTTION:                          /**< ��֤��һ��� */
			break;
		case ALARM_GATE_STATUS:										/**< ��բ״̬ */
			{
				
                printf("ALARM_GATE_STATUS\n");
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_GATE;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;

                memset(ai.AiGate,0,MAX_AIOBJ_NUM*sizeof(AI_GATE_S));
				/** <aiStatus:0 - ��բ���ڿ�;1 - ��բ���ڹ�;2 - ��բ����λ;3 - ��բ�ص�λ;4 - ��բ���ź�;5 - ��բ  */
				/** <ai.AiGate[aiId].u8Alarm:0-�أ�1-�� */
				ai.AiGate[aiId].u8Alarm = (aiStatus==0)|| (aiStatus==2)? 1 : 0;
                ai.AiGate[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				
                printf("ALARM_GATE_STATUS callback\n");
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 				
			}
			break;
			
		case ALARM_SYSTEM_STATUS:                                   /**< ϵͳ����״̬: add by shl  */
			break;
		case ALARM_GATEWAY_STATUS:                                  /**< ͨ����բ״̬ */   
			break;
		case ALARM_THERMOMETRY_ALARM:                                /**< �����¶ȸ澯 */
			{
                printf("ALARM_THERMOMETRY_INFO\n");
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_THERMOMETRYALARM;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;
				
                memset(ai.AiThermometryAlarm,0,MAX_AIOBJ_NUM*sizeof(AI_THERMOMETRY_ALARM_S));
				ai.AiThermometryAlarm[aiId].u8Alarm = aiStatus;
                ai.AiThermometryAlarm[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				
                printf("ALARM_THERMOMETRY_INFO callback\n");
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 				
			}
			break;
		case ALARM_PTZ_CONTROL_ALARM:                                  /**< ��̨���Ƹ澯 */   
			{
                printf("ALARM_PTZ_ALARM_INFO\n");
				ALARM_AI_RPT_S ai = {0};
                ai.stTime = aiTime; 
                ai.u8ChnNo = aiChnNo;
                ai.u8AiType = AI_TYPE_PTZ_ALARM;
                ai.u8Preset = aiPreset;
                ai.u8AreaType = 0;
                ai.u16PicHeight = 0;
                ai.u16PicWidth = 0;
                ai.s32Status = 1;
				
                memset(ai.AiPTZControlAlarm,0,MAX_AIOBJ_NUM*sizeof(AI_PTZ_ALARM_S));
				ai.AiPTZControlAlarm[aiId].u8Alarm = aiStatus;
                ai.AiPTZControlAlarm[aiId].u8Enable = 1;
				strcpy(ai.s8PicName, aiPicName.c_str());
				
				int size = sizeof(ai);	
				iType = ALARM_AI;
				
                printf("ALARM_PTZ_ALARM_INFO callback\n");
				pConn->m_pfnAlarmCallback(&pAlarm, iType, (ALARM_AI_RPT_S*)&ai, size, pConn->m_pAlarmUserData); 				
			}
			break;
		default:
			{
				return -1;
			}
			break;
		}			 
	}
	return 0;
}

//************************************************************************
//�������ƣ�C2RecordCallback(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata)
//����˵��������¼���¼�ص���������
//����������
//    (out) record    ����¼���¼�ṹ
//    (out) realNum   �ܹ�ʵ�ʼ�¼��
//    (out) fromIndex ��¼��ʼ���, ��1��ʼ
//    (out) toIndex   ��¼�������
//    (out) userdata  �û�����
//����ֵ��void
//************************************************************************/
void C2RecordCallback(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata)
{
	C2PlatformConn *pConn = (C2PlatformConn *)userdata;
	
	REC_DETAIL_RSP_S recDetail;
	recDetail.u32Size = record->size;
	recDetail.u32ChnNO = record->channelNo;
	recDetail.stBegin.u32Year = record->start.year;
	recDetail.stBegin.u32Month = record->start.month;
	recDetail.stBegin.u32Day = record->start.day;
	recDetail.stBegin.u32Hour = record->start.hour;
	recDetail.stBegin.u32Minute = record->start.minute;
	recDetail.stBegin.u32Second = record->start.second;
	
	recDetail.stEnd.u32Year = record->end.year;
	recDetail.stEnd.u32Month = record->end.month;
	recDetail.stEnd.u32Day = record->end.day;
	recDetail.stEnd.u32Hour = record->end.hour;
	recDetail.stEnd.u32Minute = record->end.minute;
	recDetail.stEnd.u32Second = record->end.second;
	
	if(record->type == 1048576)
	{
		recDetail.u32Type = 0;  //aaaaaaaaaaa
	}
	strcpy(recDetail.s8FileName, record->filename);
	recDetail.u32StSize = sizeof(recDetail);
	pConn->m_pfnQueryDetailCallback(&recDetail, pConn->m_pUserRecord);
}

//************************************************************************
//�������ƣ�C2PresetCallback(c2_preset_s *preset, void *pUser)
//����˵�����豸�б����ݻص�
//����������
//    (out) preset    Ԥ��λ��Ϣ
//    (out) pUser  �û�����
//����ֵ��void
//************************************************************************/
void C2PresetCallback(c2_preset_s *preset, void *pUser)
{
	C2PlatformConn *pConn = (C2PlatformConn *)pUser;
	
	pConn->m_pfnPresetListCallback(preset->index, preset->name, pConn->m_pUserPreset);
}

//************************************************************************
//�������ƣ�C2PresetCallbackEx(c2_preset_s *preset, void *pUser)
//����˵�����豸�б����ݻص����¹�Լ��
//����������
//    (out) preset    Ԥ��λ��Ϣ
//    (out) pUser  �û����� 
//����ֵ��void
//************************************************************************/
void C2PresetCallbackEx(c2_preset_s *preset, void *pUser)
{
	C2PlatformConn *pConn = (C2PlatformConn *)pUser;
	
	//preset->nameΪutf-8��ʽ����ת��ΪUnicode����
	int len = MultiByteToWideChar(CP_UTF8, 0, preset->name, -1, NULL, 0);
	wchar_t *strName = new wchar_t[len + 1];
	wmemset(strName, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, preset->name, -1, strName, len);
	
	//�ٰ�Unicode����ת��ΪGBK����
	len = WideCharToMultiByte(CP_ACP, 0, strName, -1, NULL, 0, NULL, NULL);
	char* presetName = new char[len];
	WideCharToMultiByte(CP_ACP, 0, strName, -1, presetName, len, NULL, NULL);
	
	pConn->m_pfnPresetListCallbackEx(preset->index, presetName, preset->type, pConn->m_pUserPreset);
	
	delete []strName;
	delete []presetName;
}

//************************************************************************
//�������ƣ�Start(const NET_CONN_PARAM_S *pParam)
//����˵����������¼
//����������
//    (in) pParam ��¼����
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformConn::Start(const NET_CONN_PARAM_S *pParam)
{
	do 
	{
		if(NULL == pParam)
		{
			break;
		}
		
		c2_login_s loginInfo = {0};
		
		if(NULL != pParam->s8Ip)//�豸IP
		{
			strcpy(m_ip, pParam->s8Ip);
			loginInfo.servIp = m_ip;			 
		}
		else 
		{
			loginInfo.servIp = NULL;
		}
		m_port =  pParam->u16Port;
		loginInfo.servPort = m_port;
		
		if(NULL != pParam->ps8Username)//�豸IP
		{
			strcpy(m_subName, pParam->ps8Username);
			loginInfo.username = m_subName;		 
		}
		else 
		{
			loginInfo.username = NULL;
		}	
		
		if(NULL !=  pParam->ps8Password)//�豸IP
		{
			strcpy(m_password, pParam->ps8Password);
			loginInfo.password = m_password;		 
		}
		else 
		{
			loginInfo.password = NULL;
		}	
		
		loginInfo.subscribe = true; 
		
		if(NULL != pParam->fConnCallback)
		{
			m_pfnConnCallback = pParam->fConnCallback;
		}
		
		if(NULL != pParam->fAlarmCallback)
		{
			m_pfnAlarmCallback = pParam->fAlarmCallback;
		}
		m_pUserLogin = pParam->pUser;
		
		if (0==strcmp(pParam->ps8Username, "admin") && 
			(0==strcmp(pParam->ps8Password, "111111")))
		{
			c2_ex_param_s md5 = { "disableMD5", "no"};
			loginInfo.exLen = 1 * sizeof(c2_ex_param_s);
            loginInfo.exParam = &md5;
		}
		else
		{
			c2_ex_param_s md5 = { "disableMD5", "yes"};
			loginInfo.exLen = 1 * sizeof(c2_ex_param_s);
            loginInfo.exParam = &md5;
		}
		
		m_hLogin = c2_login(&loginInfo, C2LoginCallback, this);
		if (NULL == m_hLogin)
		{
			break;
		}
		
		m_pAlarmUserData = pParam->pUser;
		c2_subscribe(m_hLogin, 0xFFFFFFFFFFFFFFFF, C2AlarmCallback, this); //aaaa
		
		return TRUE;
		
	} while (0);
	
	return FALSE;
}

//************************************************************************
//�������ƣ�Stop
//����˵����ֹͣ
//������������    
//����ֵ����
//************************************************************************/
VOID C2PlatformConn::Stop()
{
	{		
		AutoLock lock(m_mtxPView);
		for(size_t i=0; i<m_vcPViews.size(); i++)
		{
			delete m_vcPViews[i];
		} 
		m_vcPViews.clear();
	}
	
	if(NULL != m_hLogin)
	{	
		c2_logout(m_hLogin);
		m_hLogin = NULL;	
	}
	
	m_pfnAlarmCallback = NULL;
	m_pfnConnCallback = NULL;
	m_pfnDeviceListCallback = NULL;
	m_pfnQueryDetailCallback = NULL;
	m_pfnQueryCalendarCallback = NULL;
	m_pfnPresetListCallback = NULL;
	m_pfnPresetListCallbackEx = NULL;
	m_pfnUserListCallback = NULL; 
	m_pfnWFLinkageCallback= NULL;
}

//************************************************************************
//�������ƣ�AddStandby
//����˵�������һ����¼����������ַ
//����������
//    (in) ip ��������IP
//	  (in) port ���������˿�
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformConn::AddStandby(const S8 *ip, U32 port)
{ 
	if((NULL != ip) && (port > 0) && (NULL != m_hLogin))
	{
		long lRet = c2_add_standby(m_hLogin, ip, port);
		if(0 == lRet)
		{
			return TRUE;
		}
	}	
	return FALSE;
}	

//************************************************************************
//�������ƣ�DelStandby
//����˵����ɾ��һ����¼����������ַ
//����������
//    (in) ip ��������IP
//	  (in) port ���������˿�
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformConn::DelStandby(const S8 *ip, U32 port)
{
	if((NULL != ip) && (port > 0) && (NULL != m_hLogin))
	{
		long lRet = c2_del_standby(m_hLogin, ip, port);
		if(0 == lRet)
		{
			return TRUE;
		}
	}	
	return FALSE;
}	



//************************************************************************
//�������ƣ�GetLoginRsp(LOGIN_RSP_S *pRsp)
//����˵������ȡ��¼��ý���������Ϣ
//����������
//    (in) pRsp �豸�����Ϣ�ṹ��
//����ֵ����
//************************************************************************/
BOOL C2PlatformConn::GetLoginRsp(LOGIN_RSP_S *pRsp)
{
	ASSERT(pRsp);
	
	if(NULL == m_hLogin)
	{
		return FALSE;
	}
	
	c2_servinfo_s info;
	long lret = c2_get_servinfo(m_hLogin, &info);
	if (lret < 0)
	{
		return FALSE;
	}
	
	strcpy(pRsp->s8SWVersion, info.version);
	strcpy(pRsp->s8SystemName, info.msvname);
    strcpy(pRsp->s8AddrCode, info.msvcode);
	pRsp->u32DeviceNum = info.deviceNum;
	
	strcpy(pRsp->s8HWVerison, "");  //Ӳ���汾
	strcpy(pRsp->s8IEVerison, "");	//IE�汾  
	pRsp->u32UserKey = 0; //��Կ
	
	pRsp->stUserRight.u8OptLevel = info.optLevel;
	pRsp->stUserRight.u8MngLevel = info.mgrLevel;
	
	pRsp->stUserRight.u32MngRightBit = info.mgrBitmap;
	pRsp->stUserRight.u32OptRightBit = info.optBitmap;
	pRsp->stUserRight.u32PVChannel = 0;
	pRsp->stUserRight.u32PBChannel = 0;
	pRsp->stUserRight.u32PtzChannel = 0;
	pRsp->stUserRight.u32AlarmChannel = 0;
	
	pRsp->u32ChnInfoSize = 0;//ͨ����
	
	return FALSE;
}

//************************************************************************
//�������ƣ�GetUserList(const NET_USER_PARAM_S *pParam)
//����˵������ȡ�û��б�
//����������
//    (in) pParam    ������Ϣ�ṹ��
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::GetUserList(const NET_USER_PARAM_S *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{
		m_pfnUserListCallback = pParam->fUserListCallback;
		m_pUserData = pParam->pUser;	
		
		long lRet = c2_query_user_v1(m_hLogin, C2UserInfoCallback, this);
		
		if(0 == lRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�RequestIFrame(const NET_REQI_PARAM_S *pParam)
//����˵��������ʵʱ�������ϲ���һ��I֡
//����������
//    (in) pParam    ����I֡�ṹ��
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::RequestIFrame(const NET_REQI_PARAM_S *pParam)
{ 
	return FALSE;
}

//************************************************************************
//�������ƣ�PreviewSwitch(const NET_SWITCH_PARAM_S *pParam)
//����˵����Ԥ�������л�
//����������
//    (in) pParam    Ԥ���л��ṹ��
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformConn::PreviewSwitch(const NET_SWITCH_PARAM_S *pParam)
{ 
	return FALSE;
}

//************************************************************************
//�������ƣ�GetDeviceList(const NET_DEVICE_PARAM_S *pParam)
//����˵������ȡ�豸�б�
//����������
//    (in) pParam �豸��Ϣ�ṹ�� 
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/
BOOL C2PlatformConn::GetDeviceList(const NET_DEVICE_PARAM_S *pParam)
{
	ASSERT(pParam);
	ASSERT(pParam->ps8AddrCode);
	ASSERT(pParam->ps8DeviceIp);
	
	if((NULL != m_hLogin) || (NULL != pParam->ps8DeviceIp))
	{
		c2_query_devinfo_s stReq = {0};	
		stReq.deviceHost = pParam->ps8DeviceIp;  //��ȡIP	
		stReq.devicePort = pParam->u16DevicePort; //��ȡ�˿�
		stReq.type = 100;	//�豸����
		stReq.addrcode = pParam->ps8AddrCode;		
		stReq.exLen = 0;
		stReq.exParam = NULL;
		
		m_pfnDeviceListCallback = pParam->fDeviceListCallback;
		m_pUserDevice = pParam->pUser;
		long lRet = c2_query_device_v1(m_hLogin, &stReq, C2DeviceListCallback, this);
		
		if(lRet == 0)
		{
			return TRUE;
		}
		
	}
	return FALSE;
}

vector<C2PlatformPreview *> C2PlatformConn::GetPreview()
{
	return m_vcPViews;
}

//************************************************************************
//�������ƣ�StartPreview(const NET_PVIEW_PARAM_S *pParam)
//����˵������ʼԤ��
//����������
//    (in) pParam Ԥ����Ϣ�ṹ�� 
//����ֵ������Ԥ������
//************************************************************************/
HANDLE C2PlatformConn::StartPreview(const NET_PVIEW_PARAM_S *pParam)
{ 
	if((NULL != pParam) && (NULL != pParam->ps8DeviceIp))
	{
		AutoLock lock(m_mtxPView);
		C2PlatformPreview* preview = NULL; 
		
		size_t i= 0;
		for(i=0; i<m_vcPViews.size(); i++)  //��ѯ�Ƿ��Ѵ��ڵ���Ƶͨ��
		{
			if(m_vcPViews[i]->IsSameStream(pParam))
			{
				// ?????
				preview = m_vcPViews[i];
				return preview;
			}
		}
		
		if(NULL == preview)
		{
			preview = new C2PlatformPreview(this);  //����Ԥ������
			if(NULL == preview)
			{
				return NULL;
			}		
		}
		
		PreviewClient * pPViewClient = preview->Start(pParam);
	 
		m_vcPViews.push_back(preview);
		
		return pPViewClient;
	}   
	return NULL; 
}

//************************************************************************
//�������ƣ�StopPreview(HANDLE *hPViewCilent)
//����˵����ֹͣԤ��
//����������
//    (in) hPView Ԥ�����
//����ֵ����
//************************************************************************/
VOID C2PlatformConn::StopPreview(HANDLE *hPViewCilent)
{ 
	if(NULL != *hPViewCilent)
	{
		AutoLock lock(m_mtxPView);
		
		PreviewClient* pViewClient = (PreviewClient*)(*hPViewCilent);
		C2PlatformPreview* preview = (C2PlatformPreview *)pViewClient->pPView;
		if(NULL == preview)
		{
			return;
		}	
		
		vector<C2PlatformPreview *>::iterator i = find(m_vcPViews.begin(), m_vcPViews.end(), preview);
		ASSERT(i != m_vcPViews.end());
		if(i == m_vcPViews.end())
		{
			return;
		}
		m_vcPViews.erase(i);
		delete preview;			
	}	
}



//************************************************************************
//�������ƣ�PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
//����˵������̨����
//����������
//    (in) u32Cmd    ��̨��������,�ο�PTZ_CMD_E����
//    (in) pParam  ��̨���Ʋ���,�ο�c2_ptz_s����
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	if((NULL == m_hLogin) || (NULL == pParam))
	{
		return FALSE;
	}
	
	c2_ptz_s ptz ={0};
	ptz.channelNo = pParam->u32ChnNO;	
	ptz.deviceHost = pParam->ps8DeviceIp;
	ptz.devicePort = pParam->u16DevicePort;
	ptz.subServHost = pParam->ps8SubServIp;
	ptz.subServPort = pParam->u16SubServPort;
	ptz.addrcode = pParam->ps8FrontAddrCode;
	
	int cmd = PTZ_CMD_NONE_C2;
	switch(u32Cmd)
	{ 	 
	case PTZ_CMD_UP:		
		{
			cmd = PTZ_CMD_UP_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWN:
		{
			cmd = PTZ_CMD_DOWN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_LEFT:
		{
			cmd = PTZ_CMD_LEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_RIGHT:
		{
			cmd = PTZ_CMD_RIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPLEFT: 
		{
			cmd = PTZ_CMD_UPLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPRIGHT:
		{
			cmd = PTZ_CMD_UPRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNLEFT:
		{
			cmd = PTZ_CMD_DOWNLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNRIGHT:
		{
			cmd = PTZ_CMD_DOWNRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMIN:
		{
			cmd = PTZ_CMD_ZOOMIN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMOUT:
		{
			cmd = PTZ_CMD_ZOOMOUT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSNEAR:
		{
			cmd = PTZ_CMD_FOCUSNEAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSFAR:
		{
			cmd = PTZ_CMD_FOCUSFAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_IRISOPEN:
		{
			cmd = PTZ_CMD_IRISOPEN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
    case PTZ_CMD_IRISCLOSE:
		{
			cmd = PTZ_CMD_IRISCLOSE_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_SETPRESET:
		{
			cmd = PTZ_CMD_SETPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CLEARPRESET:
		{
			cmd = PTZ_CMD_CLEARPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CALLPRESET:
		{
			cmd = PTZ_CMD_CALLPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;		
	case PTZ_CMD_AUTOSCAN:  
		{
			cmd = PTZ_CMD_AUTOSCAN_C2; 
			return FALSE;
		}
		break;
	case PTZ_CMD_UNKNOW:
		{
			cmd = PTZ_CMD_NONE_C2;
			return FALSE;
		}
		break;
	case PTZ_CMD_CRUISE:
		{
			return FALSE;
		}
		break;
	case PTZ_CMD_RAIN_ON:
		{
			cmd = PTZ_CMD_RAIN_ON_C2;
		}
		break;
	case PTZ_CMD_RAIN_OFF:
		{
			cmd = PTZ_CMD_RAIN_OFF_C2;
		}
		break;
	case PTZ_CMD_3D_POSITION:
		{
			return FALSE;
		}		
		break;
	}
	
	long lRet = c2_ptz_control(m_hLogin, cmd, &ptz);	
	if(0 == lRet)
	{
		return TRUE;
	}
	
	return FALSE;
}

//************************************************************************
//�������ƣ�PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
//����˵������̨����
//����������
//    (in) u32Cmd    ��̨��������,�ο�PTZ_CMD_E����
//    (in) pParam  ��̨���Ʋ���,�ο�c2_ptz_s����
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	if((NULL == m_hLogin) || (NULL == pParam))
	{
		return FALSE;
	}
	
	c2_ptz_s ptz ={0};
	ptz.channelNo = pParam->u32ChnNO;	
	ptz.deviceHost = pParam->ps8DeviceIp;
	ptz.devicePort = pParam->u16DevicePort;
	ptz.subServHost = pParam->ps8SubServIp;
	ptz.subServPort = pParam->u16SubServPort;
	ptz.addrcode = pParam->ps8FrontAddrCode;
	
	int cmd = PTZ_CMD_NONE_C2;
	switch(u32Cmd)
	{ 	 
	case PTZ_CMD_UP:		
		{
			cmd = PTZ_CMD_UP_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWN:
		{
			cmd = PTZ_CMD_DOWN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_LEFT:
		{
			cmd = PTZ_CMD_LEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_RIGHT:
		{
			cmd = PTZ_CMD_RIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPLEFT: 
		{
			cmd = PTZ_CMD_UPLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPRIGHT:
		{
			cmd = PTZ_CMD_UPRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNLEFT:
		{
			cmd = PTZ_CMD_DOWNLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNRIGHT:
		{
			cmd = PTZ_CMD_DOWNRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMIN:
		{
			cmd = PTZ_CMD_ZOOMIN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMOUT:
		{
			cmd = PTZ_CMD_ZOOMOUT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSNEAR:
		{
			cmd = PTZ_CMD_FOCUSNEAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSFAR:
		{
			cmd = PTZ_CMD_FOCUSFAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_IRISOPEN:
		{
			cmd = PTZ_CMD_IRISOPEN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
    case PTZ_CMD_IRISCLOSE:
		{
			cmd = PTZ_CMD_IRISCLOSE_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_SETPRESET:
		{
			cmd = PTZ_CMD_SETPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CLEARPRESET:
		{
			cmd = PTZ_CMD_CLEARPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CALLPRESET:
		{
			cmd = PTZ_CMD_CALLPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;		
	case PTZ_CMD_AUTOSCAN:  
		{
			cmd = PTZ_CMD_AUTOSCAN_C2; 
			return FALSE;
		}
		break;
	case PTZ_CMD_UNKNOW:
		{
			cmd = PTZ_CMD_NONE_C2;
			return FALSE;
		}
		break;
	case PTZ_CMD_CRUISE:
		{
			return FALSE;
		}
		break;
	case PTZ_CMD_RAIN_ON:
		{
			cmd = PTZ_CMD_RAIN_ON_C2;
		}
		break;
	case PTZ_CMD_RAIN_OFF:
		{
			cmd = PTZ_CMD_RAIN_OFF_C2;
		}
		break;
	case PTZ_CMD_3D_POSITION:
		{
			return FALSE;
		}		
		break;
	}
	
	long lRet = c2_ptz_control(m_hLogin, cmd, &ptz);	
	if(0 == lRet)
	{
		return TRUE;
	}
	
	return FALSE;
}

//************************************************************************
//�������ƣ�QueryRecord(const NET_QRECORD_PARAM_S *pParam)
//����˵����¼�����
//����������
//    (in) pParam    ¼����Ϣ�ṹ�� 
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::QueryRecord(const NET_QRECORD_PARAM_S *pParam)
{
	if((NULL == m_hLogin) || (NULL == pParam))
	{
		return FALSE;
	}
	
	c2_query_record_s queryRec = {0};
	queryRec.channelNo = pParam->u32ChnNO;
	switch(pParam->u32Type)
	{
	case RECORD_NORMAL: //����¼��
		{
			queryRec.type = 0;
		}
		break;
		
	case RECORD_ALARM: //����¼��
		{
			queryRec.type = 3;   //����¼��
		}
		break;
		
	case RECORD_TIME://��ʱ¼��
		{
			queryRec.type = 1;   //�ƻ�¼��
		}
		break;
		
	case RECORD_ALL://����¼��
		{
			queryRec.type = 0;   //����¼��
		}
		break;	
	default:
		{
			queryRec.type = 0;   //����¼��
		}
		break;
	}
	
	
	queryRec.start.year = pParam->stBegin.u32Year;
	queryRec.start.month = pParam->stBegin.u32Month;
	queryRec.start.day = pParam->stBegin.u32Day;
	queryRec.start.hour = pParam->stBegin.u32Hour;
	queryRec.start.minute = pParam->stBegin.u32Minute;
	queryRec.start.second = pParam->stBegin.u32Second;
	
	queryRec.end.year = pParam->stEnd.u32Year;
	queryRec.end.month = pParam->stEnd.u32Month;
	queryRec.end.day = pParam->stEnd.u32Day;
	queryRec.end.hour = pParam->stEnd.u32Hour;
	queryRec.end.minute = pParam->stEnd.u32Minute;
	queryRec.end.second = pParam->stEnd.u32Second;
	
	queryRec.fromIndex = 1;	//�������صĿ�ʼ��¼���, ��1��ʼ Ĭ��
	queryRec.toIndex = 1000;//�������صĽ�����¼���(��)   Ĭ��
	
	queryRec.deviceHost = pParam->ps8DeviceIp;
	queryRec.devicePort = pParam->u16DevicePort;
	queryRec.subServHost = pParam->ps8SubServIp;
	queryRec.subServPort = pParam->u16SubServPort;
	
	queryRec.addrcode = pParam->ps8FrontAddrCode;
	
	queryRec.exLen = 0;
	queryRec.exParam = NULL;
	
	m_pfnQueryDetailCallback = pParam->fDetailCallback;
	m_pfnQueryCalendarCallback = pParam->fCalendarCallback;
	
	m_pUserRecord = pParam->pUser;
	
	long lRet = c2_query_record_v1(m_hLogin, &queryRec, C2RecordCallback, this);
	if(0 == lRet)
	{		
		return TRUE;
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam)
//����˵����¼�����(���Ӹ澯����)
//����������
//    (in) pParam    ¼����Ϣ�ṹ�� 
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam)
{
	if((NULL == m_hLogin) || (NULL == pParam))
	{
		return FALSE;
	}
	
	c2_query_record_s queryRec = {0};
	queryRec.channelNo = pParam->pRecordReq->u32ChnNO;
	switch(pParam->pRecordReq->u32Type)
	{
	case RECORD_NORMAL: //����¼��
		{
			queryRec.type = 0;
		}
		break;
		
	case RECORD_ALARM: //����¼��
		{
			queryRec.type = 3;   //����¼��
		}
		break;
		
	case RECORD_TIME://��ʱ¼��
		{
			queryRec.type = 1;   //�ƻ�¼��
		}
		break;
		
	case RECORD_ALL://����¼��
		{
			queryRec.type = 0;   //����¼��
		}
		break;	
	default:
		{
			queryRec.type = 0;   //����¼��
		}
		break;
	}
	
	
	queryRec.start.year = pParam->pRecordReq->stBegin.u32Year;
	queryRec.start.month = pParam->pRecordReq->stBegin.u32Month;
	queryRec.start.day = pParam->pRecordReq->stBegin.u32Day;
	queryRec.start.hour = pParam->pRecordReq->stBegin.u32Hour;
	queryRec.start.minute = pParam->pRecordReq->stBegin.u32Minute;
	queryRec.start.second = pParam->pRecordReq->stBegin.u32Second;
	
	queryRec.end.year = pParam->pRecordReq->stEnd.u32Year;
	queryRec.end.month = pParam->pRecordReq->stEnd.u32Month;
	queryRec.end.day = pParam->pRecordReq->stEnd.u32Day;
	queryRec.end.hour = pParam->pRecordReq->stEnd.u32Hour;
	queryRec.end.minute = pParam->pRecordReq->stEnd.u32Minute;
	queryRec.end.second = pParam->pRecordReq->stEnd.u32Second;
	
	queryRec.fromIndex = 1;	//�������صĿ�ʼ��¼���, ��1��ʼ Ĭ��
	queryRec.toIndex = 1000;//�������صĽ�����¼���(��)   Ĭ��
	
	queryRec.deviceHost = pParam->pRecordReq->ps8DeviceIp;
	queryRec.devicePort = pParam->pRecordReq->u16DevicePort;
	queryRec.subServHost = pParam->pRecordReq->ps8SubServIp;
	queryRec.subServPort = pParam->pRecordReq->u16SubServPort;
	
	queryRec.addrcode = pParam->pRecordReq->ps8FrontAddrCode;
	
	queryRec.exLen = 0;
	queryRec.exParam = NULL;
	
	m_pfnQueryDetailCallback = pParam->pRecordReq->fDetailCallback;
	m_pfnQueryCalendarCallback = pParam->pRecordReq->fCalendarCallback;
	
	m_pUserRecord = pParam->pRecordReq->pUser;
	
	long lRet = c2_query_record_v1(m_hLogin, &queryRec, C2RecordCallback, this);
	if(0 == lRet)
	{		
		return TRUE;
	}
	return FALSE;
}


VOID C2PlatformConn::MainMsgLoop()
{

}

HANDLE C2PlatformConn::StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam)
{
	C2PlatformRecord *pRecPlay = new C2PlatformRecord(this);
	if (pRecPlay)
	{
		if (pRecPlay->Start(pParam))
		{
			return pRecPlay;
		}
		
		delete pRecPlay;
	}
    return NULL;
}


HANDLE C2PlatformConn::StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
	C2PlatformDownload *pDownload = new C2PlatformDownload(this);			
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}
	
    return NULL;
}

HANDLE C2PlatformConn::StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
	C2PlatformDownloadByTime *pDownload = new C2PlatformDownloadByTime(this);
	
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}
    return NULL; 
}

//************************************************************************
//�������ƣ�MatrixSwitch(const NET_MATRIX_PARAM_S *pParam)
//����˵���������л�
//����������
//    (in) pParam    ������Ϣ�ṹ�� 
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::MatrixSwitch(const NET_MATRIX_PARAM_S *pParam)
{
	ASSERT(pParam);
	ASSERT(pParam->ps8DeviceIp);
	
	if(NULL != m_hLogin)
	{
		c2_matrix_s matrixInfo = {0};
		matrixInfo.inPort = pParam->u32InPort;
		matrixInfo.outPort = pParam->u32OutPort;		
		matrixInfo.deviceHost = pParam->ps8DeviceIp;
		matrixInfo.devicePort = pParam->u16DevicePort;
		matrixInfo.subServHost = pParam->ps8SubServIp;
		matrixInfo.subServPort = pParam->u16SubServPort;
		
		int iRet = c2_matrix_switch(m_hLogin, &matrixInfo);	
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	
	return FALSE;	
}

//************************************************************************
//�������ƣ�DecoderSwitch(const NET_DECODER_PARAM_S *pParam)
//����˵������������ʾ�л�
//����������
//    (in) pParam    ������Ϣ�ṹ��
//����ֵ���ɹ�����true��ʧ�ܷ���false
//************************************************************************/  
BOOL C2PlatformConn::DecoderSwitch(const NET_DECODER_PARAM_S *pParam)
{
	ASSERT(pParam);
	ASSERT(pParam->ps8DeviceIp);
	
	if(NULL != m_hLogin)
	{
		c2_decoder_param_s decoderInfo = {0}; 
		decoderInfo.mode = pParam->u32Mode; //��ʾģʽ��NET_SHOW_MODE_E, 1, 4, 9 ,16
		decoderInfo.channelNo = pParam->u32Channel; //��Ҫ������ʾ��ͨ����һλ��Ӧһ��ͨ��
		// decoderInfo.outPort = pParam->u32OutPort;	//��ʾ����˿ڣ���1��ʼ //aaaaaaaaaaaaaa
		
		decoderInfo.deviceHost = pParam->ps8DeviceIp;
		decoderInfo.devicePort = pParam->u16DevicePort;
		decoderInfo.subServHost = pParam->ps8SubServIp;
		decoderInfo.subServPort = pParam->u16SubServPort;
		
		int iRet = c2_decoder_switch(m_hLogin, &decoderInfo);	
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

//************************************************************************
//�������ƣ�DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam)
//����˵������ȡ��������ʾͨ��
//����������
//    (in) pParam    ��������ʾ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�DecoderStartDynamic(U8 *pu8DeviceIp, U16 u16DevicePort, U8 *pu8SubServIp, U16 u16SubServPort, S32 s32DecChnNo, S32 s32DisOutPort, S32 s32DisWndNo, const NET_DEC_CHN_PARAM_S *pParam)
//����˵������������ʼ��̬����
//����������
//    (in) pu8DeviceIp		�豸IP��ַ
//    (in) u16DevicePort	�豸�˿ں�
//    (in) pu8SubServIp		������ý���������ַ
//    (in) u16SubServPort	������ý��˿ں�
//    (in) s32DecChnNo		�豸ͨ����
//    (in) s32DisOutPort	����˿ں�
//    (in) s32DisWndNo		��ʾ���ں�
//    (in) pParam		����ͨ������
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::DecoderStartDynamic(U8 *pu8DeviceIp, U16 u16DevicePort, U8 *pu8SubServIp, U16 u16SubServPort, S32 s32DecChnNo, S32 s32DisOutPort, S32 s32DisWndNo, const NET_DEC_CHN_PARAM_S *pParam)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�DecoderStopDynamic(U8 *pu8DeviceIp, U16 u16DevicePort, U8 *pu8SubServIp, U16 u16SubServPort, S32 s32DisOutPort)
//����˵����������ֹͣ��̬����
//����������
//    (in) pu8DeviceIp		�豸IP��ַ
//    (in) u16DevicePort	�豸�˿ں�
//    (in) pu8SubServIp		������ý���������ַ
//    (in) u16SubServPort	������ý��˿ں�
//    (in) s32DisOutPort	����˿ں�
//    (in) pParam		����ͨ������
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::DecoderStopDynamic(U8 *pu8DeviceIp, U16 u16DevicePort, U8 *pu8SubServIp, U16 u16SubServPort, S32 s32DisOutPort)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser)
//����˵����������������ص�
//����������
//    (in) fWFLinkageCallback	����ص�����
//    (in) pUser	�û�����
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser)
{
	m_pfnWFLinkageCallback = fWFLinkageCallback;
	m_pUserWF = (void*)pUser;
	return FALSE;
}

//************************************************************************
//�������ƣ�GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser)
//����˵������ȡ���߿ͻ���Ϣ
//����������
//    (in) fGetClientCallback	��ȡ���߿ͻ��ص�����
//    (in) pUser	�û�����
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SynchrTime(const NET_SYNTIME_PARAM_S *pParam)
//����˵����ͬ��ʱ��
//����������
//    (in) pParam	ͬ��ʱ�����
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SynchrTime(const NET_SYNTIME_PARAM_S *pParam)
{
// 	if((NULL != m_hLogin) && (NULL != pParam))
// 	{
// 		c2_synct_s synct = {0};
// 		synct.time.year = pParam->stTime.u32Year;
// 		synct.time.month = pParam->stTime.u32Month;
// 		synct.time.day = pParam->stTime.u32Day;
// 		synct.time.hour = pParam->stTime.u32Hour;
// 		synct.time.minute = pParam->stTime.u32Minute;
// 		synct.time.second = pParam->stTime.u32Second;
// 		
// 		if(1 == pParam->u8IsSetLoaclTime)
// 		{	
// 			synct.setLocal = true;
// 		}
// 		else
// 		{
// 			synct.setLocal = false;
// 		}
// 
// 		string zone = "Asia/Shanghai";
// 		synct.zone = const_cast<char*>(zone.c_str());	
// 		 
// 		synct.setLocal = (pParam->u8IsSetLoaclTime == 0) ? false : true;
// 		
// 		long lRet = c2_sync_time(m_hLogin, &synct);
// 		if(0 == lRet)
// 		{
// 			return TRUE;
// 		}
// 	}		
//	return FALSE;

	return TRUE; //Ŀǰ������Уʱ����Уʱ��ָ����ϴ��룬ע�ͱ��д��롣
}

//************************************************************************
//�������ƣ�QueryTime(DATETIME_S *pDateTime)
//����˵������ѯʱ��
//����������
//    (out) pDateTime	����ʱ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::QueryTime(DATETIME_S *pDateTime)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�GetDeviceState(U8 u8Ip[], U16 u16Port)
//����˵������ȡ�豸״̬
//����������
//    (in) u8Ip	IP��ַ
//    (in) u16Port	�˿ں�
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetDeviceState(U8 u8Ip[], U16 u16Port)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser)
//����˵������Դ��ȡ
//����������
//    (in) fResourceCallback ��Դ�ص�����
//    (in) pUser	�û�����
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser)
{
	return FALSE;
}

HANDLE C2PlatformConn::InitDualtalk(const NET_DTALK_PARAM_S *pParam)
{
	
	return NULL;
}

VOID C2PlatformConn::UnInitDualtalk(HANDLE hDTalk)
{
	if(NULL != hDTalk)
	{
		
	}
}

//************************************************************************
//�������ƣ�LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans)
//����˵��������ת��
//����������
//    (in) pLinkTrans ����ת����Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�GetDeviceData(U32 *u32DevNum)
//����˵������ȡ�豸������Ϣ
//����������
//    (in) u32DevNum �豸����
//����ֵ������CHN_INFO_S��Ϣ����
//************************************************************************/
CHN_INFO_S * C2PlatformConn::GetDeviceData(U32 *u32DevNum)
{
	c2_servinfo_s info;
	long lret = c2_get_servinfo(m_hLogin, &info);
	if (lret < 0)
	{
		return NULL;
	}
	
	char s8AddrCode[MAX_ADDR_LEN] = {0};
    strcpy(s8AddrCode, info.msvcode);
	*u32DevNum = info.deviceNum;
	
	
	if((NULL != m_hLogin)  )
	{
		c2_query_devinfo_s stReq = {0};	
		stReq.deviceHost = "";  //��ȡIP	
		stReq.devicePort = 0; //��ȡ�˿�
		stReq.type = 100;	//�豸����
		stReq.addrcode = s8AddrCode;		
		stReq.exLen = 0;
		stReq.exParam = NULL;
		
		m_vcDeviceInfo.clear();	 
		long lRet = c2_query_device_v1(m_hLogin, &stReq, C2GetDeviceDataCallback, this);
		
		if(lRet == 0)
		{
			if (NULL != m_devInfo)
			{
				delete [] m_devInfo;
				m_devInfo = NULL;
			}
			
			if (NULL == m_devInfo)
			{
				m_devInfo = new CHN_INFO_S[info.deviceNum];
			}
			
			if (NULL != m_devInfo)
			{
				for(int j=0; j<m_vcDeviceInfo.size(); j++)  //��ѯ�Ƿ��Ѵ��ڵ���Ƶͨ��
				{
					c2_devinfo_s devInfo = m_vcDeviceInfo[j]; 
					strcpy(m_devInfo[j].s8AddrCode, devInfo.addrcode); 
					strncpy(m_devInfo[j].s8DevName, devInfo.name, sizeof(m_devInfo[j].s8DevName));
						
					INET_PTON(devInfo.host, m_devInfo[j].u8IPAddr);
					
					m_devInfo[j].u16Port = devInfo.port;
					m_devInfo[j].u16DevType = devInfo.devtype;
					m_devInfo[j].u8ChannelNum = devInfo.channelNum;
					
					m_devInfo[j].u8ManuType = devInfo.manutype;
					m_devInfo[j].u8AlarmInNum = 0;
					m_devInfo[j].u8AlarmOutNum = 0;
					m_devInfo[j].u8IsOnline = devInfo.status;
				}
			}
		} 
	}
	return m_devInfo;
}

//************************************************************************
//�������ƣ�RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
//����˵������������Ʋ���
//����������
//    (in) u32Cmd ��̨��������
//    (in) pParam ���Ʋ���
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	if((NULL == m_hLogin) || (NULL == pParam))
	{
		return FALSE;
	}
	
	c2_ptz_s ptz ={0};
	ptz.channelNo = pParam->u32ChnNO;	
	ptz.deviceHost = pParam->ps8DeviceIp;
	ptz.devicePort = pParam->u16DevicePort;
	ptz.subServHost = pParam->ps8SubServIp;
	ptz.subServPort = pParam->u16SubServPort;
	ptz.addrcode = pParam->ps8FrontAddrCode;
	
	int cmd = PTZ_CMD_NONE_C2;
	switch(u32Cmd)
	{ 	 
	case PTZ_CMD_UP:		
		{
			cmd = PTZ_CMD_UP_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWN:
		{
			cmd = PTZ_CMD_DOWN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_LEFT:
		{
			cmd = PTZ_CMD_LEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_RIGHT:
		{
			cmd = PTZ_CMD_RIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPLEFT: 
		{
			cmd = PTZ_CMD_UPLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_UPRIGHT:
		{
			cmd = PTZ_CMD_UPRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNLEFT:
		{
			cmd = PTZ_CMD_DOWNLEFT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_DOWNRIGHT:
		{
			cmd = PTZ_CMD_DOWNRIGHT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMIN:
		{
			cmd = PTZ_CMD_ZOOMIN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_ZOOMOUT:
		{
			cmd = PTZ_CMD_ZOOMOUT_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSNEAR:
		{
			cmd = PTZ_CMD_FOCUSNEAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_FOCUSFAR:
		{
			cmd = PTZ_CMD_FOCUSFAR_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_IRISOPEN:
		{
			cmd = PTZ_CMD_IRISOPEN_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
    case PTZ_CMD_IRISCLOSE:
		{
			cmd = PTZ_CMD_IRISCLOSE_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}	
		break;
	case PTZ_CMD_SETPRESET:
		{
			cmd = PTZ_CMD_SETPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CLEARPRESET:
		{
			cmd = PTZ_CMD_CLEARPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;
	case PTZ_CMD_CALLPRESET:
		{
			cmd = PTZ_CMD_CALLPRESET_C2;
			ptz.speed = 5;
			ptz.preset = pParam->stPreset.u32Index;
		}
		break;		
	case PTZ_CMD_AUTOSCAN:  
		{
			cmd = PTZ_CMD_AUTOSCAN_C2; 
			return FALSE;
		}
		break;
	case PTZ_CMD_UNKNOW:
		{
			cmd = PTZ_CMD_NONE_C2;
			return FALSE;
		}
		break;
	case PTZ_CMD_CRUISE:
		{
			return FALSE;
		}
		break;
	case PTZ_CMD_RAIN_ON:
		{
			cmd = PTZ_CMD_RAIN_ON_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}
		break;
	case PTZ_CMD_RAIN_OFF:
		{
			cmd = PTZ_CMD_RAIN_OFF_C2;
			ptz.speed = pParam->stNormal.u32Speed;
			ptz.preset = 0;
			ptz.stop = (pParam->stNormal.u32Flag == 0) ? false : true;
		}
		break;
	case PTZ_CMD_3D_POSITION:
		{
			return FALSE;
		}		
		break;
	}
	
	long lRet = c2_ptz_control(m_hLogin, cmd, &ptz);	
	if(0 == lRet)
	{
		return TRUE;
	}
	
	return FALSE;
}

//************************************************************************
//�������ƣ�ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart)
//����˵����Զ��������ֹͣDVR��ĳ��ͨ��¼��
//����������
//    (in) pParam ͨ������
//    (in) bStart ������־ 1-����¼��0ֹͣ¼��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SetParameter(const NET_PARAMSET_PARAM_S *pParam)
//����˵������������(������ģʽ)
//����������
//    (in) pParam ���ò�����Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetParameter(const NET_PARAMSET_PARAM_S *pParam)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SetParameter(const NET_PARAMSET_PARAM_S *pParam)
//����˵������������(jso
//����������
//    (in) pParam json��ʽ�ַ���
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetParameterEx(const S8 *pParam)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SetDTalkState(U32 u32DTalkState)
//����˵�������ÿͻ�������Ƶ�Խ�״̬
//����������
//    (in) u32DTalkState ����Ƶ�Խ�״̬
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetDTalkState(U32 u32DTalkState)
{	
	return FALSE;
}

//************************************************************************
//�������ƣ�GetChnPreset(const NET_GETPRESET_PARAM_S *pParam)
//����˵������ȡͨ��Ԥ��λ��Ϣ
//����������
//    (in) pParam Ԥ��λ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetChnPreset(const NET_GETPRESET_PARAM_S *pParam)
{ 
	if((NULL != m_hLogin) && (NULL != pParam))
	{
		c2_query_preset_s stReq = {0};
		
		stReq.addrcode = pParam->ps8FrontAddrCode;
		stReq.deviceHost = pParam->ps8DeviceIp;
		stReq.devicePort = pParam->u16DevicePort;
		stReq.subServHost = pParam->ps8SubServIp;
		stReq.subServPort = pParam->u16SubServPort;
		
		stReq.channelNo = pParam->u32ChnNo;
		
		m_pUserPreset = pParam->pUser;
		m_pfnPresetListCallback = pParam->fnPresetListCallback;
		
		long lRet = c2_get_chn_preset_v1(m_hLogin, &stReq, &C2PresetCallback, this);
		if(0 == lRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�SetChnPreset(const NET_SETPRESET_PARAM_S *pParam)
//����˵���������豸ͨ��Ԥ��λ��Ϣ
//����������
//    (in) pParam Ԥ��λ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetChnPreset(const NET_SETPRESET_PARAM_S *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{
		c2_set_preset_s stReq = {0};
		
		stReq.addrcode = pParam->ps8FrontAddrCode;
		stReq.deviceHost = pParam->ps8DeviceIp;
		stReq.devicePort = pParam->u16DevicePort;
		stReq.subServHost = pParam->ps8SubServIp;
		stReq.subServPort = pParam->u16SubServPort;
		
		stReq.channelNo = pParam->u32ChnNo;
		stReq.presetNum = pParam->u32PresetNum;
		
		stReq.presetList = new c2_preset_s[pParam->u32PresetNum];
		for(int index = 0; index < pParam->u32PresetNum; index++)
		{
			stReq.presetList[index].index = pParam->pstPresetList[index].u32PresetNo;
			stReq.presetList[index].name = pParam->pstPresetList[index].ps8PresetName;
			stReq.presetList[index].type = 0;
			stReq.presetList[index].isIVS = 0;	
		}
		
		long lRet = c2_set_chn_preset(m_hLogin, &stReq);
		if(0 == lRet)
		{
			delete []stReq.presetList;
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam)
//����˵������ȡͨ��Ԥ��λ��Ϣ���¹�Լ��
//����������
//    (in) pParam Ԥ��λ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{
		string ip="", subIp="", addrcode="";
		c2_query_preset_s stReq = {0};
		
		if(NULL != pParam->ps8DeviceIp)//�豸IP
		{
			ip = pParam->ps8DeviceIp;
			stReq.deviceHost = const_cast<char*>(ip.c_str());	
		}
		else 
		{
			stReq.deviceHost = NULL;
		}		
		
		if(NULL != pParam->ps8SubServIp)
		{
			subIp = pParam->ps8SubServIp;
			stReq.subServHost = NULL;	
		}
		else 
		{
			stReq.subServHost = NULL;
		}		
		
		if(NULL != pParam->ps8FrontAddrCode)
		{
			addrcode = pParam->ps8FrontAddrCode;
			stReq.addrcode = const_cast<char*>(addrcode.c_str());	
		}
		else 
		{
			stReq.addrcode = NULL;
		}		
		
		stReq.devicePort = pParam->u16DevicePort; 
		stReq.subServPort = pParam->u16SubServPort;
		
		stReq.channelNo = pParam->u32ChnNo;
		stReq.exLen = 0;
		stReq.exParam = NULL;
		
		m_pfnPresetListCallbackEx = pParam->fnPresetListCallback;
		m_pUserPreset = pParam->pUser;
		
		long lRet = c2_get_chn_preset_v1(m_hLogin, &stReq, C2PresetCallbackEx, this);
		if(0 == lRet)
		{			 
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam)
//����˵���������豸ͨ��Ԥ��λ��Ϣ(�¹�Լ)
//����������
//    (in) pParam Ԥ��λ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{
		c2_set_preset_s stReq = {0};
		string ip="", subIp="", addrcode="";
		if(NULL != pParam->ps8DeviceIp)//�豸IP
		{
			ip = pParam->ps8DeviceIp;
			stReq.deviceHost = const_cast<char*>(ip.c_str());	
		}
		else 
		{
			stReq.deviceHost = NULL;
		}		
		
		if(NULL != pParam->ps8SubServIp)
		{
			subIp = pParam->ps8SubServIp;
			stReq.subServHost = const_cast<char*>(subIp.c_str());	
		}
		else 
		{
			stReq.subServHost = NULL;
		}		
		
		if(NULL != pParam->ps8FrontAddrCode)
		{
			addrcode = pParam->ps8FrontAddrCode;
			stReq.addrcode = const_cast<char*>(addrcode.c_str());	
		}
		else 
		{
			stReq.addrcode = NULL;
		}		
		
		stReq.devicePort = pParam->u16DevicePort; 
		stReq.subServPort = pParam->u16SubServPort;
		
		stReq.channelNo = pParam->u32ChnNo;
		stReq.presetNum = pParam->u32PresetNum;
		
		stReq.presetList = new c2_preset_s[pParam->u32PresetNum];
		for(int index = 0; index < pParam->u32PresetNum; index++)
		{
			stReq.presetList[index].index = pParam->pstPresetList[index].u32PresetNo;
			stReq.presetList[index].name = pParam->pstPresetList[index].ps8PresetName;
			stReq.presetList[index].type = pParam->pstPresetList[index].u32PresetType;
			stReq.presetList[index].isIVS = 0;	
		}
		
		long lRet = c2_set_chn_preset(m_hLogin, &stReq);
		if(0 == lRet)
		{
			delete []stReq.presetList;
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�DrawPreset(const NET_PRESET_DRAW *pParam)
//����˵���������ַ�����Ƶ������
//����������
//    (in) pParam �ַ���Ϣ�ṹ��
//����ֵ��void
//************************************************************************/
VOID C2PlatformConn::DrawPreset(const NET_PRESET_DRAW *pParam)
{
	
}

//************************************************************************
//�������ƣ�SetRegDraw(const NET_REGDRAW_PARAM *pParam)
//����˵����ע���ͼ����
//����������
//    (in) pParam  ע���ͼ��Ϣ�ṹ��
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetRegDraw(const NET_REGDRAW_PARAM *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{	
		PreviewClient* pViewClient = (PreviewClient*)(*(pParam->hPView));
		C2PlatformPreview* preview = (C2PlatformPreview *)pViewClient->pPView;
		
		long lRet = preview->SetDrawCallBack(preview->m_hPreview, pParam);				
		if(0 == lRet)
		{
			return TRUE;
		}		
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam)
//����˵�������ý������ݻص�����
//����������
//    (in) pParam  ������Ϣ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam)
{
	C2PlatformPreview *pView = (C2PlatformPreview*)pParam->hPView;
	if(NULL != pView)
	{
		 pView->SetDecDataCallBack(pParam->pfnDecCb, pParam->hUser);
	
	}
	
	return FALSE;
}

//************************************************************************
//�������ƣ�StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
//����˵�����������ܷ���(����ʱps8IvaIpΪ"0.0.0.0")
//����������
//    (in) pParam  ���ܷ�������
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{	
		string ivaIP = pParam->ps8IvaIp;
		int ivaPort = pParam->u16IvaPort;
		string devIP = pParam->ps8DevIp;
		int devPort = pParam->u16DevPort;
		int chn = pParam->u8DevChn;
		int preset = pParam->u8DevPreset;
		
		SHL_Json::Value jsonReq;
		jsonReq["DevIp"] = ivaIP;
		jsonReq["DevPort"] = ivaPort;		
		jsonReq["chnIp"] = devIP; 
		jsonReq["chnNo"] = chn;
		jsonReq["presetNo"] = preset; 		
		jsonReq["Method"]  = "startIva"; 
		
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		const char* cReq= strReq.c_str();
		char* cStrReq = const_cast<char*>(cReq);
		int len = strReq.size();
		int iRet = c2_send_command(m_hLogin, cStrReq, len); //aaaaaaaaaaa
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
//����˵����ֹͣ���ܷ���(����ʱps8IvaIpΪ"0.0.0.0")
//����������
//    (in) pParam  ���ܷ�������
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{	
		string ivaIP = pParam->ps8IvaIp;
		int ivaPort = pParam->u16IvaPort;
		string devIP = pParam->ps8DevIp;
		int devPort = pParam->u16DevPort;
		int chn = pParam->u8DevChn;
		int preset = pParam->u8DevPreset;
		
		SHL_Json::Value jsonReq;
		jsonReq["DevIp"] = ivaIP;
		jsonReq["DevPort"] = ivaPort;		
		jsonReq["chnIp"] = devIP; 
		jsonReq["chnNo"] = chn;
		jsonReq["presetNo"] = preset; 		
		jsonReq["Method"]  = "stopIva"; 
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		const char* cReq= strReq.c_str();
		char* cStrReq = const_cast<char*>(cReq);
		int len = strReq.size();
		int iRet = c2_send_command(m_hLogin, cStrReq, len); //aaaaaaaaaaa
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}


//************************************************************************
//�������ƣ�SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam)
//����˵�������ú������Ԥ��λ�¹������
//����������
//    (in) pParam  ��������б�
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{ 
		int ruleListNum =pParam->u32RuleListNum;
		
		SHL_Json::Value jsonReq; 
		jsonReq["Version"]  = "V1.00.00";
		jsonReq["Method"]  = "ThermometryRuleCfg";
		
		SHL_Json::Value listNode;
		
		for(int i=0; i<ruleListNum; i++)
		{
			NET_THERM_RULE_INFO ruleInfo = pParam->pThermRuleList[i];
			string devIp = ruleInfo.ps8DevIp;
			int port = ruleInfo.u16DevPort;
			int chn = ruleInfo.u8ChnNo;
			int preset = ruleInfo.u8PresetNo;
			int enable = ruleInfo.u8Enable;
			string ruleName = ruleInfo.ps8RuleName;
			int id = ruleInfo.u8RuleId;
			float emissivity = ruleInfo.fEmissivity;
			float distance = ruleInfo.fDistance;
			float reflexvalue = ruleInfo.fReflexValue;
			float fromRatioX = ruleInfo.stFrom.fxRatio;			
			float fromRatioY = ruleInfo.stFrom.fyRatio;			
			float toRatioX = ruleInfo.stTo.fxRatio;
			float toRatioY = ruleInfo.stTo.fyRatio;
			
			
			listNode["DevIp"] = devIp;
			listNode["DevPort"] = port;
			listNode["Chn"]= chn;
			listNode["Preset"] = preset;
			listNode["RuleName"] = ruleName;
			listNode["Enable"] = enable;
			listNode["Id"] = id;
			listNode["Emissivity"] = emissivity;
			listNode["Distance"] = distance;
			listNode["Reflexvalue"] = reflexvalue;
			listNode["FromRatioX"] = fromRatioX;
			listNode["FromRatioY"] = fromRatioY;
			listNode["ToRatioX"] = toRatioX;
			listNode["ToRatioY"] = toRatioY;
			
			jsonReq["Item"].append(listNode);
		}
		
		
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		const char* cReq= strReq.c_str();
		char* cStrReq = const_cast<char*>(cReq);
		int len = strReq.size();
		int iRet = c2_send_command(m_hLogin, cStrReq, len); //aaaaaaaaaaa
		if(0 == iRet)
		{
			return TRUE;
		} 
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam)
//����˵�������ú����������ʾ
//����������
//    (in) pParam  ��ʾģʽ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam)
{
	if((NULL != m_hLogin) && (NULL != pParam))
	{ 
		string ip =pParam->ps8DevIp;
		int port = pParam->u16DevPort;
		int chn = pParam->u8ChnNo;
		int mode = pParam->mode;
		
		SHL_Json::Value jsonReq;
		jsonReq["Ip"] = ip;
		jsonReq["Port"] = port;
		jsonReq["Chn"] = chn;
		jsonReq["Mode"] = mode; 		 	
		jsonReq["Method"]  = "ThermDisplayMode";
		jsonReq["Version"]  = "V1.00.00"; 	
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		const char* cReq= strReq.c_str();
		char* cStrReq = const_cast<char*>(cReq);
		int len = strReq.size();
		int iRet = c2_send_command(m_hLogin, cStrReq, len); //aaaaaaaaaaa
		if(0 == iRet)
		{
			return TRUE;
		} 
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�GetPresetInfo(const S8 *ps8PresetReq, U32 u32Size, S8 **ps8PresetInfo)
//����˵������ȡ��ý����Ԥ��λ��Ϣ
//����������
//    (in) ps8PresetReq  ������Ϣ(ȫ�����ߵ���)
//    (in) u32Size  ������Ϣ�ֽ���
//    (in) ps8PresetInfo  ����Ԥ��λ��Ϣjson��ʽ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetPresetInfo(const S8 *ps8PresetReq, U32 u32Size, S8 **ps8PresetInfo)
{
	if((NULL != m_hLogin) && (NULL != ps8PresetReq))
	{
		m_presetList = "";

		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if(!reader.parse(ps8PresetReq, rootNode))
		{
			return FALSE;
		}
		
		string strVersion = (rootNode["Version"].asString()); //V1.00.00
		string strmethod = (rootNode["Method"].asString()); //Get_AllPreset
		
		if(0 == strcmp(strmethod.c_str(), "Get_AllPreset"))
		{
			//��ȡ��ý��������Ԥ��λ
		    
		}
		else if(0 == strcmp(strmethod.c_str(), "Get_OnePreset"))
		{
            string strDevIp = rootNode["Ip"].asString();
			int    iDevPort = rootNode["Port"].asInt();
			int    iDevChn  = rootNode["Chn"].asInt();

            c2_query_preset_s query;
			memset(&query, 0, sizeof(c2_query_preset_s));

			query.deviceHost = (char*)strDevIp.c_str();
			query.devicePort = iDevPort;
			query.channelNo = iDevChn;

			char jsonData[100*1024] = {0};
			int sz = 100*1024;
			long ret = c2_get_chn_preset_v2(m_hLogin, &query, jsonData, &sz);
			
			SHL_Json::Value presetJson;
			SHL_Json::Reader presetReader;
			if(!presetReader.parse(jsonData, presetJson))
			{
				return FALSE;
			}

            SHL_Json::Value presetList = presetJson["presetList"];
			int presetsz = presetList.size();

			SHL_Json::FastWriter writeJson;
            SHL_Json::Value itemList;
			for (int index = 0; index < presetsz; ++index)
			{
               SHL_Json::Value ele = presetList[index];

               SHL_Json::Value item;
			   item["Ip"]   = strDevIp;
			   item["Port"] = iDevPort;
			   item["Chn"] = iDevChn;
			   item["Name"] = utf2asc(ele["name"].asString().c_str());
               item["Preset"] = ele["idx"];
			   item["IAEnable"] = 1;

			   itemList["Item"].append(item);
			}

           m_presetList =  writeJson.write(itemList);

		   *ps8PresetInfo = (char*)m_presetList.c_str();

		}
	}
	return TRUE;
}

//************************************************************************
//�������ƣ�SendHSResource(const S8 *ps8Resource, S32 s32Size)
//����˵�����·���ʢ��(AXF|EST|DV|LINE)
//����������
//    (in) ps8Resource  һ��ͨ��Դ��Ϣjson��ʽ
//    (in) s32Size  ������Ϣ�ֽ��� 
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SendHSResource(const S8 *ps8Resource, S32 s32Size)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SendPresetInfo(const S8 *ps8PresetInfo, S32 s32Size)
//����˵�������û�ɾ����ý����Ԥ��λ��Ϣ
//����������
//    (in) ps8PresetInfo  ����Ԥ��λ��Ϣjson��ʽ
//    (in) s32Size  �����ֽڴ�С
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SendPresetInfo(const S8 *ps8PresetInfo, S32 s32Size)
{
	if((NULL != m_hLogin) && (NULL != ps8PresetInfo))
	{		
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if(!reader.parse(ps8PresetInfo, rootNode))
		{
			return FALSE;
		}
		
		string strVersion = (rootNode["Version"].asString()); //V1.00.00
		string strmethod = (rootNode["Method"].asString()); //Update_PresetInfo
		
		SHL_Json::Value list  = rootNode["Item"];
		S32 s32RealNum    = list.size();
		
		S8 s8Info[128] = {0};
		SHL_Json::Value element;
		
		if(0 == strcmp(strmethod.c_str(), "Update_PresetInfo"))
		{
			for (S32 s32I = 0; s32I < s32RealNum; ++s32I)
			{
				element          = list[s32I];
				U16    u16Chn    = element["Chn"].asInt();			
				U8     u8IaPoint = element["IAPoint"].asInt();
				string strIp     = element["Ip"].asCString();  //aaaaaaaaa
				char cIP[MAX_ADDR_LEN] = {0};
				strcpy(cIP, strIp.c_str());
				string strName   = element["Name"].asCString();
				char cName[MAX_NAME_LEN_C2] = {0};
				strcpy(cName, strName.c_str());
				U16    u16Port   = element["Port"].asInt();	
				U16    u16Preset = element["Preset"].asInt();
				
				c2_set_preset_s stReq = {0};
				stReq.addrcode = NULL;
				stReq.deviceHost = cIP;
				stReq.devicePort = u16Port;
				stReq.subServHost = NULL;
				stReq.subServPort = 0;
				stReq.channelNo = u16Chn; 
				stReq.presetNum = 1;
				
				stReq.presetList = new c2_preset_s[1];  //aaaaaaaaaa
				for(int index = 0; index < 1; index++)
				{
					stReq.presetList[index].index = u16Preset;
					stReq.presetList[index].name = cName;
					stReq.presetList[index].type = 0;
					stReq.presetList[index].isIVS = 0;	
				}
				
				long lRet = c2_set_chn_preset(m_hLogin, &stReq);
				
				delete []stReq.presetList;
			}
			return TRUE;
		}	
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�SendCommand(const S8 *ps8info, S32 s32Size)
//����˵�����·������ý��(json��ʽ)
//����������
//    (in) ps8info  ��������
//    (in) s32Size  �����ֽڴ�С
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::SendCommand(const S8 *ps8info, S32 s32Size)
{
	//����json (3d,call answer)
   	if((NULL != m_hLogin) && (NULL != ps8info))
	{	
		int iRet = c2_send_command(m_hLogin, (char*)ps8info, s32Size); //aaaaaaaaaaa
		if(0 == iRet)
		{
			return TRUE;
		} 	
// 		SHL_Json::Value rootNode;
// 		SHL_Json::Reader reader;
// 		if(!reader.parse(ps8info, rootNode))
// 		{
// 			return FALSE;
// 		}		
// 		string strVersion = (rootNode["Version"].asString()); //V1.00.00
// 		string strMethod = (rootNode["Method"].asString()); 		
// 		if(strMethod == "3D_Control")
// 		{
// 			string strIp = rootNode["DevIp"].asString();
// 			int port = rootNode["DevPort"].asInt();
// 			int chn = rootNode["DevChn"].asInt();  
// 			long xTop = rootNode["XTop"].asInt(); 
// 			long yTop = rootNode["YTop"].asInt(); 
// 			long xBottom = rootNode["XBottom"].asInt(); 
// 			long yBottom = rootNode["YBottom"].asInt(); 
// 			long wndWidth = rootNode["WndWidth"].asInt(); 
// 			long wndHeight = rootNode["WndHeight"].asInt(); 
// 			
// 			SHL_Json::Value jsonReq;
// 			
// 			jsonReq["DevIp"] = strIp;
// 			jsonReq["DevPort"] = port;
// 			jsonReq["chnNo"] = chn; 
// 			jsonReq["X1"] = xTop; 
// 			jsonReq["Y1"] = yTop; 
// 			jsonReq["X2"] = xBottom; 
// 			jsonReq["Y2"] = yBottom; 		
// 			jsonReq["Method"]  = "ptzPostion3D";
// 			jsonReq["version"]  = "1.0"; 	
// 			SHL_Json::FastWriter req;
// 			string strReq = req.write(jsonReq);
// 			
// 			int iRet = c2_send_command(m_hLogin, (char*)strReq.c_str(), strReq.size()); 
// 			if(0 == iRet)
// 			{
// 				return TRUE;
// 			} 	
// 		}
// 		else if(strMethod == "OnCallAnswer")
// 		{
// 			string strIp = rootNode["DevIp"].asString();
// 			int port = rootNode["DevPort"].asInt();
// 			int command = rootNode["Command"].asInt();  
// 			
// 			SHL_Json::Value jsonReq;
// 			jsonReq["DevIp"] = strIp;
// 			jsonReq["DevPort"] = port;
// 			jsonReq["Command"] = command; 
// 			jsonReq["Method"]  = "OnCallAnswer";
// 			jsonReq["Version"]  = "V1.00.00"; 	
// 			SHL_Json::FastWriter req;
// 			string strReq = req.write(jsonReq);
// 			
// 			const char* cReq= strReq.c_str();
// 			char* cStrReq = const_cast<char*>(cReq);
// 			int len = strReq.size();
// 			int iRet = c2_send_command(m_hLogin, cStrReq, len); //aaaaaaaaaaa
// 			if(0 == iRet)
// 			{
// 				return TRUE;
// 			} 		 
// 		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�GetRemoteIvaAlarmData(const S8 *ps8Req, U32 u32Size, S8 **ps8AlarmInfo)
//����˵������ȡ���ܷ������ٸ澯
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//    (out) ps8AlarmInfo  ����ֵ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetRemoteIvaAlarmData(const S8 *ps8Req, U32 u32Size, S8 **ps8AlarmInfo)
{  
	char jout[MAX_STRING_LEN] = {0};	
	memset(out, 0, sizeof(out));
	
	if((NULL != m_hLogin) && (NULL != ps8Req))
	{		
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if(!reader.parse(ps8Req, rootNode))
		{
			return FALSE;
		}
		
		string strVersion = (rootNode["Version"].asString()); //V1.00.00
		string strMethod = (rootNode["Method"].asString()); //Update_PresetInfo
		string strIp = rootNode["IvaIp"].asString();
		int port = rootNode["IvaPort"].asInt();
		
		if(0 == strcmp(strMethod.c_str(), "Call_IvaAlarmData"))
		{						
			SHL_Json::Value jsonReq;
			jsonReq["DevIp"] = strIp;
			jsonReq["DevPort"] = port; 
			jsonReq["Method"]  = "callAll"; 
			SHL_Json::FastWriter req;
			string strReq = req.write(jsonReq);
			
			const char* cReq= strReq.c_str();
			char* cStrReq = const_cast<char*>(cReq);
			int len = strReq.size();
			
			int sz = MAX_STRING_LEN;  
			int iRet = c2_get_config(m_hLogin, cStrReq, len, jout, &sz); //aaaaaaaa					  
			if(0 == iRet)
			{				
				//{"FromIndex":1,"IvaServer":"192.168.80.206","Method":504,
				//"RealNum":0,"RtnCode":0,"StateMsg":"success!","SubNum":0,
				//"TaskList":null,"ToIndex":1000,"Version":"1.8.4","version":null}
				
				
				//	{"Item":[{"channel":0,"devip":"192.168.60.223","devport":3000,"ivaresult":"1,0,1,0,0,0,","lmtip":"192.168.60.154","lmtport":9000,"presetno":1,"time":"2017-10-23 08:06:15"},
				//         {"channel":0,"devip":"192.168.60.250","devport":8000,"ivaresult":"1,0,0,0,0,0,","lmtip":"192.168.60.154","lmtport":9000,"presetno":1,"time":"2017-10-23 08:05:56"}],
				//	"Method":"Call_IvaAlarmData","Version":"V1.00.00"}

                SHL_Json::Value c2ivaNode;
		        SHL_Json::Reader c2ivareader;
				if(!c2ivareader.parse(jout, c2ivaNode))
				{
					return FALSE;
				}

				SHL_Json::Value roilist = c2ivaNode["RoiList"];
				if (0 == roilist.size())
				{
					return FALSE;
				}

				int itemNum = roilist.size();

				SHL_Json::Value jsonRsp;
				jsonRsp["Version"]  = "V1.00.00";
				jsonRsp["Method"]   = "Call_IvaAlarmData";

				map<string, char*> mapDevToIvalist;

				SHL_Json::Value eleIva;
				for(int i=0; i < itemNum; i++)
				{ 
					eleIva = roilist[i];

					char devInfo[128] = {0};
					string devip      = eleIva["DevIp"].asString();
					int    port       = eleIva["DevPort"].asInt();
					int    chn        = eleIva["DevChn"].asInt();
					int    preset     = eleIva["PresetId"].asInt();
					int    roiid      = eleIva["RoiId"].asInt();
				    sprintf(devInfo, "%s_%d_%d_%d", devip.c_str(), port, chn, preset);

					string alarmState = eleIva["AlarmMsg"].asString();   
					int alarmVal = 0;
					if ("INVALID" != alarmState)
					{
						int t =atoi(alarmState.c_str());
                        alarmVal = (t != 0) ? ((t != -1) ? 1:128) : 0;
					}

					map<string, char*>::iterator pos = mapDevToIvalist.find(devInfo);
					if (pos == mapDevToIvalist.end())
					{
						if (roiid >= 6)
						{
							continue;
						}
						
						char *index = new char[6];
						memset(index, 0, 6 * sizeof(char));
						
						index[roiid] = alarmVal;
						mapDevToIvalist[devInfo] = index;
					}
					else
					{
                         char *index = pos->second;
                         index[roiid] = alarmVal;
					}
				}			
				
				SHL_Json::Value c2ivaAlarmNode;
				
				for(int i=0; i < itemNum; i++)
				{ 
					eleIva = roilist[i];

					string devip      = eleIva["DevIp"].asString();
					int    port       = eleIva["DevPort"].asInt();
					int    chn        = eleIva["DevChn"].asInt();
					int    preset     = eleIva["PresetId"].asInt();
				    
					c2ivaAlarmNode["devip"]     = devip;
					c2ivaAlarmNode["devport"]   = port;
					c2ivaAlarmNode["channel"]   = (chn);
					c2ivaAlarmNode["lmtip"]     = eleIva["LmtIp"].asString();
					c2ivaAlarmNode["lmtport"]   = 9000;
					c2ivaAlarmNode["presetno"]  = preset;
					c2ivaAlarmNode["quality"]   = 1;
					string strAlarmTime;
					GetDateTime(strAlarmTime);
					c2ivaAlarmNode["time"]      = strAlarmTime; 

					char devInfo[128] = {0};
					sprintf(devInfo, "%s_%d_%d_%d", devip.c_str(), port, chn, preset);
					map<string, char*>::iterator posItem = mapDevToIvalist.find(devInfo);
					if (posItem != mapDevToIvalist.end())
					{
						char *index = posItem->second;

						char alarmData[128] = {0};
						sprintf(alarmData, "%d,%d,%d,%d,%d,%d,", index[0], index[1], index[2], index[3], index[4], index[5]);

						c2ivaAlarmNode["ivaresult"] = alarmData;

						delete posItem->second;  
						mapDevToIvalist.erase(posItem);				                    
					}
					else
					{
						continue;
					}

					jsonRsp["Item"].append(c2ivaAlarmNode);
				} 

				strReq = req.write(jsonRsp);				 
				strcpy(out, strReq.c_str());
				*ps8AlarmInfo = (S8 *)out;
				
				return TRUE;
			}
		}
	}  
	*ps8AlarmInfo = (S8 *)out;
	return FALSE;
}

//************************************************************************
//�������ƣ�GetIvaCruiseList(const S8 *ps8Req, U32 u32Size, S8 **ps8IvaCruiseList)
//����˵������ȡ���ܷ������ٸ澯
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//    (in) ps8IvaCruiseList  ����ֵ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetIvaCruiseList(const S8 *ps8Req, U32 u32Size, S8 **ps8IvaCruiseList)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�ControlIvaCruise(const S8 *ps8Req, U32 u32Size)
//����˵�����������ܷ�����Ѳ��ͣ
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::ControlIvaCruise(const S8 *ps8Req, U32 u32Size)
{
	return FALSE;
}

//************************************************************************
//�������ƣ�QueryIvaRunList(const S8 *ps8Req, U32 u32Size, S8 **ps8IvaList)
//����˵������ѯ���ܷ��������б�
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//    (in) ps8IvaList  ����ֵ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::QueryIvaRunList(const S8 *ps8Req, U32 u32Size, S8 **ps8IvaList)
{
	return FALSE;
}


//************************************************************************
//�������ƣ�:QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState)
//����˵������ȡӲ��¼���ͨ����¼��״̬
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//    (in) ps8IvaList  ����ֵ
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState)
{
	char jout[MAX_STRING_LEN] = {0};
	if((NULL != m_hLogin) && (NULL != ps8Req))
	{
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if(!reader.parse(ps8Req, rootNode))
		{
			return FALSE;
		}
		string strMethod = (rootNode["Method"].asString()); 
		string strDevIp = rootNode["Ip"].asString();
		int    iDevPort = rootNode["Port"].asInt();
//	    string strVersion = (rootNode["Version"].asString()); 
		if(strMethod == "Query_RecordState")
		{
			SHL_Json::Value jsonReq;
			jsonReq["DevIp"]   = strDevIp;
			jsonReq["DevPort"] = iDevPort; 
			jsonReq["Method"]  = "getRecordWorkStatus"; 
			jsonReq["version"] =  "v1.00";
			SHL_Json::FastWriter req;
			string strReq      = req.write(jsonReq);
			
			const char* cReq= strReq.c_str();
			char* cStrReq = const_cast<char*>(cReq);
			int len = strReq.size();
			int sz = MAX_STRING_LEN;
			
			int iRet = c2_get_config(m_hLogin, cStrReq, len, jout, &sz); 	
			if(0==iRet)
			{				           
				m_chanRecordStatus =  jout;
				*ps8ChanRecordState = (char*)m_chanRecordStatus.c_str();
				return TRUE;
			}
		}
	
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�GetResponse
//����˵�������������ȡ���ؽ��
//����������
//    (in) ps8Req  ����
//    (in) u32Size �����С
//    (in) p8Rsp  ���ջ���
//    (in) u32RspSize  ������ջ����С,����ʵ�����ݴ�С
//����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformConn::GetResponse(const S8 * ps8Req, U32 u32Size, S8 *p8Rsp,U32 &u32RspSize)
{

	if((NULL != m_hLogin) && (NULL != ps8Req) && (NULL != p8Rsp)&& (0 < u32RspSize))
	{
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if(!reader.parse(ps8Req, rootNode))
		{
			return FALSE;
		}
		string strMethod = (rootNode["Method"].asString()); 
		string strDevIp = rootNode["Ip"].asString();
		int    iDevPort = rootNode["Port"].asInt();
		int		iChanNo = rootNode["ChannelNo"].asInt();

		SHL_Json::Value jsonReq;
		jsonReq["DevIp"]   = strDevIp;
		jsonReq["DevPort"] = iDevPort; 
		jsonReq["Method"]  = strMethod.c_str(); 
		jsonReq["ChannelNo"]  = iChanNo; 
		jsonReq["version"] =  "v1.00";
		SHL_Json::FastWriter req;
		string strReq      = req.write(jsonReq);
		
		const char* cReq= strReq.c_str();
		char* cStrReq = const_cast<char*>(cReq);
		int len = strReq.size();
		int sz = u32RspSize;
		
		int iRet = c2_get_config(m_hLogin, cStrReq, len, p8Rsp, &sz); 	
		if(0==iRet)
		{				           
			//m_chanRecordStatus =  jout;
			//*ps8response = (char*)m_chanRecordStatus.c_str();
			u32RspSize = sz;
			return TRUE;
		}		
	}
	return FALSE;
}

HANDLE C2PlatformConn::InitDevTalk(int audioType)
{
    if (NULL != m_pDualTalkEx)
    {
        return m_pDualTalkEx;	
    }

    m_pDualTalkEx = new C2PlatformDualtalkEx(this, audioType);

    if (NULL == m_pDualTalkEx)
    {	    
        return NULL;
    }
	
    S32 s32Ret = m_pDualTalkEx->Start();
    if (s32Ret < 0)
    {
		delete m_pDualTalkEx;
		m_pDualTalkEx = NULL;
        return NULL;
    }
	
    return m_pDualTalkEx;				
}
