/**
 * @file      servmgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     server define
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:36:06
 *
 * @note
 *  servmgr.h defines
 *
 *  void setGuUi(OnUiUpdate *ui, int type = 0)
 *  type 0-realstrem 1-playback/download stream
 *
 *  ServMgr observer sysCfg
 */

#ifndef __SERVMGR_H__
#define __SERVMGR_H__
#include "exports.h"
#include "onuiupdate.h"
#include "observer.h"

#define MAX_SERVIN_NUM       (64)

class Server;
class Thread;
class ThreadInfo;
class Mutex;

class MS_API ServMgr : public list<Server *>,  public Observer
{
    friend class System;
    enum
    {
        T_TIMER,
        T_UPDATE_UI,
    };

public:
    void               setUi(OnUiUpdate *ui);

    void               setPuUi(OnUiUpdate *ui);
    void               setPuUi(const string& addrcode, OnUiUpdate* ui);
    void               setGuUi(OnUiUpdate *ui, int type = 0);
    void               setGuUi(const string &addrcode, OnUiUpdate *ui, int type = 0);

    void               setUserUi(OnUiUpdate *ui);
    void               setUserUi(const string &id, OnUiUpdate *ui);

    void               setSystemStatusUi(OnUiUpdate *ui);


    void               setSuperUi(OnUiUpdate *ui);

    int                authent(const string &user, const string &pswd, uint64_t &mbitmap, uint64_t &obitmap);
    void               setlopt(const string &user, bool islopt);

    int                writeLog(const string &json, const string& table);

    int                startAll();
    void               stopAll();

    static ServMgr   * instance();
    static char      * version();
    static char      * buildPTS();

    typedef void(*pfnMsgcallback)(const char* data, int size, void *puser);
    void               setMessageCallback(pfnMsgcallback pfnMsgCb, void *pUser);

    void               setregisterstatus(string &addrcode, bool bregister);
    void               setuseivalocalcfg(string &addrcode, bool bUseivalocal);

    int                QuerySubPlatResouce(string addrcode, int fromIndex, int toIndex, string &json);


protected:
    virtual int        start();
    virtual void       stop();
    virtual void       update(const std::string &table, const std::string &json, int type);

protected:
    void               onTimer();
    int                updateUi();

    static void *      threadProc(ThreadInfo *);

private:
    ServMgr();
    virtual ~ServMgr();

private:
    Thread           * m_uiThread;
    Thread           * m_timerThread;
    OnUiUpdate       * m_ui;
    Mutex            * m_uimtx;

    bool               m_islopt;                           /* is local user operating UI */
    char               m_luser[36];                        /* local user */

private:
    bool               m_start;
};

#endif
