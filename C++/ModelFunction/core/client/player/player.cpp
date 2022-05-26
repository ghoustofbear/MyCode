#include "player.h"
#include "playerlistener.h"
#include "tls_error.h"


string Player::m_jvm2nativePath = "";
int
Player::initLib(const char* jvm2nativePath)
{


     m_jvm2nativePath = jvm2nativePath;

     ERRI_SDK("Player lib path %s\n", m_jvm2nativePath.c_str());
    
    return 0;
}

void
Player::uninitLib()
{

}

Player:: Player(PlayerListener * listener)
{
    m_encode   = 0;
    m_wnd      = 0;
    m_isOpen   = false;
    m_pos      = 0;
    m_status   = S_STOP;
    m_speed    = 0;
    m_repos    = false;
    m_encode   = false;
    m_picw     = 0;
    m_pich     = 0;
    m_sound    = 0;
    m_listener = listener;

	m_tmcnt    = 0;
}

Player::~Player()
{

}

void
Player::setPlayerListener(PlayerListener* listener)
{
    m_listener = listener;
}

int
Player::type()
{
    return m_type;
}

int
Player::changeWndSize(int width, int height)
{
    return 0;
}

int
Player::changeWndPos(int width, int height)
{
    return 0;
}

void
Player::notifyEmpty(uint32_t size)
{
    //ERRI_SDK("Play buffer is going empty.");

    if (m_listener)
    {
        //printf("Empty\n");
        m_listener->onEmpty(size);
    }
}

void
Player::notifyFull(uint32_t size)
{
    //ERRI_SDK("Play buffer is going full.");

    if (m_listener)
    {
        //printf("full\n");
        m_listener->onFull(size);
    }
}

void
Player::notifyEnd()
{
    ERRI_SDK("Play buffer is empty.");

    if (m_listener)
    {
        m_listener->onEnd();
    }
}

void
Player::notifyError()
{
    if (m_listener)
    {
        m_listener->onError();
    }
}

void
Player::notifyYUV(int type, int w, int h, void *yuv, int size)
{
    if (m_listener)
    {
        m_listener->onYUV(type, w, h, yuv, size);
    }
}

void
Player::notifyHDC(HDC hdc, HWND wnd, int w, int h)
{
    if (m_listener)
    {
        m_listener->onHDC(hdc, wnd, w, h);
    }
}

void
Player::reposEnd(bool end)
{
    m_repos = end ? false : true;
}
