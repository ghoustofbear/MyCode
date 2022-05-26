/**
 * @file      putree.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-31 04:38:24
 *
 * @note
 *  findPu(string host, int port, string &addrcode); 
 *            find the first local device(DVR/MSV) which has the host:port, 
 *            and return the match host:port device(DVR) addrcode
 */
#ifndef __PUTREE_H__
#define __PUTREE_H__
#include "ttree.h"
#include "observer.h"
#include "thread.h"

/* #define TIMERITSELF */
class Pu;
class TriggerListener;
class OnUiUpdate;

class PuTree : public ttree<string>, public Observer
{
    enum
    {
        T_TIMER,
    };

public:
    PuTree();
    virtual ~PuTree();

public:
    int                start();
    void               stop();

    int                addPu(int    protocol,
                             int    type,
                             string addrcode,
                             string name,
                             int    chnNum,
                             string user,
                             string pswd,
                             string host,
                             int    port,
                             int    domainType,
                             int    callListen = 0,
                             int    linktype = 0);


    int                addPu(int    protocol,
                             int    type,
                             string addrcode,
                             string name,
                             string host,
                             int    sport,
                             int    rport,
                             int    hport,
                             int    rtport
                            );

    int                delPu(string addrcode);

    Pu               * findPu(string host, int port);

	Pu               * findPu(string addrcode);

	string             findDevCode(string host, int port);

    string             findCode(string host, int port, int channelNo);
    
	void			   findIvaPu(vector<Pu *> &IvaPu);

    void               addTriggerListener(TriggerListener *listener);
    void               delTriggerListener(TriggerListener *listener);

    int                syncTime(datetime_s &dt, const string &tzone);

    void               setUi(OnUiUpdate *ui);
	void               setUi(const string &addrcode,OnUiUpdate *ui);
    void               updateUi();
	

    void               onTimer();

    /////
    typedef void(*pfnMsgcallback)(const char* data, int size, void *puser);
    void               setMessageCallback(pfnMsgcallback pfnMsgCb, void *pUser);

    void               pushMsg(const char *data, int size);

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);
    static  void     * threadEntry(ThreadInfo *arg);

private:
    string             m_root;
    Thread             m_timerThread;
    OnUiUpdate       * m_ui;

	string             m_puuiAddrcode;
	OnUiUpdate       * m_puui;
    uint64_t           m_lastSync;

    /////
    pfnMsgcallback     m_pfnMsgcallback;
    void             * m_pUser;

    Mutex              m_uimtx;
};

struct nodedata_t
{
    int            type;                                   /*< 0:this msv, 1:local device, 2:remote device, 3:channel */
    string         name;
    string         host;
    int            port;
    int            channel;
    int            subnum;
    int            status;                                 /*< 0:off-line 1:on-line */
    int            ablity;

    void         * data;
	int            devtype;                                /*< dvr nvr */
    int            manutype;                               /*< hik dahua tiandy */
    int            linktype;                              

    nodedata_t(int t, void *v = NULL, string n = "", string h = "", int p = 0, int c = 0, int num = 0, int s = 0, int abt = 1, int dtype = 360, int mtype = -1, int ltype = 0)
    {
        type    = t;
        data    = v;
        name    = n;
        host    = h;
        port    = p;
        channel = c;
        subnum  = num;
        status  = s;
        ablity  = abt;
		devtype = dtype;
        manutype = mtype;
        linktype = ltype;
    };
};

extern PuTree * putree;
extern void get_nodedata(string key, void *nodedata, void *userdata);
extern void get_nodedevicedata(string key, void *nodedata, void *devicedata);
#endif
