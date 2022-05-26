/**
 * @file      preview.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-06 11:22:54
 *
 * @note
 *  preview.h defines
 */
#ifndef __PREVIEW_H__
#define __PREVIEW_H__
#include "def.h"
#include "streamlistener.h"
#include "playerlistener.h"
#include "c2sdk.h"
#include <mutex>
#include "thread.h"
#include "tlist.h"
#include "tmap.h"

class Player;
class Cu;
class IWriter;

#ifdef UNIX_DRAW_RECT
extern int g_rectShowType; //0:wai jie,1:not
extern int g_roiShow; //0:show,1:not show
extern Mutex g_sdkParamMtx;

struct ROIRgb
{
    int r;
    int g;
    int b;
};

struct ROIPoint
{
    float x;
    float y;
};

struct ROIRect
{
    ROIPoint point[2];
    int rioId;
    string status;
    Mutex mtx;
};

typedef  struct ROIRect ROILine;
struct ROIPolygon
{
    list<ROIPoint> lsp;
    int rioId;
    string status;
};
#endif

class Preview : public StreamListener, public PlayerListener
{
#define STATUS(code) \
    do \
    { \
        if (m_statusCB) \
        { \
            m_statusCB(code, m_statusData); \
        } \
    } while(0);

public:
    Preview(Cu *cu);
    virtual ~Preview();

    int                start(string json, HWND wnd, c2_stream_cb streamCB, void *userData, c2_status_cb statusCB, void *statusData);
    void               stop();

    int                startTalk();
    void               stopTalk();
    int                getStatus();

    string             getJson() { return m_json; }
    Cu               * getCu() { return m_cu; }

    int                capture(string filepath, int type);

    int                startREC(string filepath);
    void               stopREC();


    int                addWnd(HWND wnd);
    int                delWnd(HWND wnd);

    int                changeWndSize(int width, int height);
    int                changeWndPos(int x, int y);

    int                setDisplayRegion(float left, float right, float top, float bottom);

    int                setDrawCallback(c2_draw_cb drawCB, void *userData);
    int                setDecCallback(c2_decode_cb decodeCB, void *userData);

    int                opensound();
    int                closesound();
    int                setvolume(int vol);

    void               timerdriver();
	
	#ifdef UNIX_DRAW_RECT
    int                getChannelNo(){return m_channelNo;}
	string             getDeviceIp() { return m_devIp; }
	int                getDevicePort() { return m_devPort; }
    void                drawROI(std::string json) ;
    int                 ctrlROI(std::string json) {return 0;}
    int                 clearROI(std::string json) {return 0;}
    void                onIvaResult(std::string json);
    virtual void       onDrawRoi(string strAlarmMsg);
	virtual void       onCtrlROI(string strAlarmMsg);
    void               drawRect(HDC hdc, HWND wnd, int w, int h);
    void               drawLine(HDC hdc, HWND wnd, int w, int h);
    void               drawPol(HDC hdc, HWND wnd, int w, int h);

    void                combRectShowModel();
	#endif
protected:
    virtual void       onStream(void *stream, uint32_t size);
    virtual void       onStatus(int code);

    virtual void       onEmpty(uint32_t size);
    virtual void       onFull(uint32_t size);
    virtual void       onEnd();
    virtual void       onError();

    virtual void       onYUV(int type, int w, int h, void *yuv, int size);
    virtual void       onHDC(HDC hdc, HWND wnd, int w, int h);
private:
    Cu               * m_cu;
    string             m_json;
    int                m_channelNo;
	string             m_devIp;
	int                m_devPort;
    HWND               m_wnd;
    c2_stream_cb       m_streamCB;
    void             * m_streamData;
    c2_status_cb       m_statusCB;
    void             * m_statusData;


    c2_draw_cb         m_drawCB;
    void             * m_drawData;
    c2_decode_cb       m_decodeCB;
    void             * m_decodeData;

    Player           * m_player;
    int                m_pre_status;
	int                m_width, m_height;


#ifdef __ANDROID__
    mutex              m_wrmtx;
    IWriter          * m_wr;
#endif

#ifdef UNIX_DRAW_RECT
    tmap<int, list<ROIRect>> m_mapRect;
    tmap<int, list<ROILine>> m_mapLine;
    tmap<int, list<ROIPolygon>> m_mapPol;
    
    Mutex m_mtx;

	string             m_strAlarmMsg;
#endif
};

#endif
