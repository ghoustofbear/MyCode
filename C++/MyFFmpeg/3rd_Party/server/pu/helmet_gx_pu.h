#ifndef __HELMET_GX_PU_H__
#define __HELMET_GX_PU_H__

#include "pu.h"
#include "3rd/helmet/iCVSC.h"
#include "3rd/helmet/iCVSC_base.h"
#include "3rd/helmet/gx_fmtapi.h"

#include "tlist.h"
#include "tmap.h"
#include <map>
#include <string>

class HelmetGXPu : public Pu 
{
public:

    HelmetGXPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HelmetGXPu();

    virtual int        start();
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    IC_HANDLE          getHandle(int channelNo, string key);

protected:
    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);
    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }
    virtual void       onTimer();

    int                parseEvent(const string& json);    
    int                updateChnStatus();

private:
    map<uint8_t, IC_HANDLE>             m_mapChnToTalk;
    uint64_t                            m_chnOnlineStatus;

    bool                                m_bExit; //songhongliang
    struct HTCHN_ST
    {
        IC_HANDLE                 hand;
        IC_STR                    name;
        IC_STR                    puid;
        tmap<string, IC_HANDLE>   subDev;
    };

public:
    IC_HANDLE                           m_lUserID;
    Mutex                               m_mtxLock;
    tmap<string, IC_HANDLE>             m_chnHandle;

    IC_HANDLE                           m_testDual;
};

#endif
