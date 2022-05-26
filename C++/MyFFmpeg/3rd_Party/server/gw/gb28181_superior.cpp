#include "gb28181_superior.h"
#include "sipua.h"
#include "putree.h"
#include "platform.h"
#include "cfg_device.h"
#include "j2x.h"
#include "cfg_sup_platform.h"
#include "pu.h"
#include "system.h"
#include "cfg_camera_belong_platform.h"
#include "cfg_sub_platform.h"
#include "cfg_catalog_info.h"
#include "cfg_preset.h"
#include "gsessmgr.h"
#include "gb28181_sipua.h"
#include "json/json.h"

using namespace std;
struct GB28181Superior::Param_T
{
    Thread *m_thread;
    void *m_pThis;
    string m_addrcode;
    Param_T(Thread *thread, void *pThis, string addrcode)
        :m_thread(thread),  m_pThis(pThis), m_addrcode(addrcode){
        ;
    }
};

struct GB28181Superior::ParamSource_T : public Param_T{
    string m_josn;
    ParamSource_T(Thread *thread, void *pThis, string addrcode, string json) 
        :Param_T(thread, pThis, addrcode), m_josn(json){
        ;
    }
};

GB28181Superior::GB28181Superior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauslocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
    m_alramtype = 0;
}


GB28181Superior::~GB28181Superior()
{
}


int
GB28181Superior::start()
{
    Superior::start();
    m_keekaliveThread.setEntry(threadProc, this, T_KEEPALIVE);
    int ret  = m_keekaliveThread.start();
    return ret;
}

int
GB28181Superior::on_RecordInfo(const string &json, string &xml)
{
    Thread *thread = new Thread;
    ParamSource_T *param = new ParamSource_T(thread, this, addrcode(), json);
    thread->setEntry(threadProcEx, param, T_RECORD);
    thread->start();
    thread->detach();

    return 0;
}

int        
GB28181Superior::on_resource(const string &json, string &xml)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}
	string cmdType = rvalue["Query"]["CmdType"].asString();
	T_RESOURCE resource_type;
	if ("Catalog" == cmdType)
	{
        if (GetCatalogType() == "catalog_district")
            resource_type = T_CATALOG_DISTRICT;
        else
            resource_type = T_CATALOG_BUSINESS;
	}
	else if ("PresetQuery" == cmdType)
	{
		resource_type = T_PRESET;
    }
	else
	{
		return -1;
	}
    Thread *thread = new Thread;
    ParamSource_T *param = new ParamSource_T(thread, this, addrcode(), json);
    thread->setEntry(threadProcEx, param, resource_type);
    thread->start();
    thread->detach();

    return 0;
}

int
GB28181Superior::push_mediaStatus(const string &xml)
{

    //SHL_Json::StreamWriterBuilder writer;
    //SHL_Json::Value               wvalue;


    //wvalue["Notify"]["CmdType"] = "MediaStatus";
    //wvalue["Notify"]["SN"] = to_string(GenerateSN());
    //wvalue["Notify"]["DeviceID"] = gucode;
    //wvalue["Notify"]["NotifyType"] = strType;

    //string xml, jresp;
    //json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    //sipUA->message(platform->addrcode(), platform->ip(), platform->sport(), m_addrcode, fromh, fromp, gbxml, gbxml);

    return -1;
}

int        
GB28181Superior::on_CatalogQuery_District(const string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  rvalueDev;
    SHL_Json::Value  rvalueCamBep;

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;
    SHL_Json::FastWriter fastwriter;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["Query"]["DeviceID"].asString();
    string SN = rvalue["Query"]["SN"].asString();
    int    SNN = atoi(SN.c_str());
    string fromh = rvalue["ip"].asString();
    int fromp = rvalue["port"].asInt();

    if (SNN < 1)
    {
        LOGE_SIP("invalidate args, SN: %s,SN minInclusivevalue == 1", SN.c_str());
        return -1;
    }


    //2.cache device info
    map<string, int> mapDev;
    string jsonDev = "";
    int ret = deviceCfg->query(jsonDev);
    if (0 == ret && jsonDev != "")
    {
        LOGE_SIP("deviceCfg->query(jsonDev) failed! \n");

        if (!reader.parse(jsonDev, rvalueDev, false))
        {
            LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonDev.c_str());
            return -1;
        }
        for (int i = 0; i < rvalueDev.size(); ++i)
        {
            if (!rvalueDev[i]["addrcode"].isNull())
            {
                mapDev[rvalueDev[i]["addrcode"].asString()] = i;
            }
        }
    }


    if (systm->addrcode() == addrcode)
    {
        //find cam_belong_platform
        char cond[256] = { 0 };
        snprintf(cond, 256, "platform_addrcode='%s';", m_addrcode.c_str());
        string jsonCamBeP = "";
        ret = cameraBelongPlatformCfg->query(jsonCamBeP, cond);
        if (0 != ret)
        {
            LOGE_SIP("superCfg->query(jsonCamBeP) failed! \n");
            return -1;
        }
        if (!reader.parse(jsonCamBeP, rvalueCamBep, false))
        {
            LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonCamBeP.c_str());
            return -1;
        }

        snprintf(cond, 256, "platform_addrcode='%s';", m_addrcode.c_str());
        int realNum = cameraBelongPlatformCfg->count(cond);

        int count = 0;
        int sendIndex = 0;
        for (int i = sendIndex; i < rvalueCamBep.size(); ++i)
        {
            string camAddr = "";
            if (!rvalueCamBep[i]["camera_addrcode"].isNull() /*&& !rvalueDev[mapDev[camAddr]]["name"].isNull()*/)
            {
                camAddr = rvalueCamBep[i]["camera_addrcode"].asString();

                wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = camAddr;//rvalueDev[mapDev[camAddr]]["addrcode"].asString();
                wvalue["Response"]["DeviceList"]["Item"][count]["Status"] = "ON";
                wvalue["Response"]["DeviceList"]["Item"][count]["Manufacturer"] = "Manufacturer";
                wvalue["Response"]["DeviceList"]["Item"][count]["Model"] = "Camera";
                wvalue["Response"]["DeviceList"]["Item"][count]["Owner"] = "Owner";
                wvalue["Response"]["DeviceList"]["Item"][count]["CivilCode"] = "CivilCode";
                wvalue["Response"]["DeviceList"]["Item"][count]["Parental"] = "0";
                wvalue["Response"]["DeviceList"]["Item"][count]["SafetyWay"] = "0";
                wvalue["Response"]["DeviceList"]["Item"][count]["RegisterWay"] = "1";
                wvalue["Response"]["DeviceList"]["Item"][count]["Secrecy"] = "0";
                wvalue["Response"]["DeviceList"]["Item"][count]["Longitude"] = "0.000";
                wvalue["Response"]["DeviceList"]["Item"][count]["Latitude"] = "0.000";				
				wvalue["Response"]["DeviceList"]["Item"][count]["Info"]["PTZType"] = 1;


                map<string, int>::iterator it = mapDev.find(camAddr);
                if (it != mapDev.end())
                {
                    //local camera
                    wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = rvalueDev[mapDev[camAddr]]["name"].asString();
                }
                else
                {
                    //sub camera
                    do
                    {
                        string subCamJson = "";
                        if (0 == suberCamInfoCfg->query(subCamJson, rvalueCamBep[i]["remark"].asString()))
                        {
                            SHL_Json::Value subCamValue;
                            bool retPar = false;
                            if ("" != subCamJson && (retPar = reader.parse(subCamJson, subCamValue, false)))
                            {
                                bool bBreak = false;
                                for (int j = 0; j < subCamValue.size(); ++j)
                                {
                                    if (subCamValue[j]["camera_addrcode"].asString() == camAddr)
                                    {
                                        wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = subCamValue[j]["camera_name"];
                                        bBreak = true;
                                        break;
                                    }
                                }
                                if (bBreak)
                                    break;
                            }
                        }
                        wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = camAddr;
                    } while (0);
                }
                ++sendIndex;
                ++count;
            }
            if (count == UT_SUPERIOR_PR_NUM)
            {
                wvalue["Response"]["CmdType"] = "Catalog";
                wvalue["Response"]["SN"] = SN;
                wvalue["Response"]["DeviceID"] = addrcode;
                wvalue["Response"]["SumNum"] = std::to_string(realNum);
                wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

                string xml;
                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
                //remove_copy(xml.begin(), xml.end(), back_inserter(xml2), '\t');
#ifdef WIN32
            string gbxml = sys->utf2ascii(xml);
#else
            string gbxml = sys->utf2ascii(xml);
#endif
                sys->StrFindReplace(gbxml, "utf-8", "gb2312");
                sipUA->message(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, fromh, fromp, gbxml, gbxml);

                wvalue.clear();
                count = 0;
            }
        }
        if (count > 0)
        {
            wvalue["Response"]["CmdType"] = "Catalog";
            wvalue["Response"]["SN"] = SN;
            wvalue["Response"]["DeviceID"] = addrcode;
            wvalue["Response"]["SumNum"] = std::to_string(realNum);
            wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

            string xml;
            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
            //remove_copy(xml.begin(), xml.end(), back_inserter(xml2), '\t');
#ifdef WIN32
            string gbxml = sys->utf2ascii(xml);
#else
            string gbxml = sys->utf2ascii(xml);
#endif
            sys->StrFindReplace(gbxml, "utf-8", "gb2312");

            sipUA->message(platform->addrcode(),m_ifnetip, platform->sport(), m_addrcode, fromh, fromp, gbxml, gbxml);
        }
        return 0;
    }

    return -1;
}

int        
GB28181Superior::on_CatalogQuery_Business(const string json)//????????
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  rvalueCatalogInfo;
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["Query"]["DeviceID"].asString();
    string       SN = rvalue["Query"]["SN"].asString();
    int         SNN = atoi(SN.c_str());
    string fromh = rvalue["ip"].asString();
    int fromp = rvalue["port"].asInt();


    if (SNN < 1)
    {
        LOGE_SIP("invalidate args, SN: %s,SN minInclusivevalue == 1", SN.c_str());
        return -1;
    }

    if (systm->addrcode() == addrcode)
    {
        string jsonCatalogInfo = "";
		int ret = catalogInfoCfg->query(jsonCatalogInfo, "");
        if (0 != ret)
        {
            LOGE_SIP("superCfg->query(jsonCatalogInfo) failed! \n");
            return -1;
        }
		if (!reader.parse(jsonCatalogInfo, rvalueCatalogInfo, false))
        {
			LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonCatalogInfo.c_str());
            return -1;
        }

		int realNum = catalogInfoCfg->count("");

        int count = 0;
		for (int i = 0; i < rvalueCatalogInfo.size(); ++i)
        {
			if (!rvalueCatalogInfo[i]["addrcode"].isNull())
            { 
				string cata_addr     = rvalueCatalogInfo[i]["addrcode"].asString();
				string cata_name     = rvalueCatalogInfo[i]["name"].asString();
				string cata_type     = rvalueCatalogInfo[i]["comment"].asString();
				int    cata_parentId = rvalueCatalogInfo[i]["parentId"].asInt();
				int    circle_time   = rvalueCatalogInfo.size();
				switch (atoi(cata_type.c_str()))
				{
				case 2://????
					wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
					wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = cata_name;
					wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode;
					break;
				case 3://????
					wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
					wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = cata_name;
					while (circle_time > 0)
					{
						int j = 0;
						for (; j < rvalueCatalogInfo.size(); j++)
						{
							if (cata_parentId == rvalueCatalogInfo[j]["id"].asInt())//to find the parent node
							{
								cata_parentId = rvalueCatalogInfo[j]["parentId"].asInt();
								if (circle_time == rvalueCatalogInfo.size() && "3" == rvalueCatalogInfo[j]["comment"].asString())
								{
									wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = rvalueCatalogInfo[j]["addrcode"];
								}
								break;
							}
						}
						circle_time--;
						if (-1 == cata_parentId)//to find the group of the organization 
						{
							wvalue["Response"]["DeviceList"]["Item"][count]["BusinessGroupID"] = rvalueCatalogInfo[j]["addrcode"];
							break;
						}
						if (j == rvalueCatalogInfo.size())
						{
							break;
						}
					}
					break;
				case 4://??
					wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
					wvalue["Response"]["DeviceList"]["Item"][count]["Name"]     = cata_name;
					wvalue["Response"]["DeviceList"]["Item"][count]["Status"]   = "ON";
					wvalue["Response"]["DeviceList"]["Item"][count]["Manufacturer"] = "Manufacturer";
					wvalue["Response"]["DeviceList"]["Item"][count]["Model"]    = "Model";
					wvalue["Response"]["DeviceList"]["Item"][count]["Owner"]    = "Owner";
					wvalue["Response"]["DeviceList"]["Item"][count]["CivilCode"]= "CivilCode";
					wvalue["Response"]["DeviceList"]["Item"][count]["Parental"] = "0";
					wvalue["Response"]["DeviceList"]["Item"][count]["SafetyWay"] = "0";
					wvalue["Response"]["DeviceList"]["Item"][count]["RegisterWay"] = "1";
					wvalue["Response"]["DeviceList"]["Item"][count]["Secrecy"] = "0";
					wvalue["Response"]["DeviceList"]["Item"][count]["Longitude"] = "0.000";
					wvalue["Response"]["DeviceList"]["Item"][count]["Latitude"] = "0.000";
					{
						int j = 0;
						for (; j < rvalueCatalogInfo.size(); j++)
						{
							if (cata_parentId == rvalueCatalogInfo[j]["id"].asInt() && "3" == rvalueCatalogInfo[j]["comment"].asString())
							{
								break;
							}
						}
						if (j != rvalueCatalogInfo.size())
						{
							wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode + "/" + rvalueCatalogInfo[j]["addrcode"].asString();
						}
						else
						{
							wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode;
						}
					}
					break;
				default:
					break;
				}
                ++count;
            }
            if (count == UT_SUPERIOR_PR_NUM)
            {
                wvalue["Response"]["CmdType"] = "Catalog";
                wvalue["Response"]["SN"] = SN;
                wvalue["Response"]["DeviceID"] = addrcode;
                wvalue["Response"]["SumNum"] = std::to_string(realNum);
                wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

                string xml;
                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
#ifdef WIN32
            string gbxml = sys->utf2ascii(xml);
#else
            string gbxml = xml;
#endif
                sys->StrFindReplace(gbxml, "utf-8", "gb2312");
                sipUA->message(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, fromh, fromp, gbxml, gbxml);
                wvalue.clear();
                count = 0;
            }
        }
        if (count > 0)
        {
            wvalue["Response"]["CmdType"] = "Catalog";
            wvalue["Response"]["SN"] = SN;
            wvalue["Response"]["DeviceID"] = addrcode;
            wvalue["Response"]["SumNum"] = std::to_string(realNum);
            wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

            string xml;
            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
#ifdef WIN32
            string gbxml = sys->utf2ascii(xml);
#else
            string gbxml = xml;
#endif
            sys->StrFindReplace(gbxml, "utf-8", "gb2312");

            sipUA->message(platform->addrcode(),  m_ifnetip, platform->sport(), m_addrcode, fromh, fromp,  gbxml, gbxml);
        }
        return 0;
    }
    return -1;
}

int GB28181Superior::on_PresetQuery(const string json)//??????????
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  rvaluePresetInfo;

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string addrcode = rvalue["Query"]["DeviceID"].asString();//??????
	string       SN = rvalue["Query"]["SN"].asString();
    int         SNN = atoi(SN.c_str());
    string fromh = rvalue["ip"].asString();
    int fromp = rvalue["port"].asInt();

	if (SNN < 1)
	{
		LOGE_SIP("invalidate args, SN: %s,SN minInclusivevalue == 1", SN.c_str());
		return -1;
	}

	if (!addrcode.empty())
	{
		char cond[256] = { 0 };
		snprintf(cond, 256, "addrcode='%s';", addrcode.c_str());
		string jsonPresetInfo = "";
		int ret = presetCfg->query(jsonPresetInfo,cond);
		if (0 != ret)
		{
			LOGE_SIP("presetCfg->query(jsonPresetInfo) failed! \n");
			return -1;
		}
		if (!reader.parse(jsonPresetInfo, rvaluePresetInfo, false))
		{
			LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonPresetInfo.c_str());
			return -1;
		}

		int realNum = presetCfg->count(cond);
		int i = 0;
		for (; i < rvaluePresetInfo.size(); ++i)
		{
			if (!rvaluePresetInfo[i]["idx"].isNull() && !rvaluePresetInfo[i]["name"].isNull())
			{
				string name = rvaluePresetInfo[i]["name"].asString();
				int    idx  = rvaluePresetInfo[i]["idx"].asInt();
				wvalue["Response"]["PresetList"]["Item"][i]["PresetName"] = name;
				wvalue["Response"]["PresetList"]["Item"][i]["PresetID"] = std::to_string(idx);
			}

		}
		if (i == rvaluePresetInfo.size())
		{
			wvalue["Response"]["CmdType"] = "PresetQuery";
			wvalue["Response"]["SN"] = SN;
			wvalue["Response"]["DeviceID"] = addrcode;
			wvalue["Response"]["PresetList"]["@Num"] = realNum;

			string xml;
			json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
			string gbxml = sys->utf2ascii(xml);
			sys->StrFindReplace(gbxml, "utf-8", "gb2312");

            sipUA->message(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, fromh, fromp,  gbxml, gbxml);
		}
		return 0;
	}

	return -1;
}


int
GB28181Superior::on_RecordInfoEx(const string json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter fastwriter;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode  = rvalue["Query"]["DeviceID"].asString(); //34020000001310000001
    string SN        = rvalue["Query"]["SN"].asString();       //8
    string StartTime = rvalue["Query"]["StartTime"].asString();//2020-11-16T00:00:00
    string EndTime   = rvalue["Query"]["EndTime"].asString();  //2020-11-16T23:59:59
    string Type      = rvalue["Query"]["Type"].asString();     //all
    string fromh = rvalue["ip"].asString();
    int fromp = rvalue["port"].asInt();

    //2.cache record info
    Pu *pu = putree->findPu(addrcode);
    if (!pu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    datetime_s b;
    datetime_s e;
    sys->strptime(StartTime, b);
    sys->strptime(EndTime, e);
    string records;

    if ((Pu::PROTO_TYPE_GW_B == pu->protocol()) ||
        (Pu::PROTO_TYPE_GW_B_HS == pu->protocol()) ||
        (Pu::PROTO_TYPE_GW_B_HIK == pu->protocol()) ||
        (Pu::PROTO_TYPE_GW_B_UT == pu->protocol()) ||
        (Pu::PROTO_TYPE_GB28181 == pu->protocol()))
    {
        //if (pu->queryRecord(addrcode, atoi(type.c_str()), b, e, atoi(fromIndex.c_str()), start + number, records, usercode) < 0)
        {
            LOGE_SIP(getError());
            return -1;
        }
        return 0;
    }
    else
    {
        int recordtype = Pu::RECORD_TYPE_ALL;
        if (Type == "all")
        {
            recordtype = Pu::RECORD_TYPE_ALL;
        }
        
#if 1

        if (pu->queryRecord(addrcode, recordtype, b, e, 1, 100, records, "") < 0)
        {
            LOGE_SIP(getError());
            return -1;
        }

        if (!reader.parse(records, rvalue, false))
        {
            LOGE_SIP("parse err: %s", records.c_str());
            return -1;
        }

        string guname = pu->guname(addrcode);
        int realNum = rvalue["realNum"].asInt();

        wvalue["Response"]["CmdType"]  = "RecordInfo";
        wvalue["Response"]["SN"]       = SN;
        wvalue["Response"]["DeviceID"] = addrcode;
        wvalue["Response"]["Name"]     = guname;
        wvalue["Response"]["SumNum"]   = std::to_string(realNum);
        wvalue["Response"]["RecordList"]["@Num"] = std::to_string(realNum);

        for (uint32_t i = 0; i < realNum; ++i)
        {
            string startT = rvalue["records"][i]["start"].asString();
            string endT   = rvalue["records"][i]["end"].asString();
            startT.pop_back();
            endT.pop_back();

            wvalue["Response"]["RecordList"]["Item"][i]["DeviceID"]  = addrcode;
            wvalue["Response"]["RecordList"]["Item"][i]["Name"]      = guname;
            wvalue["Response"]["RecordList"]["Item"][i]["FilePath"]  = rvalue["records"][i]["filename"].asString();
            wvalue["Response"]["RecordList"]["Item"][i]["Address"]   = "Address 1";
            wvalue["Response"]["RecordList"]["Item"][i]["StartTime"] = startT;
            wvalue["Response"]["RecordList"]["Item"][i]["EndTime"]   = endT;
            wvalue["Response"]["RecordList"]["Item"][i]["FileSize"] = rvalue["records"][i]["size"].asInt();
            wvalue["Response"]["RecordList"]["Item"][i]["Secrecy"]   = "0";
            wvalue["Response"]["RecordList"]["Item"][i]["Type"]      = "time";
        }

        std::string strJson1 = fastwriter.write(wvalue);
        json2xml(strJson1.c_str(), xml);

        //string xml2;
        //remove_copy(xml.begin(), xml.end(), back_inserter(xml2), '\t');
        //string xml3("utf-8");
        //int pos = xml2.find(xml3);
        //if (pos != -1)
        //{
        //    xml2.replace(pos, xml3.length(), "gb2312");
        //}
        sipUA->message(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, fromh, fromp, xml, xml);
#endif

    }

    return 0;
}

void
GB28181Superior::timerdriver()
{
    Superior::timerdriver();
}

int        
GB28181Superior::push_resource()
{
    return 0;
}

int        
GB28181Superior::keepaliveLoop()
{
    while (m_keekaliveThread.active())
    {
        if (m_registed)
        {
            SHL_Json::Reader reader;
            SHL_Json::Value  rvalueCamBep;

            SHL_Json::StreamWriterBuilder writer;
            SHL_Json::Value               wvalue;
            SHL_Json::FastWriter fastwriter;

            wvalue["Notify"]["CmdType"] = "Keepalive";
            wvalue["Notify"]["SN"] = "0";
            wvalue["Notify"]["DeviceID"] = systm->addrcode();
            wvalue["Notify"]["Status"] = "OK";

            //find cam_belong_platform
            char cond[256] = { 0 };
            snprintf(cond, 256, "platform_addrcode='%s';", m_addrcode.c_str());
            string jsonCamBeP = "";
            int ret = cameraBelongPlatformCfg->query(jsonCamBeP, cond);
            if (0 != ret)
            {
                LOGE_SIP("superCfg->query(jsonCamBeP) failed! \n");
                sys->sleep(1000);
                continue;
            }
            if (!reader.parse(jsonCamBeP, rvalueCamBep, false))
            {
                LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonCamBeP.c_str());
                sys->sleep(1000);
                continue;
            }
#if 1
            for (int i = 0; i < rvalueCamBep.size(); ++i)
            {
                wvalue["Notify"]["Info"]["DeviceID"][i]["#"] = rvalueCamBep[i]["camera_addrcode"].asString();
            }
#else
            string deviceID[2] = { "34020000001310000001", "34020000001310000002" };
            for (int i= 0; i < 0; ++i)
            {
                wvalue["Notify"]["Info"]["DeviceID"][i]["#"] = deviceID[i];
            }
#endif

            std::string xml;
            string xml2;
            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

            //remove_copy(xml.begin(), xml.end(), back_inserter(xml2), '\t');
            string gbxml = sys->utf2ascii(xml);
            sys->StrFindReplace(gbxml, "utf-8", "gb2312");

            sipUA->message(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, gbxml, gbxml);
        
            sys->sleep(1000 * 60);
        }
        else
        {
            sys->sleep(1000);
        }
    }
    return 0;
}

void* 
GB28181Superior::threadProcEx(ThreadInfo *info)
{
    Param_T *param = (Param_T*)(info->data);
    GB28181Superior * sup = (GB28181Superior *)(param->m_pThis);
    int ret = -1;
    string rspXml = "";

    switch (info->id)
    {
    case T_CATALOG_BUSINESS:
    {
        ParamSource_T *param_t = (ParamSource_T*)(param);
        sup->on_CatalogQuery_Business(param_t->m_josn);
        break;
    }
	case T_PRESET:
	{
	    ParamSource_T *param_t = (ParamSource_T*)(param);
        sup->on_PresetQuery(param_t->m_josn);
        break;
	}
    case T_CATALOG_DISTRICT:
    {
        ParamSource_T *param_t = (ParamSource_T*)(param);
        sup->on_CatalogQuery_District(param_t->m_josn);
    }
        break;
    case T_RECORD:
    {
        ParamSource_T *param_t = (ParamSource_T*)(param);
        sup->on_RecordInfoEx(param_t->m_josn, rspXml);
        break;
    }
    default:
        break;
    }
    if (param->m_thread)
    {
        delete param->m_thread;
    }

    return NULL;

}

void
GB28181Superior::onAlarm(string &json, void *data, uint32_t size)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return;
    }

    SHL_Json::StreamWriterBuilder writer;
    string alarms = SHL_Json::writeString(writer, rvalue["alarms"]);
    sipUA->push_alarm(rvalue["time"].asString(), alarms, m_subscribedid, this);
    sipUA->push_status(rvalue["time"].asString(), alarms, m_subscribedid, this);
    CatalogChange_Business();
}


int
GB28181Superior::CatalogChange_Business()//??????,????
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalueCatalogInfo;
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    uint32_t SN = ((GB28181_SipUA*)sipUA)->GenerateSN();
    string addrcode = systm->addrcode();
    if (systm->addrcode() == addrcode)
    {
        string jsonCatalogInfo = "";
        int ret = catalogInfoCfg->query(jsonCatalogInfo, "");
        if (0 != ret)
        {
            LOGE_SIP("superCfg->query(jsonCatalogInfo) failed! \n");
            return -1;
        } 
        if (!reader.parse(jsonCatalogInfo, rvalueCatalogInfo, false))
        {
            LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonCatalogInfo.c_str());
            return -1;
        }

        int realNum = catalogInfoCfg->count("");

        int count = 0;
        for (int i = 0; i < rvalueCatalogInfo.size(); ++i)
        {
            if (!rvalueCatalogInfo[i]["addrcode"].isNull())
            {
                string cata_addr = rvalueCatalogInfo[i]["addrcode"].asString();
                string cata_name = rvalueCatalogInfo[i]["name"].asString();
                string cata_type = rvalueCatalogInfo[i]["comment"].asString();
                int    cata_parentId = rvalueCatalogInfo[i]["parentId"].asInt();
                int    circle_time = rvalueCatalogInfo.size();
                switch (atoi(cata_type.c_str()))
                {
                case 2://????
                    wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
                    wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = cata_name;
                    wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode;
                    break;
                case 3://????
                    wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
                    wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = cata_name;
                    while (circle_time > 0)
                    {
                        int j = 0;
                        for (; j < rvalueCatalogInfo.size(); j++)
                        {
                            if (cata_parentId == rvalueCatalogInfo[j]["id"].asInt())//to find the parent node
                            {
                                cata_parentId = rvalueCatalogInfo[j]["parentId"].asInt();
                                if (circle_time == rvalueCatalogInfo.size() && "3" == rvalueCatalogInfo[j]["comment"].asString())
                                {
                                    wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = rvalueCatalogInfo[j]["addrcode"];
                                }
                                break;
                            }
                        }
                        circle_time--;
                        if (-1 == cata_parentId)//to find the group of the organization 
                        {
                            wvalue["Response"]["DeviceList"]["Item"][count]["BusinessGroupID"] = rvalueCatalogInfo[j]["addrcode"];
                            break;
                        }
                        if (j == rvalueCatalogInfo.size())
                        {
                            break;
                        }
                    }
                    break;
                case 4://??
                    wvalue["Response"]["DeviceList"]["Item"][count]["DeviceID"] = cata_addr;
                    wvalue["Response"]["DeviceList"]["Item"][count]["Name"] = cata_name;
                    wvalue["Response"]["DeviceList"]["Item"][count]["Status"] = "ON";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Manufacturer"] = "Manufacturer";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Model"] = "Model";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Owner"] = "Owner";
                    wvalue["Response"]["DeviceList"]["Item"][count]["CivilCode"] = "CivilCode";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Parental"] = "0";
                    wvalue["Response"]["DeviceList"]["Item"][count]["SafetyWay"] = "0";
                    wvalue["Response"]["DeviceList"]["Item"][count]["RegisterWay"] = "1";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Secrecy"] = "0";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Longitude"] = "0.000";
                    wvalue["Response"]["DeviceList"]["Item"][count]["Latitude"] = "0.000";
                    {
                        int j = 0;
                        for (; j < rvalueCatalogInfo.size(); j++)
                        {
                            if (cata_parentId == rvalueCatalogInfo[j]["id"].asInt() && "3" == rvalueCatalogInfo[j]["comment"].asString())
                            {
                                break;
                            }
                        }
                        if (j != rvalueCatalogInfo.size())
                        {
                            wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode + "/" + rvalueCatalogInfo[j]["addrcode"].asString();
                        }
                        else
                        {
                            wvalue["Response"]["DeviceList"]["Item"][count]["ParentID"] = addrcode;
                        }
                    }
                    break;
                default:
                    break;
                }
                ++count;
            }
            if (count == UT_SUPERIOR_PR_NUM)
            {
                wvalue["Response"]["CmdType"] = "Catalog";
                wvalue["Response"]["SN"] = SN;
                wvalue["Response"]["DeviceID"] = addrcode;
                wvalue["Response"]["SumNum"] = std::to_string(realNum);
                wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

                string xml;
                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
                string gbxml = sys->utf2ascii(xml);
                sys->StrFindReplace(gbxml, "utf-8", "gb2312");
                
                sipUA->push_catalog(sys->currentTimeMillis2Str(), xml, m_subscribeCatalogDid, this);
                wvalue.clear();
                count = 0;
            }
        }
        if (count > 0)
        {
            wvalue["Response"]["CmdType"] = "Catalog";
            wvalue["Response"]["SN"] = SN;
            wvalue["Response"]["DeviceID"] = addrcode;
            wvalue["Response"]["SumNum"] = std::to_string(realNum);
            wvalue["Response"]["DeviceList"]["@Num"] = std::to_string(count);

            string xml;
            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
            string gbxml = sys->utf2ascii(xml);
            sys->StrFindReplace(gbxml, "utf-8", "gb2312");

            sipUA->push_catalog(sys->currentTimeMillis2Str(), xml, m_subscribeCatalogDid, this);
        }
        return 0;
    }
    return -1;
}