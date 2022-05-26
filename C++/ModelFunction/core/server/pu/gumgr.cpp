#include "gumgr.h"
#include "onuiupdate.h"
#include "gu.h"
#include "json/json.h"

GuMgr _realGuMgr;
GuMgr _recordGuMgr;
GuMgr *realGuMgr   = &_realGuMgr;
GuMgr *recordGuMgr = &_recordGuMgr;

GuMgr::GuMgr()
{
    m_ui = NULL;
}

GuMgr::~GuMgr()
{

}

void
GuMgr::addGu(Gu *gu)
{
    AutoLock locker(_mtx);

    list<Gu *>::const_iterator i = find(begin(), end(), gu);
    if (i == end())
    {
        push_back(gu);
    }
}

void
GuMgr::delGu(Gu *gu)
{
    AutoLock locker(_mtx);

    list<Gu *>::iterator i = find(begin(), end(), gu);
    if (i != end())
    {
        erase(i);
    }
}

void
GuMgr::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
GuMgr::setUi(const string &addrcode, OnUiUpdate *ui)
{
    AutoLock locker(_mtx);

    for (list<Gu *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (addrcode == (*i)->addrcode())
        {
            (*i)->setUi(ui);
            break;
        }
    }
}

void
GuMgr::updateUi()
{
    if (m_ui)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        AutoLock locker(_mtx);

        double totalBps = 0;
        int    totalRef = 0;
        for (list<Gu *>::const_iterator i = begin(); i != end(); ++i)
        {
            SHL_Json::Value item;
            item["puname"] = (*i)->puname();
            item["puip"]   = (*i)->puip();
            item["guname"] = (*i)->name();
            item["gucode"] = (*i)->addrcode();
            item["ref"]    = (*i)->size();
            item["streamType"] = (*i)->streamType();
            item["streamProtoType"] = (*i)->streamProtoType();
			item["guid"] = (*i)->guid();

            double bps  = (*i)->bps();
            item["bps"] = bps;

            totalRef += (*i)->size();
            totalBps += bps;

            value["gus"].append(item);
        }

        value["totalBps"] = totalBps;
        value["totalRef"] = totalRef;

        string json = SHL_Json::writeString(writer, value);
        m_ui->onUpdate(json, NULL, 0);
    }

    {
        AutoLock locker(_mtx);
        for (list<Gu *>::const_iterator i = begin(); i != end(); ++i)
        {
            (*i)->updateUi();
        }
    }
}

void
GuMgr::bandWidth(uint32_t &u32Bandwidth, uint32_t &u32StreamNum)
{
    AutoLock locker(_mtx);
    for (list<Gu *>::const_iterator i = begin(); i != end(); ++i)
    {
        u32StreamNum += (*i)->size();
        double bps = (*i)->bps();
        u32Bandwidth += (bps * ((*i)->size()));
    }
}