#include "priv_task.h"
#include "priv_server.h"
#include "bufferev.h"
#include "system.h"
#include "json/json.h"
#include "stk/stk_message_http.h"
#include "tls_error.h"
#include "utility.h"
#include "usermgr.h"
#include "client.h"
#include "climgr.h"
#include "putree.h"
#include "session.h"
#include "cfg_user.h"
#include "cfg_group.h"
#include "view_user_group.h"
#include "pu.h"

#define MAX_BODY_SIZE	     (1024 * 1024)

PrivTask::PrivTask(Bufferev* bev)
{
    m_http = "";
    m_bodySize = 0;

    m_bev = bev;
    m_bev->addBuffevListener(this);

}

PrivTask::~PrivTask()
{

    m_bev->delBuffevListener(this);

}

void
PrivTask::stop()
{
    Task::stop();    
}


void
PrivTask::onRead(Bufferev *bev, void *data, uint32_t size)
{
    m_bev = bev;

    unsigned char * buf = (unsigned char *)data;
	for (uint32_t index = 0; index < size; ++index)
    {
        m_recvBuf.push_back(buf[index]);
    }


    while (1)
    {
        int index = 0;
        int rnNum = 0;
        for (index = 0; index < m_recvBuf.size(); ++index)
        {
            if (m_recvBuf[index] == '\r' ||
                m_recvBuf[index] == '\n')
            {
                rnNum++;
            }
            else
            {
                rnNum = 0;
            }

            if (4 == rnNum)
            {
                break;
            }
        }

        if (4 == rnNum)
        {
            stk::HttpMsg httpHeadMsg((char*)&m_recvBuf[0], index);
            int bodyTotal = httpHeadMsg.GetContentLength_int();
            if (m_recvBuf.size() >= (bodyTotal + index + 1))
            {
                int req_size = bodyTotal + index + 1;
                string str_http = (char*)&m_recvBuf[0];
                m_http += str_http.substr(0, bodyTotal + index + 1);
                m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + req_size);

                //LOGD_MSV(m_http.c_str());
                start();
            }
            else
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

void
PrivTask::onClose(Bufferev *bev)
{
	//m_bev = NULL;
	//m_status = S_CLOSED;
}

int
PrivTask::onLogin(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        SHL_Json::FastWriter writer;
        SHL_Json::Value      reason;

        ERRE_MSV("Invalidate request parameter.");
        reason["reason"] = getError();
        string body      = writer.write(reason);

        response("login", 404, body);

        return -1;
    }

    string user = rvalue["username"].asString();
    string pswd = rvalue["password"].asString();

    uint32_t code     = 200;
    uint64_t mright   = 0;
    uint64_t oright   = 0;
    bool     relogin  = true;
    string   userId   = "";
    string   usercode = "";
	int res = -1;
	if (userMgr != NULL)
	{
		 res = userMgr->authent(user, pswd, mright, oright, relogin, userId, usercode);
	}
    if (0 != res)
    {
        switch (res)
        {
        case 1:
            code = 401;
            ERRE_MSV("(%s) authentication failure", user.c_str());
            break;

        case 2:
            code = 401;
            ERRE_MSV("(%s) AAA authentication failure", user.c_str());
            break;

        case 3:
            code = 403;
            ERRE_MSV("(%s) has logined", user.c_str());
            break;

        case 4:
            code = 403;
            ERRE_MSV("(%s) permission denied", user.c_str());
            break;

        case 5:
            code = 404;
            ERRE_MSV("(%s) not exist", user.c_str());
            break;

        default:
            code = 400;
            ERRE_MSV("(%s) unkown reason", user.c_str());
            break;
        }

        SHL_Json::FastWriter writer;
        SHL_Json::Value      reason;

        reason["reason"] = getError();
        string body      = writer.write(reason);

        response("login", code, body);

        return -1;
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    GnuID userkey;
    userkey.generate();

    value["version"]   = "1.0";
    value["key"]       = userkey.toStr();
    value["msvcode"]   = systm->addrcode();
    value["msvname"]   = systm->name();
    value["usercode"]  = usercode;
    value["mright"]    = mright;
    value["oright"]    = oright;
    value["relogin"]   = relogin;
    value["deviceNum"] = putree->child_num(systm->addrcode());

    string body        = writer.write(value);
    response("login", 200, body);
	if (cliMgr != NULL)
	{
		cliMgr->addClient(m_bev, Client::T_PRIVATE, user, userId, usercode, mright, oright, userkey.toStr());
	}

    return 0;
}

int
PrivTask::onSubscribe(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    uint32_t     code = 200;
    Client     * cli  = NULL;
    uint64_t     type = 0;
    string       body = "";

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter,json:%s", json.c_str());
        goto exit;
    }
	if (cliMgr != NULL)
	{
		cli = cliMgr->findClient(rvalue["userKey"].asString());
	}
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (!cli->hasRight(Client::R_PUSH))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        goto exit;
    }

    cli->clearPush();

    type = rvalue["typeBitmap"].asUInt64();
    cli->setPush(type);

    response("subscribe", code, body);

    return 0;

exit:
    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    body = writer.write(reason);
    response("subscribe", code, body);

    return -1;
}

int PrivTask::onSubscribeStatus(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    uint32_t     code = 200;
    Client     * cli  = NULL;
    uint64_t     type = 0;
    string       body = "";

    do
    {
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 403;
            ERRE_MSV("Illegal invasion");
            break;
        }

        int enableTrigger = rvalue["enable"].asInt();
        if (enableTrigger == 1)
        {
            systm->addTriggerListener(cli);
        }
        else
        {
            systm->delTriggerListener(cli);
        }

        response("subscribeStatus", code, body);
        return 0;
    }while(0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    body = writer.write(reason);
    response("subscribeStatus", code, body);

    return -1;
}

int
PrivTask::onQueryUser()
{
	SHL_Json::Value rvalue;
	SHL_Json::Reader reader;
	SHL_Json::Value va;
	SHL_Json::FastWriter wr;

    std::string user;
    std::string group;
    std::string userGr;
    std::string body = "";

	do 
	{
		int ret = userCfg->query(user);
		if (ret < 0)
		{
			break;
		}

		if (!(reader.parse(user, rvalue, false) && rvalue.isArray()))
		{
			break;
        }
		va["users"] = rvalue;
        body = wr.write(va);

        //20200620 zhangshenyu add
        ret = groupCfg->query(group);
        if (ret < 0)
        {
            break;
        }

        if (!(reader.parse(group, rvalue, false) && rvalue.isArray()))
        {
            break;
        }
        va["groups"] = rvalue;
        body = wr.write(va);

        ret = usrgrView->query(userGr);
        if (ret < 0)
        {
            break;
        }

        if (!(reader.parse(userGr, rvalue, false) && rvalue.isArray()))
        {
            break;
        }
        va["userGrs"] = rvalue;
        body = wr.write(va);


		response("queryUser", 200, body);

		return 0;
	} while (0);

	body = "query error";
	response("queryUser", 404, body);
    return -1;
}

int PrivTask::onQueryChannelStatus(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter writer;

    uint32_t code = 200;
    Client * cli  = NULL;
    string   body = "";


    do
    {
        wvalue["version"]   = "v1.0";
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            wvalue["reason"] = getError();
            break;
        }

        cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 405;
            ERRE_MSV("Illegal invasion");
            wvalue["reason"] = getError();
            break;
        }

        if (!cli->hasRight(Client::R_QUERY))
        {
            code = 403;
            ERRE_MSV("Permission denied");
            wvalue["reason"] = getError();
            break;
        }

        if (cli->queryChannelStatus(json, body) < 0)
        {
            code = 404;
            wvalue["reason"] = getError();
            break;
        }
        response("queryChannel", code, body);
        return 0;

    }while(0);

    body = writer.write(wvalue);
    response("queryChannel", code, body);
    return 0;
}

int
PrivTask::onQueryDevice(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::Value      array;
    SHL_Json::FastWriter writer;

    uint32_t code = 200;
    Client * cli  = NULL;
    string   jout = "";
    string   cond = "";
    int      type = -1;

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter,json:%s", json.c_str());
        goto exit;
    }

    cli = cliMgr->findClient(rvalue["userKey"].asString());
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (!cli->hasRight(Client::R_QUERY))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        goto exit;
    }

    type = rvalue["type"].asInt();

	if (rvalue["addrcode"].isNull() || rvalue["addrcode"].asString() == "")
    {
		if (rvalue["deviceHost"].isNull())
		{
			code = 404;
			ERRE_MSV("Invalidate request parameter");
			goto exit;
		}
		else
		{
			string t_addrcode = putree->findDevCode(rvalue["deviceHost"].asString(), rvalue["devicePort"].asInt());
			if (t_addrcode == "")
			{
				code = 404;
				ERRE_MSV("Invalidate request parameter");
				goto exit;
			}
			putree->traversal_child(t_addrcode, get_nodedata, (void *)&wvalue["devices"]);
		}
    }
	else
		putree->traversal_child(rvalue["addrcode"].asString(), get_nodedata, (void *)&wvalue["devices"]);

    wvalue["version"]   = rvalue["version"];
    wvalue["deviceNum"] = wvalue["devices"].size();

    jout = writer.write(wvalue);

    if (jout.size() > MAX_BODY_SIZE)
    {
        code = 404;
        ERRE_MSV("Body is too larger: %d", jout.size());
        goto exit;
    }

    response("queryDevice", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("queryDevice", code, body);

    return -1;
}

int
PrivTask::onQueryPreset(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter writer;

    string   jout;
    uint32_t code = 200;
    Client * cli = NULL;

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter");
        goto exit;
    }

    cli = cliMgr->findClient(rvalue["userKey"].asString());
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (cli->queryPreset(json, jout) < 0)
    {
        code = 404;
        goto exit;
    }

    if (!reader.parse(jout, wvalue, false))
    {
        code = 404;
        ERRE_MSV("Query preset fail");
        goto exit;
    }

    wvalue["version"] = rvalue["version"];

    jout = writer.write(wvalue);

    if (jout.size() > MAX_BODY_SIZE)
    {
        code = 404;
        ERRE_MSV("Body is too larger: %d", jout.size());
        goto exit;
    }

    response("queryPreset", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["reason"] = getError();
    string body = writer.write(reason);

    response("queryPreset", code, body);

    return -1;
}

int
PrivTask::onSetPreset(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter writer;

    string   jout;
    uint32_t code = 200;
    Client * cli = NULL;

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter");
        goto exit;
    }

    cli = cliMgr->findClient(rvalue["userKey"].asString());
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (cli->setPreset(json, jout) < 0)
    {
        code = 404;
        goto exit;
    }

    jout = "";
    response("setPreset", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["reason"] = getError();
    string body = writer.write(reason);

    response("setPreset", code, body);

    return -1;
}

int
PrivTask::onPreview(string json)
{
    uint32_t  code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        int channelNo = 0;
        string addrcode = "";
        int obtainType = rvalue["obtainType"].asInt();
        int streamType = rvalue["streamType"].asInt();
        int transport = rvalue["transport"].asInt();
        Pu* pu = NULL;
        if (0 == obtainType || 1 == obtainType)
        {
            string deviceHost = rvalue["deviceHost"].asString();
            int devicePort = rvalue["devicePort"].asInt();
            channelNo = rvalue["channelNo"].asInt();
            pu = putree->findPu(deviceHost, devicePort);
        }
        else
        {
            addrcode = rvalue["addrcode"].asString();
            pu = putree->findPu(addrcode);
        }
        if (pu == NULL)
        {
            code = 405;
            ERRE_MSV("device not find");
            break;
        }
        bool isv = false;
        if (0 == obtainType || 1 == obtainType)
        {
            isv = pu->validate(channelNo);
        }
        else
        {
            isv = pu->validate(addrcode);
        }

        if (isv == false)
        {
            code = 406;
			ERRE_MSV("chn:%d not online", channelNo);
            break;
        }
        string body = "";
        response("preview", code, body);

        if (cliMgr->startPreview(json, rvalue["userKey"].asString(), m_bev, code) < 0)
        {
            LOGI_MSV("cliMgr->startPreview %s", getError());
            //break;
        }

        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("preview", code, body);

    return -1;
}

int
PrivTask::onPtzCtrl(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;

    uint32_t code = 200;
    Client * cli  = NULL;
    string   body = "";

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter");
        goto exit;
    }

    cli = cliMgr->findClient(rvalue["userKey"].asString());
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (!cli->hasRight(Client::R_CTRL))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        goto exit;
    }

    /* Control rights arbitration */
    /* if (0 != cliMgr->ctrlRightArbit(cli)) */
    if (false)
    {
        code = 403;
        /* reason["reason"] = "High level user is operating"; */
        ERRE_MSV("High level user is operating");
        goto exit;

    }

    if (cli->ptzCtrl(json) < 0)
    {
        code = 404;
        goto exit;
    }

    response("ptzCtrl", code, body);

    return 0;

exit:
    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    body = writer.write(reason);

    response("ptzCtrl", code, body);

    return -1;
}




int
PrivTask::onQueryRecord(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter writer;

    string   jout;
    uint32_t code = 200;
    Client * cli  = NULL;

    if (!reader.parse(json, rvalue, false))
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter");
        goto exit;
    }

    cli = cliMgr->findClient(rvalue["userKey"].asString());
    if (!cli)
    {
        code = 403;
        ERRE_MSV("Illegal invasion");
        goto exit;
    }

    if (!cli->hasRight(Client::R_PLAYBACK)
            || !cli->hasRight(Client::R_DOWNLOAD))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        goto exit;
    }

    if (cli->queryRecord(json, jout) < 0)
    {
        code = 404;
        goto exit;
    }

    if (!reader.parse(jout, wvalue, false))
    {
        code = 404;
        ERRE_MSV("Query record fail");
        goto exit;
    }

    wvalue["version"] = rvalue["version"];

    jout = writer.write(wvalue);

    if (jout.size() > MAX_BODY_SIZE)
    {

        code = 404;
        ERRE_MSV("Body is too larger: %d", jout.size());
        goto exit;

    }

    response("queryRecord", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("queryRecord", code, body);

    return -1;
}

int
PrivTask::onPlayback(string json)
{
    uint32_t code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        int channelNo = 0;
        string addrcode = "";
        int obtainType = rvalue["obtainType"].asInt();
        int streamType = rvalue["streamType"].asInt();
        int transport = rvalue["transport"].asInt();
        Pu* pu = NULL;
        if (0 == obtainType || 1 == obtainType)
        {
            string deviceHost = rvalue["deviceHost"].asString();
            int devicePort = rvalue["devicePort"].asInt();
            channelNo = rvalue["channelNo"].asInt();
            pu = putree->findPu(deviceHost, devicePort);
        }
        else
        {
            addrcode = rvalue["addrcode"].asString();
            pu = putree->findPu(addrcode);
        }
        if (pu == NULL)
        {
            code = 405;
            ERRE_MSV("device not find");
            break;
        }
        string body = "";
        response("playback", code, body);

        if (cliMgr->startPlayback(json, rvalue["userKey"].asString(), m_bev, code) < 0)
        {
            //LOGI_MSV("%s", getError());
            //break;
        }

        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("playback", code, body);

    return -1;
}

int
PrivTask::onDownload(string json)
{
    uint32_t code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        int channelNo = 0;
        string addrcode = "";
        int obtainType = rvalue["obtainType"].asInt();
        int streamType = rvalue["streamType"].asInt();
        int transport = rvalue["transport"].asInt();
        Pu* pu = NULL;
        if (0 == obtainType || 1 == obtainType)
        {
            string deviceHost = rvalue["deviceHost"].asString();
            int devicePort = rvalue["devicePort"].asInt();
            channelNo = rvalue["channelNo"].asInt();
            pu = putree->findPu(deviceHost, devicePort);
        }
        else
        {
            addrcode = rvalue["addrcode"].asString();
            pu = putree->findPu(addrcode);
        }
        if (pu == NULL)
        {
            code = 405;
            ERRE_MSV("device not find");
            break;
        }

        string body = "";
        response("download", code, body);

        if (cliMgr->startDownload(json, rvalue["userKey"].asString(), m_bev, code) < 0)
        {
            //LOGI_MSV("%s", getError());
            //break;
        }


        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("download", code, body);

    return -1;
}

int
PrivTask::onSyncTime(string json)
{
    uint32_t code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        Client * cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 403;
            ERRE_MSV("Illegal invasion");
            break;
        }

        if (!cli->hasRight(Client::R_SYNCTIME))
        {
            code = 403;
            ERRE_MSV("Permission denied");
            break;
        }

        if (cli->syncTime(json) < 0)
        {
            code = 404;
            LOGI_MSV("%s", getError());
            break;
        }

        string body = "";
        response("syncTime", code, body);

        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body      = writer.write(reason);

    response("syncTime", code, body);

    return -1;
}

int
PrivTask::onDualTalk(string json)
{
    uint32_t code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }



#if 0
        Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 403;
            ERRE_MSV("Illegal invasion");
            break;
        }

        if (!cli->hasRight(Client::R_DUALTALK))
        {
            code = 403;
            ERRE_MSV("Permission denied");
            break;
        }
        
        DSession * rs = cli->startDualTalk(json);
        if (!rs)
        {
            code = 404;
            LOGI_MSV("%s", getError());
            break;
        }


        privSV->addBuffevListener(m_bev, rs);
        rs->setbev(m_bev);
#endif

        if (cliMgr->startTalk(json, rvalue["userKey"].asString(), m_bev, code) < 0)
        {
            LOGI_MSV("%s", getError());
            break;
        }



        string body = "";
        if (response("dualtalk", code, body) < 0)
        {
            return -1;
        }


        return 0;
    } while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body = writer.write(reason);

    response("dualtalk", code, body);

    return -1;

}

int        
PrivTask::onCustomCommand(string json)
{
    uint32_t code = 200;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 403;
            ERRE_MSV("Illegal invasion");
            break;
        }



        int ret = cli->customCommand(json);
        if (0 != ret)
        {
            code = 404;
            LOGI_MSV("%s", getError());
            break;
        }

        string body = "";
        if (response("customCommand", code, body) < 0)
        {
            return -1;
        }

        return 0;
    } while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["reason"] = getError();
    string body = writer.write(reason);

    response("customCommand", code, body);

    return -1;
}

int
PrivTask::onGetConfig(string json)
{
    uint32_t code = 200;
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::FastWriter writer;

    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            ERRE_MSV("Invalidate request parameter");
            break;
        }

        Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
        if (!cli)
        {
            code = 403;
            ERRE_MSV("Illegal invasion");
            break;
        }

        string   jout;
        int ret = cli->requestConfig(json, jout);
        if (0 != ret)
        {
            code = 404;
            LOGI_MSV("%s", getError());
            break;
        }



        if (!reader.parse(jout, wvalue, false))
        {
            code = 404;
            ERRE_MSV("Query record fail");
            break;
        }

        wvalue["version"] = rvalue["version"];

        jout = writer.write(wvalue);

        if (jout.size() > MAX_BODY_SIZE)
        {
            code = 404;
            ERRE_MSV("Body is too larger: %d", jout.size());
            break;
        }

        if (response("getConfig", code, jout) < 0)
        {
            return -1;
        }

        return 0;
    } while (0);

    
    SHL_Json::Value      reason;
    reason["reason"] = getError();
    string body = writer.write(reason);

    response("getConfig", code, body);

    return -1;
}

int
PrivTask::response(string type, int code, string &body)
{
    char buf[100] = {0};
    snprintf_s(buf, sizeof(buf), "HTTP/1.1 %d OK\r\n", code);

    stk::HttpMsg http(buf, strlen(buf));

    if (body.size() > 0)
    {
        http.SetContentType("application/json");
        http.SetBody(body.c_str());
    }

    resp_s resp;
    resp.type = type;
    http.to_str(resp.buff);

	if (m_bev)
	{
		m_bev->push((uint8_t*)resp.buff.c_str(), resp.buff.size());
	}
    

    m_status = S_CLOSE_WAIT;

    return 0;
}

int
PrivTask::process()
{
    stk::HttpMsg http(m_http.c_str(), m_http.size());
    string url = http.GetReqUrl();

    if ("/login" == url)
    {
        onLogin(http.GetBody());
		LOGW_MSV("(url:peerIP:peerPort)=%s:%s:%d, body:%s", url.c_str(), m_bev->peerip().c_str(), m_bev->peerport(), http.GetBody().c_str());
    }
    else if ("/subscribe" == url)
    {
        onSubscribe(http.GetBody());
    }

    else if ("/preview" == url)
    {
        onPreview(http.GetBody());
    }

    else if ("/playback" == url)
    {
        onPlayback(http.GetBody());
    }

    else if ("/download" == url)
    {
        onDownload(http.GetBody());
    }

    else if ("/queryDevice" == url)
    {
        onQueryDevice(http.GetBody());
    }

    else if ("/queryPreset" == url)
    {
        onQueryPreset(http.GetBody());
    }

    else if ("/setPreset" == url)
    {
        onSetPreset(http.GetBody());
    }

    else if ("/queryRecord" == url)
    {
        onQueryRecord(http.GetBody());
    }

    else if ("/ptzCtrl" == url)
    {
        onPtzCtrl(http.GetBody());
		LOGW_MSV("(url:peerIP:peerPort)=%s:%s:%d, body:%s", url.c_str(), m_bev->peerip().c_str(), m_bev->peerport(), http.GetBody().c_str());
    }
    else if ("/syncTime" == url)
    {
        onSyncTime(http.GetBody());
    }

    else if ("/dualtalk" == url)
    {
        onDualTalk(http.GetBody());
    }

    else if ("/customCommand" == url)
    {
        onCustomCommand(http.GetBody());
		LOGW_MSV("(url:peerIP:peerPort)=%s:%s:%d, body:%s", url.c_str(), m_bev->peerip().c_str(), m_bev->peerport(), http.GetBody().c_str());
    }

    else if ("/getConfig" == url)
    {
        onGetConfig(http.GetBody());
    }
    
    else if ("/setFace" == url)
    {
        onCustomCommand(http.GetBody());
    }
    else if ("/lockFile" == url)
    {
        onCustomCommand(http.GetBody());
    }
    else if ("/ptzPatternCtrl" == url)
    {
        onCustomCommand(http.GetBody());
    }
	else if ("/setSystemTime" == url)
    {
        onCustomCommand(http.GetBody());
    }
    else if ("/subscribeStatus" == url)
    {
        onSubscribeStatus(http.GetBody());
    }
	else if ("/queryUser" == url)
	{
		onQueryUser();
	}
    else if ("/queryChannel" == url)
    {
        onQueryChannelStatus(http.GetBody());
    }
    else
    {
        SHL_Json::Value      wvalue;
        SHL_Json::FastWriter writer;
        wvalue["version"] = "v1.0";
        wvalue["reason"] = "not found";
        string body = writer.write(wvalue);
        response("unknowRequest", 404, body);
    }

    //privSV->delBuffevListener(m_bev, this);
    

    m_http = "";
    m_bodySize = 0;
    return 0;
}
