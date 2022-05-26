#ifndef  _UT_IVA_PU_H_
#define  _UT_IVA_PU_H_

#include "pu.h"
#include "utiva/utivasdk.h"

class UtIvaPu : public Pu
{
public:
	UtIvaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
	virtual ~UtIvaPu();
	virtual int        start();
    static void        AlarmCallback(const char* strJson, void *pUser);
    static void        LoginCallback(const char* strJson, void * pUser);
    int                queryAbility(string addrcode, string &xml);
    int                taskDispatch(const string &json, string &xml);
    int                reqAllTask(const string &json, string &xml);
    int                reqSignalTask(const string &json, string &xml);
    int                cameraSnap(const string &json, string &xml);
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

	virtual int        onCustomCommand(string& json);
	virtual int        onRequestConfig(string & jin, string& jout);

    string             setParamPreset(const char *json, int size);
    string             getParamPreset(const char *json, int size);
    string             startanaylyse(const char *json, int size);
    string             stopanaylyse(const char *json, int size);
    string             getPresetState(const char *json, int size);
    

	string             callAll();
    bool               sendIvaRequest(string json, string &res);

	int					queryIvaLmt();

	virtual void       onTimer();
    void                sendLmtInfo();

private:
    int onSetIvaPresetParam(const string & jin);

	int onSetIvaParamRule(const string & jin);

	int onDeleteIvaParamRule(const string & jin);

	int onDeleteIvaPresetParam(const string & jin);


private:
    int onGetIvaPresetParam(const string & jin, string& jout);
    int onGetIvaPresetState(const string & jin, string& jout);
	int onGetIvaRulrList(const string & jin, string& jout);


private:
    string                 sendStartIVAAlarm(string json);
    static int          ivaTypeCast(int AnalyseType ,int AnalyseSubType);
    static void         ivaTypeCast(const int type, int &AnalyseType, int &AnalyseSubType);
private:
    void              *m_Cnn;
	std::string		   m_lmtIp;
	int				   m_lmtPort;
	std::string		   m_lmtUser;
	std::string		   m_lmtPsaawd;
    bool               m_sendLmt;
};

extern UtIvaPu * utIvaPu;

#endif
