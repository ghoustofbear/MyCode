#include "session.h"
#include "bufferev.h"
#include "su.h"

Session::Session()
{
    m_status = S_SOK;
    m_active = true;
    m_bev    = NULL;
}

Session::~Session()
{
  
}

int
Session::status()
{
    return m_status;
}

void
Session::setbev(Bufferev *bev)
{
    //m_status = S_SOK;
    //m_active = true;

    //AutoLock locker(m_bevMtx);
    //m_bev = bev;
}

void
Session::onDestory(Bufferev *bev)
{
    AutoLock locker(m_bevMtx);
    m_bev = NULL;
}

void
Session::onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen)
{
    if (bev)
    {
        bev->push(data, inlen);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

DSession::DSession()
{
    m_status = SS_UNINIT;
    m_active = true;
    m_bev = NULL;
}

DSession::~DSession()
{
    m_status = SS_CLOSED;
    AutoLock locker(_mtx);
    clear();
}

void
DSession::addStreamSpeaker(Su* speaker)
{
    AutoLock locker(_mtx);
    list<Su *>::const_iterator i = find(begin(), end(), speaker);
    if (i == end())
    {
        push_back(speaker);
    }
}

void
DSession::delStreamSpeaker(Su* speaker)
{
    AutoLock locker(_mtx);
    list<Su *>::iterator i = find(begin(), end(), speaker);
    if (i != end())
    {
        erase(i);
    }
}


void
DSession::notifyVideoStream(void *stream, uint32_t size)
{
    AutoLock lock(_mtx);
    for (list<Su*>::const_iterator i = cbegin(); i != cend(); ++i)
    {
        (*i)->doVideoStream(stream, size);
    }
}

void
DSession::notifyAudioStream(void *stream, uint32_t size)
{
    AutoLock lock(_mtx);
    for (list<Su*>::const_iterator i = cbegin(); i != cend(); ++i)
    {
        (*i)->doAudioStream(stream, size);
    }
}

void
DSession::notifyStatus(int code)
{
    AutoLock lock(_mtx);
    for (list<Su*>::const_iterator i = cbegin(); i != cend(); ++i)
    {
        (*i)->doStatus(code);
    }
}

int
DSession::status()
{
    return m_status;
}


void
DSession::setbev(Bufferev *bev)
{
    //m_status = SS_OPENING;
    //m_active = true;

    //AutoLock locker(m_bevMtx);
    //m_bev = bev;
}

void
DSession::onDestory(Bufferev *bev)
{
    m_status = SS_CLOSED;
    AutoLock locker(m_bevMtx);
    m_bev = NULL;
}

void
DSession::onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen)
{
    if (bev)
    {
        bev->push(data, inlen);
    }
}