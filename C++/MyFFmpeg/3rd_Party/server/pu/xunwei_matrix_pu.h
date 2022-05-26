#ifndef  _XUNWEI_MAXTRIX_H_
#define  _XUNWEI_MAXTRIX_H_

#include "pu.h"
#include "socket.h"

class XunWeiMatrixPu : public Pu
{
public:
	XunWeiMatrixPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);

	~XunWeiMatrixPu();

protected:

	virtual int        start();
	virtual int        login();
	virtual void       logout();

	virtual bool       validate(int channelNo){ return true; }
	virtual bool       validate(string gucode) { return true; }

	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
	virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

	virtual int        onCustomCommand(string& json);
	virtual void       onTimer();

private:
	void               sendheart();
private:
	Socket*            m_sock;
	time_t             m_tmPrev;
};

#endif
