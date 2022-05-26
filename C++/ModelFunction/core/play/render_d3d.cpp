#include "render_d3d.h"
#include "renderlistener.h"
#include "tls_error.h"

D3DRender::D3DRender(HWND wnd)
    : Render(wnd)
{
    m_type   = R_D3D;
    m_d3d9   = NULL;
    m_d3dd   = NULL;
    m_d3ds   = NULL;
    memset(&m_view, 0, sizeof(m_view));
}

D3DRender::~D3DRender()
{
    if (m_d3d9)
    {
        m_d3d9->Release();
        m_d3d9 = NULL;
    }

    if (m_d3dd)
    {
        m_d3dd->Release();
        m_d3dd = NULL;
    }

    if (m_d3ds)
    {
        m_d3ds->Release();
        m_d3ds = NULL;
    }
}

int
D3DRender::init()
{
    if (m_init)
    {
        if (m_d3d9)
        {
            m_d3d9->Release();
            m_d3d9 = NULL;
        }

        if (m_d3dd)
        {
            m_d3dd->Release();
            m_d3dd = NULL;
        }

        if (m_d3ds)
        {
            m_d3ds->Release();
            m_d3ds = NULL;
        }
        m_init = false;
    }

    HRESULT result = -1;

    m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_d3d9)
    {
        ERRE_PLY("D3D CreateDevice failed.");

        goto exit;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.Windowed         = TRUE;
    d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    GetClientRect(m_wnd, &m_view);

    result = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_wnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                  &d3dpp, &m_d3dd);
    if (FAILED(result))
    {
        ERRE_PLY("D3D CreateDevice failed.");

        goto exit;
    }

    result = m_d3dd->CreateOffscreenPlainSurface(
                 m_width, m_height,
                 (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
                 D3DPOOL_DEFAULT,
                 &m_d3ds,
                 NULL);

    if (FAILED(result))
    {
        ERRE_PLY("D3D CreateOffscreenPlainSurface failed.");

        goto exit;
    }

    m_init = true;

    return 0;

exit:
    m_init = false;

    return -1;
}

void
D3DRender::onYUV(void *data, int size, int w, int h)
{
    do
    {
        if (!m_init || m_width != w || m_height != h)
        {
            m_width  = w;
            m_height = h;

            init();
            break;
        }

        RECT rc;
        GetClientRect(m_wnd, &rc);

        if (m_view.bottom != rc.bottom || m_view.left != rc.left)
        {
            init();
            break;
        }

        HRESULT result = m_d3dd->TestCooperativeLevel();
        if (result == D3DERR_DEVICELOST)
        {
            LOGT_PLY("D3DERR_DEVICELOST.");
        }
        else if (result == D3DERR_DEVICENOTRESET)
        {
            LOGW_PLY("D3DERR_DEVICENOTRESET.");
            init();
        }
    }
    while (0);

    if (!m_init)
    {
        return;
    }

    HRESULT result = -1;

    if (!m_d3dd || !m_d3ds)
    {
        return;
    }

    D3DLOCKED_RECT d3d_rect;
    result = m_d3ds->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
    if (FAILED(result))
    {
        ERRE_PLY("D3DS LockRect failed.");
        return;
    }

    uint8_t * src    = (uint8_t *)data;
    uint8_t * dest   = (uint8_t *)d3d_rect.pBits;
    int       stride = d3d_rect.Pitch;

    for (int i = 0; i < h; i++)
    {
        memcpy(dest + i * stride, src + i * w, w);
    }

    for (int i = 0; i < h / 2; i++)
    {
        memcpy(dest + stride * h + i * stride / 2, src + w * h + w * h / 4 + i * w / 2, w / 2);
    }

    for (int i = 0; i < h / 2; i++)
    {
        memcpy(dest + stride * h + stride * h / 4 + i * stride / 2, src + w * h + i * w / 2, w / 2);
    }

    result = m_d3ds->UnlockRect();
    if (FAILED(result))
    {
        return;
    }

    if (!m_d3dd)
    {
        return;
    }

    m_d3dd->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    m_d3dd->BeginScene();
    IDirect3DSurface9 * d3ds = NULL;

    m_d3dd->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3ds);
    m_d3dd->StretchRect(m_d3ds, NULL, d3ds, &m_view, D3DTEXF_LINEAR);
    m_d3dd->EndScene();
    m_d3dd->Present(NULL, NULL, NULL, NULL);

    if (m_listener)
    {
        HDC hdc = NULL;
        if (S_OK == d3ds->GetDC(&hdc))
        {
            m_listener->onHDC(hdc, m_wnd, m_view.right - m_view.left, m_view.bottom - m_view.top);
            d3ds->ReleaseDC(hdc);
        }
    }
    d3ds->Release();
}

void
D3DRender::onRGB(void *data, int size, int w, int h)
{

}
