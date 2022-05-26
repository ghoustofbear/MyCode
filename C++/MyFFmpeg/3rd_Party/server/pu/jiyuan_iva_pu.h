#ifndef  _JIYUAN_IVA_PU_H_
#define  _JIYUAN_IVA_PU_H_

#include "pu.h"
#include "curl/curl.h"


class JiYuanIvaPu : public Pu
{
public:

    friend size_t readjson_callback(void *ptr, size_t size, size_t nmemb, void *userp);

	JiYuanIvaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
	virtual ~JiYuanIvaPu();

	virtual int        start();
	int                taskDispatch(const string &json, string &xml);
	int                cameraSnap(const string &json, string &xml);

    int                sendcommand(string &json);

protected:
	virtual void       stop();
	virtual int        login();
	virtual void       logout();

	virtual bool       validate(int channelNo){ return true; }
	virtual bool       validate(string gucode) { return true; }

	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
	virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

	virtual int        onCustomCommand(string& json) { return 0; }
	virtual int        onRequestConfig(string & jin, string& jout) { return 0; }

    static void *      threadProc(ThreadInfo *info);

    void               onTimer();

private:
	CURL             * m_pLiveCurl;
    uint64_t           m_tmPrev;
};

#endif
