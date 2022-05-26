#include "ut_superior.h"
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


UtSuperior::UtSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauslocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
}


UtSuperior::~UtSuperior()
{
}


int        
UtSuperior::on_resource(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  rvalueDev;
    SHL_Json::Value  rvalueCamBep;

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    string fromIndex = rvalue["SIP_XML"]["Item"]["@FromIndex"].asString();
    string toIndex = rvalue["SIP_XML"]["Item"]["@ToIndex"].asString();
    int    start = atoi(fromIndex.c_str()) - 1;
    int    number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;
    int    end = start;
    if (number > UT_SUPERIOR_PR_NUM)
    {
        end = start + UT_SUPERIOR_PR_NUM;
    }
    else
    {
        end = start + number;
    }

    if (start < 0 || number < 1)
    {
        LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
        return -1;
    }


    //2.cache device info
    map<string, int> mapDev;
    string jsonDev = "";
    int ret = deviceCfg->query(jsonDev);
    if (0 == ret && jsonDev!="")
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
        snprintf(cond, 256, "platform_addrcode='%s' limit '%d','%d';", m_addrcode.c_str(), start, end);
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

        snprintf(cond, 256, "platform_addrcode='%s' limit '%d','%d';", m_addrcode.c_str(), atoi(fromIndex.c_str())-1, atoi(toIndex.c_str())-1);
        int realNum = cameraBelongPlatformCfg->count(cond);

        int count = 0;
        for (int i = 0; i < rvalueCamBep.size();++i)
        {
            string camAddr = "";
            if (!rvalueCamBep[i]["camera_addrcode"].isNull() /*&& !rvalueDev[mapDev[camAddr]]["name"].isNull()*/)
            {
                camAddr = rvalueCamBep[i]["camera_addrcode"].asString();

                wvalue["SIP_XML"]["SubList"]["Item"][count]["@Code"] = camAddr;//rvalueDev[mapDev[camAddr]]["addrcode"].asString();
                map<string, int>::iterator it = mapDev.find(camAddr);
                if (it != mapDev.end())
                {
                    //local camera
                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@Name"] = rvalueDev[mapDev[camAddr]]["name"].asString();
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
                                        wvalue["SIP_XML"]["SubList"]["Item"][count]["@Name"] = subCamValue[j]["camera_name"];
                                        bBreak = true;
                                        break;
                                    }
                                }
                                if (bBreak)
                                    break;
                            }
                        }
                        wvalue["SIP_XML"]["SubList"]["Item"][count]["@Name"] = camAddr;
                    } while (0);
                }

                wvalue["SIP_XML"]["SubList"]["Item"][count]["@Status"] = "1";
                wvalue["SIP_XML"]["SubList"]["Item"][count]["@DecoderTag"] = "100";
                wvalue["SIP_XML"]["SubList"]["Item"][count]["@Longitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][count]["@Latitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][count]["@SubNum"] = std::to_string(rvalueDev[mapDev[camAddr]]["chnNum"].asInt());
                ++count;
            }
        }
        if (count>0)
        {
            wvalue["SIP_XML"]["@EventType"] = "Response_Resource";
            wvalue["SIP_XML"]["SubList"]["@Code"] = addrcode;
            wvalue["SIP_XML"]["SubList"]["@RealNum"] = std::to_string(realNum);
            wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(count);
            wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
            wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(end);

            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
            return 0;
        }
    }

    return -1;
}

void
UtSuperior::timerdriver()
{
    Superior::timerdriver();
}

int        
UtSuperior::push_resource()
{
    //1.find up platform
    //2.cache device info
    //3.find cam_belong_platform
    //4.generate resource data json

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalueSup;
    SHL_Json::Value  rvalueDev;
    SHL_Json::Value  rvalueCamBep;

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;


    //1.find up platform
    string jsonSup = "";
    int ret = superCfg->query(jsonSup);
    if (0 != ret)
    {
        LOGE_SIP("superCfg->query(jsonSup) failed! \n");
        return -1;
    }

    if (!reader.parse(jsonSup, rvalueSup, false))
    {
        LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonSup.c_str());
        return -1;
    }
    //2.cache device info
    map<string, int> mapDev;
    string jsonDev = "";
    ret = deviceCfg->query(jsonDev);
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


    //3.find cam_belong_platform
    for (int i = 0; i < rvalueSup.size(); ++i)
    {
        string supAddr = "";
        int count = 0;
        if (!rvalueSup[0]["addrcode"].isNull())
        {
            supAddr = rvalueSup[0]["addrcode"].asString();

            char cond[256] = { 0 };
            snprintf(cond, 256, "platform_addrcode='%s';", supAddr.c_str());
            string jsonCamBeP = "";
            ret = cameraBelongPlatformCfg->query(jsonCamBeP, cond);
            if (0 != ret)
            {
                LOGE_SIP("superCfg->query(jsonCamBeP) failed! \n");
                continue;
            }
            if (!reader.parse(jsonCamBeP, rvalueCamBep, false))
            {
                LOGE_SIP("SHL_Json parse failed,json:%s \n", jsonCamBeP.c_str());
                continue;
            }
            for (int j = 0; j < rvalueCamBep.size(); ++j)
            {
                //find camera info
                string camAddr = "";
                if (!rvalueCamBep[j]["camera_addrcode"].isNull() /*&& !rvalueDev[mapDev[camAddr]]["name"].isNull()*/)
                {
                    camAddr = rvalueCamBep[j]["camera_addrcode"].asString();

                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@Code"] = camAddr;
                    
                    if (!rvalueDev[mapDev[camAddr]]["name"].isNull())
                        wvalue["SIP_XML"]["SubList"]["Item"][count]["@Name"] = rvalueDev[mapDev[camAddr]]["name"].asString();
                    else
                        wvalue["SIP_XML"]["SubList"]["Item"][count]["@Name"] = camAddr;

                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@Status"] = "1";
                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@DecoderTag"] = "100";
                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@Longitude"] = "0";
                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@Latitude"] = "0";
                    wvalue["SIP_XML"]["SubList"]["Item"][count]["@SubNum"] = std::to_string(rvalueDev[mapDev[camAddr]]["chnNum"].asInt());
                    ++count;
                }

                if (UT_SUPERIOR_PR_NUM == count)
                {
                    wvalue["SIP_XML"]["@EventType"] = "Push_Resourse";
                    wvalue["SIP_XML"]["Code"] = platform->addrcode();
                    wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(count);

                    string xml;
                    string jresp;
                    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

                    sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);

                    count = 0;
                    wvalue.clear();
                }
            }
            if (count > 0)
            {
                wvalue["SIP_XML"]["@EventType"] = "Push_Resourse";
                wvalue["SIP_XML"]["Code"] = platform->addrcode();
                wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(count);

                string xml;
                string jresp;
                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

                sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);
            }
        }
    }
    return 0;
}