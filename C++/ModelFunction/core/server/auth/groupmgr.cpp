#include "groupmgr.h"
#include "group.h"
#include "sys.h"
#include "cfg_group.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

GroupMgr::GroupMgr()
{
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    groupCfg->addObserver(this);
}

GroupMgr::~GroupMgr()
{
    groupCfg->deleteObserver(this);

    {
        AutoLock locker(_mtx);
        for (map<string, Group *>::const_iterator i = begin(); i != end(); ++i)
        {
            delete i->second;
        }
        clear();
    }
}

int
GroupMgr::init()
{
    string json;
#if 0
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["id"]       = sys->generateUID();
    value["type"]     = 0;
    value["name"]     = "MGroup1";
    value["rightBit"] = 0xFFFFFF;
    value["level"]    = 2;

    json = writer.write(value);
    groupCfg->add(json);

    value["id"]       = sys->generateUID();
    value["type"]     = 1;
    value["name"]     = "OGroup1";
    value["rightBit"] = 0xFFFFFF;
    value["level"]    = 3;

    json = writer.write(value);
    groupCfg->add(json);
#else
    groupCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    for (uint32_t i = 0; i < rvalue.size(); i++)
    {
        AutoLock locker(_mtx);
        Group *group = new Group(
            rvalue[i]["id"         ].asString(),
            rvalue[i]["type"       ].asInt(),
            rvalue[i]["name"       ].asString(),
            rvalue[i]["rightBit"   ].asUInt64(),
            rvalue[i]["previewBit" ].asUInt64(),
            rvalue[i]["playbackBit"].asUInt64(),
            rvalue[i]["ctrlBit"    ].asUInt64(),
            rvalue[i]["level"      ].asInt()
        );
        (*this)[rvalue[i]["id"].asString()] = group;
    }
#endif

    return 0;
}

Group *
GroupMgr::group(string id)
{
    AutoLock locker(_mtx);
    return (*this)[id];
}

void
GroupMgr::update(const std::string &table, const std::string &json, int type)
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


        string gid = rvalue["id"].asString();
        int tttype = rvalue["type"].asInt();

        uint64_t bi = rvalue["rightBit"].asUInt64();

        bi = rvalue["previewBit"].asUInt64();
        bi = rvalue["playbackBit"].asUInt64();
        bi = rvalue["ctrlBit"].asUInt64();

        Group *group = new Group(
            rvalue["id"         ].asString(),
            rvalue["type"       ].asInt(),
            rvalue["name"       ].asString(),
            rvalue["rightBit"   ].asUInt64(),
            rvalue["previewBit" ].asUInt64(),
            rvalue["playbackBit"].asUInt64(),
            rvalue["ctrlBit"    ].asUInt64(),
            rvalue["level"      ].asInt()
        );

        (*this)[rvalue["id"].asString()] = group;
    }
    else if (O_DELETE == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        Group* group = (*this)[rvalue["id"].asString()];

        delete group;
        erase(rvalue["id"].asString());
    }
    else if (O_MODIFY == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        Group* group = (*this)[rvalue["id"].asString()];
        group->modify(
            rvalue["name"       ].asString(),
            rvalue["rightBit"   ].asUInt64(),
            rvalue["previewBit" ].asUInt64(),
            rvalue["playbackBit"].asUInt64(),
            rvalue["ctrlBit"    ].asUInt64(),
            rvalue["level"      ].asInt()
        );
    }
}
