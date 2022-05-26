#include "client.h"
#include "socket.h"
#include "tmulti_queue.h"
#include "bitoperation.h"
#include "putree.h"
#include "pu.h"
#include "mempool.h"
#include "usermgr.h"

#include "stk/stk_message_http.h"
#include "json/json.h"
#include "priv_server.h"
#include "priv_realsession.h"
#include "priv_recordsession.h"
#include "dualtalksession.h"

#include "tls_error.h"
#include "event.h"
#include "bufferev.h"
#include "ntp.h"
#include "system.h"
#include "preset.h"
#include "websock_server.h"
#include "websock_realsession.h"
#include "websock_recordsession.h"
#include "global_str_def.h"
#include "system.h"
#include <thread>
#include <chrono>

#include "cfg_device.h"
#include "cfg_ivarel.h"
#include "cfg_gate.h"
#include "cfg_camera_plate.h"
#include "cfg_preset.h"
#include "cfg_system.h"

#include "captauresession.h"

#include "launch_pu.h"
#include "3rd/hik/sdk-plat.h"
#include "base64_ex.h"
const string       STRQUERYRECORD_C = "setQueryRecordCallback_c";
struct Query_Record_Resp_T
{
    int status;
    int protocol;
    string str;
};

void        
query_record_cb(int type, int protocol, string &jout, void *userdata)
{
    tevent<string, Query_Record_Resp_T> *evt = (tevent<string, Query_Record_Resp_T>*)userdata;
    Query_Record_Resp_T resp;
    if (evt)
    {
        if (type == 0)
        {
            resp.str = jout;
            resp.protocol = protocol;
            resp.status = 0;
        }
        evt->notify(STRQUERYRECORD_C, resp);
    }
}

Client::Client(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey)
{
    m_active = true;
    m_bev = NULL;
    m_name = name;
    m_userId = userId;
    m_userCode = addrcode;
    m_mright = mright;
    m_oright = oright;
    m_lastHeart = sys->currentTimeMillis();
    //m_key.generate();
    m_userKey = userKey;
    memset(m_typeBits, 0, sizeof(m_typeBits));

    assert(bev != NULL);
    m_bev = bev;
    m_bev->addBuffevListener(this);


    LOGI_MSV("new client key %s construct!", userKey.c_str());
    //writeUserLog(STR_MSV_REMOTE_LOGIN);
}

Client::~Client()
{
	if (m_bev)
	{
		m_bev->delBuffevListener(this);
	}
    {
        AutoLock locker(m_sessMgr._mtx);
        for (tlist<Session *>::const_iterator i = m_sessMgr.begin(); i != m_sessMgr.end(); ++i)
        {
            delete (*i);
        }
        m_sessMgr.clear();
    }

    {
        AutoLock locker(m_dutalkSessMgr._mtx);
        for (std::list<DSession *>::const_iterator i = m_dutalkSessMgr.begin(); i != m_dutalkSessMgr.end(); ++i)
        {
            delete (*i);
        }
        m_dutalkSessMgr.clear();
    }


    LOGI_MSV("a client key %s distruct!", m_userKey.c_str());
    //writeUserLog(STR_MSV_REMOTE_LOGOUT);
}

int
Client::start()
{
    putree->addTriggerListener(this);
    userMgr->addAuthListener(m_name, this);
    //systm->addTriggerListener(this);

    m_time = sys->currentTime();
    m_active = true;

    return 0;
}

void
Client::stop()
{
    putree->delTriggerListener(this);
    userMgr->delAuthListener(m_name, this);
    systm->delTriggerListener(this);

    {
        AutoLock locker(m_sessMgr._mtx);
        for (tlist<Session *>::const_iterator i = m_sessMgr.begin(); i != m_sessMgr.end(); ++i)
        {
            (*i)->stop();
        }
    }

    {
        AutoLock locker(m_dutalkSessMgr._mtx);
        for (std::list<DSession *>::const_iterator i = m_dutalkSessMgr.begin(); i != m_dutalkSessMgr.end(); ++i)
        {
            (*i)->stop();
        }
    }


	LOGI_MSV("a client key %s distruct!", m_userKey.c_str());
}

int
Client::type()
{
    return m_type;
}

bool
Client::active()
{
    return m_active;
}

string
Client::key()
{
    return m_userKey;
}

void
Client::clearPush()
{
    memset(m_typeBits, 0, sizeof(m_typeBits));
}

void
Client::setPush(uint64_t type)
{
    for (size_t i = 0; i < sizeof(type) * 8; i++)
    {
        if (CHK_BIT_LL(&type, i))
        {
            SET_BIT_BYTE(m_typeBits, i);
        }
    }
}

bool
Client::hasRight(uint32_t rightBit)
{
    uint64_t tmp = m_oright | m_mright;

    return CHK_BIT_BYTE((uint8_t *)&tmp, rightBit);
}

int
Client::writeUserLog(string  info)
{
    SHL_Json::FastWriter userLogWriter;
    SHL_Json::Value      userLogValue;
    userLogValue["time"] = sys->currentTime();
    userLogValue["info"] = info;
    userLogValue["userId"] = m_name;
    std::string userLogJson = userLogWriter.write(userLogValue);
    return logOb->writeLog("tbl_user_log", userLogJson);
}

Session *
Client::startPreview(string &json, Bufferev *bev)
{
    AutoLock locker(m_sessMgr._mtx);
    RealSession *ss = NULL;
    if (T_PRIVATE == m_type)
    {
        ss = new PrivRealSession(this, bev);
    }
    else if (T_WEBSOCK == m_type)
    {
        ss = new WebSockRealSession(this, bev);
    }

    
    if (ss->start(json) < 0)
    {
        
        //ss->start() 该接口可能阻塞
        ss->stop();
        delete ss;
        return NULL;
    }

    m_sessMgr.push_back(ss);

    //writeUserLog(STR_MSV_REQUEST_PREVIEW);

    return ss;
}

int
Client::ptzCtrl(string &json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	int iRet = -1;
	Pu *pu = NULL;
	if (!reader.parse(json, rvalue, false))
	{
		return iRet;
	}

	int  cmd = rvalue["cmd"].asInt();
	int  channelNo = rvalue["channelNo"].asInt();
	int  speed = rvalue["speed"].asInt();
	int  preset = rvalue["preset"].asInt();
	bool stop = rvalue["stop"].asBool();
	string addrcode = "";

	if (!rvalue["deviceHost"].isNull())
	{
		pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
		if (pu)
		{
			if (!pu->validate(channelNo))
			{
				LOGE_MSV("device is offline,PTZ is failed,ip: %s, channelNo: %d, cmd: %d", rvalue["deviceHost"].asString().c_str(),
					channelNo + 1, cmd);
				return -1;
			}
			addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);
			if (Pu::PROTO_TYPE_UT == pu->protocol())
			{
				iRet = pu->ptzCtrl(cmd, addrcode, speed, preset, stop);
			}
			else
			{
				string priorName;
				string priorIp;
				int u16ClientOpt = userMgr->optlevel(m_userId);
				if (pu->CtrlPtzRight(m_ip, m_name, m_userKey, (uint32_t)u16ClientOpt, (uint32_t)channelNo, stop, priorIp, priorName) < 0)
				{
					LOGI_MSV("ip: %s, channelNo: %d, cmd: %d, preset: %d, stop: %d CtrlPtzRight failed!!!!", rvalue["deviceHost"].asString().c_str(),
						channelNo + 1, cmd, preset, stop);
					return -1;
				}

				iRet = pu->ptzCtrl(cmd, channelNo, speed, preset, stop);
			}
		}
	}
	else if (!rvalue["addrcode"].isNull())
	{
		addrcode = rvalue["addrcode"].asString();
		pu = putree->findPu(addrcode);
		if (!pu->validate(channelNo))
		{
			LOGE_MSV("device is offline,PTZ is failed,ip: %s, channelNo: %d, cmd: %d", addrcode.c_str(),
				channelNo + 1, cmd);
			return -1;
		}
		if (pu)
		{
			iRet = pu->ptzCtrl(cmd, addrcode, speed, preset, stop);
		}
	}
	
	//设置预置位字符串到视频画面
	if (systm->bShowPresetName() != 0 && pu != NULL)
	{
		SHL_Json::Value drawInfo;
		drawInfo["Method"] = "setShowStringV1";
		drawInfo["chnNo"] = channelNo + 1;
		drawInfo["ItemCount"] = 1;

		if (cmd == 18)
		{
			std::string output;
			char condiction[256];
			sprintf(condiction, "addrcode='%s' and idx=%d", addrcode.c_str(), preset);
			/*presetCfg->query(output, condiction);
			SHL_Json::Value outValue;
			reader.parse(output, outValue, false);*/
			
			/*for (int i = 0; i < outValue.size(); i++)
			{
				if (outValue[i]["idx"].asInt() == preset)
				{*/
					std::string name = pset->getName(addrcode,preset);
					drawInfo["items"][0]["X"] = systm->X();
					drawInfo["items"][0]["Y"] = systm->Y();
					drawInfo["items"][0]["bShowString"] = 1;
					drawInfo["items"][0]["stringLength"] = strlen(name.c_str());
					drawInfo["items"][0]["content"] = name;
					drawInfo["items"][0]["index"] = 3;

					SHL_Json::FastWriter writer;
					string inputStr = writer.write(drawInfo);
					pu->onCustomCommand(inputStr);
					//break;
				//}
			//}
		}
		else{
			drawInfo["items"][0]["X"] = 0;
			drawInfo["items"][0]["Y"] = 0;
			drawInfo["items"][0]["bShowString"] = 0;
			drawInfo["items"][0]["stringLength"] = 0;
			drawInfo["items"][0]["content"] = "";
			drawInfo["items"][0]["index"] = 3;
			SHL_Json::FastWriter writer;
			string inputStr = writer.write(drawInfo);
			pu->onCustomCommand(inputStr);
		}


	}
	

	if (stop)
	{
		//if (iRet != 0)
		//{
		//	writeUserLog(STR_MSV_PTZ_CONTROL_FAIL);
		//}
		//else
		//{
		//	writeUserLog(STR_MSV_PTZ_CONTROL_SUCC);
		//}

	}
	if (0 == iRet)
	{
		//TODO:Push alarm at the beginning of PTZ control
		if (cmd > 15 || false == stop)
		{
			SHL_Json::FastWriter writer;
			SHL_Json::Value      value;

			value["version"] = "1.0";
			value["time"] = sys->currentTime();
			value["deviceName"] = pu->name();
			value["deviceHost"] = pu->host();
			value["devicePort"] = pu->port();
			value["addrcode"] = pu->addrcode();
			value["clientIp"] = peerip();
			value["clientPort"] = peerport();
			SHL_Json::Value item;
			item["type"] = TriggerListener::TR_PTZ_CONTROL_ALARM;
			item["gucode"] = pu->gucode(channelNo);
			item["name"] = pu->guname(channelNo);
			item["channelNo"] = channelNo;
			item["preset"] = preset;
			item["speed"] = speed;
			item["stop"] = stop;
			value["alarms"].append(item);


			string json = writer.write(value);
			pu->notifyAlarm(json, NULL, 0);
		}
	}
	return iRet;
}
int Client::onVehicleCtrl(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse failed\n");
        return -1;
    }
    
    //find plate camera pu
    string ip = rvalue["DevIp"].asString();
    int port = rvalue["DevPort"].asInt();
    int chan = rvalue["chnNo"].asInt();

    Pu *pu = putree->findPu(ip, port);
    if (!pu)
    {
        ERRE_MSV("device not find, ip:%s, port:%d\n", ip.c_str(), port);
        return -1;
    }
	if (!pu->online())
	{
		ERRE_MSV("device not online, ip:%s, port:%d", ip.c_str(), port);
		return -1;
	}

    //find terminal pu
    string gateJson;
    char cond[512] = { 0 };
    sprintf(cond, "camaddr=\"%s\";", pu->addrcode().c_str());
    int ret = gateCfg->query(gateJson, cond);
    if (0 == ret)
    {
        SHL_Json::Reader readerGate;
        SHL_Json::Value  rvalueGate;

        if (!readerGate.parse(gateJson, rvalueGate, false))
        {
            ERRE_MSV("SHL_Json parse failed\n");
            return -1;
        }
        SHL_Json::FastWriter twrite;
        if (rvalueGate.size() < 1)
        {
            ERRE_MSV("gateCfg->query no data\n");
            return -1;
        }

        string terminal_code = rvalueGate[0]["terminal_code"].asString();
        pu = putree->findPu(terminal_code);
        if (pu != NULL)
        {
            return pu->onCustomCommand(json);
        }
    }
    return -1;

}

/** 20200603 zhangshenyu add **/
int 
Client::onGateCtrl(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        ERRE_MSV("SHL_Json parse failed\n");
        return -1;
    }

    //20200511 zhangshenyu add
    //video window ctrl
    if (!rvalue.isMember("gateAddr"))
    {
        string ip = rvalue["DevIp"].asString();
        int port = rvalue["DevPort"].asInt();
        int chan = rvalue["chnNo"].asInt();

        Pu *pu = putree->findPu(ip, port);
        if (!pu)
        {
            ERRE_MSV("device not find, ip:%s, port:%d\n", ip.c_str(), port);
            return -1;
        }
#if 1
        //ctrl by camera
        pu->onCustomCommand(json);
#else
        //ctrl by terminal
        //find laneName
        string gateJson;
        char cond[512] = { 0 };
        sprintf(cond, "camaddr=\"%s\";", pu->addrcode().c_str());
        int ret = gateCfg->query(gateJson, cond);
        if (0 == ret)
        {
            SHL_Json::Reader readerGate;
            SHL_Json::Value  rvalueGate;

            if (!readerGate.parse(gateJson, rvalueGate, false))
            {
                ERRE_MSV("SHL_Json parse failed\n");
                return -1;
            }
            SHL_Json::FastWriter twrite;
            if (rvalueGate.size() < 1)
            {
                ERRE_MSV("gateCfg->query no data\n");
                return -1;
            }
            string laneName = rvalueGate[0]["lanename"].asString();
            rvalue["laneName"] = laneName;
            string tjson = twrite.write(rvalue);

            //find terminal pu
            string terminal_code = rvalueGate[0]["terminal_code"].asString();
            pu = putree->findPu(terminal_code);
            if (pu != NULL)
            {
                return pu->onCustomCommand(tjson);
            }
        }
#endif
    }
    //graphics window ctrl
    else
    {
        int gateAddr = rvalue["gateAddr"].asInt();
#if 1
        //ctrl by camera
        //find camera
        string gateJson;
        char cond[512] = { 0 };
        sprintf(cond, "gateaddr=%d;", gateAddr);
        int ret = gateCfg->query(gateJson, cond);
        if (0 == ret)
        {
            SHL_Json::Reader readerGate;
            SHL_Json::Value  rvalueGate;
            if (!readerGate.parse(gateJson, rvalueGate, false))
            {
                ERRE_MSV("SHL_Json parse failed\n");
                return -1;
            }
            if (rvalueGate.size() <  1)
            {
                ERRE_MSV("gateCfg->query no data\n");
                return -1;
            }
            string camaddr = rvalueGate[0]["camaddr"].asString();

            //find ipc pu
            Pu * pu = putree->findPu(camaddr);
            if (pu)
            {
                return pu->onCustomCommand(json);
            }
            else
            {
                ERRE_MSV("device not find, camera addrcode:%s", camaddr.c_str());
                return -1;

            }
        }
#else
        //ctrl by terminal
        //find landName
        string gateJson;
        char cond[512] = { 0 };
        sprintf(cond, "gateaddr=%d;", gateAddr);
        int ret = gateCfg->query(gateJson, cond);
        if (0 == ret)
        {
            SHL_Json::Reader readerGate;
            SHL_Json::Value  rvalueGate;
            if (!readerGate.parse(gateJson, rvalueGate, false))
            {
                ERRE_MSV("SHL_Json parse failed\n");
                return -1;
            }
            if (rvalueGate.size() <  1)
            {
                ERRE_MSV("gateCfg->query no data\n");
                return -1;
            }
            string terminal_code = rvalueGate[0]["terminal_code"].asString();
            string laneName = rvalueGate[0]["lanename"].asString();

            //find terminal pu
            Pu * pu = putree->findPu(terminal_code);
            if (pu)
            {

                SHL_Json::FastWriter twrite;
                rvalue["laneName"] = laneName;
                string tjson = twrite.write(rvalue);
                return pu->onCustomCommand(tjson);
            }
            else
            {
                ERRE_MSV("device not find, terminal_code addrcode:%s", terminal_code.c_str());
                return -1;

            }
        }
#endif
    }
    return -1;
}


int
Client::customCommand(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string jMethod = rvalue["Method"].asString();

    if ("gateCtrl" == jMethod)
    {
	/** 20200603 zhangshenyu add **/
        return onGateCtrl(json);        
    }
    else if ("vehicleCtrl" == jMethod)
    {
        return onVehicleCtrl(json);
    }
	else if ("GateCtrl" == jMethod)
	{
		string devIp = rvalue["DevIp"].asString();
		int    devPort = rvalue["DevPort"].asInt();

		Pu * pu = putree->findPu(devIp, devPort);
		if (pu)
		{
			pu->onCustomCommand(json);
		}
	}
	else if ("RecordCtrl" == jMethod)
	{
		string devIp = rvalue["DevIp"].asString();
		int    devPort = rvalue["DevPort"].asInt();

		Pu * pu = putree->findPu(devIp, devPort);
		if (!pu->online())
		{
			ERRE_MSV("device not online, ip:%s, port:%d", devIp.c_str(), devPort);
			return -1;
		}
		if (pu)
		{
			pu->onCustomCommand(json);
		}
	}
    else
    {
        string ip = rvalue["DevIp"].asString();
        int port = rvalue["DevPort"].asInt();

        if (("0.0.0.0" == ip) || (0 == port))
        {
			string camIp  = rvalue["chnIp"].asString();
            int    camChn = rvalue["chnNo"].asInt();
            string camJson;
            string devJson;
            char cond[512] = { 0 };
			sprintf(cond, "host='%s';", camIp.c_str());
            int ret = deviceCfg->query(devJson, cond);
            if (0 == ret)
            {
                SHL_Json::Reader readerDev;
                SHL_Json::Value  rvalueDev;
                if (!readerDev.parse(devJson, rvalueDev, false))
               {
                    return -1;
                }
				if (rvalueDev.isArray() && rvalueDev.size() >= 1)
				{
					rvalueDev = rvalueDev[0];
				}
                string strParentAddr = rvalueDev["addrcode"].asString();
                sprintf(cond, "parentCode='%s' and chnNo=%d;", strParentAddr.c_str(), camChn);
                ret = deviceCfg->query(camJson, cond);
                if (0 == ret)
                {
                    SHL_Json::Reader readerCam;
                    SHL_Json::Value  rvalueCam;
                    if (!readerCam.parse(camJson, rvalueCam, false))
                    {
                        return -1;
                    }
					if (rvalueCam.isArray() && rvalueCam.size() >= 1)
					{
						rvalueCam = rvalueCam[0];
					}
                    string strCamAddr = rvalueCam["addrcode"].asString();
                    sprintf(cond, "camera_addrcode='%s';", strCamAddr.c_str());
                    string ivaJson;
                    ret = ivarelCfg->query(ivaJson, cond);
                    if (0 == ret)
                    {
                        SHL_Json::Reader readerIva;
                        SHL_Json::Value  rvalueIva;
                        if (!readerIva.parse(ivaJson, rvalueIva, false))
                        {
                            return -1;
                        }
						if (rvalueIva.isArray() && rvalueIva.size() >= 1)
						{
							rvalueIva = rvalueIva[0];
						}
                        string ivaAddrcode = rvalueIva["iva_addrcode"].asString();

                        Pu * pu = putree->findPu(ivaAddrcode);
						if (!pu->online())
						{
							ERRE_MSV("device not online, ip:%s, port:%d", ivaAddrcode.c_str());
							return -1;
						}
                        if (pu)
                        {
							SHL_Json::FastWriter writer;
							rvalue["DevIp"] = pu->ip();
							rvalue["DevPort"] = pu->port();
							json = writer.write(rvalue);
                            return pu->onCustomCommand(json);
                        }
                    }
                }
            }
        }
        else
        {
            Pu *pu = putree->findPu(ip, port);
            if (!pu)
            {
                ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
                return -1;
            }
			if (!pu->online())
			{
				ERRE_MSV("device not online, ip:%s, port:%d", ip.c_str(), port);
				return -1;
			}
            return pu->onCustomCommand(json);
        }
    }

    return 0;
}

int
Client::customCommandEx(string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string jMethod = rvalue["command"].asString();
    string ip = rvalue["DevIp"].asString();
    int port = rvalue["DevPort"].asInt();



    Pu *pu = putree->findPu(ip, port);
    if (!pu)
    {
        ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
        return -1;
    }

    //20200827 zhangshenyu add start
    //ut_iva_pu set face 
    if (jMethod == "setFace")
    {
        //find tbl_camerareliva
        char cond[512] = { 0 };
        string cam_code = pu->gucode(0);
        sprintf(cond, "camera_addrcode='%s';", cam_code.c_str());
        string ivaJson;
        int ret = ivarelCfg->query(ivaJson, cond);
        if (0 == ret && ivaJson != "")
        {
            SHL_Json::Reader readerIva;
            SHL_Json::Value  rvalueIva;
            if (!readerIva.parse(ivaJson, rvalueIva, false))
            {
                ERRE_MSV("SHL_Json parse failed\n");
                return -1;
            }
            string ivaAddrcode = rvalueIva[0]["iva_addrcode"].asString();

            Pu * ivapu = putree->findPu(ivaAddrcode);
            if (ivapu) 
            {
                LOGI_MSV("set fasce and finded ut_iva_pu:%s!",pu->name().c_str());
                pu = ivapu;
                return pu->onCustomCommand(json);
            }
        }
    }
    //20200827 zhangshenyu add end

    return pu->onCustomCommand(json);
}

int
Client::requestConfig(string &jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(jin, rvalue, false))
    {
        return -1;
    }
    string jMethod = rvalue["Method"].asString();

    if ("getBlackWhiteList" == jMethod)
    {
	/** 20200603 zhangshenyu add **/
        int gateAddr = rvalue["gateAddr"].asInt();

        //2é?ò??ó|í¨μà
        string gateJson;
        char cond[512] = { 0 };
        sprintf(cond, "gateaddr='%d'", gateAddr);
        int ret = gateCfg->query(gateJson, cond);
        if (0 == ret)
        {
            SHL_Json::Reader readerGate;
            SHL_Json::Value  rvalueGate;
            if (!readerGate.parse(gateJson, rvalueGate, false))
            {
                return -1;
            }
            string camAddr = rvalueGate["camaddr"].asString();

            Pu * pu = putree->findPu(camAddr);
            if (pu)
            {
				if (!pu->online())
				{
					ERRE_MSV("device not online, ip:%s, port:%d", camAddr.c_str());
					return -1;
				}
                return pu->onRequestConfig(jin, jout);
            }
            else
            {
                ERRE_MSV("device not find, camera addrcode:%s", camAddr.c_str());
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
	else if ("getSystemTime" == jMethod)//to get LMT system time ,added by quxiaohui 20210422
	{
		SHL_Json::Value wvalue;
		SHL_Json::FastWriter writer;
		std::string lmt_time = sys->currentTime();
		wvalue["lmttime"] = lmt_time;
		jout = writer.write(wvalue);
		return 0;
	}
	else if ("getClientInfo" == jMethod)
	{
		SHL_Json::Value wvalue;
		SHL_Json::FastWriter writer;
		wvalue["clientPort"] = peerport();
		jout = writer.write(wvalue);
		return 0;
	}
	else if ("queryLaunchDeviceStatus" == jMethod)
	{
		if (NULL != launchPu)
		{
			return launchPu->onRequestConfig(jin, jout);
		}
		else//若无朗驰设备，直接返回可用
		{
			SHL_Json::FastWriter writer;
			rvalue["status"] = 1;
			jout = writer.write(rvalue);
			return 0;
		}
		return -1;
	}
	else if ("callAll" == jMethod)
	{
		string ip = rvalue["DevIp"].asString();
		int port = rvalue["DevPort"].asInt();
		if ("0.0.0.0" == ip)
		{
			vector<Pu *> IvaPu;
			putree->findIvaPu(IvaPu);
			int count = IvaPu.size();
			if (count == 0)
			{
				ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
				return -1;
			}
			else
			{
				int infocount = 0;
				SHL_Json::Value  CallALlinfo;
				string jsoutmp;
				for (int i = 0; i < count; i++)
				{
					SHL_Json::Value  Ivavalue;
					int res = IvaPu[i]->onRequestConfig(jin, jsoutmp);
					if (jsoutmp == "")
					{
						continue;
					}
					if (!reader.parse(jsoutmp, Ivavalue, false))
					{
						return -1;
					}
					CallALlinfo.append(Ivavalue);
					infocount++;
				}
				SHL_Json::Value  array;;
				array["size"] = infocount;
				array["callInfo"] = CallALlinfo;
				SHL_Json::StreamWriterBuilder writer;
				jout = SHL_Json::writeString(writer, array);
				return 0;
			}
		}
		else
		{
			Pu *pu = putree->findPu(ip, port);
			if (!pu)
			{
				ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
				return -1;
			}
			if (!pu->online())
			{
				ERRE_MSV("device not online, ip:%s, port:%d", ip.c_str(), port);
				return -1;
			}
			return pu->onRequestConfig(jin, jout);
		}

	}
	else if ("getrealpicture" == jMethod)
	{

		SHL_Json::Value      value;
		SHL_Json::StreamWriterBuilder writer;
		int pos;
		int res = -1;
		string ip = rvalue["DevIp"].asString();
		int port = rvalue["DevPort"].asInt();
		Pu *pu = putree->findPu(ip, port);
		if (!pu)
		{
			ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
			return -1;
		}
		CaptaureSession *ss = new CaptaureSession();
		long data = 0;
		ss->m_evtCaptaurepicture.init("CAPTAUREPICTURE", data);
		if (ss->start(jin) < 0)
		{
			ss->stop();
			delete ss;
			return -1;
		}
		
		if (!ss->m_evtCaptaurepicture.wait(10000) || data == -1)
		{
			ERRD_MSV("m_evtCaptaurepicture failed\n");
			ss->stop();
			if (ss)
				delete ss;
			return -1;
		};
#ifdef WIN32
		string tmpplatePath = sys->utf2ascii(systm->facePath());
#else
		string tmpplatePath = systm->facePath();
#endif
		value["version"] = "1.0";
		value["time"] = sys->currentTime();
		value["deviceName"] = pu->name();
		value["deviceHost"] = pu->host();
		value["devicePort"] = pu->port();
		value["addrcode"] = pu->addrcode();
		value["picSize"] = (int)ss->dwCapSize;
		value["width"] = ss->m_weidth;
		value["height"] = ss->m_height;
		//读取图片文件
		string picData = base64_encode((const unsigned char *)ss->m_pCapBuf, ss->dwCapSize);
		value["picData"] = picData;

		jout = SHL_Json::writeString(writer, value);
		string  strCurTime = sys->currentTime();
		pos = strCurTime.find(' ');
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
		FILE* pic = NULL;
		int fres = 0;
		sprintf(cFilename, "%s/JPEG_%s.jpeg", \
			tmpplatePath.c_str(), strCurTime.c_str());


		pic = fopen(cFilename, "wb");
		if (pic)
		{
			fwrite(ss->m_pCapBuf, ss->dwCapSize, 1, pic);
			fclose(pic);
		}
		ss->stop();
		if (ss)
			delete ss;
		return 0;
	}
    else
    {
        string jMethod = rvalue["Method"].asString();
        string ip = rvalue["DevIp"].asString();
        int port = rvalue["DevPort"].asInt();

        Pu *pu = putree->findPu(ip, port);
        if (!pu)
        {
            ERRE_MSV("device not find, ip:%s, port:%d", ip.c_str(), port);
            return -1;
        }
		if (!pu->online())
		{
			ERRE_MSV("device not online, ip:%s, port:%d", ip.c_str(), port);
			return -1;
		}
        return pu->onRequestConfig(jin, jout);
    }
}

int
Client::queryPreset(string &jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(jin, rvalue, false))
    {
        return -1;
    }

    int channelNo = rvalue["channelNo"].asInt();


    if (!rvalue["deviceHost"].isNull())
    {
        Pu *pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
        if (!pu)
        {
            return -1;
        }

        if (Pu::PROTO_TYPE_UT == pu->protocol())
        {
            string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);
            return pu->queryPreset(addrcode, jout);
        }
        else
        {
            return pu->queryPreset(channelNo, jout);
        }
    }
    else if (!rvalue["addrcode"].isNull())
    {
        string addrcode = rvalue["addrcode"].asString();
        Pu *pu = putree->findPu(addrcode);
        if (!pu)
        {
            return -1;
        }
        return pu->queryPreset(addrcode, jout);
    }

    return -1;
}

int Client::queryChannelStatus(string &jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    int iRet = -1;

    reader.parse(jin, rvalue, false);
    if (!rvalue["deviceHost"].isNull())
    {
        Pu *pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
        if (pu)
        {
            int channelNo = rvalue["channelNo"].asInt();
            iRet = pu->getChnStatus(channelNo, jout);
        }
    }
    else if (!rvalue["addrcode"].isNull())
    {
        string addrcode = rvalue["addrcode"].asString();
        Pu *pu = putree->findPu(addrcode);
        if (pu)
        {
            iRet = pu->getChnStatus(addrcode, jout);
        }
    }

    return iRet;
}

int
Client::setPreset(string &jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    int iRet = -1;
    if (!reader.parse(jin, rvalue, false))
    {
        ERRE_MSV("json format is invilid");
        return -1;
    }

    int channelNo = rvalue["channelNo"].asInt();


    if (!rvalue["addrcode"].isNull())
    {
        string addrcode = rvalue["addrcode"].asString();
        Pu *pu = putree->findPu(addrcode);
        if (pu)
        {
            iRet = pu->setPreset(addrcode, jin);
        }
    }
    else if(!rvalue["deviceHost"].isNull())
    {
        Pu *pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
        if (pu)
        {
            if (Pu::PROTO_TYPE_UT == pu->protocol())
            {
                string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);
                iRet = pu->setPreset(addrcode, jin);

            }
            else
            {
                iRet = pu->setPreset(channelNo, jin);
            }
        }
    }
    //if (iRet != 0)
    //{
    //    writeUserLog(STR_MSV_SET_PRESET_FAIL);
    //}
    //else
    //{
    //    writeUserLog(STR_MSV_SET_PRESET_SUCC);
    //}

    return iRet;
}

int
Client::queryRecord(string &jin, string &jout)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    int iRet = -1;

    if (!reader.parse(jin, rvalue, false))
    {
        return -1;
    }

    int type = rvalue["type"].asInt();
    int channelNo = rvalue["channelNo"].asInt();
    int fromIndex = rvalue["fromIndex"].asInt();
    int toIndex = rvalue["toIndex"].asInt();

    if (rvalue["start"].isNull() || rvalue["end"].isNull())
    {
        ERRE_MSV("invalidate start or end time string parameter.");
        return -1;
    }

    string     sstart = rvalue["start"].asString();
    string     send = rvalue["end"].asString();
    datetime_s start;
    datetime_s end;

    sys->strptime(sstart, start);
    sys->strptime(send, end);

    if (!rvalue["deviceHost"].isNull())
    {
        /* Pu *pu = puMgr->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt()); */
        Pu *pu = putree->findPu(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
        if (pu)
        {
            if (Pu::PROTO_TYPE_UT == pu->protocol())
            {
                string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);
                iRet = pu->queryRecord(addrcode, type, start, end, fromIndex, toIndex, jout, "");
            }
            else if (Pu::PROTO_TYPE_HIK == pu->protocol() && pu->type() == Pu::DEV_TYPE_IPC)
            {
			/** 20200603 zhangshenyu add  **/
                string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);
                string plateJson;
                char cond[512] = { 0 };
                sprintf(cond, "plate_camaddr='%s'", addrcode.c_str());
                int ret = cameraPlateCfg->query(plateJson, cond);
                if (0 == ret)
                {
                    SHL_Json::Reader readerPlate;
                    SHL_Json::Value  rvaluePlate;
                    if (!readerPlate.parse(plateJson, rvaluePlate, false))
                    {
                        return -1;
                    }
                    if (rvaluePlate.size() != 1)
                    {
                        return -1;
                    }
                    addrcode = rvaluePlate[0]["nvr_camaddr"].asString();
                    pu = putree->findPu(addrcode);
                }
                iRet = pu->queryRecord(addrcode, type, start, end, fromIndex, toIndex, jout, "");
              
            }
            else if (Pu::PRTO_TYPE_ONVIF == pu->protocol())
            {
				string addrcode = putree->findCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt(), channelNo);

				iRet = pu->queryRecord(channelNo, type, start, end, fromIndex, toIndex, jout);

            }
            else
            {
                int recordtype = Pu::RECORD_TYPE_ALL;
                if (0 == type)
                {
                    recordtype = Pu::RECORD_TYPE_ALL;
                }
                else if (1 == type)
                {
                    recordtype = Pu::RECORD_TYPE_TIMER;
                }
                else if (2 == type)
                {
                    recordtype = Pu::RECORD_TYPE_MD;
                }
                else if (3 == type)
                {
                    recordtype = Pu::RECORD_TYPE_VCOVER;
                }
                else if (4 == type)
                {
                    recordtype = Pu::RECORD_TYPE_LABEL;
                }

                iRet = pu->queryRecord(channelNo, recordtype, start, end, fromIndex, toIndex, jout);
            }
        }
    }
    else if (!rvalue["addrcode"].isNull())
    {
        string addrcode = rvalue["addrcode"].asString();

        Pu *pu = putree->findPu(addrcode);
        if (pu)
        {
		//20200603 zhangshenyu add
            string plateJson;
            char cond[512] = { 0 };
            sprintf(cond, "plate_camaddr='%s'", addrcode.c_str());
            int ret = cameraPlateCfg->query(plateJson, cond);
            if (0 == ret && plateJson != "")
            {
                SHL_Json::Reader readerPlate;
                SHL_Json::Value  rvaluePlate;
                if (!readerPlate.parse(plateJson, rvaluePlate, false))
                {
                    return -1;
                }
                if (rvaluePlate.size() != 1)
                {
                    return -1;
                }
                addrcode = rvaluePlate[0]["nvr_camaddr"].asString();
                pu = putree->findPu(addrcode);
            }

            if (Pu::PROTO_TYPE_GW_B == pu->protocol() ||
                Pu::PROTO_TYPE_GW_B_UT == pu->protocol() ||
                Pu::PROTO_TYPE_GW_B_HIK == pu->protocol() ||
                Pu::PROTO_TYPE_GW_B_HS == pu->protocol())
            {
                tevent<string, Query_Record_Resp_T> evtQueryRecord;
                pu->setQueryRecordCB(query_record_cb, &evtQueryRecord);//sip queryRecord is asynchronous
                Query_Record_Resp_T resp;
                while (1)
                {
                    evtQueryRecord.init(STRQUERYRECORD_C, resp);
                    iRet = pu->queryRecord(addrcode, type, start, end, fromIndex, toIndex, jout, "");
                    if (!evtQueryRecord.wait(5000) || resp.str == "")
                    {
                        ERRD_MSV("pu->queryRecord failed");
                        iRet = -1;
                        break;
                    }

                    if (resp.protocol == Pu::PROTO_TYPE_GW_B ||
                        resp.protocol == Pu::PROTO_TYPE_GW_B_UT ||
                        resp.protocol == Pu::PROTO_TYPE_GW_B_HIK ||
                        resp.protocol == Pu::PROTO_TYPE_GW_B_HS)
                    {
                        if (0 != Client::Gw2PrivRecordJson(resp.str, jout, fromIndex, toIndex))
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                int recordtype = Pu::RECORD_TYPE_ALL;
                if (0 == type)
                {
                    recordtype = Pu::RECORD_TYPE_ALL;
                }
                else if (1 == type)
                {
                    recordtype = Pu::RECORD_TYPE_TIMER;
                }
                else if (2 == type)
                {
                    recordtype = Pu::RECORD_TYPE_MD;
                }
                else if (3 == type)
                {
                    recordtype = Pu::RECORD_TYPE_VCOVER;
                }
                else if (4 == type)
                {
                    recordtype = Pu::RECORD_TYPE_LABEL;
                }

                iRet = pu->queryRecord(addrcode, recordtype, start, end, fromIndex, toIndex, jout, "");
            }
        }
    }

    //if (iRet != 0)
    //{
    //    writeUserLog(STR_MSV_QUERY_RECORD_FAIL);
    //}

    return iRet;
}

Session *
Client::startPlayback(string &json, Bufferev *bev)
{
    AutoLock locker(m_sessMgr._mtx);

    RecordSession *rs = NULL;

    if (T_PRIVATE == m_type)
    {
        rs = new PrivRecordSession(this, bev, 0);
    }
    else if (T_WEBSOCK == m_type)
    {
		rs = new WebsockRecordSession(this, bev, 0);
    }

    if (!rs)
    {
        return NULL;
    }

    if (rs->start(json, 0) < 0)
    {
        writeUserLog(STR_MSV_REQUEST_PLAYBACK_FAIL);
        delete rs;
        return NULL;
    }


    m_sessMgr.push_back(rs);
    writeUserLog(STR_MSV_REQUEST_PLAYBACK_SUCC);

    return rs;
}

Session *
Client::startDownload(string &json, Bufferev *bev)
{
    AutoLock locker(m_sessMgr._mtx);

    RecordSession *rs = NULL;

    if (T_PRIVATE == m_type)
    {
        rs = new PrivRecordSession(this, bev);
    }
    else if (T_WEBSOCK == m_type)
    {
		rs = new WebsockRecordSession(this, bev);
    }

    if (!rs)
    {
        return NULL;
    }

    if (rs->start(json, 1) < 0)
    {
        writeUserLog(STR_MSV_REQUEST_DOWNLOAD_FAIL);

        delete rs;
        return NULL;
    }

    m_sessMgr.push_back(rs);

    writeUserLog(STR_MSV_REQUEST_DOWNLOAD_SUCC);

    return rs;
}

DSession  *
Client::startDualTalk(string &json, Bufferev *bev)
{
    AutoLock locker(m_dutalkSessMgr._mtx);
    DualtalkSession *ds = new DualtalkSession(this, bev);

    if (!ds)
    {
        ERRE_MSV("new DualtalkSession failed");
        return NULL;
    }

    if (ds->start(json) < 0)
    {
        delete ds;
        return NULL;
    }

    m_dutalkSessMgr.push_back(ds);
    writeUserLog(STR_MSV_REQUEST_DUALTALK_SUCC);

    return ds;
}


int
Client::syncTime(string &json)
{
    /* TODO:  <08-05-18, yourname> */
    if (!ntpTime->beTime())
    {
        ERRE_MSV("be timing is off");
        return -1;
    }

    if (ntpTime->allowIp() != this->peerip())
    {

        ERRE_MSV("ip: %s is no allowed", peerip().c_str());
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string     stime = rvalue["time"].asString();
    datetime_s dtime;

    sys->strptime(stime, dtime);

    string tzone;
    if (!rvalue["zone"].isNull())
    {
        tzone = rvalue["zone"].asString();
    }

    if (sys->setLocalTime(dtime, tzone) < 0)
    {
        ERRE_MSV("sys->setLocalTime faild, time=%d-%d-%d %d:%d:%d",
            dtime.year, dtime.month, dtime.day, dtime.hour, dtime.minute, dtime.second);
        return -1;
    }

    return putree->syncTime(dtime, tzone);
}

void
Client::onTimer()
{
    {
        if (sys->currentTimeMillis() - m_lastHeart > 6000)
        {
            LOGE_MSV("heartbeat timeout, peerport:%d", peerport());
            m_active = false;
        }
    }


    {
        AutoLock locker(m_sessMgr._mtx);
        for (tlist<Session*>::iterator iter = m_sessMgr.begin(); iter != m_sessMgr.end();)
        {
            Session * ss = (*iter);
            if (!ss->active())
            {
                iter = m_sessMgr.erase(iter);
                ss->stop();
                delete ss;
            }
            else
            {
                ++iter;
            }
        }
    }

    {
        AutoLock locker(m_dutalkSessMgr._mtx);
        for (std::list<DSession *>::const_iterator i = m_dutalkSessMgr.begin(); i != m_dutalkSessMgr.end();)
        {
            DSession * ds = (*i);

            if (ds->status() == DSession::SS_CLOSED)
            {
                std::list<DSession *>::const_iterator ii = i++;
                m_dutalkSessMgr.erase(ii);
                ds->stop();
                delete ds;
            }
            else
            {
                i++;
            }
        }
    }
}

void
Client::setbev(Bufferev *bev)
{
    m_bev = bev;
    m_ip = m_bev->peerip();
}

void
Client::onRead(Bufferev *bev, void *data, uint32_t size)
{
    stk::HttpMsg httpMsg((const char *)data, size);

    string url = httpMsg.GetReqUrl();
    if ("/heartbeat" == httpMsg.GetReqUrl())
    {
        m_lastHeart = sys->currentTimeMillis();

        //LOGI_MSV("server recv message heartbeat!");

        const char *str = "HTTP/1.1 200 OK\r\n"
            "Connection: Keep-Alive\r\n\r\n";

        stk::HttpMsg rspMsg(str, strlen(str));
        string strMsg;
        rspMsg.to_str(strMsg);
        bev->push((uint8_t *)strMsg.c_str(), strMsg.size());
    }
    else
    {
        LOGE_MSV("Client::onRead:%s", (char*)data);
    }
}

void
Client::onClose(Bufferev *bev)
{
    m_active = false;
}

void
Client::onAlarm(string &json, void *data, uint32_t size)
{
    char *bufstream = new char[100 * 1024];
    if (!bufstream)
    {
        return;
    }
    memset(bufstream, 0, 100 * 1024);
    int  bufsize = 0;
    bool bret = buildResponsePacket(json, bufstream, bufsize);
    if (bret)
    {
        if (m_bev)
        {
            m_bev->push((uint8_t*)bufstream, bufsize);
        }
    }
    delete []bufstream;
}

void
Client::onAIYX(string &json, void *data, uint32_t size)
{

}

void
Client::onAIYC(string &json, void *data, uint32_t size)
{

}

void
Client::onChanged(const string &json)
{
    /* user has changed */
    m_active = false;
}

string
Client::peerip()
{
    if (m_bev)
    {
        return m_bev->peerip();
    }

    return "";
}

int
Client::peerport()
{
    if (m_bev)
    {
        return m_bev->peerport();
    }

    return 0;
}

string
Client::peertime()
{
    return m_time;
}

int	       
Client::GB281812PrivRecordJson(const string gwJson, string &privJson)
{
    SHL_Json::Reader reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  wvalue;

    if (!reader.parse(gwJson, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["Response"]["DeviceID"].asString();
    wvalue["fromIndex"] = 1;
    wvalue["realNum"] = atoi(rvalue["Response"]["SumNum"].asString().c_str());
    wvalue["toIndex"] = atoi(rvalue["Response"]["SumNum"].asString().c_str());

    SHL_Json::Value  item;

    if (!rvalue["Response"]["RecordList"]["Item"].isArray())
    {
        item["channelNo"] = Pu::GetChanNoByCode(addrcode);
        item["end"] = rvalue["Response"]["RecordList"]["Item"]["EndTime"];
        string fileUrl = rvalue["Response"]["RecordList"]["Item"]["FilePath"].asString();
        string url = fileUrl.substr(fileUrl.find("@") + 1, fileUrl.length());
        url = "rtsp:" + url;

        item["filename"] = url;
        item["fileurl"] = url;

        if (wvalue["records"].isNull())
        {
            item["index"] = 1;
        }
        else
        {
            item["index"] = wvalue["records"].size() + 1;
        }
        item["locked"] = 0;
        item["size"] = 1;
        item["start"] = rvalue["Response"]["RecordList"]["Item"]["StartTime"];
        item["type"] = atoi(rvalue["Response"]["RecordList"]["Item"]["Type"].asString().c_str());

        wvalue["records"].append(item);
    }
    else
    {
        for (int i = 0; i < rvalue["Response"]["RecordList"]["Item"].size(); ++i)
        {
            item["channelNo"] = Pu::GetChanNoByCode(addrcode);
            item["end"] = rvalue["Response"]["RecordList"]["Item"][i]["EndTime"];
            string fileUrl = rvalue["Response"]["RecordList"]["Item"][i]["FilePath"].asString();
            string url = fileUrl.substr(fileUrl.find("@") + 1, fileUrl.length());
            url = "rtsp:" + url;

            item["filename"] = url;
            item["fileurl"] = url;

            if (wvalue["records"].isNull())
            {
                item["index"] = 1;
            }
            else
            {
                item["index"] = wvalue["records"].size() + 1;
            }
            item["locked"] = 0;
            item["size"] = 1;
            item["start"] = rvalue["Response"]["RecordList"]["Item"][i]["StartTime"];
            item["type"] = atoi(rvalue["Response"]["RecordList"]["Item"][i]["Type"].asString().c_str());

            wvalue["records"].append(item);
        }
    }
    privJson = writer.write(wvalue);

    return 0;
}

int Client::Gw2PrivRecordJson(const string gwJson, string &privJson, int &fromIndex, int &toIndex)
{
    SHL_Json::Reader reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  wvalue;

    if (!reader.parse(gwJson,rvalue,false))
    {
        return -1;
    }

    if (privJson != "" && !reader.parse(privJson, wvalue, false))
    {
        return -1;
    }

    string addrcode = "";
    int FromIndex = 0;
    int ToIndex = 0;
    int RealNum = 0;
    int SubNum = 0;

    if (!rvalue["SIP_XML"]["SubList"]["@Code"].isNull())
    {
        addrcode = rvalue["SIP_XML"]["SubList"]["@Code"].asString();
    }
    if (!rvalue["SIP_XML"]["SubList"]["@FromIndex"].isNull())
    {
        FromIndex = atoi(rvalue["SIP_XML"]["SubList"]["@FromIndex"].asString().c_str());
    }
    if (!rvalue["SIP_XML"]["SubList"]["@ToIndex"].isNull())
    {
        ToIndex = atoi(rvalue["SIP_XML"]["SubList"]["@ToIndex"].asString().c_str());
    }
    if (!rvalue["SIP_XML"]["SubList"]["@RealNum"].isNull())
    {
        RealNum = atoi(rvalue["SIP_XML"]["SubList"]["@RealNum"].asString().c_str());
    }
    if (!rvalue["SIP_XML"]["SubList"]["@SubNum"].isNull())
    {
        SubNum = atoi(rvalue["SIP_XML"]["SubList"]["@SubNum"].asString().c_str());
    }

    if (wvalue["fromIndex"].isNull())
    {
        wvalue["fromIndex"] = FromIndex;
        wvalue["realNum"] = SubNum;
    }
    else
    {
        wvalue["realNum"] = wvalue["realNum"].asInt() + SubNum;
    }

    if (toIndex > RealNum)
    {
        toIndex = RealNum;
    }


    SHL_Json::Value valueArray;
    if (!rvalue["SIP_XML"]["SubList"]["Item"].isNull())
    {
        SHL_Json::Value  item;
        valueArray = rvalue["SIP_XML"]["SubList"]["Item"];
        if (1 == SubNum)
        {
            item["channelNo"] = Pu::GetChanNoByCode(addrcode);
            item["end"] = valueArray["@EndTime"];
            string fileUrl = valueArray["@FileUrl"].asString();
            string url = fileUrl.substr(fileUrl.find("@") + 1, fileUrl.length());
            url = "rtsp:" + url;

            item["filename"] = url;
            item["fileurl"] = url;

            if (wvalue["records"].isNull())
            {
                item["index"] = 1;
            }
            else
            {
                item["index"] = wvalue["records"].size() + 1;
            }
            item["locked"] = 0;
            item["size"] = atoi(valueArray["@Size"].asString().c_str());
            item["start"] = valueArray["@BeginTime"];
            item["type"] = atoi(valueArray["@Type"].asString().c_str());

            wvalue["records"].append(item);
        }
        else
        {
            for (int i = 0; i < SubNum; ++i)
            {
                item["channelNo"] = Pu::GetChanNoByCode(addrcode);
                item["end"] = valueArray[i]["@EndTime"];
                string fileUrl = valueArray[i]["@FileUrl"].asString();
                string url = fileUrl.substr(fileUrl.find("@") + 1, fileUrl.length());
                url = "rtsp:" + url;

                item["filename"] = url;
                item["fileurl"] = url;

                if (wvalue["records"].isNull())
                {
                    item["index"] = 1;
                }
                else
                {
                    item["index"] = wvalue["records"].size() + 1;
                }

                item["locked"] = 0;
                item["size"] = atoi(valueArray[i]["@Size"].asString().c_str());
                item["start"] = valueArray[i]["@BeginTime"];
                item["type"] = atoi(valueArray[i]["@Type"].asString().c_str());

                wvalue["records"].append(item);
            }
        }
    }
    privJson = writer.write(wvalue);

    if (ToIndex < toIndex)
    {
        fromIndex = ToIndex + 1;
        return 0;
    }
    else
    {
        return 1;
    }
}