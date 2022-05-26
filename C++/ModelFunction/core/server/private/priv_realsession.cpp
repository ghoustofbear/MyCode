#include "priv_realsession.h"
#include "priv_server.h"
#include "bufferev.h"

PrivRealSession::PrivRealSession(Client *client, Bufferev* bev)
    : RealSession(client)
{
    assert(bev != NULL);
    m_bev = bev;    
    m_bev->addBuffevListener(this);
}

PrivRealSession::~PrivRealSession()
{
    m_bev->delBuffevListener(this);
}

//void
//PrivRealSession::stop()
//{
//    //privSV->delBuffevListener(m_bev, this);
//
//    RealSession::stop();
//}
