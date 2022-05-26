#include "render.h"

Render::Render(HWND wnd)
{
    m_wnd      = wnd;
    m_width    = 0;
    m_height   = 0;
    m_init     = false;
    m_listener = NULL;
}

Render::~Render()
{

}
bool
Render::isSame(HWND wnd)
{
    return (wnd == m_wnd);
}

void
Render::setRenderListener(RenderListener *listener)
{
    m_listener = listener;
}

int
Render::changeWndSize(int width, int height)
{
    return 0;
}

int
Render::changeWndPos(int left, int top)
{
    return 0;
}
