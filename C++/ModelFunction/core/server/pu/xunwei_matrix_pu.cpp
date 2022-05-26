#include "xunwei_matrix_pu.h"
#include "sys.h"
#include "json/json.h"

XunWeiMatrixPu::XunWeiMatrixPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
	m_sock = sys->createSocket();
	m_tmPrev = time(NULL);

	m_protocol = PROTO_TYPE_XUNWEI;
	m_port = port;
}

XunWeiMatrixPu::~XunWeiMatrixPu()
{
	if (NULL != m_sock)
	{
		delete m_sock;
		m_sock = NULL;
	}
}

int 
XunWeiMatrixPu::start()
{
	m_init = true;

	if (Pu::start() < 0)
	{
		return -1;
	}

	return 0;
}

int
XunWeiMatrixPu::login()
{
	if (m_login)
	{
		return -1;
	}

	char ipaddr[128] = { 0 };
	sprintf(ipaddr, "%s:%d", m_host.c_str(), m_port);
	Host host(ipaddr, 0);

	if (m_sock->open(host) < 0 ||
		m_sock->connect() < 0)
	{
		m_sock->close();
		m_login = false;
		return -1;
	}

	m_login = true;
	m_first = true;
	return 0;
}

void
XunWeiMatrixPu::logout()
{
	if (NULL != m_sock)
	{
		m_sock->close();
		m_login = false;
		m_first = false;
	}
}

unsigned char HexToChar(unsigned char hex)
{
	unsigned char res = 0;
	if (hex >= 0 && hex < 10)
	{
		res = hex + '0';
	}
	else if (hex >= 10 && hex < 16)
	{
		res = hex + 'A';
	}

	return  res;
}

int
XunWeiMatrixPu::onCustomCommand(string& json)
{
	if (!m_login)
	{
		return -1;
	}

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string jMethod = rvalue["Method"].asString();
	if ("SwitchPort" == jMethod)
	{
		uint32_t u32InPort = rvalue["InPort"].asInt();
		uint32_t u32OutPort = rvalue["OutPort"].asInt();

		unsigned char u8Buf[7] = { 0 };
		//设备号 +输出端口号(两位) + 切换方式 + 输入端口+!
		u8Buf[0] = '*';

		u8Buf[1] = HexToChar(u32OutPort / 10);
		u8Buf[2] = HexToChar(u32OutPort % 10);

		u8Buf[3] = '<';

		u8Buf[4] = HexToChar(u32InPort / 10);
		u8Buf[5] = HexToChar(u32InPort % 10);

		u8Buf[6] = '!';

		int ret = m_sock->write(u8Buf, 7);
		if (ret < 0)
		{
			logout();
			return -1;
		}
	}

	return 0;
}

void 
XunWeiMatrixPu::sendheart()
{
	unsigned char u8Buf[7] = { 0 };
	int ret = m_sock->write(u8Buf, 7);
	if (ret < 0)
	{
		logout();
	}
}

void
XunWeiMatrixPu::onTimer()
{
	if (m_login)
	{
		time_t tmNow = time(NULL);
		if (tmNow - m_tmPrev >= 5)
		{
			sendheart();
			m_tmPrev = tmNow;
		}
	}
}