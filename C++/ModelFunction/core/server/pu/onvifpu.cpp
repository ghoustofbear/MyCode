#include "onvifpu.h"
#include "cfg_device.h"
#include "json/json.h"
#include<memory>
#include<list>
#define ONVIF_GETSERVICE "http://%s:%d/onvif/device_service"

onvifPu::onvifPu(string host, int port, string user, string password)
	:Pu(0, "", user, 0, "", "", host)
{
	char onvif_url[128];
	string onvif_device = onvif_url;
	snprintf(onvif_url, sizeof(onvif_url), ONVIF_GETSERVICE, this->host(), this->port());
	onvifDevice = new OnvifClientDevice(onvif_device, this->userName(), this->passWord());
	m_profiles = new _trt__GetProfilesResponse;
}

onvifPu::onvifPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType, int linktype)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
	string a = "Dahua";
	m_port = port;
	m_protocol = PRTO_TYPE_ONVIF;
	m_domainType = domainType;
	m_login = false;
	char onvif_url[512];
	snprintf(onvif_url, sizeof(onvif_url), ONVIF_GETSERVICE, host.c_str(), port);
	string onvif_device = onvif_url;
	onvifDevice = new OnvifClientDevice(onvif_device, this->userName(), this->passWord());
	m_profiles = new _trt__GetProfilesResponse;
	m_media = new OnvifClientMedia(*onvifDevice);
	m_Ptz = new OnvifClientPTZ(*onvifDevice);
	m_Recording = new OnvifClientRecording(*onvifDevice);
	m_Replay = new OnvifClientReplay(*onvifDevice);
	if (name.find(a))
	{
		//m_DahuaPu = new DahuaPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType, linktype);
		/*m_protocol = PRTO_TYPE_ONVIF;*/
	}
	
}

onvifPu::~onvifPu()
{
	if (onvifDevice)
		delete onvifDevice;
	if (m_profiles)
		delete m_profiles;
	if (m_Ptz)
		delete m_Ptz;
	if (m_DahuaPu)
		delete m_DahuaPu;
	if (m_Recording)
		delete m_Recording;
	if (m_Replay)
		delete m_Replay;
}

int
onvifPu::start()
{
	Pu::start();

	int res = onvifDevice->GetCapabilities(); //onvif获取设备能力以及连接设备

	res = m_media->GetProfiles(*m_profiles); //onvif 获取媒体信息
    
	res = m_Recording->GetRecordings(recordings);
	for (vector<tt__GetRecordingsResponseItem* >::iterator iter =
		recordings.RecordingItem.begin();
		iter < recordings.RecordingItem.end(); ++iter)
	{

		tt__GetRecordingsResponseItem* pRecordings = *iter;
		printf("pRecordings %s\n", pRecordings->RecordingToken.c_str());
		tt__RecordingConfiguration* pRecordingConf = pRecordings->Configuration;
		if (pRecordingConf->Source)
		{
			printf("Recording SourceId %s\n", pRecordingConf->Source->SourceId.c_str());
			printf("Recording Name %s\n", pRecordingConf->Source->Name.c_str());
			printf("Recording Location %s\n", pRecordingConf->Source->Location.c_str());
			printf("Recording Description %s\n", pRecordingConf->Source->Description.c_str());
			printf("Recording Address %s\n", pRecordingConf->Source->Address.c_str());
		}

		string replayUrl;
		
		m_Replay->GetReplayUri(pRecordings->RecordingToken, replayUrl);
		printf("Relay Url %s\n", replayUrl.c_str());
		m_rtsp.insert(make_pair(pRecordings->RecordingToken, replayUrl));
	}

	if (res == 0)
		m_login = true;

	return res;
}

void
onvifPu::stop()
{

}

int
onvifPu::login()
{
	if (m_DahuaPu)
	{
		
	}
	return 0;
}

void
onvifPu::onTimer()
{

	Pu::onTimer();
}


void
onvifPu::logout()
{

}


int
onvifPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
	string ptzurl;
	int res;
	res = onvifDevice->GetPTZUrl(ptzurl);
	_tptz__ContinuousMoveResponse MoveResponse;
	tt__PTZSpeed PTZSpeed;
	PTZSpeed.PanTilt = new tt__Vector2D();
	PTZSpeed.Zoom = new tt__Vector1D();
	float speedfloat = speed;
	_tptz__SetPresetResponse tptz__SetPresetResponse;
	_tptz__RemovePresetResponse RemovePresetResponse;
	_tptz__GetPresetsResponse GetPresetsResponse;
	_tptz__GotoPresetResponse GotoPresetRespons;
	char PresentName[128];
	string Presentname;
	m_Ptz->GetPresent(GetPresetsResponse, m_profiles->Profiles[channelNo]->token);
	if (res == 1)
	{
		switch (cmd)
		{
		case  PTZ_CMD_LEFT:
			PTZSpeed.PanTilt->x = -speedfloat / 10;
			PTZSpeed.PanTilt->y = 0;
			break;
		case  PTZ_CMD_RIGHT:
			PTZSpeed.PanTilt->x = speedfloat / 10;
			PTZSpeed.PanTilt->y = 0;
			break;
		case  PTZ_CMD_UP:
			PTZSpeed.PanTilt->x = 0;
			PTZSpeed.PanTilt->y = speedfloat / 10;
			break;
		case  PTZ_CMD_DOWN:
			PTZSpeed.PanTilt->x = 0;
			PTZSpeed.PanTilt->y = -speedfloat / 10;
			break;
		case PTZ_CMD_UPLEFT :
			PTZSpeed.PanTilt->x = -speedfloat / 10;
			PTZSpeed.PanTilt->y = speedfloat / 10;
			break;
		case PTZ_CMD_UPRIGHT:
			PTZSpeed.PanTilt->x = speedfloat / 10;
			PTZSpeed.PanTilt->y = speedfloat / 10;
			break;
		case PTZ_CMD_DOWNLEFT:
			PTZSpeed.PanTilt->x = -speedfloat / 10;
			PTZSpeed.PanTilt->y = -speedfloat / 10;
			break;
		case PTZ_CMD_DOWNRIGHT:
			PTZSpeed.PanTilt->x = speedfloat / 10;
			PTZSpeed.PanTilt->y = -speedfloat / 10;
			break;
		case PTZ_CMD_SETPRESET:
			snprintf(PresentName, 128, "present_%d", preset);
			 Presentname = PresentName;
			 res = m_Ptz->SetPresent(tptz__SetPresetResponse, m_profiles->Profiles[channelNo]->token, *GetPresetsResponse.Preset[preset]->token, Presentname);
			 if (res == 0)
				 return 0;
			 else
				 return -1;
		case PTZ_CMD_CALLPRESET:
			res = m_Ptz->GotoPresent(GotoPresetRespons, m_profiles->Profiles[channelNo]->token, *GetPresetsResponse.Preset[preset]->token);
			if (res == 0)
				return 0;
			else
				return -1;
		case PTZ_CMD_CLEARPRESET:
			res = m_Ptz->DeletePresent(RemovePresetResponse, m_profiles->Profiles[channelNo]->token, *GetPresetsResponse.Preset[preset]->token);
			if (res == 0)
				return 0;
			else
				return -1;
		default:
			break;
		}
	}
	res = m_Ptz->ContinuousMove(MoveResponse, PTZSpeed, m_profiles->Profiles[channelNo]->token);
	_tptz__StopResponse StopResponse;
	if (stop = true)
		sys->sleep(100);
		res = m_Ptz->Stop(StopResponse, true, false, m_profiles->Profiles[channelNo]->token);
	return res;
}

int
onvifPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
	SHL_Json::Value value;
	SHL_Json::Value array;

	NET_TIME startTM = { 0 };
	startTM.dwYear = start.year;
	startTM.dwMonth = start.month;
	startTM.dwDay = start.day;
	startTM.dwHour = start.hour;
	startTM.dwMinute = start.minute;
	startTM.dwSecond = start.second;

	NET_TIME endTM = { 0 };
	endTM.dwYear = end.year;
	endTM.dwMonth = end.month;
	endTM.dwDay = end.day;
	endTM.dwHour = end.hour;
	endTM.dwMinute = end.minute;
	endTM.dwSecond = end.second;

	DWORD dwstream = 0;
	/*m_profiles->Profiles*/
	//
	//// 设置语音对讲模式,客户端方式还是服务器方式(pValue内存由用户申请释放，大小参照EM_USEDEV_MODE对应的结构体)
	//CLIENT_SetDeviceMode(m_DahuaPu->getUserID(), DH_RECORD_STREAM_TYPE, &dwstream);

	//LLONG recordHand = CLIENT_FindFile(m_DahuaPu->getUserID(), channelNo, 0, NULL, &startTM, &endTM, false, 2000);

	//if (0 == recordHand)
	//{
	//	ERRE_MSV("CLIENT_FindFile erro");
	//	return -1;
	//}
	//int index = 0;
	//while (true)
	//{
	//	NET_RECORDFILE_INFO recordInfo = { 0 };

	//	int res = CLIENT_FindNextFile(recordHand, &recordInfo);
	//	
	//	if (1 == res)
	//	{
	//		index++;

	//		if (index < fromIndex || index > toIndex)
	//		{
	//			continue;
	//		}

	//		char filename[128] = { 0 };

	//		snprintf(filename,
	//			128,
	//			"ch%02d-%04d%02d%02d-%02d%02d%02d-%04d%02d%02d-%02d%02d%02d.dav",
	//			recordInfo.ch,
	//			recordInfo.starttime.dwYear,
	//			recordInfo.starttime.dwMonth,
	//			recordInfo.starttime.dwDay,
	//			recordInfo.starttime.dwHour,
	//			recordInfo.starttime.dwMinute,
	//			recordInfo.starttime.dwSecond,
	//			recordInfo.endtime.dwYear,
	//			recordInfo.endtime.dwMonth,
	//			recordInfo.endtime.dwDay,
	//			recordInfo.endtime.dwHour,
	//			recordInfo.endtime.dwMinute,
	//			recordInfo.endtime.dwSecond
	//		);

	//		ERRE_MSV("filename: %s, real: %s ", filename, recordInfo.filename);

			/*SHL_Json::Value record;*/
	//		record["filename"] = filename;
	//		record["fileurl"] = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(filename);
	//		record["channelNo"] = (int)channelNo;
	//		record["size"] = recordInfo.size * 1024;
	//		record["index"] = index;

	//		/** record["type"] < 0-ËùÓÐÂ¼Ïñ 1-¼Æ»®Â¼Ïñ 2 ÊÖ¶¯Â¼Ïñ 3-±¨¾¯Â¼Ïñ */

	//		if (1 == recordInfo.bRecType)
	//		{
	//			record["type"] = 3;
	//		}
	//		else if (0 == recordInfo.bRecType)
	//		{
	//			record["type"] = 1;
	//		}
	//		else
	//		{
	//			record["type"] = 0;
	//		}


			//datetime_s start;
			//datetime_s end;

			//start.year = recordInfo.starttime.dwYear;
			//start.month = recordInfo.starttime.dwMonth;
			//start.day = recordInfo.starttime.dwDay;
			//start.hour = recordInfo.starttime.dwHour;
			//start.minute = recordInfo.starttime.dwMinute;
			//start.second = recordInfo.starttime.dwSecond;

			//record["start"] = sys->strftime(start);

			//end.year = recordInfo.endtime.dwYear;
			//end.month = recordInfo.endtime.dwMonth;
			//end.day = recordInfo.endtime.dwDay;
			//end.hour = recordInfo.endtime.dwHour;
			//end.minute = recordInfo.endtime.dwMinute;
			//end.second = recordInfo.endtime.dwSecond;

			//record["end"] = sys->strftime(end);
			//array.append(record);


	//	}
	//	else if (-1 == res)
	//	{
	//		ERRE_MSV("CLIENT_FindNextFile error");
	//		return -1;
	//	}
	//	else if (0 == res)
	//	{
	//		break;
	//	}

	

	//CLIENT_FindCloseEx(recordHand);

	//value["records"] = array;
	//value["realNum"] = index;
	//value["fromIndex"] = fromIndex;
	//value["toIndex"] = fromIndex + array.size();

	//SHL_Json::StreamWriterBuilder writer;
	//json = SHL_Json::writeString(writer, value);
//for (int i = 0; i < m_rtsp.size(); i++)
//{
//	SHL_Json::Value record;
//	record["Record_Token"] = m_rtsp->first;
//	record["channelNo"] = (int)channelNo;
//}

SHL_Json::Value data;
std::map<string, string>::iterator it1;//使用迭代器
for (it1 = m_rtsp.begin();it1!= m_rtsp.end(); it1++)
{
	//cout << "a[" << it->first << "]=" << it->second << endl;
	SHL_Json::Value Record_Token;
	Record_Token["Record_Token"] = it1->first;
	Record_Token["channelNo"] = (int)channelNo;
	Record_Token["fileurl"] = it1->second;
	data.append(Record_Token);

}

value["records"] = SHL_Json::Value(data);
value["NVR_Type"] = "dahua";
SHL_Json::StreamWriterBuilder writer;
json = SHL_Json::writeString(writer, value);
	return 0;

}

void
onvifPu::UpdateAliveTime()
{
}