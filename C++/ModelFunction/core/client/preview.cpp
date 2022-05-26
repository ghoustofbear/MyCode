#include "preview.h"
#include "cu.h"
#include "streamgr.h"
#include "frame.h"
#include "tls_error.h"
#include "hik_player.h"
#include "c2_player.h"
#include "json/json.h"
#include "hik_ps_player.h"
#include "dahua_player.h"
#include "localavstream.h"
#include "tindy_player.h"
#include "fsrw.h"
#include <map>
#ifdef _WIN32
#include "helmet_player.h"
#endif

#ifdef UNIX_DRAW_RECT
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>

int g_rectShowType = 0;
int g_roiShow = 1;
Mutex g_sdkParamMtx;

#endif






/* #include "dahua_player.h" */

Preview::Preview(Cu *cu)
{
    ASSERT(cu);

    m_cu         = cu;
    m_player     = NULL;

    m_wnd        = 0;
    m_streamCB   = NULL;
    m_streamData = NULL;
    m_statusCB   = NULL;
    m_statusData = NULL;

    m_drawCB     = NULL;
    m_drawData   = NULL;
    m_decodeCB   = NULL;
    m_decodeData = NULL;

	m_width = m_height = 0;

    m_pre_status = -1;
#ifdef __ANDROID__
    m_wr         = NULL;
#endif


#ifdef UNIX_DRAW_RECT
    
	m_strAlarmMsg = "";
#endif
}

Preview::~Preview()
{
    if (m_player)
    {
        delete m_player;
        m_player = NULL;
    }
#ifdef __ANDROID__
    if (m_wr)
    {
        delete m_wr;
        m_wr = NULL;
    }
#endif
}

int
Preview::start(string json, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    m_json       = json;
    m_wnd        = wnd;
    m_streamCB   = streamCB;
    m_streamData = streamData;
    m_statusCB   = statusCB;
    m_statusData = statusData;
	
#ifdef UNIX_DRAW_RECT
    m_channelNo = 0;
    
    SHL_Json::Reader reader;
    SHL_Json::Value rvalue;
    if (reader.parse(m_json,rvalue,false))
    {
        m_channelNo = rvalue["channelNo"].asInt();
		m_devIp = rvalue["deviceHost"].asString(); 
		m_devPort = rvalue["devicePort"].asInt();
    }
#endif
    return streamMgr->addRealStream(m_cu, m_json, this);
}



void
Preview::stop()
{
    stopTalk();

    streamMgr->delRealStream(m_cu, m_json, this);
    if (m_player)
    {
        m_player->close();
    }
}

int
Preview::getStatus()
{
    return m_pre_status;
}


int                
Preview::startTalk()
{
    //return streamMgr->addTalkStream(m_cu, m_json, this);
    return 0;
}

void
Preview::stopTalk()
{
    //m_pre_status = 0;
    //streamMgr->delTalkStream(m_cu, m_json, this);
}

void
Preview::onStream(void *stream, uint32_t size)
{
    ut_frame_s *hdr = (ut_frame_s *)stream;
    do
    {
        if (!m_wnd && !m_decodeCB)
        {
            break;
        }

        if (!m_player)
        {
#ifdef UNIX_DRAW_RECT
			drawROI(m_strAlarmMsg);
#endif
#ifdef __ANDROID__
            if (TAG_STD == hdr->tag)
            {
                m_player = new C2Player(this);
            }
#else
            if (TAG_STD == hdr->tag)
            {
              m_player = new HikPsPlayer(this);
            }
#endif
#ifndef __ANDROID__
            else if (TAG_HIK == hdr->tag || TAG_STD_PS == hdr->tag)
            {
                m_player = new HikPlayer(this, true);
            }
            else if (TAG_HIK_N == hdr->tag)
            {
                m_player = new HikPlayer(this, false);
            }
            else if (TAG_DAHUA == hdr->tag)
            {
                m_player = new Dahualayer(this, false);
            }

            else if (TAG_TIANDY == hdr->tag)
            {
                m_player = new TindyPlayer(this, false);
            }
#ifdef _WIN32
            else if (TAG_HELMET_GX == hdr->tag)
            {
                m_player = new HelmetPlayer(this);
            }
#endif
#endif

            if (!m_player)
            {
                onStatus(CODE_PLAY_FAIL);
                break;
			}
            if (m_player->open(m_wnd, Player::STREAM_REAL, hdr->encode, MAX_PVPOOL_SIZE) < 0)
            {
                delete m_player;
                m_player = NULL;

                onStatus(CODE_PLAY_FAIL);

                break;
            }

            onStatus(CODE_PLAY_SUCCESS);

            if (m_decodeCB)
            {
                m_player->enableDecode();
            }

            if (m_drawCB)
            {
                m_player->enableDraw();
            }

			//if (m_width != 0 || m_height != 0)
			//{				
			//	m_player->changeWndSize(m_width, m_height);
			//}
        }

        if (m_player)
        {
            int result = -1;
            if (TAG_STD == hdr->tag)
            {
                result = m_player->inputData(stream, size);
            }
            else
            {
                //result = m_player->inputData(hdr->buf, hdr->size);
                result = m_player->inputData(stream, size);
            }

            if (result < 0)
            {
                m_player->close();
                delete m_player;
                m_player = NULL;
                onStatus(CODE_PLAY_FAIL);
            }
        }
    }
    while (0);

    if (m_streamCB)
    {
        m_streamCB(m_wnd, stream, size, m_streamData);
    }

#ifdef __ANDROID__
    {
        lock_guard<mutex> locker(m_wrmtx);
        if (m_wr)
        {
            m_wr->write(stream);
        }
    }
#endif
}

void
Preview::onStatus(int code)
{
    m_pre_status = code;
    STATUS(code);
}

void
Preview::onEmpty(uint32_t size)
{

}

void
Preview::onFull(uint32_t size)
{

}

void
Preview::onEnd()
{
    onStatus(CODE_PLAY_END);
}

void
Preview::onError()
{
    onStatus(CODE_PLAY_FAIL);
}

void
Preview::onYUV(int type, int w, int h, void *yuv, int size)
{
    if (m_decodeCB)
    {
        m_decodeCB(type, w, h, yuv, size, m_decodeData);
    }
}

void
Preview::onHDC(HDC hdc, HWND wnd, int w, int h)
{
    if (m_drawCB)
    {
        m_drawCB(hdc, m_wnd, w, h, m_drawData);
    }
#ifdef UNIX_DRAW_RECT
    int roiShow = 0;
    {
        AutoLock lock(g_sdkParamMtx);
        roiShow = g_roiShow;
    }
    if (0 == roiShow)
    {
        drawRect(hdc, wnd, w, h);
        drawLine(hdc, wnd, w, h);
        drawPol(hdc, wnd, w, h);
    }
#endif
}

int
Preview::capture(string filepath, int type)
{
    if (m_player)
    {
        return m_player->capture(filepath, type);
    }

    ERRE_SDK("not playing.");
    onStatus(CODE_CAP_FAIL);

    return -1;
}

int
Preview::startREC(string filepath)
{
#ifdef __ANDROID__
    lock_guard<mutex> locker(m_wrmtx);
    if (m_wr)
    {
        delete m_wr;
        m_wr = NULL;
    }

    m_wr = new RecordWriter();
    filepath += ".utv";
    if (m_wr->open(filepath.c_str()) < 0)
    {
        delete m_wr;
        m_wr = NULL;
        return -1;
    }
    return 0;
#else
    if (m_player)
    {
        return m_player->startREC(filepath);
    }
#endif
    return -1;
}

void
Preview::stopREC()
{
#ifdef __ANDROID__
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_wr)
    {
        return;
    }

    m_wr->close();
    delete m_wr;
    m_wr = NULL;
#else
    if (m_player)
    {
        return m_player->stopREC();
    }
#endif
}


int
Preview::addWnd(HWND wnd)
{
    m_wnd = wnd;

    if (m_player)
    {
        return m_player->addWnd(wnd);
    }
    return -1;
}

int
Preview::delWnd(HWND wnd)
{


    if (m_player)
    {
        return m_player->delWnd(wnd);
    }
    return -1;
}

int
Preview::changeWndSize(int width, int height)
{
#ifdef UNIX_DRAW_RECT
    {
        AutoLock lock(m_mtx);

    }
#endif

	m_width = width;
	m_height = height;

    if (m_player)
    {		
        return m_player->changeWndSize(width, height);
    }
    ERRE_SDK("not playing. w:%d h:%d", m_width, m_height);
    
    return -1;
}

int
Preview::changeWndPos(int x, int y)
{
    if (m_player)
    {
        return m_player->changeWndPos(x, y);
    }

    ERRE_SDK("not playing.");
    onStatus(CODE_CAP_FAIL);

    return -1;
}


int
Preview::setDrawCallback(c2_draw_cb drawCB, void *userData)
{
    if (m_player)
    {
        m_player->enableDraw();
    }

    m_drawCB   = drawCB;
    m_drawData = userData;

    return 0;
}

int
Preview::setDecCallback(c2_decode_cb decodeCB, void *userData)
{
    if (m_player)
    {
        m_player->enableDecode();
    }

    m_decodeCB   = decodeCB;
    m_decodeData = userData;

    return 0;
}

int
Preview::opensound()
{
    if (!m_player)
    {
        ERRE_SDK("not playing.");
        return -1;
    }

    return m_player->opensound();
}

int
Preview::closesound()
{
    if (!m_player)
    {
        ERRE_SDK("not playing.");
        return -1;
    }

    return m_player->closesound();
}

int
Preview::setvolume(int vol)
{
    if (!m_player)
    {
        ERRE_SDK("not playing.");
        return -1;
    }

    return m_player->setVolume(vol);
}

void
Preview::timerdriver()
{
    if (m_player)
    {
        m_player->timerdriver();
    }
}

int
Preview::setDisplayRegion(float left, float right, float top, float bottom)
{
    if (m_player)
    {
        return m_player->setDisplayRegion(left, right, top, bottom);
    }
    return -1;
}

#ifdef UNIX_DRAW_RECT
void
Preview::onDrawRoi(string strAlarmMsg)
{
	m_strAlarmMsg = strAlarmMsg;

	drawROI(m_strAlarmMsg);
}

void 
Preview::onCtrlROI(string strAlarmMsg)
{
	ctrlROI(strAlarmMsg);
}


int getCombRoiId(map<int, vector<int>> &mapvRoi,int roiId)
{
    int key = -1;
    map<int, vector<int>>::iterator it = mapvRoi.begin();
    for (; it != mapvRoi.end(); ++it)
    {
        vector<int>::iterator vit = it->second.begin();
        for (; vit != it->second.end(); ++vit)
        {
            if (roiId == (*vit))
            {
                key = it->first;
                break;
            }
        }
    }
    return key;
}

void Preview::combRectShowModel()
{
    int rectShowType;
    {
        AutoLock lock(g_sdkParamMtx);
        rectShowType = g_rectShowType;
    }
    switch (rectShowType)
    {
    case 0:
    {
        {
            AutoLock lock(m_mapRect._mtx);
            tmap<int, list<ROIRect>>::iterator it = m_mapRect.begin();
            for (; it != m_mapRect.end(); ++it)
            {
                ROIRect trect;
                trect = *((*it).second.begin());
                (*it).second.clear();
                (*it).second.push_back(trect);
            }
        }
        {
            AutoLock lock(m_mapPol._mtx);
            m_mapPol.clear();
        }
    }
        break;
    case 1:
    {
        AutoLock lock(m_mapRect._mtx);
        tmap<int, list<ROIRect>>::iterator it = m_mapRect.begin();
        for (; it != m_mapRect.end(); ++it)
        {
            (*it).second.pop_front();
        }
    }
        break;
    default:
        break;
    }
}

void                
Preview::onIvaResult(std::string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return;
    }

    int type = rvalue["type"].asInt();
    int roiId = rvalue["roiId"].asInt();
    string status = rvalue["status"].asString();
    if (type == ALARM_STARTED_IVA)
    {
        return;
    }
    {
        AutoLock lock(m_mapRect._mtx);
        tmap<int, list<ROIRect>>::iterator it = m_mapRect.begin();
        for (; it != m_mapRect.end();++it)
        {
            if (it->first == roiId)
            {
                list<ROIRect>::iterator lsit = it->second.begin();
                for (; lsit != it->second.end();++lsit)
                {
                    lsit->status = status;
                }
                break;
            }
        }
    }

    {
        AutoLock lock(m_mapPol._mtx);
        tmap<int, list<ROIPolygon>>::iterator it = m_mapPol.begin();
        for (; it != m_mapPol.end(); ++it)
        {
            if (it->first == roiId)
            {
                list<ROIPolygon>::iterator lsit = it->second.begin();
                for (; lsit != it->second.end(); ++lsit)
                {
                    lsit->status = status;
                }
                break;
            }
        }
    }
    

    {
        AutoLock lock(m_mapLine._mtx);
        tmap<int, list<ROILine>>::iterator it = m_mapLine.begin();
        for (; it != m_mapLine.end(); ++it)
        {
            if (it->first == roiId)
            {
                list<ROILine>::iterator lsit = it->second.begin();
                for (; lsit != it->second.end(); ++lsit)
                {
                    lsit->status = status;
                }
                break;
            }
        }
    }
}

void                
Preview::drawROI(std::string json)
{
    //json = "{\"RoiList\":[{\"RoiId\":1,\"roiPts\":[{\"ptX\":0.36888888478279114,\"ptY\":0.63036304712295532},{\"ptX\":0.36888888478279114,\"ptY\":0.52145212888717651},{\"ptX\":0.58666664361953735,\"ptY\":0.16171617805957794},{\"ptX\":0.65866667032241821,\"ptY\":0.18976897001266479},{\"ptX\":0.65600001811981201,\"ptY\":0.28547856211662292},{\"ptX\":0.43466666340827942,\"ptY\":0.63861387968063354}],\"type\":30},{\"RoiId\":0,\"roiPts\":[{\"ptX\":0.039999999105930328,\"ptY\":0.5231022834777832},{\"ptX\":0.045333333313465118,\"ptY\":0.41584157943725586},{\"ptX\":0.16177777945995331,\"ptY\":0.028052804991602898},{\"ptX\":0.22933332622051239,\"ptY\":0.079207919538021088},{\"ptX\":0.22400000691413879,\"ptY\":0.17326731979846954},{\"ptX\":0.10133333504199982,\"ptY\":0.54620462656021118}],\"type\":30}],\"SerialOut\":[{\"CombineRoi\":[{\"AlarmRoi\":0,\"RoiId\":[0,1]}],\"Roi0\":[{\"RoiId\":1}],\"Roi1\":[{\"RoiId\":0}]}],\"alarmdesc\":\"\",\"channelNo\":32,\"gucode\":\"362110000003010133\",\"name\":\"......33\",\"preset\":3,\"roiId\":0,\"status\":0,\"type\":106}";
    SHL_Json::Reader reader;
    SHL_Json::Value rvalue;

    LOGI_SDK("drawROI json:%s",json.c_str());
    if (!reader.parse(json, rvalue, false))
    {
        return;
    }
    {
        AutoLock lock(m_mapRect._mtx);
        m_mapRect.clear();
    }
    {
        AutoLock lock(m_mapLine._mtx);
        m_mapLine.clear();
    }
    {
        AutoLock lock(m_mapPol._mtx);
        m_mapPol.clear();
    }

    map<int, vector<int>> mapvRoi;
    for (int i = 0; i < rvalue["SerialOut"][0]["CombineRoi"].size(); ++i)
    {
        vector<int> vRoi;
        int AlarmRoi = rvalue["SerialOut"][0]["CombineRoi"][i]["AlarmRoi"].asInt();
        for (int j = 0; j < rvalue["SerialOut"][0]["CombineRoi"][i]["RoiId"].size(); ++j)
        {
            vRoi.push_back(rvalue["SerialOut"][0]["CombineRoi"][i]["RoiId"][j].asInt());
        }
        mapvRoi[AlarmRoi] = vRoi;
    }

    SHL_Json::Value RoiList = rvalue["RoiList"];
    int type = -1;
    if (RoiList.isArray())
    {
        for (int i = 0; i < RoiList.size(); ++i)
        {
            type = RoiList[i]["type"].asInt();
            int roiId = RoiList[i]["RoiId"].asInt();

            if (30 == type )//wai jie ju xing (rect or Polygon)
            {
                int key = getCombRoiId(mapvRoi, roiId);
                if (-1 == key)
                {
                    continue;
                }
                if (RoiList[i]["roiPts"].size() == 2)
                {
                    ROIRect rect;
                    for (int j = 0; j < RoiList[i]["roiPts"].size(); ++j)
                    {
                        rect.point[j].x = RoiList[i]["roiPts"][j]["ptX"].asFloat();
                        rect.point[j].y = RoiList[i]["roiPts"][j]["ptY"].asFloat();
                    }
                    rect.rioId = roiId;
                    rect.status = "0";

                    AutoLock lock(m_mapRect._mtx);
                    m_mapRect[key].push_back(rect);
                    if (m_mapRect[key].size() == 1)
                    {
                        m_mapRect[key].push_back(rect);
                    }
                    else
                    {
                        ROIRect trect = (*m_mapRect[key].begin());
                        trect.point[0].x > rect.point[0].x ? trect.point[0].x = rect.point[0].x : 1;
                        trect.point[0].y > rect.point[0].y ? trect.point[0].y = rect.point[0].y : 1;
                        trect.point[1].x < rect.point[1].x ? trect.point[1].x = rect.point[1].x : 1;
                        trect.point[1].y < rect.point[1].y ? trect.point[1].y = rect.point[1].y : 1;
                        trect.rioId = key;
                        trect.status = "0";
                        *m_mapRect[key].begin() = trect;
                    }
                }
                else
                {
                    ROIPolygon pol;
                    for (int j = 0; j < RoiList[i]["roiPts"].size(); ++j)
                    {
                        ROIPoint point;
                        point.x = RoiList[i]["roiPts"][j]["ptX"].asFloat();
                        point.y = RoiList[i]["roiPts"][j]["ptY"].asFloat();
                        pol.lsp.push_back(point);
                    }
                    pol.rioId = roiId;
                    pol.status = "0";                    
                    {
                        AutoLock lock(m_mapPol._mtx);
                        m_mapPol[roiId].push_back(pol);
                    }

                    ROIRect rect;//Polygon zui xiao wai jie ju xing
                    list <ROIPoint>::iterator itPo= pol.lsp.begin();
                    for (; itPo != pol.lsp.end();++itPo)
                    {
                        if (itPo == pol.lsp.begin())
                        {
                            rect.point[0].x = itPo->x;
                            rect.point[0].y = itPo->y;
                            rect.point[1].x = itPo->x;
                            rect.point[1].y = itPo->y;
                        }
                        else
                        {
                            rect.point[0].x = rect.point[0].x > itPo->x ? itPo->x : rect.point[0].x;
                            rect.point[0].y = rect.point[0].y > itPo->y ? itPo->y : rect.point[0].y;
                            rect.point[1].x = rect.point[1].x > itPo->x ? rect.point[1].x : itPo->x;
                            rect.point[1].y = rect.point[1].y > itPo->y ? rect.point[1].y : itPo->y;
                        }
                    }
                    {
                        AutoLock lock(m_mapRect._mtx);
                        if (m_mapRect[key].size() == 0)
                        {
                            m_mapRect[key].push_back(rect);
                        }
                        else
                        {
                            ROIRect trect = (*m_mapRect[key].begin());
                            trect.point[0].x > rect.point[0].x ? trect.point[0].x = rect.point[0].x : 1;
                            trect.point[0].y > rect.point[0].y ? trect.point[0].y = rect.point[0].y : 1;
                            trect.point[1].x < rect.point[1].x ? trect.point[1].x = rect.point[1].x : 1;
                            trect.point[1].y < rect.point[1].y ? trect.point[1].y = rect.point[1].y : 1;
                            trect.rioId = key;
                            trect.status = "0";
                            *m_mapRect[key].begin() = trect;
                        }
                    }                    
                }


            }
            else if (ALARM_AI_INDICATOR == type)//rect or Polygon
            {
                if (RoiList[i]["roiPts"].size() == 2)
                {
                    ROIRect rect;
                    for (int j = 0; j < RoiList[i]["roiPts"].size(); ++j)
                    {
                        rect.point[j].x = RoiList[i]["roiPts"][j]["ptX"].asFloat();
                        rect.point[j].y = RoiList[i]["roiPts"][j]["ptY"].asFloat();
                    }
                    rect.rioId = roiId;
                    rect.status = "0";
                    AutoLock lock(m_mapRect._mtx);
                    m_mapRect[roiId].push_back(rect);
                }
                else
                {
                    ROIPolygon pol;
                    for (int j = 0; j < RoiList[i]["roiPts"].size(); ++j)
                    {
                        ROIPoint point;
                        point.x = RoiList[i]["roiPts"][j]["ptX"].asFloat();
                        point.y = RoiList[i]["roiPts"][j]["ptY"].asFloat();
                        pol.lsp.push_back(point);
                    }
                    pol.rioId = roiId;
                    pol.status = "0";
                    AutoLock lock(m_mapPol._mtx);
                    m_mapPol[roiId].push_back(pol);
                }
            }
            else if (ALARM_AI_TRIPWIRE == type)//line
            {
                ROILine line;
                for (int j = 0; j < RoiList[i]["roiPts"].size(); ++j)
                {
                    line.point[j].x = RoiList[i]["roiPts"][j]["ptX"].asFloat();
                    line.point[j].y = RoiList[i]["roiPts"][j]["ptY"].asFloat();
                }
                line.rioId = roiId;
                line.status = "0";
                AutoLock lock(m_mapLine._mtx);
                m_mapLine[roiId].push_back(line);
            }
        }
    }
    if (30 == type)
        combRectShowModel();    

    if (m_player)
    {
        //m_player->enableDraw(true);
    }
}

void               
Preview::drawLine(HDC hdc, HWND wnd, int w, int h)
{
    tmap<int, list<ROILine>> mapLine;
    {
        AutoLock lock(m_mapLine._mtx);
        mapLine = m_mapLine;
    }

    tmap<int, list<ROILine>>::iterator it = m_mapLine.begin();
    for (; it != m_mapLine.end(); ++it)
    {
        list<ROILine> lsLine = (*it).second;
        list<ROILine>::iterator lsit = lsLine.begin();
        for (; lsit != lsLine.end(); ++lsit)
        {
            float x11, y11, x22, y22;
            {
                x11 = lsit->point[0].x;
                y11 = lsit->point[0].y;
                x22 = lsit->point[1].x;
                y22 = lsit->point[1].y;
            }
            int width, height;
            {
                AutoLock lock(m_mtx);
                width = m_width;
                height = m_height;
            }

            ROIRgb rgb;
            memset(&rgb, 0, sizeof(ROIRgb));
            if ("0" == lsit->status)
                rgb.r = 255;
            else if ("1" == lsit->status)
                rgb.g = 255;
            else{
                rgb.r = 255;
                rgb.g = 255;
			}

#ifdef _WIN32
            float x1 = x11*width;
            float y1 = y11*height;
            float x2 = x22*width;
            float y2 = y22*height;
            if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
            {
                return;
            }
            COLORREF cPen = RGB(rgb.r, rgb.g, rgb.b);
            HPEN hpen = CreatePen(PS_SOLID &PS_ENDCAP_ROUND, 1, cPen);
            SelectObject(hdc, hpen);
            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
            DeleteObject(hpen);
#else
            float x1 = 2 * (x11 - 0.5);
            float y1 = 2 * (-1 * y11 + 0.5);
            float x2 = 2 * (x22 - 0.5);
            float y2 = 2 * (-1 * y22 + 0.5);
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;
            float line_width = 1.0f;
            r = rgb.r;
            g = rgb.g;
            b = rgb.b;

            glLineWidth(line_width);
            glBegin(GL_LINES);
            glColor3f(r, g, b);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glEnd();
#endif // _WIN32
        }
    }
}
void               
Preview::drawPol(HDC hdc, HWND wnd, int w, int h)
{
    tmap<int, list<ROIPolygon>> mapPol;
    {
        AutoLock lock(m_mapPol._mtx);
        mapPol = m_mapPol;
    }

    tmap<int, list<ROIPolygon>>::iterator it = m_mapPol.begin();
    for (; it != m_mapPol.end(); ++it)
    {
        list<ROIPolygon> lsPol = (*it).second;
        list<ROIPolygon>::iterator lsit = lsPol.begin();
        for (; lsit != lsPol.end(); ++lsit)
        {
            list<ROIPoint>::iterator lineit = lsit->lsp.begin();
            for (; lineit != lsit->lsp.end(); ++lineit)
            {
                list<ROIPoint>::iterator nextlineit = lineit;
                nextlineit++;
                if (nextlineit == lsit->lsp.end())
                {
                    nextlineit = lsit->lsp.begin();
                }
                
                float x11, y11, x22, y22;

                x11 = lineit->x;
                y11 = lineit->y;
                x22 = nextlineit->x;
                y22 = nextlineit->y;

                int width, height;
                {
                    AutoLock lock(m_mtx);
                    width = m_width;
                    height = m_height;
                }

                ROIRgb rgb;
                memset(&rgb, 0, sizeof(ROIRgb));
                if ("0" == lsit->status)
                    rgb.r = 255;
                else if ("1" == lsit->status)
                    rgb.g = 255;
	            else{
	                rgb.r = 255;
	                rgb.g = 255;
				}

#ifdef _WIN32
                float x1 = x11*width;
                float y1 = y11*height;
                float x2 = x22*width;
                float y2 = y22*height;
                if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
                {
                    return;
                }
                COLORREF cPen = RGB(rgb.r, rgb.g, rgb.b);
                HPEN hpen = CreatePen(PS_SOLID &PS_ENDCAP_ROUND, 1, cPen);
                SelectObject(hdc, hpen);
                MoveToEx(hdc, x1, y1, NULL);
                LineTo(hdc, x2, y2);
                DeleteObject(hpen);
#else
                float x1 = 2 * (x11 - 0.5);
                float y1 = 2 * (-1 * y11 + 0.5);
                float x2 = 2 * (x22 - 0.5);
                float y2 = 2 * (-1 * y22 + 0.5);
                float r = 0.0f;
                float g = 0.0f;
                float b = 0.0f;
                float line_width = 1.0f;
                r = rgb.r;
                g = rgb.g;
                b = rgb.b;

                glLineWidth(line_width);
                glBegin(GL_LINES);
                glColor3f(r, g, b);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
                glEnd();
#endif // _WIN32
            }
        }
    }
}
void               
Preview::drawRect(HDC hdc, HWND wnd, int w, int h)
{
    tmap<int, list<ROIRect>> mapRect;
    {
        AutoLock lock(m_mapRect._mtx);
        mapRect = m_mapRect;
    }
    
    tmap<int, list<ROIRect>>::iterator it = mapRect.begin();
    for (; it != mapRect.end();++it)
    {
        list<ROIRect> lsRect = (*it).second;
        list<ROIRect>::iterator lsit = lsRect.begin();
        for (; lsit != lsRect.end(); ++lsit)
        {
            float x11, y11, x22, y22;
            {
                x11 = lsit->point[0].x;
                y11 = lsit->point[0].y;
                x22 = lsit->point[1].x;
                y22 = lsit->point[1].y;
            }
    	    int width, height;
            {
                AutoLock lock(m_mtx);        
			    width = m_width;
        	    height = m_height;
            }

            ROIRgb rgb;
            memset(&rgb, 0, sizeof(ROIRgb));
            if ("0" == lsit->status)
                rgb.g = 255;
            else if ("1" == lsit->status)
                rgb.r = 255;
            else                
            {
                rgb.r = 255;
                rgb.g = 255;
            }
#ifdef _WIN32
            float x1 = x11*width;
            float y1 = y11*height;
            float x2 = x22*width;
            float y2 = y22*height;
            if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
            {
                return;
            }
            RECT rctA = { x1, y1, x2, y2 };
            //COLORREF cPen = RGB(0, 0, 255);
            COLORREF cBrush = RGB(rgb.r, rgb.g, rgb.b); 
            //HPEN hpen = CreatePen(PS_SOLID &PS_ENDCAP_ROUND, 1, cPen);
            HBRUSH hbrush = CreateSolidBrush(cBrush);
            SelectObject(hdc, hbrush);
            //SelectObject(hdc, hpen);
            FrameRect(hdc, &rctA, hbrush);
            //DeleteObject(hpen);
            DeleteObject(hbrush);
#else
	        float x1 = 2*(x11 - 0.5);
	        float y1 = 2*(-1 * y11 + 0.5);
	        float x2 = 2*(x22 - 0.5);
	        float y2 = 2*(-1 * y22 + 0.5);
	        float r = 0.0f;
            float g = 0.0f;
	        float b = 0.0f;
	        float line_width = 1.0f;

            r = rgb.r;
            g = rgb.g;
            b = rgb.b;


	        glLineWidth(line_width);
	        glBegin(GL_LINES);
	        glColor3f(r, g, b);
	        glVertex2f(x1, y1);
	        glVertex2f(x1, y2);
	        glEnd();
	        glBegin(GL_LINES);
	        glColor3f(r, g, b);
	        glVertex2f(x1, y1);
	        glVertex2f(x2, y1);
	        glEnd();
	        glBegin(GL_LINES);
	        glColor3f(r, g, b);
	        glVertex2f(x2, y1);
	        glVertex2f(x2, y2);
	        glEnd();
	        glBegin(GL_LINES);
	        glColor3f(r, g, b);
	        glVertex2f(x1, y2);
	        glVertex2f(x2, y2);
	        glEnd();
#endif // _WIN32
        }
    }
}

#endif


