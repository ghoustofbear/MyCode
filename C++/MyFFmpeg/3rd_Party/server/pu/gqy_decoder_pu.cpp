#include "gqy_decoder_pu.h"
#include "global_str_def.h"
#include "json/json.h"
#include "log.h"
#include "putree.h"
#include "cfg_device.h"


GqyDecoderPu::GqyDecoderPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
	m_protocol = PROTO_TYPE_GQY;
	m_port = port;
	m_domainType = domainType;
	m_tmPrev = time(NULL);
	m_width = 4;
	m_init = false;
}

GqyDecoderPu::~GqyDecoderPu()
{
	stop();
}

int GqyDecoderPu::start()
{

	if (Pu::start() < 0)
	{
		return -1;
	}
	string json;
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	deviceCfg->query(json, "protocol=18");
	if (reader.parse(json, rvalue, false))
	{
		for (uint32_t i = 0; i < rvalue.size(); i++)
		{
			if (this->addrcode() == rvalue[i]["addrcode"].asString())
			{
				int width = atoi(rvalue[i]["comment"].asString().c_str());
				if(width != 0)
				{
					m_width = width;
				}
				break;
			}
		}
	}
	m_sock = sys->createSocket();

	m_init = true;

	return 0;
}

void GqyDecoderPu::stop()
{
	if (!m_init)
		return;

	m_init = false;

	logout();

	Pu::stop();
}


int GqyDecoderPu::login()
{
	if (m_login)
		return 0;

	m_sock->setWriteTimeout(2000);
	m_sock->setReadTimeout(2000);
	Host h(this->host(), this->port());
	if (m_sock->open(h) < 0 ||
		m_sock->connect() < 0)
	{
		m_sock->close();
		sys->sleep(200);
		return -1;
	}
	m_login = true;
	writeDevLog(STR_MSV_ON_LINE);
	return 0;
}


void GqyDecoderPu::logout()
{
	if (!m_login)
	{
		return ;
	}
	m_sock->close();
	m_login = false;
	writeDevLog(STR_MSV_OFF_LINE);
	return ;
}

int
GqyDecoderPu::onCustomCommand(string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string jMethod = rvalue["Method"].asString();
	SHL_Json::Value itemlist = rvalue["Item"];
	int realNum = itemlist.size();
	int outPort = rvalue["DisPort"].asInt();
	if (0 == realNum)
	{
		return 0;
	}
	if (jMethod == "WindowDecoder")
	{
		if (0 != OpenWindow(000, realNum))
		{
			return -1;
		}

		int  i = 0;
		for (i = 0; i < realNum; ++i)
		{
			SHL_Json::Value element;
			element = itemlist[i];

			string strIp = element["Ip"].asCString();
			int    u16Port = element["Port"].asInt();
			int    u16Chn = element["Chn"].asInt();

			Pu *pDecPu = putree->findPu(strIp, u16Port);
			if (pDecPu)
			{
				videodecoder(outPort, i, strIp, u16Port, u16Chn, pDecPu->userName(), pDecPu->passWord());
			}
		}
		return 0;
	}
	return -1;
}

int GqyDecoderPu::OpenWindow(int idisport, int iwndnum)
{
	if (0 == iwndnum)
	{
		return -1;
	}

	int height = (idisport - 1) / m_width;
	int width = (idisport - 1) % m_width;

	char packet[128] = { 0 };
	sprintf(packet, "<OpenWin,0,1,%d,%d,%d,%d,%d,%d>", (idisport - 1), \
		(100 + idisport), width * 1920, height * 1080, 1920, 1080);
	bool bRet = m_sock->writeTo(packet, strlen(packet));
	if (bRet)
	{
		memset(packet, 0, sizeof(packet));
		int wndNum = 36;
		if (iwndnum <= 1)
		{
			wndNum = 1;
		}
		else if (iwndnum <= 4)
		{
			wndNum = 4;
		}
		else if (iwndnum <= 6)
		{
			wndNum = 6;
		}
		else if (iwndnum <= 8)
		{
			wndNum = 8;
		}
		else if (iwndnum <= 9)
		{
			wndNum = 9;
		}
		else if (iwndnum <= 16)
		{
			wndNum = 16;
		}
		else if (iwndnum <= 25)
		{
			wndNum = 25;
		}
		else if (iwndnum <= 36)
		{
			wndNum = 36;
		}

		sprintf(packet, "<IPVSplit,0,%d,%d>", (100 + idisport), wndNum);
		m_sock->writeTo(packet, strlen(packet));
	}

	return 0;
}

int GqyDecoderPu::videodecoder(int idisport, int index, string  ps8ip, int iport, int ichn, string ps8username, string ps8password)
{
	if (!m_login)
	{
		return -1;
	}

	char rtspaddr[128] = { 0 };
	sprintf(rtspaddr, "rtsp://%s:%s@%s:554/h264/ch%d/main/av_stream", ps8username.c_str(), ps8password.c_str(), ps8ip.c_str(), (ichn));

	char packet[128] = { 0 };
	sprintf(packet, "<IPVSignal,0,%d,%d,1,H264,%s>", (100 + idisport), index, rtspaddr);
	m_sock->writeTo(packet, strlen(packet));

	return 0;
}

void GqyDecoderPu::DelWindow(int disport)
{
	if (!m_login)
	{
		return;
	}

	char packet[128] = { 0 };
	sprintf(packet, "<CloseWin,0,%d>", (100 + disport));
	bool bRet = m_sock->writeTo(packet, strlen(packet));
}

bool GqyDecoderPu::Ping()
{
	if (!m_login)
	{
		return false;
	}

	char packet[128] = { 0 };
	strcpy(packet, "<Ping>");
	bool bRet = m_sock->writeTo(packet, strlen(packet));
	if (!bRet)
	{
		return false;
	}

	return true;
}

void GqyDecoderPu::onTimer()
{
	if (!m_login)
	{
		return;
	}

	time_t tmNow = time(NULL);
	if (tmNow - m_tmPrev >= 5)
	{
		m_tmPrev = tmNow;
		bool bRet = Ping();
		if (!bRet)
		{
			logout();
		}
	}
}