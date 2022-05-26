/**
 * @file      usermgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 09:56:46
 *
 * @note
 *  UserMgr observer UserCfg
 */
#ifndef __USERMGR_H__
#define __USERMGR_H__
#include "observer.h"
#include "tmap.h"

class User;
class OnUiUpdate;
class AuthListener;
class UserMgr : public tmap<string, User *>, public Observer
{
public:
    UserMgr();
    virtual ~UserMgr();

    int                init();

    int                authent(const string &user, const string &crypto, uint64_t &mbitmap, uint64_t &obitmap, bool &relogin, string &id, string &usercode);

    void               addAuthListener(const string &user, AuthListener *listener);
    void               delAuthListener(const string &user, AuthListener *listener);

    void               setUi(OnUiUpdate *ui);
    void               setUi(const string &id, OnUiUpdate *ui);
    void               updateUi();
	uint32_t           optlevel(const string &userid);
protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    OnUiUpdate       * m_ui;
};

extern UserMgr *userMgr;
#endif
