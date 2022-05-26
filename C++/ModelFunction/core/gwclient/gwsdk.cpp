#include "gw_sipua.h"
#include "gwsdk.h"
#include "preview.h"
#include "playback.h"
#include "soundtalk.h"
#include "soundbroad.h"
#include "gw_sipuaex.h"

#ifdef _WIN32
#include "wsys.h"
#else
#include "usys.h"
#endif

Sys     * sys = NULL;

GWSDK_API void GWSDKInit()
{
#ifdef _WIN32
    sys = new WSys();
#else
    sys = new USys();
#endif
}


GWSDK_API long CreateLocalSipServer(const char *addrcode, const char *host, int port)
{
    SipUA * sipUa = new GWSipUA(host, port, (char*)addrcode);
    if (sipUa)
    {
        if (sipUa->start() < 0)
        {
            delete sipUa;
            return NULL;
        }

        return (long)sipUa;
    }

    return NULL;
}


GWSDK_API void DestroyLocalSipServer(long lServer)
{
    SipUA * sipUa = (SipUA *)lServer;
    if (sipUa)
    {
        sipUa->stop();
        delete sipUa;
    }
}


GWSDK_API long CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport)
{
	SipUA * sipUa = new GWSipUAEx(addrcode, rhhost, rhport, lshost, lsport);
	if (sipUa)
	{
		if (sipUa->start() < 0)
		{
			delete sipUa;
			return NULL;
		}

		return (long)sipUa;
	}
	return  NULL;
}

GWSDK_API bool  QueryResource(long lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
	GWSipUAEx *sipUaEx = dynamic_cast<GWSipUAEx*>(sipUa);

	if (sipUaEx)
    {
		return sipUaEx->query_resource(toaddroce, tohost, toport, code, fnCb, pUser);
    }
	else
	{
		return sipUa->query_resource(toaddroce, tohost, toport, code, fnCb, pUser);
	}

    return false;
}

GWSDK_API bool  ControlCamera(long lServer, const char *addrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
    if (sipUa)
    {
        return sipUa->ctrl_camera(addrcode, tohost, toport, cameraaddr, cmd, param1, param2, param3);
    }

    return false;
}

GWSDK_API bool  QueryAlarm(long lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, char *begin, char *end, int type,
    pfnAlarmCallback fnCb, void *pUser)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	GWSipUAEx *sipUaEx = dynamic_cast<GWSipUAEx*>(sipUa);

	if (sipUaEx)
    {
		return sipUaEx->query_alarm(toaddrcode, tohost, toport, useraddrcode, begin, end, type, 0, fnCb, pUser);
    }
	else
	{
		return sipUa->query_alarm(toaddrcode, tohost, toport, useraddrcode, begin, end, type, 0, fnCb, pUser);
	}

    return false;
}

GWSDK_API bool  QueryRecord(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, char *begin, char *end, int type,
    pfnRecordCallback fnCb, void *pUser)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	GWSipUAEx *sipUaEx = dynamic_cast<GWSipUAEx*>(sipUa);

	if (sipUaEx)
    {
		return sipUaEx->query_record(cameraaddr, tohost, toport, usercode, begin, end, type, fnCb, pUser);
    }
	else
	{
		return sipUa->query_record(cameraaddr, tohost, toport, usercode, begin, end, type, fnCb, pUser);
	}

    return false;
}

GWSDK_API long  StartPreview(long lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
    preview * pthis = new preview(sipUa);
    if (pthis)
    {
        bool ret = pthis->start(cameraaddr, tohost, toport, hWnd);
        if (!ret)
        {
            delete pthis;
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return (long)pthis;
}

GWSDK_API void  StopPreview(long lPreview)
{
    preview * pthis = (preview*)lPreview;
    if (pthis)
    {
        delete pthis;
        pthis = NULL;
    }
}

GWSDK_API long  StartPlayback(long lServer, const char *url, HWND hWnd)
{
    GWSipUA  * sipUa = (GWSipUA *)lServer;
    Playback * pthis = new Playback(sipUa);
    if (pthis)
    {
        bool ret = pthis->start(url, hWnd);
        if (!ret)
        {
            delete pthis;
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return (long)pthis;
}

GWSDK_API void  StopPlayback(long lPlayback)
{
    Playback * pthis = (Playback *)lPlayback;
    if (pthis)
    {
        pthis->stop();
        delete pthis;
    }
}

GWSDK_API void  PlayCtrl(long lPlayback, int cmd)
{
    Playback * pthis = (Playback *)lPlayback;
    if (pthis)
    {
        pthis->playCtrl(cmd);
    }
}

GWSDK_API bool  Subscribe(long lServer, const char *toaddrcode, const char *tohost, int toport, int mode, int alarmtype, int expires)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
    if (sipUa)
    {
        return sipUa->subscribe_event(toaddrcode, tohost, toport, toaddrcode, mode, alarmtype, expires);
    }
    return false;
}

GWSDK_API void  SetSubsribeEventCallback(long lServer, void(*pfnsubscribecb)(const char *data, int size, void *puser), void *puser)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
    if (sipUa)
    {
        sipUa->setsubsribecb(pfnsubscribecb, puser);
    }
}

GWSDK_API long  StartTalk(long lServer, long lPreview, const char *tohost, int toport)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
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
        
        return (long)talk;
    }

    return 0;
}

GWSDK_API void  StopTalk(long lTalk)
{
    SoundTalk *talk = (SoundTalk*)lTalk;
    if (talk)
    {
        talk->stop();
        delete talk;
    }
}

GWSDK_API long  StartSoundBroadcast(long lServer, const char *json, int size)
{
    GWSipUA * sipUa = (GWSipUA *)lServer;
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
        return (long)soundBroad;
    }
    return 0;
}

GWSDK_API void  StopSoundBroadcast(long lBroadcast)
{
    SoundBroadcast *soundBroad = (SoundBroadcast*)lBroadcast;
    if (soundBroad)
    {
        soundBroad->stop();
        delete soundBroad;
    }
}

GWSDK_API bool  QueryDevAbility(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		return sipUa->query_dev_ability(cameraaddr, tohost, toport, cameraaddr, fnDevAbiltyCb, pUser);
	}
	return false;
}

GWSDK_API bool  DoAlgorithmTask(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		return sipUa->do_algorithm_task(cameraaddr, tohost, toport, cameraaddr, json, size);
	}
	return false;
}

GWSDK_API bool  DoPictureSnap(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		return sipUa->do_picture_snap(cameraaddr, tohost, toport, cameraaddr, json, size);
	}
	return false;
}

GWSDK_API bool  DoQueryDevRule(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		sipUa->query_dev_rule(cameraaddr, tohost, toport, cameraaddr, fnDevRuleCb, pUser);
	}
	return false;
}

GWSDK_API bool  DoQueryAllTask(long lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		return sipUa->query_all_task(toaaddrcode, tohost, toport, fnAllTaskCb, pUser);
	}
	return false;
}

GWSDK_API int  QueryFlow(long lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum)
{
	GWSipUA * sipUa = (GWSipUA *)lServer;
	if (sipUa)
	{
		return sipUa->query_flow(toaddr, toshost, sport, useraddr, bandWidth, streamNum);
	}

	return 0;
}