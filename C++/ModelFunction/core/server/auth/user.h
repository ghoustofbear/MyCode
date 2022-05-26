/**
 * @file      user.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-25 17:02:51
 *
 * @note
 *  User observer GroupCfg, UserGroupCfg
 */
#ifndef __UESR_H__
#define __UESR_H__
#include "tlist.h"
#include "authlistener.h"
#include "observer.h"
#include "tvector.h"
#include "onuiupdate.h"

class User : public tlist<AuthListener *>, public Observer
{
public:
    enum
    {
        E_OK,
        E_FAILED,
        E_AAA_FAIL,
        E_HAS_LOGINED,
        E_PERM_DENIED,
        E_NOT_EXIST,
        E_UNKONW
    };


    User(string id, string addrcode, string name, string realName, string pswd, string identity, string phone, string email, bool relogin, bool aaa);
    virtual ~User();

    int                init();

    string             id();
    string             name();
    string             addrcode();
    bool               online();

    uint64_t           mright();
    uint64_t           oright();
    uint64_t           optlevel();

    int                authent(const string &crypto, uint64_t &mbitmap, uint64_t &obitmap, bool &relogin);

    void               addAuthListener(AuthListener *listener);
    void               delAuthListener(AuthListener *listener);

    void               notifyChanged(const string &json);

    void               modify(string name, string realName, string pswd, string identity, string phone, string email, bool relogin, bool aaa);

    void               setUi(OnUiUpdate *ui);
    void               updateUi();
protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    string             m_id;
    string             m_addrcode;
    string             m_name;
    string             m_realName;
    string             m_pswd;
    string             m_identity;
    string             m_phone;
    string             m_email;
    bool               m_relogin;
    bool               m_aaa;

    tvector<string>      m_mright;
    tvector<string>      m_oright;

    OnUiUpdate       * m_ui;
};
#endif
