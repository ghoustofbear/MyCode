#ifdef _WIN32
#include "wsys.h"
#else
#include "usys.h"
#endif
#include "playback.h"
#include "preview.h"
#include "download.h"
#include "gb28181sdk.h"
#include "gb28181_sipua.h"
#include <string>
#include <iostream>
#include "soundtalk.h"
#include "soundbroad.h"
#include "log4x.h"
using namespace std;




GB28181_API void GB28181_Init()
{
    std::cout << "gb28181sdk Inited\n";

#ifdef _WIN32
    sys = new WSys();
#else
    sys = new USys();
#endif

    //log4x::ilog4x::instance()->config(".");
    //log4x::ilog4x::instance()->start();

#if _DEBUG
    DEBUG_LEVEL_SIP(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_COM(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_SDK(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_PLY(LOG_LEVEL_DEBUG);
#else
    DEBUG_LEVEL_SIP(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_COM(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_SDK(LOG_LEVEL_DEBUG);
    DEBUG_LEVEL_PLY(LOG_LEVEL_DEBUG);
#endif
}

GB28181_API void GB28181_UnInit()
{
    if (sys)
    {
        delete sys;
        sys = NULL;
    }
    std::cout << "gb28181sdk UnInited\n";
}

GB28181_API HANDLE GB28181_CreateLocalSipServer(const char *addrcode, const char *host, int port)
{
    GB28181_SipUA * sipUa = new GB28181_SipUA(host, port, (char*)addrcode);
    if (sipUa)
    {
        if (sipUa->start() < 0)
        {
            delete sipUa;
            return NULL;
        }

        return (HANDLE)sipUa;
    }

    return NULL;
}


GB28181_API void GB28181_DestroyLocalSipServer(HANDLE lServer)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {
        sipUa->stop();
        delete sipUa;
        sipUa = NULL;
    }
}


GB28181_API HANDLE GB28181_CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport)
{
    //GB28181_SipUA * sipUa = new GB28181_SipUA(addrcode, rhhost, rhport, lshost, lsport);
    //if (sipUa)
    //{
    //    if (sipUa->start() < 0)
    //    {
    //        delete sipUa;
    //        return NULL;
    //    }

    //    return (HANDLE)sipUa;
    //}
    return  NULL;
}

GB28181_API bool  GB28181_QueryResource(HANDLE lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {
        return sipUa->query_resource(toaddroce, tohost, toport, code, fnCb, pUser) == 0 ? true : false;
    }
    return false;
}

GB28181_API bool  GB28181_ControlCamera(HANDLE lServer, const char *addrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {
        return sipUa->ctrl_camera(addrcode, tohost, toport, cameraaddr, cmd, param1, param2, param3) == 0?true:false;
    }

    return false;
}

GB28181_API bool  GB28181_QueryAlarm(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, uint64_t begin, uint64_t end, int type,
    pfnAlarmCallback fnCb, void *pUser)
{
    return false;
}

GB28181_API bool  GB28181_QueryRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, int begin, int end, int type,
    pfnRecordCallback fnCb, void *pUser)
{
	GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
	sipUa->query_record(cameraaddr, tohost, toport, usercode, begin, end, type, fnCb, pUser);
    return true;
}

GB28181_API HANDLE  GB28181_StartPreview(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    preview * pthis = new preview(sipUa);
    if (!pthis)
        return NULL;

    bool ret = pthis->start(cameraaddr, tohost, toport, hWnd);
    if (!ret)
    {
        delete pthis;
        return NULL;
    }

    return (HANDLE)pthis;
}

GB28181_API void  GB28181_StopPreview(HANDLE lPreview)
{
    preview * pthis = (preview*)lPreview;
    if (pthis)
    {
        delete pthis;
        pthis = NULL;
    }
}

GB28181_API HANDLE  GB28181_StartPlayback(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, uint32_t start, uint32_t end, HWND hWnd)
{
	GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
	Playback * pthis = new Playback(sipUa);
	if (!pthis)
		return NULL;

	bool ret = pthis->start(cameraaddr, tohost, toport,  start, end, hWnd);
	if (!ret)
	{
		delete pthis;
		return NULL;
	}

	return (HANDLE)pthis;
}

GB28181_API void  GB28181_StopPlayback(HANDLE lPlayback)
{
    Playback * pthis = (Playback*)lPlayback;
    if (pthis)
    {
        delete pthis;
        pthis = NULL;
    }
}

GB28181_API void  GB28181_PlayCtrl(HANDLE lPlayback, int cmd, int param)
{
    Playback * ply = (Playback *)lPlayback;
    if (ply)
    {
        return ply->playCtrl(cmd, param);
    }

    return;
}

GB28181_API void  GB28181_ChangeWinSize(HANDLE lPre_Pb_view)
{
    BasePlay *pthis = (BasePlay*)lPre_Pb_view;
    if (pthis)
    {
        pthis->changeWinSize();
    }
}

GB28181_API int  GB28181_DragZoom(HANDLE lPreview, int startX, int startY, int endX, int endY, int winW, int winH)
{
    preview * ply = (preview *)lPreview;
    if (ply)
    {
        if (startX - endX > 2 || startX < endX)
        {
            return ply->DragZoom(startX, startY, endX, endY, winW, winH);
        }
    }

    return -1;
}

GB28181_API void  GB28181_DrawRect(HANDLE lPreview, int x1, int y1, int x2, int y2, bool enable)
{
    preview * ply = (preview *)lPreview;
    if (ply)
    {
        ply->DrawRect(x1, y1, x2, y2, enable);
    }

}
GB28181_API HANDLE  GB28181_DownloadRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char* savefile, const char *recordfile, uint32_t start, uint32_t end, uint64_t fileSize)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    Download * pthis = new Download(sipUa);

    if (!pthis)
        return NULL;

    bool ret = pthis->start(cameraaddr, tohost, toport, savefile, recordfile, start, end, fileSize);
    if (!ret)
    {
        delete pthis;
        return NULL;
    }

    return pthis;
}

GB28181_API long  GB28181_Get_Downloadpos(HANDLE dl)
{
    Download * pthis = (Download*)dl;
    if (pthis)
    {
        return pthis->downloadPos();
    }
    return -1;
}


GB28181_API void  GB28181_DownloadStop(HANDLE lDownload)
{
    Download * pthis = (Download*)lDownload;
    if (pthis)
    {
        delete pthis;
        pthis = NULL;
    }
}

GB28181_API bool  GB28181_Subscribe(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, int subType, uint64_t u64Begin, uint64_t u64End, int expire)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {       
        datetime_s begin, end;
        sys->strptime(u64Begin, begin);
        sys->strptime(u64End, end);
        return sipUa->subscribe_envent(toaddrcode, tohost, toport, subType, begin, end, expire) == 0 ? true : false;
    }
    return false;
}

GB28181_API void  GB28181_SetSubsribeEventCallback(HANDLE lServer, void(*pfnsubscribecb)(const char *data, int size, void *puser), void *puser)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {
        sipUa->setsubsribecb(pfnsubscribecb, puser);
    }
}

GB28181_API void  GB28181_SetMediaStatusCallback(HANDLE lServer, void(*pfn)(const char *data, int size, void *puser), void *puser)
{
    GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
    if (sipUa)
    {
        sipUa->setStatusCB(pfn, puser);
    }
}

GB28181_API HANDLE  GB28181_StartTalk(HANDLE lServer, HANDLE lPreview, const char *tohost, int toport)
{
	GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
	if (sipUa)
	{
		preview *stream = (preview*)lPreview;
		if (NULL == stream)
		{
			return 0;
		}
		SoundTalk *talk = new SoundTalk(sipUa, stream);
		if (NULL == talk)
		{
			return 0;
		}
		if (talk->start(tohost, toport) < 0)
		{
			delete talk;
			return 0;
		}

		return (HANDLE)talk;
	}

	return NULL;
}

GB28181_API void  GB28181_StopTalk(HANDLE lTalk)
{
	SoundTalk *talk = (SoundTalk*)lTalk;
	if (talk)
	{
		talk->stop();
		delete talk;
	}
}

GB28181_API HANDLE  GB28181_StartSoundBroadcast(HANDLE lServer, const char *json, int size)
{
	GB28181_SipUA * sipUa = (GB28181_SipUA *)lServer;
	if (sipUa)
	{
		SoundBroadcast *soundBroad = new SoundBroadcast(sipUa);
		if (NULL == soundBroad)
		{
			return 0;
		}

		if (soundBroad->start(json, size) < 0)
		{
			delete soundBroad;
			return 0;
		}
		return (HANDLE)soundBroad;
	}
    return 0;
}

GB28181_API void  GB28181_StopSoundBroadcast(HANDLE lBroadcast)
{
	SoundBroadcast *soundBroad = (SoundBroadcast*)lBroadcast;
	if (soundBroad)
	{
		soundBroad->stop();
		delete soundBroad;
	}
}

GB28181_API bool  GB28181_QueryDevAbility(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser)
{
	return false;
}

GB28181_API bool  GB28181_DoAlgorithmTask(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size)
{
	return false;
}

GB28181_API bool  GB28181_DoPictureSnap(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size)
{
	return false;
}

GB28181_API bool  GB28181_DoQueryDevRule(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser)
{
	return false;
}

GB28181_API bool  GB28181_DoQueryAllTask(HANDLE lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser)
{
	return false;
}

GB28181_API int  GB28181_QueryFlow(HANDLE lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum)
{

	return 0;
}
