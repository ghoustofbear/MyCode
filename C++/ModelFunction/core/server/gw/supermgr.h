/**
 * @file      supermgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-14 17:26:26
 *
 * @note
 *  supermgr.h defines
 */
#ifndef __SUPERMGR_H__
#define __SUPERMGR_H__
#include "tlist.h"
#include "observer.h"

class Superior;
class OnUiUpdate;

class SuperMgr : public tlist<Superior *>, public Observer
{
public:
    SuperMgr();
    virtual ~SuperMgr();

public:
    int                start();
    void               stop();

    Superior         * addSuperior(string addrcode,
                                   string name,
                                   int    protocol,
                                   string user,
                                   string pswd,
                                   string host,
                                   int    sipPort,
                                   int    rtspPort,
                                   int    httpPort,
                                   int    expires,
                                   string ifupIp,
                                   int    bregister,
                                   int    ivauselocal);

    int                delSuperior(string addrcode);

    Superior         * findSuperiorByGucode(string gucode);
    Superior         * findSuperior(string addrcode);
    Superior         * findSuperior(string host, int port);

    void               setUi(OnUiUpdate *ui);
    void               updateUi();

    void               timerdriver();

	void               push_alarm(string &json, void *data, uint32_t size);

    void               setregisterstatus(string &addrcode, bool bregister);

    void               setuseivalocalcfg(string &addrcode, bool buseivalocalcfg);

    int                push(const string &type, const string &json);

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    OnUiUpdate       * m_ui;
};

extern SuperMgr * superMgr;
#endif
