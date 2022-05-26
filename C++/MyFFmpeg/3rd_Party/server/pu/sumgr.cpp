#include "sumgr.h"
#include "onuiupdate.h"
#include "su.h"
#include "json/json.h"


SuMgr _dualtalkSuMgr;
SuMgr *dualtalkSuMgr = &_dualtalkSuMgr;

SuMgr::SuMgr()
{
    m_ui = NULL;
}

SuMgr::~SuMgr()
{

}

void
SuMgr::addSu(Su *su)
{
    AutoLock locker(_mtx);

    list<Su *>::const_iterator i = find(begin(), end(), su);
    if (i == end())
    {
        push_back(su);
    }
}

void
SuMgr::delSu(Su *su)
{
    AutoLock locker(_mtx);

    list<Su *>::iterator i = find(begin(), end(), su);
    if (i != end())
    {
        erase(i);
    }
}

void
SuMgr::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
SuMgr::setUi(const string &addrcode, OnUiUpdate *ui)
{
    AutoLock locker(_mtx);

    for (list<Su *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (addrcode == (*i)->addrcode())
        {
            (*i)->setUi(ui);
            break;
        }
    }
}

void
SuMgr::updateUi()
{
    if (m_ui)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        AutoLock locker(_mtx);

        double totalBps = 0;
        int    totalRef = 0;
        for (list<Su *>::const_iterator i = begin(); i != end(); ++i)
        {
            SHL_Json::Value item;
            item["puname"] = (*i)->puname();
            item["puip"]   = (*i)->puip();
            item["guname"] = (*i)->name();
            item["gucode"] = (*i)->addrcode();

            double bps  = (*i)->bps();
            item["bps"] = bps;

            totalRef++;
            totalBps += bps;

            value["sus"].append(item);
        }

        value["totalBps"] = totalBps;
        value["totalRef"] = totalRef;

        string json = SHL_Json::writeString(writer, value);
        m_ui->onUpdate(json, NULL, 0);
    }

    {
        AutoLock locker(_mtx);
        for (list<Su *>::const_iterator i = begin(); i != end(); ++i)
        {
            (*i)->updateUi();
        }
    }
}
