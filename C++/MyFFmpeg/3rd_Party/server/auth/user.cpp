#include "user.h"
#include "groupmgr.h"
#include "group.h"
#include "cfg_group.h"
#include "cfg_user_group.h"
#include "sys.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

User::User(string id, string addrcode, string name, string realName, string pswd, string identity, string phone, string email, bool relogin, bool aaa)
{
    m_id       = id;
    m_addrcode = addrcode;
    m_name     = name;
    m_realName = realName;
    m_pswd     = pswd;
    m_identity = identity;
    m_phone    = phone;
    m_email    = email;
    m_relogin  = relogin;
    m_aaa      = aaa;
    m_ui       = NULL;

    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    groupCfg->addObserver(this);
    usrgrCfg->addObserver(this);
}

User::~User()
{
    groupCfg->deleteObserver(this);
    usrgrCfg->deleteObserver(this);
}

int
User::init()
{
    string json;
#if 0
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["userId"]  = m_id;
    for (map<string, Group *>::const_iterator i = groupMgr->begin(); i != groupMgr->end(); ++i)
    {
        value["groupId"] = i->second->id();
        json = writer.write(value);
        usrgrCfg->add(json);
    }
#else

    usrgrCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    for (uint32_t i = 0; i < rvalue.size(); i++)
    {
        if (id() != rvalue[i]["userId"].asString())
        {
            continue;
        }

        if (0 == groupMgr->group(rvalue[i]["groupId"].asString())->type())
        {
            AutoLock locker(m_mright._mtx);
            m_mright.push_back(rvalue[i]["groupId"].asString());
        }

        if (1 == groupMgr->group(rvalue[i]["groupId"].asString())->type())
        {
            AutoLock locker(m_oright._mtx);
            m_oright.push_back(rvalue[i]["groupId"].asString());
        }
    }
#endif

    return 0;
}

string
User::id()
{
    return m_id;
}

string
User::name()
{
    return m_name;
}

string
User::addrcode()
{
    return m_addrcode;
}

bool
User::online()
{
    return size() > 0 ? true : false;
}

uint64_t
User::mright()
{
    uint64_t bitmap = 0;
    AutoLock locker(m_mright._mtx);
    for (size_t i = 0; i < m_mright.size(); ++i)
    {
        bitmap |= groupMgr->group(m_mright[i])->right();
    }

    return bitmap;
}

uint64_t
User::oright()
{
    uint64_t bitmap = 0;
    AutoLock locker(m_oright._mtx);

    for (size_t i = 0; i < m_oright.size(); ++i)
    {
        bitmap |= groupMgr->group(m_oright[i])->right();
    }

    return bitmap;
}

uint64_t
User::optlevel()
{
	int maxright = 0;
	AutoLock locker(m_oright._mtx);

	for (size_t i = 0; i < m_oright.size(); ++i)
	{
        int right = groupMgr->group(m_oright[i])->optlevel();
		if (right > maxright)
		{
			maxright = right;
		}
	}

	return maxright;
}

int
User::authent(const string &crypto, uint64_t &mbitmap, uint64_t &obitmap, bool &relogin)
{
    {
        AutoLock locker(_mtx);
        if (!m_relogin && size() > 0)
        {
            return E_HAS_LOGINED;
        }
    }

    if (m_pswd != crypto)
    {
        return E_FAILED;
    }

    mbitmap = mright();
    obitmap = oright();
    relogin = m_relogin;

    return E_OK;
}

void
User::addAuthListener(AuthListener *listener)
{
    AutoLock locker(_mtx);
    list<AuthListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
User::delAuthListener(AuthListener *listener)
{
    AutoLock locker(_mtx);
    list<AuthListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

void
User::notifyChanged(const string &json)
{
    AutoLock locker(_mtx);
    for (list<AuthListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onChanged(json);
    }
}

void
User::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && !rvalue.isNull()))
    {
        return;
    }

    if (!rvalue.isObject())
    {
        return;
    }
    


    if ("tbl_group" == table)
    {
        if (type == O_ADD)
        {
        }
        else if (type == O_DELETE)
        {
        }
        else if (type == O_MODIFY)
        {
        }
    }
    else if ("tbl_user_group" == table)
    {
        if (id() != rvalue["userId"].asString())
        {
            return;
        }

        if (type == O_ADD)
        {
            if (0 == groupMgr->group(rvalue["groupId"].asString())->type())
            {
                AutoLock locker(m_mright._mtx);
                m_mright.push_back(rvalue["groupId"].asString());
            }

            if (1 == groupMgr->group(rvalue["groupId"].asString())->type())
            {
                AutoLock locker(m_oright._mtx);
                m_oright.push_back(rvalue["groupId"].asString());
            }
        }
        else if (type == O_DELETE)
        {
            if (0 == groupMgr->group(rvalue["groupId"].asString())->type())
            {
                vector<string>::iterator it;
                for (it = m_mright.begin(); it != m_mright.end(); it++)
                {
                    if (*it == rvalue["groupId"].asString())
                    {
                        AutoLock locker(m_mright._mtx);
                        m_mright.erase(it);
                        break;
                    }
                }
            }

            if (1 == groupMgr->group(rvalue["groupId"].asString())->type())
            {
                vector<string>::iterator it;
                for (it = m_oright.begin(); it != m_oright.end(); it++)
                {
                    if (*it == rvalue["groupId"].asString())
                    {
                        AutoLock locker(m_oright._mtx);
                        m_oright.erase(it);
                        break;
                    }
                }
            }

        }
        else if (type == O_MODIFY)
        {
        }
    }

    notifyChanged(json);
}

void
User::modify(string name, string realName, string pswd, string identity, string phone, string email, bool relogin, bool aaa)
{
    m_name     = name;
    m_realName = realName;
    m_pswd     = pswd;
    m_identity = identity;
    m_phone    = phone;
    m_email    = email;
    m_relogin  = relogin;
    m_aaa      = aaa;
}

void
User::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
User::updateUi()
{
    if (m_ui && size() > 0)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        AutoLock locker(_mtx);

        for (list<AuthListener *>::const_iterator i = begin(); i != end(); ++i)
        {
            SHL_Json::Value item;
            item["ip"]   = (*i)->peerip();
            item["port"] = (*i)->peerport();
            item["time"] = (*i)->peertime();

            value["peers"].append(item);
        }

        string json = SHL_Json::writeString(writer, value);
        m_ui->onUpdate(json, NULL, 0);
    }
}
