#include "render_sdl.h"
#include "tls_error.h"
#include "renderlistener.h"

#if 0
static int eventloop(void *render)
{
    SDLRender *rdr = (SDLRender *)render;

    //Event handler
    SDL_Event e;
    while (1)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            rdr->handleEvent(e);
        }
    }

    return 0;
}
#endif

class SDLInitializer
{
public:
    SDLInitializer()
    {
        /* Initialize SDL */
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ERRF_PLY("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        }
    }
};

SDLRender::SDLRender(HWND wnd, bool useTxt)
    : Render(wnd)
{
    m_type     = R_SDL;
    m_useTxt   = useTxt;
    m_window   = NULL;
    m_renderer = NULL;
    m_texture  = NULL;
    m_windowID = -1;
    m_left = -1;
    m_top = -1;
}

SDLRender::~SDLRender()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }

    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = NULL;
    }

    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = NULL;
    }
}

int
SDLRender::init()
{
    //static SDLInitializer initializer;
    if (m_useTxt)
    {
        AutoLock locker(m_mtx);
        Uint32 pixformat = 0;

        if (m_init)
        {
            if (m_texture)
            {
                SDL_DestroyTexture(m_texture);
                m_texture = NULL;
            }

            if (m_renderer)
            {
                SDL_DestroyRenderer(m_renderer);
                m_renderer = NULL;
            }

            if (m_window)
            {
                SDL_DestroyWindow(m_window);
                m_window = NULL;
            }

            m_init = false;
        }

        m_window = SDL_CreateWindowFrom((const void *)m_wnd);
        if (!m_window)
        {
            ERRE_PLY("SDL_CreateWindowFrom failed: %s", SDL_GetError());

            goto exit;
        }

        /* fix: no video when replay in the next time */
        SDL_ShowWindow(m_window);

        /* Set texture filtering to linear */
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            ERRE_PLY("SDL_SetHint Warning: Linear texture filtering not enabled: %s", SDL_GetError());

            SDL_DestroyWindow(m_window);
            m_window = NULL;

            goto exit;
        }

        /* Create renderer for window */
#ifdef _WIN32
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
#else
        m_renderer = SDL_CreateRenderer(m_window, 1, SDL_RENDERER_ACCELERATED);
#endif
        /* m_renderer = SDL_CreateRenderer(m_window, 1, SDL_RENDERER_SOFTWARE); */
        if (!m_renderer)
        {
            ERRE_PLY("SDL_CreateRenderer failed: %s", SDL_GetError());

            SDL_DestroyWindow(m_window);
            m_window = NULL;

            goto exit;
        }

        /* Initialize renderer color */
        /* SDL_SetRenderDrawColor(m_renderer, 0x00, 0xFF, 0xFF, 0xFF); */

        /* Grab window identifier */
        m_windowID = SDL_GetWindowID(m_window);

        pixformat = SDL_PIXELFORMAT_IYUV;

        m_texture = SDL_CreateTexture(m_renderer, pixformat, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
        if (!m_texture)
        {
            ERRE_PLY("SDL_CreateTexture failed: %s", SDL_GetError());

            SDL_DestroyWindow(m_window);
            m_window = NULL;

            SDL_DestroyRenderer(m_renderer);
            m_renderer = NULL;

            goto exit;
        }

#if 0
        SDL_CreateThread(eventloop, "event", (void *)this);
#endif
        m_init = true;

        return 0;
    }
    else
    {

    }

exit:
    m_init = false;

    return -1;
}

int
SDLRender::changeWndSize(int width, int height)
{
#ifdef __LINUX__
    if (m_useTxt)
    {

        AutoLock locker(m_mtx);
        ERRE_PLY("changeWndSize w: %d h: %d\n", width, height);

        /* will crash */
        /* SDL_RenderClear(m_renderer); crash */
        SDL_RenderPresent(m_renderer);
        SDL_SetWindowSize(m_window, width, height);
        //if(m_left>=0)//songhongliang
        {
            SDL_SetWindowPosition(m_window,m_left,m_top);
        }
    }
#endif

    return 0;
}

int
SDLRender::changeWndPos(int left, int top)
{
#ifdef __LINUX__
    //songhongliang
//    if(left<0)
//        return -1;
    m_left = left;
    m_top = top;
#endif
    return 0;
}

void
SDLRender::onYUV(void *data, int size, int w, int h)
{
    if (!m_init || m_width != w || m_height != h)
    {
        m_width  = w;
        m_height = h;

        init();
    }

    if (!m_init)
    {
        return;
    }

    if (m_useTxt)
    {
        AutoLock locker(m_mtx);

        SDL_UpdateTexture(m_texture, NULL, data, w);

        SDL_Rect sdlRect;

        int width  = 0;
        int height = 0;

        SDL_GetWindowSize(m_window, &width, &height);

        /* FIX: If window is resize */
        sdlRect.x = 0;
        sdlRect.y = 0;
        sdlRect.w = width;
        sdlRect.h = height;

        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_texture, NULL, &sdlRect);

#ifdef __LINUX__
        if (m_listener)
        {
            m_listener->onHDC(m_renderer, m_wnd, width, height);
        }
#endif

#if 0
        //Draw blue horizontal line
        SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawLine(m_renderer, 0, height / 3, width / 2, height / 2);

        //Draw vertical line of yellow dots
        SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0xFF, 0xFF);

        for (int i = 0; i < height; i += 2)
        {
            SDL_RenderDrawPoint(m_renderer, width / 2, i);
        }
#endif
        SDL_RenderPresent(m_renderer);
    }
    else { }
}

void
SDLRender::onRGB(void *data, int size, int w, int h)
{

}

#if 0
void
SDLRender::handleEvent(SDL_Event& e)
{
    /* If an event was detected for this window */
    if (e.type == SDL_WINDOWEVENT && e.window.windowID == m_windowID)
    {
        switch (e.window.event)
        {
        /* Window appeared */
        case SDL_WINDOWEVENT_SHOWN:
            break;

        /* Window disappeared */
        case SDL_WINDOWEVENT_HIDDEN:
            break;

        /* Get new dimensions and repaint */
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            m_width = e.window.data1;
            m_height = e.window.data2;
            /* SDL_RenderPresent(mRenderer); */
            printf("SDL_WINDOWEVENT_SIZE_CHANGED xxxxxxxxxxxxxxxxxxx\n");
            break;

#if 0
        //Repaint on expose
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(mRenderer);
            printf("SDL_WINDOWEVENT_EXPOSED\n");
            break;

        //Mouse enter
        case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            updateCaption = true;
            printf("SDL_WINDOWEVENT_ENTER:\n");
            break;

        //Mouse exit
        case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            updateCaption = true;
            printf("SDL_WINDOWEVENT_LEAVE:\n");
            break;

        //Keyboard focus gained
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            updateCaption = true;
            printf("SDL_WINDOWEVENT_FOCUS_GAINED:\n");
            break;

        //Keyboard focus lost
        case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            updateCaption = true;
            break;

        //Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

        //Window maxized
        case SDL_WINDOWEVENT_MAXIMIZED:
            mMinimized = false;
            printf("SDL_WINDOWEVENT_MAXIMIZED:\n");
            break;

        //Window restored
        case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            printf("SDL_WINDOWEVENT_RESTORED:\n");
            break;

        //Hide on close
        case SDL_WINDOWEVENT_CLOSE:
            SDL_HideWindow(mWindow);
            printf("SDL_WINDOWEVENT_CLOSE:\n");
            break;
#endif
        }
    }
}
#endif
