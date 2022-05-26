#ifndef _GQY_DECODER_PU_H_
#define _GQY_DECODER_PU_H_

#include "pu.h"
#include "socket.h"

class GqyDecoderPu : public Pu
{
public:

	friend void *RecvDataFun(void *pArg);

	GqyDecoderPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);

	virtual ~GqyDecoderPu();
	int     start();


	int     OpenWindow(int idisport, int iwndnum);
	int     videodecoder(int idisport, int index, string ps8ip, int iport, int ichn, string ps8username, string ps8password);

	virtual void    onTimer();

	virtual void       stop();
	virtual int        login();
	virtual void       logout();

	virtual int        onCustomCommand(string& json);
	virtual bool       validate(int channelNo){ return true; }
	virtual bool       validate(string gucode) { return true; }

	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
	virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

private:
	void    DelWindow(int disport);
	bool    Ping();

private:
	time_t     m_tmPrev;
	HANDLE     m_hRecv;

	int        m_width;
	Socket     *m_sock;

};

#endif
