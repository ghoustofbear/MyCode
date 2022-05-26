#include "priv_recordsession.h"
#include "priv_server.h"
#include "bufferev.h"

PrivRecordSession::PrivRecordSession(Client *client, Bufferev *bev, int download)
    : RecordSession(client)
{
    assert(bev != NULL);
    m_bev = bev;
    m_bev->addBuffevListener(this);
}

PrivRecordSession::~PrivRecordSession()
{
    m_bev->delBuffevListener(this);
}

void
PrivRecordSession::stop()
{  
    RecordSession::stop();
}
