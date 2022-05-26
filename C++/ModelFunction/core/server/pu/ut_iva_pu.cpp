#include "ut_iva_pu.h"
#include "putree.h"
#include "json/json.h"
#include "j2x.h"
#include "triggerlistener.h"
#include <fstream>
#include <sstream>
#include "utiva/typedef.h"
#include "putree.h"
#include "tls_error.h"
#include "platform.h"
#include "cfg_iva_lmt.h"
#include "math.h"
#include "global_str_def.h"

UtIvaPu * utIvaPu = NULL;

UtIvaPu::UtIvaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_UT;
    m_type     = DEV_TYPE_AI;
    m_Cnn      = 0;
    m_port     = port;
    utIvaPu    = this;
    m_sendLmt = false;

    m_lmtPort = 0;
}

UtIvaPu::~UtIvaPu()
{
    stop();
}

int UtIvaPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }
    
    m_init = true;
    return 0;
}

void UtIvaPu::stop()
{
    if (!m_init)
        return;

    m_init = false;

    logout();

    Pu::stop();
}

int UtIvaPu::login()
{
    if (m_login)
    {
        return 0;
    }

    UTIVA_CONN_PARAM cnnParam;
    cnnParam.cIvaIp               = const_cast<char *>(m_host.c_str());
    cnnParam.iIvaPort             = m_port;
    cnnParam.cUserName            = const_cast<char *>(m_user.c_str());
    cnnParam.cUserPwd             = const_cast<char *>(m_pswd.c_str());
    cnnParam.bAutoReconn          = true;
    cnnParam.bTrackDesired        = false;
    cnnParam.pUser                = this;
	cnnParam.bPTC_B				  = false;
    cnnParam.pfnAlarmCb           = AlarmCallback;
    cnnParam.pfnLoginCb           = LoginCallback;
    m_Cnn = NET_IvaLogin(cnnParam);

    queryIvaLmt();
    return 0;
}

void UtIvaPu::logout()
{
    if(m_Cnn)
    {
        NET_IvaRelease(m_Cnn);
        m_Cnn = 0;
    }
}

int UtIvaPu::onCustomCommand(string &json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;

	SHL_Json::Reader resReader;
	SHL_Json::Value  resValue;

	SHL_Json::FastWriter writer;
	int ret = -1;
	string jMethod = "";
	reader.parse(json, rvalue, false);
	if (!rvalue["Method"].isNull())
	{
		jMethod = rvalue["Method"].asString();
	}
	else if (!rvalue["command"].isNull())
	{
		jMethod = rvalue["command"].asString();
	}



	if (jMethod == "startIva")
	{
		string devIp = rvalue["chnIp"].asString();
		int    chnNo = rvalue["chnNo"].asInt();
		int    presetNo = rvalue["presetNo"].asInt();
		int    bfile = rvalue["bFile"].asInt();
		string imgdata = rvalue["ImData"].asString();
		/*增加文件分析*/

		element["Method"] = IVA_START_IVA;
		element["Version"] = "v1.0";
		element["Token"] = "";
		element["DevIp"] = devIp;
		element["DevChn"] = chnNo;
		element["PresetId"] = presetNo;
		element["bFile"] = bfile;
		element["ImData"] = imgdata;
		string jjss = writer.write(element);

		string jpreset = getParamPreset(json.c_str(), json.length());
		sendStartIVAAlarm(jpreset);
		LOGI_MSV("UtIvaPu::onCustomCommand startIva josn:%s", json.c_str());
		if (startanaylyse(jjss.c_str(), jjss.size()) != "")
		{
			ret = 0;
		}
	}
	else if (jMethod == "stopIva")
	{
		string devIp = rvalue["chnIp"].asString();
		int    chnNo = rvalue["chnNo"].asInt();
		int    presetNo = rvalue["presetNo"].asInt();

		element["Method"] = IVA_STOP_IVA;
		element["Version"] = "v1.0";
		element["Token"] = "";
		element["DevIp"] = devIp;
		element["DevChn"] = chnNo;
		element["PresetId"] = presetNo;
		string jjss = writer.write(element);
		if (stopanaylyse(jjss.c_str(), jjss.size()) != "")
		{
			ret = 0;
		}
	}
	else if ("setFace" == jMethod)
	{
		string FaceId = "";
		string picData = "";
		int opera = rvalue["opera"].asInt();
		if (!rvalue["ID"].isNull())
		{
			FaceId = rvalue["ID"].asString();
		}
		if (!rvalue["picData"].isNull())
		{
			picData = rvalue["picData"].asString();
		}
		if (opera == 0)
		{
			//add face
			element["Method"] = IVA_SETPARAM_FACEIMG;
			element["Version"] = "v1.0";
			element["Token"] = "";
			element["FaceId"] = FaceId;
			element["ImData"] = picData;
			string jjss = writer.write(element);
			string res = "";
			if (sendIvaRequest(jjss, res))
			{
				ret = 0;
			}
		}
		else
		{
			//delete face
			//steps are as follows
			//  1.IVA_GETLIST_FACEIMG
			//  2.IVA_DELLIST_FACEIMG

			//  1.IVA_GETLIST_FACEIMG
			element["Method"] = IVA_GETLIST_FACEIMG;
			element["Version"] = "v1.0";
			element["Token"] = "";
			element["FaceId"] = FaceId;
			string jjss = writer.write(element);
			string res = "";
			if (sendIvaRequest(jjss.c_str(), res))
			{
				//  2.IVA_DELLIST_FACEIMG
				SHL_Json::Reader resReader;
				SHL_Json::Value  resValue;

				if (resReader.parse(res, resValue, false))
				{

					SHL_Json::Value  delValue;
					delValue["Method"] = IVA_DELLIST_FACEIMG;
					delValue["Version"] = "v1.0";
					delValue["Token"] = "";
					delValue["FaceImgList"] = resValue["FaceImgList"];

					string jjss = writer.write(delValue);
					string res2 = "";
					if (sendIvaRequest(jjss.c_str(), res2))
					{
						ret = 0;
					}
				}
				else
					ERRE_MSV("SHL_Json parse error,json:%s ", res.c_str());
			}
		}
	}
	else if ("prohibitAlarm" == jMethod)
	{
		string devIp = rvalue["devIp"].asString();
		int    chnNo = rvalue["chnNo"].asInt();
		int    presetNo = rvalue["presetNo"].asInt();
		int    prohibitTime = rvalue["prohibitTime"].asInt();

		element["Method"] = IVA_STOP_ALARM;
		element["Version"] = "v1.0";
		element["Token"] = "";
		element["DevIp"] = devIp;
		element["DevChn"] = chnNo;
		element["PresetId"] = presetNo;
		element["DurMin"] = prohibitTime;

		string jjss = writer.write(element);

		string res2 = "";
		if (sendIvaRequest(jjss.c_str(), res2))
		{
			ret = 0;
		}
	}
	else if ("setIvaPresetParam" == jMethod)
	{
		ret = onSetIvaPresetParam(json);
	}
	else if ("setIvaParamRule" == jMethod)
	{
		ret = onSetIvaParamRule(json);
	}
	else if ("deleteIvaParamRule" == jMethod)
	{
		ret = onDeleteIvaParamRule(json);
	}
	else if ("deleteIvaPresetParam" == jMethod)
	{
		ret = onDeleteIvaPresetParam(json);
	}

    return ret;
}

int UtIvaPu::onRequestConfig(string & jin, string& jout)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	string jMethod = rvalue["Method"].asString();

	if (jMethod == "callAll")
	{
		jout = callAll();
	}
    else if (jMethod == "getIvaPresetParam")
    {
        return onGetIvaPresetParam(jin,jout);
    }
    else if (jMethod == "getIvaPresetState")
    {
        return onGetIvaPresetState(jin, jout);
    }
	else if (jMethod == "getIvaRuleList")
	{
		return onGetIvaRulrList(jin, jout);
	}

	return 0;
}
int UtIvaPu::queryIvaLmt()
{
	char cond[128] = { 0 };
	snprintf(cond, 128, "iva_addr = '%s'", addrcode().c_str());
	std::string con(cond);
	string json;
	int ret = ivalmtCfg->query(json, cond);
	if (ret < 0)
	{
        m_lmtIp.clear();
        m_lmtPort=0;
        m_lmtUser.clear();
        m_lmtPsaawd.clear();
        ERRE_MSV("UtIvaPu::queryIvaLmt failed,addrcode = ", addrcode().c_str());
		return -1;
	}
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(json, rvalue, false))
	{
		return -1;
    }
    if (!rvalue[0]["lmt_ip"].isNull())
    {
        m_lmtIp = rvalue[0]["lmt_ip"].asString();
    }
    m_lmtPort = rvalue[0]["lmt_port"].asInt();
    if (!rvalue[0]["lmt_user"].isNull())
    {
        m_lmtUser = rvalue[0]["lmt_user"].asString();
    }
    if (!rvalue[0]["lmt_passwd"].isNull())
    {
        m_lmtPsaawd = rvalue[0]["lmt_passwd"].asString();
    }

    return 0;
}
string UtIvaPu::callAll()
{
	SHL_Json::Value  rvalue;
	rvalue["Method"] = IVA_GETLIST_PRESET;
	SHL_Json::FastWriter write;
	string jsonStr = write.write(rvalue);
	 string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
    else
    {
        return "";
    }
    return res;
}

bool UtIvaPu::sendIvaRequest(string json,string &res)
{
    bool ret = true;
    if (m_login)
    {
        SHL_Json::Reader resReader;
        SHL_Json::Value  resValue;
        res = NET_IvaRequestCmd(m_Cnn, json.c_str());
        if (res != "" && resReader.parse(res, resValue, false))
        {
            int RtnCode = resValue["RtnCode"].asInt();
            if (RtnCode != 0)
            {
                ERRE_MSV("NET_IvaRequestCmd error,json:%s ", res.c_str());
                ret = false;
            }
        }
        else
        {
            if (res == "")
                ERRE_MSV("NET_IvaRequestCmd error,res is NULL ");
            else
                ERRE_MSV("SHL_Json parse error,json:%s ", res.c_str());
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}

std::string UtIvaPu::setParamPreset(const char *json, int size)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(json, rvalue, false))
	{
		return "";
	}
	rvalue["Method"] = IVA_SETPARAM_PRESET;
	SHL_Json::FastWriter write;
	string jsonStr = write.write(rvalue);
	string res = "";
	if (m_login)
	{
		res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
	}
	else
	{
		return "";
	}
	return res;
}

string             
UtIvaPu::getParamPreset(const char *json, int size)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  wvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return "";
    }
    wvalue["Method"] = IVA_GETPARAM_PRESET;
    wvalue["Version"] = "v1.0";
    wvalue["Token"] = "";
    wvalue["DevIp"] = rvalue["chnIp"];
    wvalue["DevChn"] = rvalue["chnNo"];
    wvalue["PresetId"] = rvalue["presetNo"];
    SHL_Json::FastWriter write;
    string jsonStr = write.write(wvalue);
    string res = "";
    if (m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
    else
    {
        return "";
    }
    return res;
}

int 
UtIvaPu::onSetIvaPresetParam(const string & jin)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::FastWriter writer;
    SHL_Json::Value wvalue;

    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    wvalue = rvalue["presetParam"];

    string json = writer.write(wvalue);
    string ret = setParamPreset(json.c_str(),json.size());


    if (reader.parse(ret, rvalue, false))
    {
        if (rvalue["RtnCode"].asInt() == 0)
        {
            return 0;
        }
    }

    ERRE_MSV("SetIvaPresetParam failed,%s", ret.c_str());
    return -1;
}

int
UtIvaPu::onSetIvaParamRule(const string & jin)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::FastWriter writer;
	SHL_Json::Value wvalue;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}
	wvalue["Method"] = IVA_SETPARAM_RULE;
	wvalue["v1.0"] = "v1.0";
	wvalue["Token"] = "";
	wvalue["RuleName"] = rvalue["RuleName"];
	wvalue["RuleMode"] = rvalue["RuleMode"];
	if (rvalue["RuleParam"].isArray())
	{
		SHL_Json::Value value;
		for (int i = 0; i<int(rvalue["RuleParam"].size()); i++)
		{
			value["Period"] = rvalue["RuleParam"][i]["Period"];
			value["StartTime"] = rvalue["RuleParam"][i]["StartTime"];
			value["RunSec"] = rvalue["RuleParam"][i]["RunSec"];
			wvalue["RuleParam"].append(value);
		}
	}
	SHL_Json::FastWriter write;
	string json = writer.write(wvalue);
	string res = "";
	if (m_login)
	{
		res = NET_IvaRequestCmd(m_Cnn, json.c_str());
	}
	if (reader.parse(res, rvalue, false))
	{
		if (rvalue["RtnCode"].asInt() == 0)
		{
			return 0;
		}
	}
	ERRE_MSV("SetIvaPresetParam failed,%s", res.c_str());
	return -1;

}

int 
UtIvaPu::onDeleteIvaParamRule(const string & jin)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::FastWriter writer;
	SHL_Json::Value wvalue;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}
	wvalue["Method"] = IVA_DELPARAM_RULE;
	wvalue["v1.0"] = "v1.0";
	wvalue["Token"] = "";
	wvalue["RuleName"] = rvalue["RuleName"];
	SHL_Json::FastWriter write;
	string json = writer.write(wvalue);
	string res = "";
	if (m_login)
	{
		res = NET_IvaRequestCmd(m_Cnn, json.c_str());
	}
	if (reader.parse(res, rvalue, false))
	{
		if (rvalue["RtnCode"].asInt() == 0)
		{
			return 0;
		}
	}
	ERRE_MSV("SetIvaPresetParam failed,%s", res.c_str());
	return -1;
}


int 
UtIvaPu::onDeleteIvaPresetParam(const string & jin)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::FastWriter writer;
	SHL_Json::Value wvalue;

	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}
	wvalue["Method"] = IVA_DELPARAM_PRESET;
	wvalue["v1.0"] = "v1.0";
	wvalue["Token"] = "";
	wvalue["DevIp"] = rvalue["DevIp"];
	wvalue["DevChn"] = rvalue["DevChn"];
	wvalue["PresetId"] = rvalue["PresetId"];
	wvalue["RoiId"] = rvalue["RoiId"];
	SHL_Json::FastWriter write;
	string json = writer.write(wvalue);
	string res = "";
	if (m_login)
	{
		res = NET_IvaRequestCmd(m_Cnn, json.c_str());
	}
	if (reader.parse(res, rvalue, false))
	{
		if (rvalue["RtnCode"].asInt() == 0)
		{
			return 0;
		}
	}
	ERRE_MSV("SetIvaPresetParam failed,%s", res.c_str());
	return -1;
}

int
UtIvaPu::onGetIvaPresetState(const string & jin, string& jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    string devIp = rvalue["chnIp"].asString();
    int    chnNo = rvalue["chnNo"].asInt();
    int    presetNo = rvalue["presetNo"].asInt();

    SHL_Json::FastWriter writer;
    SHL_Json::Value  wvalue;
    wvalue["Version"] = "v1.0";
    wvalue["Token"] = "";
    wvalue["DevIp"] = devIp;
    wvalue["DevChn"] = chnNo;
    wvalue["PresetId"] = presetNo;
    string json = writer.write(wvalue);

    jout = getPresetState(json.c_str(), json.length());
    return jout == "" ? -1 : 0;
}

int
UtIvaPu::onGetIvaRulrList(const string & jin, string& jout)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  wvalue;
	if (!reader.parse(jin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}

	wvalue["Method"] = IVA_GETLIST_RULE;
	wvalue["Version"] = "v1.0";
	wvalue["Token"] = "";
	SHL_Json::FastWriter write;
	string jsonStr = write.write(wvalue);
	if (m_login)
	{
		jout = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
	}
	if (jout == "")
	{
		return -1;
	}
	return 0;
}



int
UtIvaPu::onGetIvaPresetParam(const string & jin, string& jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse error");
        return -1;
    }

    jout = getParamPreset(jin.c_str(), jin.length());
    if (jout == "")
    {
        return -1;
    }
    return 0;
}

string
UtIvaPu::sendStartIVAAlarm(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  wvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return "";
    }
    wvalue["version"] = "1.0";
    wvalue["time"] = sys->currentTime();

    wvalue["deviceName"] = "";
    wvalue["deviceHost"] = "";
    wvalue["devicePort"] = 0;
    wvalue["addrcode"] = "";
    wvalue["level"] = 1;

    string devIp = rvalue["DevIp"].asString();
    int    devPort = rvalue["DevPort"].asInt();
    Pu *pu = putree->findPu(devIp.c_str(), devPort);
    if (pu)
    {
        wvalue["deviceName"] = pu->name();
        wvalue["addrcode"] = pu->addrcode();
        wvalue["deviceHost"] = pu->host();
        wvalue["devicePort"] = pu->port();
    }
    else
    {
        return "";
    }
    SHL_Json::Value itemAlarm;


    int inputNo = rvalue["DevChn"].asInt();
    itemAlarm["type"] = TriggerListener::TR_STARTED_IVA;
    itemAlarm["channelNo"] = inputNo;
    itemAlarm["status"] = 0;
    //itemAlarm["gucode"] = "";
    //itemAlarm["name"] = "";
    itemAlarm["preset"] = rvalue["PresetId"].asInt();
    itemAlarm["alarmdesc"] = "";
    itemAlarm["roiId"] = rvalue["RoiId"].asInt();
    //itemAlarm["picName"] = "";
    itemAlarm["SerialOut"] = rvalue["SerialOut"];

    if (rvalue["RoiList"].isArray())
    {
        SHL_Json::Value roiList = rvalue["RoiList"];
        for (int i = 0; i < int(roiList.size()); ++i)
        {
            int AnalyseType = roiList[i]["AnalyseType"].asInt();
            int AnalyseSubType = roiList[i]["AnalyseSubType"].asInt();
            AnalyseSubType = pow(2, AnalyseSubType);
            int type = ivaTypeCast(AnalyseType, AnalyseSubType);
            itemAlarm["RoiList"][i]["type"] = type;
            itemAlarm["RoiList"][i]["RoiId"] = roiList[i]["RoiId"];
            if (roiList[i]["RoiPts"].isArray())
            {
                for (int j = 0; j < int(roiList[i]["RoiPts"].size()); ++j)
                {
                    itemAlarm["RoiList"][i]["roiPts"][j]["ptX"] = roiList[i]["RoiPts"][j]["PtX"];
                    itemAlarm["RoiList"][i]["roiPts"][j]["ptY"] = roiList[i]["RoiPts"][j]["PtY"];
                }
            }
        }
    }

    if (pu)
    {
        itemAlarm["gucode"] = pu->gucode(inputNo);
        itemAlarm["name"] = pu->guname(inputNo);
    }



    wvalue["alarms"].append(itemAlarm);
    string jsonAlarm = writer.write(wvalue);


    //writeAlarmLog(jsonAlarm);

    //sendto c2
    notifyAlarm(jsonAlarm, NULL, 0);
    return jsonAlarm;
}

std::string UtIvaPu::getPresetState(const char *json, int size)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return "";
    }
    rvalue["Method"] = IVA_GETSTATE_PRESET;
    SHL_Json::FastWriter write;
    string jsonStr = write.write(rvalue);
    string res = "";
    if (m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
    else
    {
        return "";
    }
    return res;
}

std::string UtIvaPu::startanaylyse(const char *json, int size)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if(!reader.parse(json, rvalue, false))
    {
        return "";
    }
	rvalue["Method"] = IVA_START_IVA;
    SHL_Json::FastWriter write;
    string jsonStr = write.write(rvalue);
    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
    else
    {
        return "";
    }
    return res;
}

std::string UtIvaPu::stopanaylyse(const char *json, int size)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if(!reader.parse(json, rvalue, false))
    {
        return "";
    }
    rvalue["Method"] = IVA_STOP_IVA;
    SHL_Json::FastWriter write;
    string jsonStr = write.write(rvalue);
    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
    else
    {
        return "";
    }
    return res;
}

int UtIvaPu::queryAbility(string addrcode, string &xml)
{
    //int  channelNo = GetChanNoByCode(addrcode);

    Pu * pu = putree->findPu(addrcode);
    if (!pu)
    {
        return -1;
    }

    int channelNo = 0;
    if (pu->ip() != m_host)
    {
         channelNo = pu->guchannel(addrcode);
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["Version"] = "V1.00.00";
    value["DevIp"]   = pu->ip();
    value["DevChn"]  = channelNo;
    value["Method"]  = IVA_GETPARAM_ABILITY;

    string json = writer.write(value);
    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, json.c_str());
    }
    else
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(res, rvalue, false))
    {
        return -1;
    }
    int rtn = rvalue["RtnCode"].asInt();
    if(rtn == -1)
    {
        return -1;
    }
    SHL_Json::Value result;
    result["SIP_XML"]["@EventType"] = "Alg_Ability_Response";
    SHL_Json::Value arglist = rvalue["AlgList"];
    int algCnt = arglist.size();

    for (int index = 0; index < algCnt; ++index)
    {
        if(algCnt == 1)
        {
            result["SIP_XML"]["Item"]["@Code"] = addrcode;
            result["SIP_XML"]["Item"]["@Ability"] = std::to_string(arglist[index]["Ability"].asInt());
            result["SIP_XML"]["Item"]["@SubAbility"] = std::to_string(arglist[index]["SubAbility"].asInt());
        }
        else if(algCnt > 1)
        {
            result["SIP_XML"]["Item"][index]["@Code"] = addrcode;
            result["SIP_XML"]["Item"][index]["@Ability"] = std::to_string(arglist[index]["Ability"].asInt());
            result["SIP_XML"]["Item"][index]["@SubAbility"] = std::to_string(arglist[index]["SubAbility"].asInt());
        }
    }
    std::string data = writer.write(result);
    printf("%s", data.c_str());
    json2xml(data.c_str(), xml);
    return 0;
}
 
void T_print(string s)
{
    printf("\n----------------------------\n");
    char c[128] = { 0 };
    memcpy(c, s.c_str(), s.size());
    for (int i = 0; i < 128;++i)
    {
        if (c[i] == 0)
            break;
        printf("%x ", c[i]);
    }
    printf("\n----------------------------\n");
}

int UtIvaPu::taskDispatch(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
    SHL_Json::FastWriter      writer;
	SHL_Json::Value           value;
	int AnalyseExe = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseExe"].asString().c_str());
	string addrcode;
	if (!rvalue["SIP_XML"]["Item"]["@Code"].isNull())
	{
		addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
	}


	Pu * pPu = putree->findPu(addrcode);
	if (!pPu)		
	{
		ERRE_MSV("UtIvaPu::taskDispatch failed! putree->findPu(addrcode) failed:addrcode = %s", addrcode.c_str());
		return -1;
	}
	int  channelNo = pPu->guchannel(addrcode) ;
	value["DevIp"] = pPu->ip();
	value["DevChn"] = channelNo;

    if (m_lmtIp.empty())
    {
        ERRE_MSV("UtIvaPu::taskDispatch failed! reason:m_lmtIp is empty");
        return -1;
    }
    value["LmtIp"] = m_lmtIp;

	if (!rvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
	{
		value["DevPort"] = pPu->port();
	}



    value["Version"]          = "V1.00.00";

	value["TaskId"] = "0";
	if (!rvalue["SIP_XML"]["Item"]["@TaskId"].isNull())
	{
		value["TaskId"] = rvalue["SIP_XML"]["Item"]["@TaskId"].asString();
	}
    
	value["TaskName"]         = "test";
	if (!rvalue["SIP_XML"]["Item"]["@TaskName"].isNull())
	{
		value["TaskName"] = rvalue["SIP_XML"]["Item"]["@TaskName"].asString();
	}

	value["Scale"] = 1;

	if (!rvalue["SIP_XML"]["Item"]["@TaskPrj"].isNull())
	{
		value["TaskPrj"] = atoi(rvalue["SIP_XML"]["Item"]["@TaskPrj"].asString().c_str());
	}
        
    if(!rvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
    {
        value["PresetId"]     = atoi(rvalue["SIP_XML"]["Item"]["@PrePoint"].asString().c_str());
    }

    if(!rvalue["SIP_XML"]["Item"]["Area"].isArray())
    {
        if(!rvalue["SIP_XML"]["Item"]["Area"]["@PointNum"].isNull())
        {
            int pointNum = atoi(rvalue["SIP_XML"]["Item"]["Area"]["@PointNum"].asString().c_str());
            SHL_Json::Value  area;
			int RoiId = 0;
            for(int j=0; j<pointNum; j++)
            {
				int width = 1;
				int height = 1;
                int picSize = pPu->getPicSize(channelNo + 1);
				if (picSize < 0)
				{
					ERRE_MSV("UtIvaPu::taskDispatch failed! pPu->getPicSize() failed:pPu->addrcode = %s", pPu->addrcode().c_str());
					return -1;
				}
				switch (picSize)
				{
				case 3: width = 704; height = 576; break;//D1
				case 19:width = 1280; height = 720; break;//720p
				case 20:width = 1280; height = 960; break;//960p
				case 27:width = 1920; height = 1080; break;//1080P
				default:
					ERRE_MSV("UtIvaPu::taskDispatch failed! pPu->getPicSize() unknown :picSize = %d", picSize);
					return -1;
					break;
				}

				char xBuf[32];
				char yBuf[32];
				sprintf(xBuf, "@Point%d_X", j + 1);
				sprintf(yBuf, "@Point%d_Y", j + 1);
				SHL_Json::Value  pt;
				pt["PtX"] = (float)(atoi(rvalue["SIP_XML"]["Item"]["Area"][xBuf].asString().c_str())) / (float)width;
				pt["PtY"] = (float)(atoi(rvalue["SIP_XML"]["Item"]["Area"][yBuf].asString().c_str())) / (float)height;

                area["RoiPts"].append(pt);
				area["IsChnIva"] = 0;
				area["DualDirect"] = 1;

                
				if (AnalyseExe == 1)
				{
                    char rulename[] = "ʵʱ����";
                    area["RuleName"] = rulename;
				}
				else if (AnalyseExe == 2)
                {
                    char rulename[] = "����ִ��";

                    char rulename1[] = "����ִ��";
                    area["RuleName"] = rulename;

                    string test = sys->ascii2utf8(rulename1);

                    char test2[100] = { 0 };
                    string test11 = sys->utf2ascii(test.c_str());
                    strcpy(test2, test11.c_str());
                    int kk = 0;
				}

				area["AnalyseType"] = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseType"].asString().c_str());
				int AnalyseSubType = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseSubType"].asString().c_str());

				for (int k = 0; k < 32; ++k )
				{
					if (AnalyseSubType & ( 1 << k ))
					{
						area["RoiId"] = RoiId++;
						area["AnalyseSubType"] = k;
						value["RoiList"].append(area);
					}
				}
            }
        }
    }
    else
    {
        for(int i=0; i<int(rvalue["SIP_XML"]["Item"]["Area"].size()); i++)
        {
			if (!rvalue["SIP_XML"]["Item"]["Area"][i]["@PointNum"].isNull())
			{
				int pointNum = atoi(rvalue["SIP_XML"]["Item"]["Area"]["@PointNum"].asString().c_str());
				SHL_Json::Value  area;
				int RoiId = 0;
				for (int j = 0; j < pointNum; j++)
				{
					int width = 1;
					int height = 1;
                    int picSize = pPu->getPicSize(channelNo + 1);
					if (picSize < 0)
					{
						ERRE_MSV("UtIvaPu::taskDispatch failed! pPu->getPicSize() failed:pPu->addrcode = %s", pPu->addrcode().c_str());
						return -1;
					}
					switch (picSize)
					{
					case 3: width = 704; height = 576; break;//D1
					case 19:width = 1280; height = 720; break;//720p
					case 20:width = 1280; height = 960; break;//960p
					case 27:width = 1920; height = 1080; break;//1080P
					default:
						ERRE_MSV("UtIvaPu::taskDispatch failed! pPu->getPicSize() unknown :picSize = %d", picSize);
						return -1;
						break;
					}

					char xBuf[32];
					char yBuf[32];
					sprintf(xBuf, "@Point%d_X", j + 1);
					sprintf(yBuf, "@Point%d_Y", j + 1);
					SHL_Json::Value  pt;
					pt["PtX"] = (float)(atoi(rvalue["SIP_XML"]["Item"]["Area"][xBuf].asString().c_str())) / (float)width;
					pt["PtY"] = (float)(atoi(rvalue["SIP_XML"]["Item"]["Area"][yBuf].asString().c_str())) / (float)height;

					area["RoiPts"].append(pt);
					area["IsChnIva"] = 0;
					area["DualDirect"] = 1;

                    string rulename = "";

					if (AnalyseExe == 1)
					{
                        rulename = "??????";
                        area["RuleName"] = sys->ascii2utf8(rulename.c_str());
					}
					else if (AnalyseExe == 2)
					{
                        rulename = "???????";

                        string test = sys->ascii2utf8(rulename.c_str());

                        string test2 = sys->utf2ascii(test);

                        area["RuleName"] = sys->ascii2utf8(rulename.c_str());
					}

					area["AnalyseType"] = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseType"].asString().c_str());
					int AnalyseSubType = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseSubType"].asString().c_str());

					for (int k = 0; k < 32; ++k)
					{
						if (AnalyseSubType & (1 << k))
						{
							area["RoiId"] = RoiId++;
							area["AnalyseSubType"] = k;
							value["RoiList"].append(area);
						}
					}
				}
			}
        }
    }


    string req = writer.write(value);
    printf("ivaReqParam:%s", req.c_str());

    string utf8str = sys->ascii2utf8(req);

    std::string response = "";
	if (AnalyseExe == 0)
	{
        response = stopanaylyse(utf8str.c_str(), utf8str.size());
	}
    else
    {
        setParamPreset(utf8str.c_str(), utf8str.size());
        response = startanaylyse(utf8str.c_str(), utf8str.size());
    }

    SHL_Json::Value  resvalue;

    if (!reader.parse(response, resvalue, false))
    {
        return -1;
    }
    int rtncode = resvalue["RtnCode"].asInt();
    if(rtncode == -1)
        return -1;
    SHL_Json::Value result;
    result["SIP_XML"]["@EventType"] = "Alg_Task_Responses";
    result["SIP_XML"]["Item"]["@Desc"] = resvalue["StateMsg"].asString();
    result["SIP_XML"]["Item"]["@TaskId"] = resvalue["TaskId"].asString();
    result["SIP_XML"]["Item"]["@Response"] = std::to_string(resvalue["RtnCode"].asInt());;
    string resJson = writer.write(result);
    printf("ivaResponse:%s", resJson.c_str());
    json2xml(resJson.c_str(), xml);
    return 0;
}

int UtIvaPu::reqAllTask(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    //    int  channelNo = GetChanNoByCode(addrcode);
    SHL_Json::FastWriter      writer;
    SHL_Json::Value           wvalue;
    wvalue["Method"]   = IVA_GETLIST_PRESET;
    wvalue["Version"]  = "";
    if(!rvalue["SIP_XML"]["Task"]["@FromIndex"].isNull())
        wvalue["FromIndex"] = atoi(rvalue["SIP_XML"]["Task"]["@FromIndex"].asString().c_str());
    if(!rvalue["SIP_XML"]["Task"]["@ToIndex"].isNull())
        wvalue["ToIndex"]   = atoi(rvalue["SIP_XML"]["Task"]["@ToIndex"].asString().c_str());
    string reqJson = writer.write(wvalue);
    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, reqJson.c_str());
    }
    else
    {
        return -1;
    }
    SHL_Json::Value resValue;
    if(!reader.parse(res, resValue, false))
        return -1;
    SHL_Json::Value value;
    value["SIP_XML"]["@EventType"]               = "Responses_Alg_Task";
    value["SIP_XML"]["Task"]["@StationCode"]     = addrcode;
    value["SIP_XML"]["Task"]["@RealNum"]         = resValue["RealNum"].asString();
    if(!resValue["SubNum"].isNull())
        value["SIP_XML"]["Task"]["@SubNum"]      = resValue["SubNum"].asString();
    value["SIP_XML"]["Task"]["@FromIndex"]       = resValue["FromIndex"].asString();
    value["SIP_XML"]["Task"]["@ToIndex"]         = resValue["ToIndex"].asString();

    for(int i=0; i<int(resValue["TaskList"].size()); i++)
    {
        value["SIP_XML"]["Task"]["Item"][i]["@TaskId"]          = resValue["TaskList"][i]["TaskId"].asString();
        value["SIP_XML"]["Task"]["Item"][i]["@TaskName"]        = resValue["TaskList"][i]["TaskName"].asString();
        string ip = resValue["TaskList"][i]["DevIp"].asString();
        int channelNo  = resValue["TaskList"][i]["DevChn"].asInt();
        Pu *pu = putree->findPu(ip, 8000);
        if (!pu)
        {
            continue;
        }

        string chnaddrcode = pu->gucode(channelNo);;
        value["SIP_XML"]["Task"]["Item"][i]["@Code"]            = chnaddrcode;
        value["SIP_XML"]["Task"]["Item"][i]["@AnalyseType"]     = resValue["TaskList"][i]["AnalyseType"].asString();
        value["SIP_XML"]["Task"]["Item"][i]["@AnalyseSubType"]  = resValue["TaskList"][i]["AnalyseSubType"].asString();
        value["SIP_XML"]["Task"]["Item"][i]["@AnalyseExe"]      = resValue["TaskList"][i]["AnalyseExe"].asString();
        if(!resValue["TaskList"][i]["TaskPrj"].isNull())
            value["SIP_XML"]["Task"]["Item"][i]["@TaskPrj"]     = resValue["TaskList"][i]["TaskPrj"].asString();
        if(!resValue["TaskList"][i]["PrePoint"].isNull())
            value["SIP_XML"]["Task"]["Item"][i]["@PrePoint"]    = to_string(resValue["TaskList"][i]["PrePoint"].asInt());
        if(!resValue["TaskList"][i]["RoiList"].isNull())
        {
            int areaCnt          = resValue["TaskList"][i]["RoiList"].size();
            value["SIP_XML"]["Task"]["Item"][i]["@AreaNum"]     = to_string(areaCnt);
            for(int j=0; j<areaCnt; j++)
            {
                if(!resValue["TaskList"][i]["RoiList"][j]["RoiPts"].isNull())
                {
                    for(int k=0; k<int(resValue["TaskList"][i]["RoiList"][j]["RoiPts"].size()); k++)
                    {
                        if(areaCnt == 1)
                        {
                            if(!resValue["TaskList"][i]["RoiList"][j]["RoiPts"].isNull())
                            {
                                value["SIP_XML"]["Task"]["Item"][i]["Area"]["@PointNum"]    = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"].size());
                                char ptx[32] = {0};
                                char pty[32] = {0};
                                sprintf(ptx, "@Point%d_X", k+1);
                                sprintf(pty, "@Point%d_Y", k+1);
                                value["SIP_XML"]["Task"]["Item"][i]["Area"][ptx] = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"][k]["PtX"].asInt());
                                value["SIP_XML"]["Task"]["Item"][i]["Area"][pty] = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"][k]["PtY"].asInt());
                            }
                        }
                        else if(areaCnt > 1)
                        {
                            if(!resValue["TaskList"][i]["RoiList"][j]["RoiPts"].isNull())
                            {
                                value["SIP_XML"]["Task"]["Item"]["Area"][j]["@PointNum"]    = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"].size());
                                char ptx[32] = {0};
                                char pty[32] = {0};
                                sprintf(ptx, "@Point%d_X", k+1);
                                sprintf(pty, "@Point%d_Y", k+1);
                                value["SIP_XML"]["Task"]["Item"][i]["Area"][j][ptx] = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"][k]["PtX"].asInt());
                                value["SIP_XML"]["Task"]["Item"][i]["Area"][j][pty] = to_string(resValue["TaskList"][i]["RoiList"][j]["RoiPts"][k]["PtY"].asInt());
                            }
                        }
                    }
                }
            }
        }
    }
    std::string result = writer.write(value);
    json2xml(result.c_str(), xml);
    printf("%s",result.c_str());
    return 0;
}

int UtIvaPu::reqSignalTask(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    Pu * pu = putree->findPu(addrcode);
    if (!pu)
    {
        return -1;
    }
	int  channelNo = GetChanNoByCode(addrcode);
    SHL_Json::FastWriter      writer;
    SHL_Json::Value           wvalue;
    wvalue["Method"]    = IVA_GETPARAM_PRESET;
    wvalue["Version"]   = "";
    wvalue["DevIp"]     = pu->ip();
    wvalue["DevChn"]    = channelNo;
    if(!rvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
        wvalue["PrePoint"]  = atoi(rvalue["SIP_XML"]["Item"]["@PrePoint"].asString().c_str());
    if(!rvalue["SIP_XML"]["Item"]["@FromIndex"].isNull())
        wvalue["FromIndex"] = atoi(rvalue["SIP_XML"]["Item"]["@FromIndex"].asString().c_str());
    if(!rvalue["SIP_XML"]["Item"]["@ToIndex"].isNull())
        wvalue["ToIndex"]   = atoi(rvalue["SIP_XML"]["Item"]["@ToIndex"].asString().c_str());
    string reqJson = writer.write(wvalue);
    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, reqJson.c_str());
    }
    else
    {
        return -1;
    }
    SHL_Json::Value           resValue;
    if(!reader.parse(res, resValue, false))
    {
        return -1;
    }
    SHL_Json::Value value;
    value["SIP_XML"]["@EventType"] = "Alg_Task_Responses";
    value["SIP_XML"]["Item"]["@Code"]                = addrcode;
    value["SIP_XML"]["Item"]["@RealNum"]             = resValue["RealNum"].asString();
    if(!resValue["SubNum"].isNull())
        value["SIP_XML"]["Item"]["@SubNum"]          = resValue["SubNum"].asString();
    if(!resValue["FromIndex"].isNull())
        value["SIP_XML"]["Item"]["@FromIndex"]       = resValue["FromIndex"].asString();
    if(!resValue["ToIndex"].isNull())
        value["SIP_XML"]["Item"]["@ToIndex"]         = resValue["ToIndex"].asString();
    //if(!resValue["AreaNum"].isNull())
    {
        int areaCnt                                  = resValue["RoiList"].size();
        value["SIP_XML"]["Item"]["@AreaNum"]         = areaCnt;
        for(int i=0; i<areaCnt; i++)
        {
            if(areaCnt == 1)
            {
                if(!resValue["RoiList"][i]["RoiPts"].isNull())
                {
                    value["SIP_XML"]["Item"]["Area"]["@PointNum"]    = resValue["RoiList"][i]["RoiPts"].size();
                    for(int j=0; j<int(resValue["RoiList"][i]["RoiPts"].size()); j++)
                    {
                        char ptx[32] = {0};
                        char pty[32] = {0};
                        sprintf(ptx, "@Point%d_X", j+1);
                        sprintf(pty, "@Point%d_Y", j+1);
                        value["SIP_XML"]["Item"]["Area"][ptx] = resValue["RoiList"][i]["RoiPts"][j]["PtX"];
                        value["SIP_XML"]["Item"]["Area"][pty] = resValue["RoiList"][i]["RoiPts"][j]["PtY"];
                    }
                }
            }
            else if(areaCnt > 1)
            {
                if(!resValue["RoiList"][i]["RoiPts"].isNull())
                {
                    value["SIP_XML"]["Item"]["Area"][i]["@PointNum"]    = resValue["RoiList"][i]["RoiPts"].size();
                    for(int j=0; j<int(resValue["RoiList"][i]["RoiPts"].size()); j++)
                    {
                        char ptx[32] = {0};
                        char pty[32] = {0};
                        sprintf(ptx, "@Point%d_X", j+1);
                        sprintf(pty, "@Point%d_Y", j+1);
                        value["SIP_XML"]["Item"]["Area"][i][ptx] = resValue["RoiList"][i]["RoiPts"][j]["PtX"];
                        value["SIP_XML"]["Item"]["Area"][i][pty] = resValue["RoiList"][i]["RoiPts"][j]["PtY"];
                    }
                }
            }
        }
    }
    std::string result = writer.write(value);
    json2xml(result.c_str(), xml);
    printf("%s",result.c_str());
    return 0;
}

int UtIvaPu::cameraSnap(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
    SHL_Json::FastWriter      writer;
    SHL_Json::Value           wvalue;
    wvalue["Version"]          = "V1.00.00";
    wvalue["Method"]           = IVA_SETPARAM_CAPIM ;
    if(rvalue["SIP_XML"]["Item"].isArray())
    {
        for(int i=0; i<int(rvalue["SIP_XML"]["Item"].size()); i++)
        {
            string addrcode = rvalue["SIP_XML"]["Item"][i]["@Code"].asString();
            Pu * pu = putree->findPu(addrcode);
            if (!pu)
            {
                return -1;
            }
			int  channelNo = GetChanNoByCode(addrcode);
            SHL_Json::Value value;
            value["DevIp"]            = pu->ip();
            value["DevChn"]           = channelNo;
            value["Code"]             = rvalue["SIP_XML"]["Item"][i]["@Code"].asString();
            value["PicServer"]        = rvalue["SIP_XML"]["Item"][i]["@PicServer"].asString();
            value["SnapType"]         = atoi(rvalue["SIP_XML"]["Item"][i]["@SnapType"].asString().c_str());
            if(!rvalue["SIP_XML"]["Item"][i]["@Range"].isNull())
                value["Range"]        = rvalue["SIP_XML"]["Item"][i]["@Range"].asString();
            if(!rvalue["SIP_XML"]["Item"][i]["@Interval"].isNull())
                value["Interval"]     = atoi(rvalue["SIP_XML"]["Item"][i]["@Interval"].asString().c_str());
            wvalue["DevList"].append(value);
        }
    }
    else
    {
        string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
        Pu * pu = putree->findPu(addrcode);
        if (!pu)
        {
            return -1;
        }
		int  channelNo = GetChanNoByCode(addrcode);
        SHL_Json::Value value;
        value["DevIp"]            = pu->ip();
        value["DevChn"]           = channelNo;
        value["Method"]           = IVA_SETPARAM_CAPIM ;
        value["Code"]             = rvalue["SIP_XML"]["Item"]["@Code"].asString();
        value["PicServer"]        = rvalue["SIP_XML"]["Item"]["@PicServer"].asString();
        value["SnapType"]         = atoi(rvalue["SIP_XML"]["Item"]["@SnapType"].asString().c_str());
        if(!rvalue["SIP_XML"]["Item"]["@Range"].isNull())
            value["Range"]        = rvalue["SIP_XML"]["Item"]["@Range"].asString();
        if(!rvalue["SIP_XML"]["Item"]["@Interval"].isNull())
            value["Interval"]     = atoi(rvalue["SIP_XML"]["Item"]["@Interval"].asString().c_str());
        wvalue["DevList"].append(value);
    }

    string res ="";
    if(m_login)
    {
        res = NET_IvaRequestCmd(m_Cnn, writer.write(wvalue).c_str());
    }
    else
    {
        return -1;
    }

    return 0;
}

#include <stdio.h>

void UtIvaPu::AlarmCallback(const char* strJson, void *pUser)
{
    UtIvaPu *pIvasdk = static_cast<UtIvaPu *>(pUser);
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(strJson, rvalue, false))
    {
        return;
    }

    SHL_Json::Value  alarmValue;

    char buf[1024] = {0};
    FILE * file1 = fopen("city.txt", "r+");
    if (file1)
    {
        fread(buf, 1, 1024, file1);
        fclose(file1);
    }
    else
    {
        sprintf(buf,"%s","{\"City\":\"zhuhai\",\"TransformerSubstation\":\"110KvStation\"}");
    }
    SHL_Json::Value  city;
    if(!reader.parse(buf, city, false))
    {
        return;
    }

    int type = rvalue["Method"].asInt();
    switch (type)
    {
    case IVA_ALARM_RESULT:
    {
        SHL_Json::FastWriter writer;
		string taskid = rvalue["TaskId"].asString();
		if ("" != taskid)
        {
            SHL_Json::Value item;
            item["City"] = city["City"].asString();
            item["TransformerSubstation"] = city["TransformerSubstation"].asString();
            item["TaskId"] = rvalue["TaskId"].asString();
            item["AnalyseResult"] = rvalue["AlarmMsg"].asString();
            item["AnalyseTime"] = rvalue["AlarmTime"].asString();
            item["AlarmState"] = rvalue["RtnCode"].asString();
            item["AnalyseDesc"] = rvalue["AlarmDesc"].asString();
            int analyseType = atoi(rvalue["AnalyseType"].asString().c_str());
            if(analyseType==SUPER_TEMP_DIFF || analyseType==SUPER_TEMP_CHANGE)
            {
                item["InfraredPic"] = rvalue["AlarmImInfrared"].asString();
                item["VisiblePic"] = rvalue["AlarmImUrl"].asString();
            }
            if(analyseType!=SUPER_TEMP_DIFF && analyseType!=SUPER_TEMP_CHANGE)
            {
                item["AnalysePictureUrl"] = rvalue["AlarmImUrl"].asString();
            }
            SHL_Json::Value value;
            value["alarms"].append(item);
            string json = writer.write(value);
            printf("alarm:%s",json.c_str());

            //send to bup
            pIvasdk->notifyYX(json, NULL, 0);
        }

        alarmValue["version"]    = "1.0";
        alarmValue["time"]       = sys->currentTime();

        alarmValue["deviceName"] = "";
        alarmValue["deviceHost"] = "";
        alarmValue["devicePort"] = 0;
        alarmValue["addrcode"]   = "";
        alarmValue["level"]      = 1;

        string devIp   = rvalue["DevIp"].asString();
        int    devPort = rvalue["DevPort"].asInt();
        Pu *pu = putree->findPu(devIp.c_str(), devPort);
        if (pu)
        {
            alarmValue["deviceName"] = pu->name();
            alarmValue["deviceHost"] = pu->host();
            alarmValue["devicePort"] = pu->port();
            alarmValue["addrcode"]   = pu->addrcode();
        }

        SHL_Json::Value itemAlarm;

        int AnalyseType    = rvalue["AnalyseType"].asInt();
        int AnalyseSubType = rvalue["AnalyseSubType"].asInt();
  //      if (!pIvasdk->m_bPTC_B)
		//{
        AnalyseSubType =int(pow(2, AnalyseSubType));
		//}


        int type = ivaTypeCast(AnalyseType, AnalyseSubType);

        int inputNo = rvalue["DevChn"].asInt();
        itemAlarm["type"]        = type;
        itemAlarm["channelNo"]   = inputNo;
        itemAlarm["status"]      = rvalue["AlarmMsg"].asString();
        itemAlarm["gucode"]      = "";
        itemAlarm["name"]        = "";
        itemAlarm["preset"]      = rvalue["PresetId"].asInt();
        itemAlarm["alarmdesc"]   = rvalue["AlarmDesc"].asString();
        itemAlarm["roiId"]       = rvalue["RoiId"].asInt();
        itemAlarm["picName"]     = rvalue["AlarmImUrl"].asString();
        if (rvalue["RoiPts"].isArray())
        {
            for (int i = 0; i < int(rvalue["RoiPts"].size());++i)
            {
                itemAlarm["roiPts"][i]["ptX"] = rvalue["RoiPts"][i]["PtX"];
                itemAlarm["roiPts"][i]["ptY"] = rvalue["RoiPts"][i]["PtY"];
            }
        }

        if (pu)
        {
            itemAlarm["gucode"]  = pu->gucode(inputNo);
            itemAlarm["name"]    = pu->guname(inputNo);
        }

        alarmValue["alarms"].append(itemAlarm);
        string jsonAlarm = writer.write(alarmValue);


        pIvasdk->writeAlarmLog(jsonAlarm);

        //sendto c2
        pIvasdk->notifyAlarm(jsonAlarm, NULL, 0);
    }
        break;
    case IVA_ALARM_LMTLIST:
    //{
    //    if (rvalue["IvaServer"].isNull() ||
    //        rvalue["StateMsg"].isNull()
    //        )
    //        break;
    //    string lmt_IvaServer = rvalue["IvaServer"].asString();
    //}
        break;
    case IVA_ALARM_STATE:
        break;
    case IVA_ALARM_TRACK:
        break;
    default:
        break;
    }
}

void UtIvaPu::LoginCallback(const char* strJson, void *pUser)
{
    UtIvaPu *pIvasdk = static_cast<UtIvaPu *>(pUser);
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(strJson, rvalue))
    {
        return;
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"]    = "1.0";
    value["time"]       = sys->currentTime();
    value["deviceName"] = pIvasdk->name();
    value["deviceHost"] = pIvasdk->host();
    value["devicePort"] = pIvasdk->port();

    int ret = rvalue["RtnCode"].asInt();
    switch (ret)
    {
    case 0:
    {
        pIvasdk->m_login = true;

        pIvasdk->writeDevLog(STR_MSV_RE_ON_LINE);

        SHL_Json::Value item;
        item["type"]   = TriggerListener::TR_DEV_STATUS;
        item["status"] = 1;
        item["name"]   = pIvasdk->name();
        item["code"]   = pIvasdk->addrcode();
        value["alarms"].append(item);

        string json = writer.write(value);
        pIvasdk->notifyAlarm(json, NULL, 0);

        break;
    }
    case -1:
    {
        if (!pIvasdk->m_login)
        {
            return;
        }
        pIvasdk->m_login = false;

        pIvasdk->writeDevLog(STR_MSV_OFF_LINE);

        SHL_Json::Value item;
        item["type"]   = TriggerListener::TR_DEV_STATUS;
        item["status"] = 0;
        item["name"]   = pIvasdk->name();
        item["code"]   = pIvasdk->addrcode();
        value["alarms"].append(item);

        string json = writer.write(value);
        pIvasdk->notifyAlarm(json, NULL, 0);

        break;
    }
    default:
        break;
    }
}

void                
UtIvaPu::sendLmtInfo()
{

    queryIvaLmt();
    if (!m_lmtIp.empty())
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        rvalue["Method"] = IVA_SETPARAM_LMT;
        rvalue["Version"] = "V1.00.00";
        rvalue["LmtIp"] = m_lmtIp;
        rvalue["LmtPort"] = m_lmtPort;
        rvalue["UserName"] = m_lmtUser;
        rvalue["UserPwd"] = m_lmtPsaawd;

        SHL_Json::FastWriter write;
        string jsonStr = write.write(rvalue);

        string res = NET_IvaRequestCmd(m_Cnn, jsonStr.c_str());
    }
}

void       
UtIvaPu::onTimer()
{
	//static time_t tmPrev = time(NULL);
	//time_t tmNow = time(NULL);

	//if (m_login && (tmNow-tmPrev >= 30))
	//{
	//	tmPrev = tmNow;
 //       sendLmtInfo();
	//}

	Pu::onTimer();
}

void
UtIvaPu::ivaTypeCast(const int type, int &AnalyseType, int &AnalyseSubType)
{
    switch (type)
    {
    case TriggerListener::TR_PB_EXCEEDHEIGHT: AnalyseType = 2; AnalyseSubType = 8; break;
    case TriggerListener::TR_AI_TRIPWIRE: AnalyseType = 6; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_PERIMETER: AnalyseType = 6; AnalyseSubType = 2; break;
    case TriggerListener::TR_AI_HOVER: AnalyseType = 2; AnalyseSubType = 32; break;
    case TriggerListener::TR_AI_ITEMLEFT: AnalyseType = 5; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_ANTITHEFT: AnalyseType = 5; AnalyseSubType = 2; break;
    case TriggerListener::TR_AI_INDICATOR: AnalyseType = 13; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_DISCONNECTOR: AnalyseType = 12; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_HELMET: AnalyseType = 1; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_JUMPER: AnalyseType = 1; AnalyseSubType = 4; break;
    case TriggerListener::TR_AI_METER: AnalyseType = 11; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_LEDDIGIT: AnalyseType = 1; AnalyseSubType = 2; break;
    case TriggerListener::TR_PERSION_TOGETHER: AnalyseType = 2; AnalyseSubType = 16; break;
    case TriggerListener::TR_PERMETER_PERSON: AnalyseType = 3; AnalyseSubType = 1; break;
    case TriggerListener::TR_PERMETER_CAR: AnalyseType = 3; AnalyseSubType = 2; break;
    case TriggerListener::TR_PERMETER_NOCAR: AnalyseType = 3; AnalyseSubType = 4; break;
    case TriggerListener::TR_PERMETER_OBJECT: AnalyseType = 3; AnalyseSubType = 8; break;
    case TriggerListener::TR_CAR_NOIN: AnalyseType = 7; AnalyseSubType = 1; break;
    case TriggerListener::TR_CAR_NOSTOP: AnalyseType = 7; AnalyseSubType = 2; break;
    case TriggerListener::TR_PB_FALLDOWN: AnalyseType = 2; AnalyseSubType = 1; break;
    case TriggerListener::TR_PB_DAJIA: AnalyseType = 2; AnalyseSubType = 4; break;
    case TriggerListener::TR_PB_SMOKE: AnalyseType = 2; AnalyseSubType = 2; break;
    case TriggerListener::TR_FIRE_CONTROL: AnalyseType = 4; AnalyseSubType = 1; break;
    case TriggerListener::TR_TEMP_AREA: AnalyseType = 8; AnalyseSubType = 1; break;
    case TriggerListener::TR_TEMP_DIFF: AnalyseType = 9; AnalyseSubType = 1; break;
    case TriggerListener::TR_TEMP_BODONG: AnalyseType = 10; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_SIGNAL_COLORCAST: AnalyseType = 15; AnalyseSubType = 1; break;
    case TriggerListener::TR_AI_SIGNAL_STRIPENOISE: AnalyseType = 15; AnalyseSubType = 2; break;
    case TriggerListener::TR_AI_SIGNAL_CONTRASTLOW: AnalyseType = 15; AnalyseSubType = 4; break;
    case TriggerListener::TR_AI_SIGNAL_VIDEOSHAKE: AnalyseType = 15; AnalyseSubType = 8; break;
    case TriggerListener::TR_AI_SIGNAL_LOST: AnalyseType = 15; AnalyseSubType = 16; break;
    case TriggerListener::TR_AI_SIGNAL_IMAGEFREEZE: AnalyseType = 15; AnalyseSubType = 32; break;
    case TriggerListener::TR_AI_SIGNAL_FUZZY: AnalyseType = 15; AnalyseSubType = 64; break;
        //case TriggerListener::TR_AI_SIGNAL_CONTRASTLOW: AnalyseType = 15; AnalyseSubType = 128; break;
    case TriggerListener::TR_AI_SIGNAL_NOISEJAMMING: AnalyseType = 15; AnalyseSubType = 256; break;
    case TriggerListener::TR_AI_SIGNAL_BLACKWHITEIMAGE: AnalyseType = 15; AnalyseSubType = 512; break;
    case TriggerListener::TR_AI_SIGNAL_VIDEOOCCLUSION: AnalyseType = 15; AnalyseSubType = 1024; break;
    case TriggerListener::TR_AI_SIGNAL_VIDEOUPHEAVAL: AnalyseType = 15; AnalyseSubType = 2048; break;
    case TriggerListener::TR_AI_SIGNAL_SCENECHANGE: AnalyseType = 15; AnalyseSubType = 4096; break;
    case TriggerListener::TR_WAIGUAN_MINPAI: AnalyseType = 14; AnalyseSubType = 1; break;
    case TriggerListener::TR_WAIGUAN_JUEYUAN: AnalyseType = 14; AnalyseSubType = 2; break;
    case TriggerListener::TR_WAIGUAN_TAOGUAN: AnalyseType = 14; AnalyseSubType = 4; break;
    case TriggerListener::TR_WAIGUAN_BIANYABT: AnalyseType = 14; AnalyseSubType = 8; break;
    case TriggerListener::TR_WAIGUAN_BILEIZHENG: AnalyseType = 14; AnalyseSubType = 16; break;
    case TriggerListener::TR_WAIGUAN_PATI: AnalyseType = 14; AnalyseSubType = 32; break;
    case TriggerListener::TR_WAIGUAN_QIANGTI: AnalyseType = 14; AnalyseSubType = 64; break;
    case TriggerListener::TR_WAIGUAN_GANTANG: AnalyseType = 14; AnalyseSubType = 128; break;
    case TriggerListener::TR_FACE_DETECTTION: AnalyseType = 0; AnalyseSubType = 1; break;
    //ol;pppp'[pcase TriggerListener::TR_AI_PERIMETER: AnalyseType = 0; AnalyseSubType = 1024; break;

    default:
        break;
    }
}

int                 
UtIvaPu::ivaTypeCast(int AnalyseType, int AnalyseSubType)
{
    int type = 0;
    if (2 == AnalyseType && 8 == AnalyseSubType)
    {
        type = TriggerListener::TR_PB_EXCEEDHEIGHT;
    }
    else if (6 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_TRIPWIRE;
    }
    else if (6 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_PERIMETER;
    }
    else if (2 == AnalyseType && 32 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_HOVER;
    }
    else if (5 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_ITEMLEFT;
    }
    else if (5 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_ANTITHEFT;
    }
    else if (13 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_INDICATOR;
    }
    else if (12 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_DISCONNECTOR;
    }
    else if (1 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_HELMET;
    }
    else if (1 == AnalyseType && 4 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_JUMPER;
    }
    else if (11 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_METER;
    }
    else if (11 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_LEDDIGIT;
    }
    else if (2 == AnalyseType && 16 == AnalyseSubType)
    {
        type = TriggerListener::TR_PERSION_TOGETHER;
    }
    else if (3 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_PERMETER_PERSON;
    }
    else if (3 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_PERMETER_CAR;
    }
    else if (3 == AnalyseType && 4 == AnalyseSubType)
    {
        type = TriggerListener::TR_PERMETER_NOCAR;
    }
    else if (3 == AnalyseType && 8 == AnalyseSubType)
    {
        type = TriggerListener::TR_PERMETER_OBJECT;
    }
    else if (7 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_CAR_NOIN;
    }
    else if (7 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_CAR_NOSTOP;
    }
    else if (2 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_PB_FALLDOWN;
    }
    else if (2 == AnalyseType && 4 == AnalyseSubType)
    {
        type = TriggerListener::TR_PB_DAJIA;
    }
    else if (2 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_PB_SMOKE;
    }
    else if (4 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_FIRE_CONTROL;
    }
    else if (8 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_TEMP_AREA;
    }
    else if (9 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_TEMP_DIFF;
    }
    else if (10 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_TEMP_BODONG;
    }
    else if (15 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_COLORCAST;
    }
    else if (15 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_STRIPENOISE;
    }
    else if (15 == AnalyseType && 4 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_CONTRASTLOW;
    }
    else if (15 == AnalyseType && 8 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_VIDEOSHAKE;
    }
    else if (15 == AnalyseType && 16 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_LOST;
    }
    else if (15 == AnalyseType && 32 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_IMAGEFREEZE;
    }
    else if (15 == AnalyseType && 64 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_FUZZY;
    }
    else if (15 == AnalyseType && 128 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_CONTRASTLOW;
    }
    else if (15 == AnalyseType && 256 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_NOISEJAMMING;
    }
    else if (15 == AnalyseType && 512 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_BLACKWHITEIMAGE;
    }
    else if (15 == AnalyseType && 1024 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_VIDEOOCCLUSION;
    }
    else if (15 == AnalyseType && 2048 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_VIDEOUPHEAVAL;
    }
    else if (15 == AnalyseType && 4096 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_SIGNAL_SCENECHANGE;
    }
    else if (14 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_MINPAI;
    }
    else if (14 == AnalyseType && 2 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_JUEYUAN;
    }
    else if (14 == AnalyseType && 4 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_TAOGUAN;
    }
    else if (14 == AnalyseType && 8 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_BIANYABT;
    }
    else if (14 == AnalyseType && 16 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_BILEIZHENG;
    }
    else if (14 == AnalyseType && 32 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_PATI;
    }
    else if (14 == AnalyseType && 64 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_QIANGTI;
    }
    else if (14 == AnalyseType && 128 == AnalyseSubType)
    {
        type = TriggerListener::TR_WAIGUAN_GANTANG;
    }
    else if (0 == AnalyseType && 1 == AnalyseSubType)
    {
        type = TriggerListener::TR_FACE_DETECTTION;
    }
    else if (0 == AnalyseType && 1024 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_PERIMETER;
    }
    else if (0 == AnalyseType && 128 == AnalyseSubType)
    {
        type = TriggerListener::TR_AI_OFFJOB;
    }
    return type;
}
