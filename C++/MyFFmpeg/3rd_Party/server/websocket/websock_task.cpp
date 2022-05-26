#define _CRT_SECURE_NO_WARNINGSs
#include <regex>
#include "websock_task.h"
#include "websock_server.h"
#include "bufferev.h"
#include "system.h"
#include "json/json.h"
#include "tls_error.h"
#include "utility.h"
#include "usermgr.h"
#include "client.h"
#include "climgr.h"
#include "putree.h"
#include "session.h"
#include "websock_frame.h"
#include "sha.h"
#include "base64_ex.h"
#include "putree.h"
#include "pu.h"

WebSocketTask::WebSocketTask()
{

}

WebSocketTask::~WebSocketTask()
{

}

void
WebSocketTask::stop()
{
    Task::stop();

    websockSV->delBuffevListener(m_bev, this); 
}

void
WebSocketTask::handShake(Bufferev *bev, void *data, uint32_t size)
{
    const char *key1 = NULL, *origin = NULL, *resource = NULL;
    char requestbuf[100*BUFSIZ];
    unsigned char responsebuf[100 * BUFSIZ];
    memset(requestbuf, 0, 100 * BUFSIZ);
    memset(responsebuf, 0, 100 * BUFSIZ);

    strncpy(requestbuf, (char*)data, size);

    char* key1pattern = "(Sec-WebSocket-Key:)[[:s:]](.+\\r\\n)";
    char* resourcePattern = "(GET)[[:s:]](/[[:alnum:]]+)";
    char* originPattern = "(Origin:)[[:s:]](.+)\\r\\n";

    match_results<const char*> m;
    regex rx;
    string s;

    //match Sec-WebSocket-Key1
    m = match_results<const char*>();
    rx = regex(key1pattern);
    regex_search(requestbuf, m, rx);
    s = m[2];
    string strNew = s.substr(0, s.length() - 2);
    key1 = strdup(strNew.c_str());

    //match origin
    m = match_results<const char*>();
    rx = regex(originPattern);
    regex_search(requestbuf, m, rx);
    s = m[2];
    origin = strdup(s.c_str());

    //match GET (resource)
    m = match_results<const char*>();
    rx = regex(resourcePattern);
    regex_search(requestbuf, m, rx);
    s = m[2];
    resource = strdup(s.c_str());

    if (key1 == NULL || resource == NULL)
    {
        return;
    }

    //create handshake response:
    std::string server_key = key1;
    server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    SHA1 sha;
    unsigned int message_digest[5];
    sha.Reset();
    sha << server_key.c_str();

    sha.Result(message_digest);

    for (int i = 0; i < 5; i++)
    {
        message_digest[i] = htonl(message_digest[i]);
    }

    server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);

    char* chandshake = (char*)calloc(BUFSIZ, sizeof(char));
    char * handshakeFormat = "HTTP/1.1 101 Switching Protocols\r\n"
                             "Upgrade: WebSocket\r\n"
                             "Connection: Upgrade\r\n"
                             "Sec-WebSocket-Accept: %s\r\n"
                             "\r\n";
    sprintf(chandshake, handshakeFormat, server_key.c_str());

    for (int i = 0; i < strlen(chandshake); i++)
    {
        responsebuf[i] = chandshake[i];
    }

    bev->write(responsebuf, 129);

    free(chandshake);

    return;
}

void
WebSocketTask::onRead(Bufferev *bev, void *data, uint32_t size)
{
	char maskKey[4] = { 0 };
	char buffer[300 * 1024] = { 0 };
	int  pos = 0;
	unsigned char * buf = (unsigned char *)data;

	if (buf[size - 2] == '\r' &&
		buf[size - 1] == '\n')
	{
		handShake(bev, data, size);
	}
	else
	{
		for (uint32_t index = 0; index < size; ++index)
		{
			m_recvBuf.push_back(buf[index]);
		}

		do
		{
			uint8_t u8TwoChar[3] = { 0 };
            if (m_recvBuf.size() <= 1)
            {
                return;
            }

			memcpy((char*)u8TwoChar, (char*)&m_recvBuf[0], 2);

			int OpCode = u8TwoChar[0];
			if (136 == OpCode)
			{
				m_status = S_CLOSED;
				break;
			}
			else
			{
				int len = u8TwoChar[1] & 0x7F;
				if (len == 0)
				{
					m_status = S_CLOSED;
					break;
				}

				int packetlen = 0;
				if (len < 126)
				{
					memcpy(maskKey, (char*)(&m_recvBuf[2]), 4);
					pos = 6;
				}
				else if (len == 126)
				{
					len = m_recvBuf[3] + (m_recvBuf[2] << 8);
					memcpy(maskKey, (char*)(&m_recvBuf[4]), 4);
					pos = 8;
				}
				else if (len == 127)
				{
					len = m_recvBuf[6];
					len = (len << 8) + m_recvBuf[7];
					len = (len << 8) + m_recvBuf[8];
					len = (len << 8) + m_recvBuf[9];

					memcpy(maskKey, (char*)(&m_recvBuf[10]), 4);
					pos = 14;
				}
				else
				{
					m_status = S_CLOSED;
					return;
				}

				packetlen = len + pos;
				if (m_recvBuf.size() < packetlen)
				{
					break;
				}

				char u8PacketBuf[500 * 1024] = { 0 };
				memcpy(u8PacketBuf, (char*)&m_recvBuf[0], packetlen);

				m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + packetlen);

				for (int i = 0; i < len; i++)
				{
					int j = i % 4;
					buffer[i] = u8PacketBuf[i + pos] ^ maskKey[j];
				}

				m_req = buffer;

				printf("recv request: %s !!!!!!!!!!!!\n", buffer);

				start();
				break;
			}
		} while (1);
	}
}

void
WebSocketTask::onClose(Bufferev *bev)
{

}

int
WebSocketTask::onLogin(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        SHL_Json::FastWriter writer;
        SHL_Json::Value      reason;

        ERRE_MSV("Invalidate request parameter.");
        reason["reason"] = getError();
        string body = writer.write(reason);

        response("login", 404, body);

        return -1;
    }

    string user = rvalue["username"].asString();
    string pswd = rvalue["password"].asString();

    uint32_t code = 200;
    uint64_t mright = 0;
    uint64_t oright = 0;
    bool     relogin = true;
    string   userId = "";
    string   usercode = "";

    int res = userMgr->authent(user, pswd, mright, oright, relogin, userId, usercode);

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
        string body = writer.write(reason);

        response("login", code, body);

        return -1;
    }


    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    GnuID userkey;
    userkey.generate();

    value["version"] = "1.0";
    value["key"]     = userkey.toStr();
    value["command"] = "login";

    value["msvcode"] = systm->addrcode();
    value["msvname"] = systm->name();
    value["usercode"] = usercode;
    value["mright"] = mright;
    value["oright"] = oright;
    value["relogin"] = relogin;
    value["deviceNum"] = putree->child_num(systm->addrcode());
    string body = writer.write(value);

    response("login", 200, body);



    cliMgr->addClient(m_bev, Client::T_WEBSOCK, user, userId, usercode, mright, oright, userkey.toStr());

    return 0;
}

int
WebSocketTask::onSubscribe(string json)
{
    return 0;
}

int
WebSocketTask::onQueryDevice(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;
    SHL_Json::Value      wvalue;
    SHL_Json::Value      array;
    SHL_Json::FastWriter writer;

    uint32_t code = 200;
    Client * cli = NULL;
    string   jout = "";
    string   cond = "";
    int      type = -1;

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

    if (!cli->hasRight(Client::R_QUERY))
    {
        code = 403;
        ERRE_MSV("Permission denied");
        goto exit;
    }

    type = rvalue["type"].asInt();

    if (rvalue["addrcode"].isNull())
    {
        code = 404;
        ERRE_MSV("Invalidate request parameter");
        goto exit;
    }

    putree->traversal_child(rvalue["addrcode"].asString(), get_nodedata, (void *)&wvalue["devices"]);

    wvalue["version"] = rvalue["version"];
    wvalue["deviceNum"] = wvalue["devices"].size();

    jout = writer.write(wvalue);

    response("queryDevice", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["reason"] = getError();
    string body = writer.write(reason);

    response("queryDevice", code, body);

    return -1;
}

int
WebSocketTask::onPreview(string json)
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
    string body = writer.write(reason);

    response("preview", code, body);

    return -1;
}

int
WebSocketTask::onPtzCtrl(string json)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      rvalue;

    uint32_t code = 200;
    Client * cli = NULL;
    string   body = "ok";

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
        ERRE_MSV("%s", getError());
        goto exit;

    }

    if (cli->ptzCtrl(json) < 0)
    {
        code = 404;
        LOGE_MSV("%s", getError());
        goto exit;
    }

    response("ptzCtrl", code, body);

    return 0;

exit:
    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["command"] = "ptzCtrl";
    reason["reason"] = getError();
    body = writer.write(reason);

    response("ptzCtrl", code, body);

    return -1;
}

int
WebSocketTask::onQueryRecord(string json)
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
        LOGE_MSV("%s", getError());
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

    response("queryRecord", code, jout);

    return 0;

exit:
    SHL_Json::Value reason;

    reason["command"] = "queryDevice";
    reason["reason"] = getError();
    string body = writer.write(reason);

    response("queryDevice", code, body);

    return -1;
}

int
WebSocketTask::onPlayback(string json)
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

        if (!cli->hasRight(Client::R_PLAYBACK))
        {
            code = 403;
            ERRE_MSV("Permission denied");
            break;
        }

        Session * rs = cli->startPlayback(json, m_bev);

        if (!rs)
        {
            code = 404;
            LOGE_MSV("%s", getError());
            break;
        }

        string body = "";

        websockSV->addBuffevListener(m_bev, rs);
        rs->setbev(m_bev);

        response("playback", code, body);
        
        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["command"] = "playback";
    reason["reason"] = getError();
    string body = writer.write(reason);

    response("playback", code, body);

    return -1;
}

int
WebSocketTask::onDownload(string json)
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

        if (!cli->hasRight(Client::R_DOWNLOAD))
        {
            code = 403;
            ERRE_MSV("Permission denied");
            break;
        }

        Session * rs = cli->startDownload(json, m_bev);

        if (!rs)
        {
            code = 404;
            LOGE_MSV("%s", getError());
            break;
        }

        string body = "";

        response("download", code, body);

        websockSV->addBuffevListener(m_bev, rs);
        rs->setbev(m_bev);

        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["command"] = "download";
    reason["reason"] = getError();
    string body = writer.write(reason);

    response("download", code, body);

    return -1;
}

int
WebSocketTask::onSyncTime(string json)
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

        /* Control rights arbitration */
        /* if (0 != cliMgr->ctrlRightArbit(cli)) */
        if (false)
        {
            code = 403;
            /* reason["reason"] = "High level user is operating"; */
            LOGE_MSV("%s", getError());
            break;
        }

        if (cli->syncTime(json) < 0)
        {
            code = 404;
            LOGE_MSV("%s", getError());
            break;
        }

        string body = "";
        response("syncTime", code, body);

        return 0;
    }
    while (0);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      reason;

    reason["command"] = "syncTime";
    reason["reason"]  = getError();
    string body = writer.write(reason);

    response("syncTime", code, body);

    return -1;
}

int
WebSocketTask::response(string type, int code, string &data)
{
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"] = "v1.0";
    value["msg"]     = type;
    value["code"]    = code;
    value["data"]    = data.c_str();

    string body = writer.write(value);

    char buffer[500 * 1024] = { 0 };
    int len = body.size() + 4;
    memcpy(buffer, &len, sizeof(int));
    memcpy(buffer + sizeof(int), (char*)body.c_str(), body.size());

    WebSockFrame dr = WebSockFrame(buffer, len);

    int nsize;
    char* msg = dr.GetBytes(nsize);
    if (msg)
    {
        if (m_bev)
        {
            m_bev->push((uint8_t*)msg, nsize);
        }

        delete[] msg;
    }

    m_status = S_CLOSE_WAIT;

    return 0;
}

int 
WebSocketTask::onSetFace(string json)
{
	uint32_t code = 200;

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	string body = "";

	do
	{
		if (!reader.parse(json, rvalue, false))
		{
			code = 404;
			ERRE_MSV("Invalidate request parameter");
			break;
		}

		SHL_Json::FastWriter writer;
		SHL_Json::Value      reason;

		reason["command"] = "setFace";
		reason["DevIp"] = rvalue["DevIp"];
		reason["DevPort"] = rvalue["DevPort"];
		reason["DevChn"] = rvalue["DevChn"];
		reason["ID"] = rvalue["ID"];
		body = writer.write(reason);

		Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
		if (!cli)
		{
			code = 403;
			ERRE_MSV("Illegal invasion");
			break;
		}

		int ret = cli->customCommandEx(json);
		if (0 != ret)
		{
			code = 401;
            LOGE_MSV("%s", getError());
			break;
		}	

        response("setFace", code, body);

		return 0;
	} while (0);

	response("setFace", code, body);

	return -1;
}

int       
WebSocketTask::onGateCtrl(string json)
{
	uint32_t code = 200;

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	string body = "";

	do
	{
		if (!reader.parse(json, rvalue, false))
		{
			code = 404;
			ERRE_MSV("Invalidate request parameter");
			break;
		}

		SHL_Json::FastWriter writer;
		SHL_Json::Value      reason;

		reason["command"] = "gateCtrl";
		reason["DevIp"] = rvalue["DevIp"];
		reason["DevPort"] = rvalue["DevPort"];
		body = writer.write(reason);
		Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
		if (!cli)
		{
			code = 403;
			ERRE_MSV("Illegal invasion");
			break;
		}

		int ret = cli->customCommandEx(json);
		if (0 != ret)
		{
			code = 401;
			LOGE_MSV("%s", getError());
			break;
		}

		response("gateCtrl", code, body);

		return 0;
	} while (0);

	response("gateCtrl", code, body);

	return -1;
}


int
WebSocketTask::process()
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(m_req, rvalue, false))
    {
        SHL_Json::FastWriter writer;
        SHL_Json::Value      reason;

        ERRE_MSV("Invalidate request parameter.");
        reason["reason"] = getError();
        string body = writer.write(reason);

        response("login", 404, body);

        return -1;
    }

    string url = rvalue["command"].asString();

    if ("login" == url)
    {
        onLogin(m_req);
    }
    else if ("subscribe" == url)
    {
        onSubscribe(m_req);
    }
    else if ("preview" == url)
    {
        onPreview(m_req);
    }
    else if ("playback" == url)
    {
        onPlayback(m_req);
    }
    else if ("download" == url)
    {
        onDownload(m_req);
    }
    else if ("queryDevice" == url)
    {
        onQueryDevice(m_req);
    }
    else if ("queryRecord" == url)
    {
        onQueryRecord(m_req);
    }
    else if ("ptzCtrl" == url)
    {
        onPtzCtrl(m_req);
    }
    else if ("syncTime" == url)
    {
        onSyncTime(m_req);
    }
    else if ("link" == url)
    {
        putree->pushMsg(m_req.c_str(), m_req.size());
    }
	else if ("setFace" == url)
	{
		onSetFace(m_req);
	}
	else if ("gateCtrl" == url)
	{
		onGateCtrl(m_req);
	}
	else if ("getConfig" == url)
	{
		onGetConfig(m_req);
	}
	else if("customCommand" == url)
	{
		onCustomCommand(m_req);
	}

    return 0;
}


int
WebSocketTask::onGetConfig(string json)
{
	uint32_t code = 200;
	SHL_Json::Reader     reader;
	SHL_Json::Value      rvalue;
	SHL_Json::Value      wvalue;
	SHL_Json::FastWriter writer;

	SHL_Json::Value      value;
	string   jout("");

	do
	{
		if (!reader.parse(json, rvalue, false))
		{
			code = 404;
			break;
		}

		Client *cli = cliMgr->findClient(rvalue["userKey"].asString());
		if (!cli)
		{
			code = 403;
			break;
		}

		int ret = cli->requestConfig(json, jout);
		if (0 != ret)
		{
			code = 404;
			break;
		}

		if (!reader.parse(jout, wvalue, false))
		{
			code = 404;
			break;
		}

	} while (0);

	wvalue["version"] = rvalue["version"];
	jout = writer.write(wvalue);

    response("getConfig", code, jout);

	return 0;
}

int        
WebSocketTask::onCustomCommand(string json)
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
WebSocketTask::onSubscribeStatus(string json)
{
	return 0;
}
