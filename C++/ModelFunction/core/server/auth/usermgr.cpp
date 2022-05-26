#include "usermgr.h"
#include "user.h"
#include "sys.h"
#include "cfg_user.h"
#include "json/json.h"

UserMgr::UserMgr()
{
    m_ui    = NULL;
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    userCfg->addObserver(this);
}

UserMgr::~UserMgr()
{
    userCfg->deleteObserver(this);
    {
        AutoLock locker(_mtx);
        for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
        {
            delete i->second;
        }
        clear();
    }
}

int
UserMgr::init()
{
    string json;
#if 0
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["id"]       = sys->generateUID();
    value["name"]     = "admin";
    value["addrcode"] = "123454321";
    value["pswd"]     = "111111";
    value["relogin"]  = true;
    value["aaa"]      = false;

    json = writer.write(value);
    userCfg->add(json);

    value["id"]       = sys->generateUID();
    value["name"]     = "ut";
    value["addrcode"] = "123456789";
    value["pswd"]     = "1";

    json = writer.write(value);
    userCfg->add(json);
#else
    userCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    for (uint32_t i = 0; i < rvalue.size(); i++)
    {
        AutoLock locker(_mtx);
        User *user = new User(
            rvalue[i]["id"      ].asString(),
            rvalue[i]["addrcode"].asString(),
            rvalue[i]["name"    ].asString(),
            rvalue[i]["realName"].asString(),
            rvalue[i]["pswd"    ].asString(),
            rvalue[i]["identity"].asString(),
            rvalue[i]["phone"   ].asString(),
            rvalue[i]["email"   ].asString(),
            rvalue[i]["relogin" ].asBool(),
            rvalue[i]["aaa"     ].asBool()
        );
        (*this)[rvalue[i]["id"].asString()] = user;
        user->init();
    }
#endif

    return 0;
}

int
UserMgr::authent(const string &user, const string &crypto, uint64_t &mbitmap, uint64_t &obitmap, bool &relogin, string &id, string &usercode)
{
    AutoLock locker(_mtx);
    for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (user == i->second->name())
        {
            int result = i->second->authent(crypto, mbitmap, obitmap, relogin);
            if (User::E_OK == result)
            {
                id       = i->second->id();
                usercode = i->second->addrcode();
            }

            return result;
        }
    }

    return User::E_NOT_EXIST;
}

void
UserMgr::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && !rvalue.isNull()))
    {
        return;
    }

    AutoLock locker(_mtx);

    if (O_ADD == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        User *user = new User(
            rvalue["id"      ].asString(),
            rvalue["addrcode"].asString(),
            rvalue["name"    ].asString(),
            rvalue["realName"].asString(),
            rvalue["pswd"    ].asString(),
            rvalue["identity"].asString(),
            rvalue["phone"   ].asString(),
            rvalue["email"   ].asString(),
            rvalue["relogin" ].asBool(),
            rvalue["aaa"     ].asBool()
        );

        (*this)[rvalue["id"].asString()] = user;
    }
    else if (O_DELETE == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        User* user = (*this)[rvalue["id"].asString()];
        user->notifyChanged(json);

        delete user;
        erase(rvalue["id"].asString());
    }
    else if (O_MODIFY == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        User* user = (*this)[rvalue["id"].asString()];
        user->modify(
            rvalue["name"    ].asString(),
            rvalue["realName"].asString(),
            rvalue["pswd"    ].asString(),
            rvalue["identity"].asString(),
            rvalue["phone"   ].asString(),
            rvalue["email"   ].asString(),
            rvalue["relogin" ].asBool(),
            rvalue["aaa"     ].asBool()
        );
        user->notifyChanged(json);
    }
}

void
UserMgr::addAuthListener(const string &user, AuthListener *listener)
{
    AutoLock locker(_mtx);
    for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (user == i->second->name())
        {
            i->second->addAuthListener(listener);
        }
    }
}

void
UserMgr::delAuthListener(const string &user, AuthListener *listener)
{
    AutoLock locker(_mtx);
    for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (user == i->second->name())
        {
            i->second->delAuthListener(listener);
        }
    }
}

void
UserMgr::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
UserMgr::setUi(const string &id, OnUiUpdate *ui)
{
    AutoLock locker(_mtx);
    map<string, User *>::const_iterator i = find(id);
    if (i != end())
    {
        i->second->setUi(ui);
    }
}

void
UserMgr::updateUi()
{
    if (m_ui && size() > 0)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        AutoLock locker(_mtx);

        int onlines = 0;
        for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
        {
            SHL_Json::Value item;
            item["name"]   = i->second->name();
            item["status"] = i->second->online();
            item["id"]     = i->second->id();
            

            if (i->second->online())
            {
                onlines++;
            }

            value["users"].append(item);
        }

        value["totalNum"] = size();
        value["onlines"]  = onlines;

        string json = SHL_Json::writeString(writer, value);
        m_ui->onUpdate(json, NULL, 0);
    }

    {
        AutoLock locker(_mtx);
        for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
        {
            i->second->updateUi();
        }
    }
}


uint32_t
UserMgr::optlevel(const string &userid)
{
    AutoLock locker(_mtx);
    for (map<string, User *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (userid == i->second->id())
        {
            return i->second->optlevel();
        }
    }

    return 0;
}