#ifndef  _HIK_TERMINAL_PU_H_
#define  _HIK_TERMINAL_PU_H_

#include "pu.h"
#include "curl/curl.h"
#include <mutex>

//20200520 zhangshenyu add
class HikTerminalPu : public Pu
{
public:
    friend size_t read_data(void* buffer, size_t size, size_t nmemb, void* userdata);
    HikTerminalPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikTerminalPu();

    virtual int        start();


    virtual int        onCustomCommand(string& json);
private:
    int                curlPost(string jin, string url, string &jout);
    int                curlGet(string &url, string &jout);

    int               sendReqLane(string jin, string &jout);   /** 获取车道数据 */
    int               sendReqGate(string jin, string jout);   /** 控制道闸 */
    int               sendReqPlate(string jin, string jout);  /** 增删改黑白名单 */
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

    virtual int        onRequestConfig(string & jin, string& jout) { return 0; }

    void               onTimer();


private:
    CURL             * m_pLiveCurl;
    uint64_t           m_tmPrev;
    static HikTerminalPu *m_self;
    std::mutex      m_mux;
};

#endif
