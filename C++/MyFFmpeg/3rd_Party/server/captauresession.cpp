#include "captauresession.h"
#include "json/json.h"
#include "putree.h"
#include "pu.h"
#include "base64_ex.h"
#include "system.h"

CaptaureSession::CaptaureSession()
{
	//int bufSize = 60 + (1920 * 1280 * 4 * 10);
	int bufSize = 60 + (1920 * 1280 * 4);
	m_pCapBuf = new uint8_t[bufSize];
	m_captaureflag = false;
	bufSize = 0;
	dwCapSize = 0;
	m_streamType = 0;
	m_playID = -1;
do{
	#ifdef _WIN32
			if (!PlayM4_GetPort((long *)&m_playID))
	#else
			if (!PlayM4_GetPort((int*)&m_playID))
	#endif
			{
				ERRE_SDK("PlayM4_GetPort fail.");
				break;
			}

			if (!PlayM4_SetStreamOpenMode(m_playID, STREAME_REALTIME))
			{

				break;
			}

			if (!PlayM4_OpenStream(m_playID, NULL, 0, 1024 * 1024))
			{
				break;
			}

	#ifdef __linux__
			if (!PlayM4_SetConfigFontPath(m_playID, "/utplat/.uires/winfonts/simsun.ttc"))
			{
				ERRE_SDK("HikPlayer not find font-library");
			}
	#endif

			if (!PlayM4_Play(m_playID, NULL))
			{
				break;
			}
	} while (0);
	
}

CaptaureSession::~CaptaureSession()
{
	PlayM4_Stop(m_playID);
	PlayM4_CloseStream(m_playID);
	PlayM4_FreePort(m_playID);
	if (m_pCapBuf)
		delete[] m_pCapBuf;
}

int
CaptaureSession::stream()
{
	return 0;
}

int
CaptaureSession::transport()
{
	return 0;
}

string
CaptaureSession::peerip()
{
	return "";
}

int
CaptaureSession::peerport()
{
	return 0;
}

string
CaptaureSession::peertime()
{
	return "";
}

int
CaptaureSession::streamtype()
{
	return 0;
}

int
CaptaureSession::start(string json, int mode)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	m_deviceHost = rvalue["DevIp"].asString();
	m_devicePort = rvalue["DevPort"].asInt();
	m_channelNo = rvalue["chnNo"].asInt();
	if (!rvalue["chnNo"].isNull())
	{
		m_streamType = rvalue["streamType"].asInt();
	}
	m_pu = putree->findPu(m_deviceHost, m_devicePort);
	int result = -1;
	if (m_pu)
	{
		result = m_pu->addRealGu(m_channelNo, m_deviceHost, m_devicePort, m_streamType, 0, this);
	}
	return result;
}

void
CaptaureSession::stop()
{
	do
	{
		if (m_pu == NULL)
		{
			return;
		}
		m_pu->delRealGu(m_channelNo, m_deviceHost, m_devicePort, m_streamType, 0, this);

	} while (0);
}

 void       
 CaptaureSession::onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag)
 {


 }

void
CaptaureSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
 {
	
 }

void
CaptaureSession::onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type)
 {
	if (!PlayM4_InputData(m_playID, (uint8_t *)stream, size))
	{
		ERRE_SDK("PlayM4_InputData is failed\n");
	}
	if (m_captaureflag == true)
		return;
	if (!PlayM4_GetJPEG(m_playID, m_pCapBuf, bufSize, &dwCapSize))
	{
		m_captaureflag = false;
		int eror = PlayM4_GetLastError(m_playID);
		ERRE_MSV("PlayM4_GetPictureSize Failed  %d\n)", PlayM4_GetLastError(m_playID));
	}
	else
	{
		m_captaureflag = true;
		GetSize((char*)m_pCapBuf, dwCapSize, m_height, m_weidth);
		ERRD_MSV("capture success\n)");
		m_evtCaptaurepicture.notify("CAPTAUREPICTURE", m_playID);		
	}
 }



 void
CaptaureSession::onStatus(int code)
 {
	 
 }

int 
CaptaureSession::GetSize(char * buffer, long length, int &height, int &weidth)
{
	 if (!buffer)
		 return -1;
	 char * temp = buffer + length, *temp1 = buffer;
	 unsigned char ff, type = 0xff;
	 int m_ret = -1, m_pos = 0, m_hpx = 0, m_wpx = 0, uits = 0;
	 if ((unsigned char)*buffer++ != 0xff || (unsigned char)*buffer++ != 0xd8)
	 {
		 printf("此非jepg图片\n");
		 return -1;
	 }
	 while (temp>buffer&&type != 0xDA)
	 {
		 do
		 {
			 ff = *buffer++;
		 } while (ff != 0xff);
		 do
		 {
			 type = *buffer++;
		 } while (type == 0xff);
		 switch (type)
		 {
		 case 0x00:
		 case 0x01:
		 case 0xD0:
		 case 0xD1:
		 case 0xD2:
		 case 0xD3:
		 case 0xD4:
		 case 0xD5:
		 case 0xD6:
		 case 0xD7:
			 break;
		 case 0xC0://SOF0段
			 temp1 = buffer;
			 m_pos = ((*buffer++) & 0xff) << 8;
			 m_pos += (*buffer++) & 0xff;
			 buffer++; //舍弃精度值
			 height = ((*buffer++) & 0xff) << 8;
			 height += (*buffer++) & 0xff;
			 weidth = ((*buffer++) & 0xff) << 8;
			 weidth += (*buffer) & 0xff;
			 m_ret++;
			 break;
		 case 0xE0: //APP0段
			 break;
		 default:
			 temp1 = buffer;
			 m_pos = ((*buffer++) & 0xff) << 8;
			 m_pos += (*buffer++) & 0xff;
			 break;
		 }
		 buffer = temp1 + m_pos;
	 }
	 return m_ret;
}


