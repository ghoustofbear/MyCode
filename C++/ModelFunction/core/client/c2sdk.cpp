#include "version.h"
#include "c2sdk.h"
#include "icommon.h"
#include "sys.h"
#include "utility.h"
#include "http_cu.h"
#include "tls_error.h"
#include "c2types_json.h"
#include "preview.h"

#include "localavstream.h"
#include "dualtalkstream.h"
#include "atalksession.h"

#include "streamgr.h"
#include "player.h"
#include "recordstream.h"
#include "playback.h"
#include "mempool.h"

//#include "gwb_cu.h"
//#include "gwa_cu.h"

StreamMgr *streamMgr = NULL;
static bool init     = false;

NET_API long
c2_init(const char* jvm2nativePath)
{
    if (init)
    {
        return 0;
    }

    mempool = new Mempool(MAX_MEMPOOL_SIZE);
    if (icommon::instance()->start() < 0)
    {
        ERRF_SDK("common start fail.");
        abort();
    }

#if _DEBUG
    /* DEBUG_LEVEL_COM(LOG_LEVEL_TRACE); */
    /* DEBUG_LEVEL_PLY(LOG_LEVEL_TRACE); */
    DEBUG_LEVEL_SDK(LOG_LEVEL_DEBUG);
#else
    /* DEBUG_LEVEL_COM(LOG_LEVEL_INFO); */
    /* DEBUG_LEVEL_SDK(LOG_LEVEL_INFO); */
    DEBUG_LEVEL_PLY(LOG_LEVEL_INFO);
#endif

    streamMgr = new StreamMgr();
    if (streamMgr)
    {
        if (streamMgr->start() < 0)
        {
            delete streamMgr;
            /* delete sys; */
            return -1;
        }
    }

    if (Player::initLib(jvm2nativePath) < 0)
    {
        delete streamMgr;
        /* delete sys; */
        ERRF_SDK("initLib fail.");
        abort();

        return -1;
    }

    init = true;

    return 0;
}

NET_API void
c2_uninit()
{
    if (!init)
    {
        return;
    }

    if (streamMgr)
    {
        streamMgr->stop();
        delete streamMgr;
        streamMgr = NULL;
    }

    icommon::instance()->stop();
    init = false;
}

NET_API char *
c2_version()
{
    static char ver[100] = {0};
    snprintf_s(ver, sizeof(ver), "%s-%s", VERSION_TAG, VERSION_SHA1);
    return ver;
}

NET_API 
char * c2_buildPTS()
{
    return (char *)VERSION_BUILD_PTS;
}

NET_API long
c2_set_error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    long ret =  vsetError(fmt, arg);
    va_end(arg);

    return ret;
}

NET_API const char *
c2_get_error()
{
    return getError();
}

NET_API HANDLE
c2_login(const c2_login_s *param, c2_login_cb loginCB, void *userData, int protocol)
{
    ASSERT(param);
    ASSERT(param->servIp);

    C2Login login(*param);
    if (!login.validate())
    {
        return NULL;
    }

    Cu* cu = NULL;
    if (0 == protocol)
    {
        /* Host host(param->servIp, param->servPort); */
        string host;
        host += param->servIp;
        host += ":";
        host += std::to_string(param->servPort);
        cu = new HttpCu(host);
    }
    else
    {
        ERRE_SDK("protocol %d is not implement. ", protocol);
        return NULL;
    }

    if (!cu)
    {
        return NULL;
    }

    cu->init();
    cu->start(login.toStr(), loginCB, userData);

    return (HANDLE)cu;
}

NET_API void
c2_logout(HANDLE login)
{
    if (login)
    {
        /**
         * Note the calling order, first delete the observer(Preview), and then delete
         * the observable(RealStream)
         * must know that this function no change the parameters of login ;
         */
        Cu* cu = (Cu *)(login);
        cu->stop();

        if (streamMgr)
        {
            streamMgr->release((Cu *)login);
        }

        cu->uninit();
        delete cu;
    }
}

NET_API bool
c2_islogin(HANDLE login)
{
    if (!login)
    {
        ERRE_SDK("invalidate login handle.");
        return false;
    }

    Cu* cu = (Cu *)(login);

    return cu->isLogin();
}

NET_API long 
c2_add_standby(HANDLE login, const char *ip, int port)
{
    if (!login || !ip)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    return cu->addStandby(ip, port);
}

NET_API long 
c2_del_standby(HANDLE login, const char *ip, int port)
{
    if (!login || !ip)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    return cu->delStandby(ip, port);
}

NET_API long
c2_get_servinfo(HANDLE login, c2_servinfo_s *info)
{
    if (!login || !info)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    return cu->getServerInfo(info);
}

NET_API long
c2_query_user_v1(HANDLE login, c2_userinfo_cb userInfoCb, void *userdata)
{
    if (!login)
    {
        return -1;
    }

    Cu* cu = (Cu *)(login);
    return cu->queryUser(userInfoCb, userdata);
}

NET_API long
c2_query_device_v1(HANDLE login, const c2_query_devinfo_s *param, c2_devinfo_cb devinfoCB, void *userData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryDevInfo query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryDevice(query.toStr(), devinfoCB, userData);
}

NET_API long 
c2_query_device_v2(HANDLE login, const c2_query_devinfo_s *param, char *json, int *size)
{
    if (!login || !param || !json || !size)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    if (*size <= 0 || *size > 2 * 1024 * 1024)
    {
        ERRE_SDK("invalidate size parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryDevInfo query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryDevice(query.toStr(), json, size);
}

NET_API long
c2_query_channel_status(HANDLE login, const c2_query_channel_s *param, char *json, int *size)
{
    if (!login || !param || !json || !size)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    if (*size <= 0 || *size > 2 * 1024 * 1024)
    {
        ERRE_SDK("invalidate size parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryChaInfo query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryChannel(query.toStr(), json, size);
}

NET_API  long
c2_subscribe(HANDLE login, uint64_t typeBitmap, c2_alarm_cb alarmCB, void *userData)
{
    if (!login)
    {
        ERRE_SDK("invalidate login handle.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    return cu->subscribe(typeBitmap, alarmCB, userData);
}


NET_API HANDLE
c2_start_preview(HANDLE login, const c2_preview_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2Preview view(*param);
    if (!view.validate())
    {
        return NULL;
    }

    return cu->startPreview(view.toStr(), wnd, streamCB, streamData, statusCB, statusData);
}

NET_API int c2_add_preview_surface(HANDLE view, HWND wnd)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->addWnd(wnd);
}

NET_API int c2_del_preview_surface(HANDLE view, HWND wnd)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->delWnd(wnd);
}

NET_API int 
c2_preview_status(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->getStatus();

}

NET_API void
c2_stop_preview(HANDLE login, HANDLE view)
{
    if (!login || !view)
    {
        return;
    }

    Cu* cu = (Cu *)(login);
    cu->stopPreview(view);
}

NET_API int 
c2_query_chn_relation_dev(HANDLE view, char* json, int *size)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    string strjson = pview->getJson();
    Cu* cu = pview->getCu();

    SHL_Json::Reader reader;
    SHL_Json::Value  devRvalue;
    SHL_Json::Value  rvalue;
    SHL_Json::StreamWriterBuilder writer;

    reader.parse(strjson, devRvalue, false);

    int obtainType = devRvalue["obtainType"].asInt();
    if (obtainType == 0 || obtainType == 1)
    {
        rvalue["DevIp"]   = devRvalue["deviceHost"].asString();
        rvalue["DevPort"] = devRvalue["devicePort"].asInt();
        rvalue["chnNo"]   = devRvalue["channelNo"].asInt();
    }
    else
    {
        rvalue["AddressCode"] = devRvalue["addrcode"].asString();
    }
    
    rvalue["obtainType"] = obtainType;
    rvalue["version"] = JSON_DATA_VERSION;
    rvalue["Method"] = "getRelationDev";
    
    string jsonIn = SHL_Json::writeString(writer, rvalue);

    cu->getConfig(jsonIn, json, size);

    return 0;
}

#ifdef UNIX_DRAW_RECT
NET_API int 
c2_ctrl_drawroi(HANDLE view, char* json)
{
    if (!view || json == NULL)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->clearROI(json);
    //return pview->ctrlROI(json);
}
#endif

NET_API long 
c2_change_wndsize(HANDLE view, int width, int height)
{
    if (!view || !(width > 0 && width < 8000) || !(height > 0 && height < 6000))
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->changeWndSize(width, height);
}


NET_API long
c2_change_wndpos(HANDLE view, int x, int y)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->changeWndPos(x, y);
}

NET_API int 
c2_opensound(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->opensound();
}

NET_API int 
c2_colsesound(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->closesound();
}

NET_API void 
c2_set_volume(HANDLE view, int volume)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return;
    }

    Preview *pview = (Preview *)view;

    pview->setvolume(volume);
}

NET_API HANDLE 
c2_init_dualtalk(int flag)
{
    LocalAVStream* lav = new LocalAVStream();
    lav->start(flag);
    return lav;
}

NET_API int 
c2_uninit_dualtalk(HANDLE dutk)
{
    if (!dutk)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    LocalAVStream* lav = (LocalAVStream*)dutk;
    lav->stop();
    delete lav;

    return 0;
}

NET_API HANDLE
c2_start_dev_dualtalk(HANDLE dt, HANDLE  view, c2_status_cb statusCB, void* userdata)
{
    if (!dt || !view)
    {
        ERRE_SDK("invalidate handle.");
        return NULL;
    }

    LocalAVStream* lav = (LocalAVStream*)dt;
    return lav->addAudioTalk((Preview*)view, statusCB, userdata);
}

NET_API int 
c2_stop_dev_dualtalk(HANDLE dualtalk, HANDLE view)
{
    if (!dualtalk || !view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    LocalAVStream* lav = (LocalAVStream*)dualtalk;
    lav->delAudioTalk((Preview*)view);

    return 0;
}

NET_API int 
c2_send_audio(HANDLE dutk, void* data, int size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    if (!dutk || !data)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    LocalAVStream* stream = (LocalAVStream*)dutk;
    stream->notifyAudioStream(data, size, encode, sps, channels, bps, pts);
    return 0;
}

NET_API int 
c2_send_video(HANDLE dutk, void* data, int size, int w, int h, int type, int encode, int fps, uint64_t pts)
{
    if (!dutk || !data)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }
    LocalAVStream* stream = (LocalAVStream*)dutk;
    stream->notifyVideoStream(data, size, encode, type, w, h, fps, pts);
    return 0;
}

NET_API int 
c2_start_talk(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }
    Preview *pview = (Preview *)view;

    return pview->startTalk();
}

NET_API int 
c2_stop_talk(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }
    Preview *pview = (Preview *)view;

    pview->stopTalk();

    return 0;
}


NET_API long 
c2_capture(HANDLE view, const char *filepath, int type)
{
    if (!view || !filepath)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->capture(filepath, type);
}

NET_API long 
c2_set_drawcb(HANDLE view, c2_draw_cb drawCB, void *userData)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->setDrawCallback(drawCB, userData);
}

NET_API long 
c2_set_decodecb(HANDLE view, c2_decode_cb decodeCB, void *userData)
{
    if (!view)
    {
        ERRE_SDK("invalidate handle.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->setDecCallback(decodeCB, userData);
}

NET_API long 
c2_ptz_control(HANDLE login, int cmd, const c2_ptz_s *param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2Ptz ptz(cmd, *param);
    if (!ptz.validate())
    {
        return -1;
    }

    return cu->ptzCtrl(ptz.toStr());
}

NET_API long 
c2_get_chn_preset_v1(HANDLE login, const c2_query_preset_s *param, c2_preset_cb presetCB, void *userdata)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryPreset query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryPreset(query.toStr(), presetCB, userdata);
}


NET_API long 
c2_get_chn_preset_v2(HANDLE login, const c2_query_preset_s *param, char* json, int *size)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryPreset query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryPreset(query.toStr(), json, size);
}

NET_API long 
c2_set_chn_preset(HANDLE login, const c2_set_preset_s *param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2SetPreset query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->setPreset(query.toStr());
}

NET_API long
c2_query_record_v1(HANDLE login, const c2_query_record_s *param, c2_record_cb recordCB, void *userData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryRecord query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryRecord(query.toStr(), recordCB, userData);
}

NET_API long
c2_query_record_v2(HANDLE login, const c2_query_record_s *param, char *json, int *size)
{
    if (!login || !param || !json || !size)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    if (*size <= 0 || *size > 2 * 1024 * 1024)
    {
        ERRE_SDK("invalidate size parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2QueryRecord query(*param);
    if (!query.validate())
    {
        return -1;
    }

    return cu->queryRecord(query.toStr(), json, size);
}

NET_API HANDLE 
c2_playback_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                 c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2PByName playback(*param);
    if (!playback.validate())
    {
        return NULL;
    }

    return cu->playbackByName(playback.toStr(), param->filename, wnd, streamCB, streamData, statusCB, statusData);
}

NET_API HANDLE
c2_playbackreverse_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2PByName playback(*param);
    if (!playback.validate())
    {
        return NULL;
    }
    playback.setReverse();
    return cu->playbackByName(playback.toStr(), param->filename, wnd, streamCB, streamData, statusCB, statusData);
}

NET_API HANDLE 
c2_playback_bytime(HANDLE login, const c2_pb_time_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                 c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2PByTime playback(*param);
    if (!playback.validate())
    {
        return NULL;
    }

    datetime_s start = param->start;
    datetime_s end   = param->end;

    return cu->playbackByTime(playback.toStr(), start, end, wnd, streamCB, streamData, statusCB, statusData);
}

NET_API int 
c2_add_playback_surface(HANDLE playback, HWND wnd)
{
    if (!playback || !wnd)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->addWnd(wnd);
}

NET_API int
c2_del_playback_surface(HANDLE playback, HWND wnd)
{
    if (!playback || !wnd)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->delWnd(wnd);
}

NET_API long
c2_set_pb_decodecb(HANDLE pb, c2_decode_cb decodeCB, void *userdata)
{
    RecordStream * stream = (RecordStream *)pb;
    if (stream)
    {
        stream->setDecCallback(decodeCB, userdata);
    }

    return 0;
}

NET_API void 
c2_stop_playback(HANDLE login, HANDLE playback)
{
    if (!login || !playback)
    {
        return;
    }

    Cu* cu = (Cu *)(login);
    cu->stopPlayback(playback);
}

NET_API long 
c2_get_playbacktime(HANDLE playback, datetime_s *time)
{
    if (!playback || !time)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->getPlayTime(*time);
}

NET_API long 
c2_get_playbackpos(HANDLE playback, datetime_s *start, datetime_s *end)
{
    if (!playback)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

#if 0
    Playback *pb = (Playback *)playback;
    return pb->pos(start, end);
#else
    RecordStream* rs = (RecordStream*)playback;
    return rs->getPosition();
#endif

    
}

NET_API long 
c2_playback_change_wndsize(HANDLE playback, int width, int height)
{
    if (!playback)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->changeWndSize(width, height);
}

NET_API long
c2_playback_change_wndpos(HANDLE playback, int x, int y)
{
    if (!playback)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->changeWndPos(x, y);
}

NET_API int 
c2_pb_opensound(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    Playback *pb = (Playback *)view;

    return pb->opensound();
}


NET_API int 
c2_pb_colsesound(HANDLE view)
{
    if (!view)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    Playback *pb = (Playback *)view;

    return pb->closesound();
}

NET_API int 
c2_pb_setvolume(HANDLE view, int vol)
{
    if (!view)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    Playback *pb = (Playback *)view;

    return pb->setVolume(vol);
}

NET_API long 
c2_playback_control(HANDLE playback, int code, void *val, int valSize)
{
    if (!playback)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

    if ((PLAY_SETPOS == code || PLAY_SETTIME == code) && !val)
    {
        ERRE_SDK("invalidate parameter.");
        return -1;
    }

    Playback *pb = (Playback *)playback;

    return pb->control(code, val, valSize);
}

NET_API long 
c2_playback_controlV1(HANDLE playback, int code, long value)
//c2_playback_controlV1(HANDLE playback, int code, int value)
{
    if (!playback)
    {
        ERRE_SDK("invalidate playback handle.");
        return -1;
    }

	void* tmp = NULL;
	int   tmpsize = 0;
	uint64_t tmps = uint64_t(value);

	datetime_s dt = {0};

    if (PLAY_SETTIME == code)
    {
		sys->strptime(tmps * 1000, dt);// value
		tmp = &dt;
		tmpsize = sizeof(dt);

		LOGE_SDK("value:%d dt:%d-%d-%d:%d:%d:%d", value, dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    }
	else if (PLAY_SETPOS == code)
	{
		tmp = &value;
		tmpsize = 4;
	}

    //if (value < 1 && value > 99)
    //{
    //    ERRE_SDK("process err!");
    //    return -1;
    //}

    Playback *pb = (Playback *)playback;

	return pb->control(code, tmp, tmpsize);
}

NET_API long 
c2_playback_capture(HANDLE view, const char *filepath, int type)
{
    if (!view || !filepath)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Playback *pb = (Playback *)view;

    return pb->capture(filepath, type);
}

NET_API HANDLE 
c2_start_download(HANDLE login, const c2_download_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2Download dl(*param);
    if (!dl.validate())
    {
        return NULL;
    }

    bool standard = false;
    if (1 == param->obtainType || 3 == param->obtainType)
    {
        standard = true;
    }

    return cu->startDownload(dl.toStr(), param->savepath, standard, streamCB, streamData, statusCB, statusData);
}

NET_API HANDLE 
c2_start_downloadByTime(HANDLE login, const c2_download_time_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2DownloadByTime dl(*param);
    if (!dl.validate())
    {
        return NULL;
    }

    bool standard = false;
    if (1 == param->obtainType || 3 == param->obtainType)
    {
        standard = true;
    }

    return cu->startDownload(dl.toStr(), param->savepath, standard, streamCB, streamData, statusCB, statusData);    
}

NET_API void 
c2_stop_download(HANDLE login, HANDLE download)
{
    if (!login || !download)
    {
        return;
    }

    Cu* cu = (Cu *)(login);
    cu->stopDownload(download);
}

NET_API long c2_get_downloadpos(HANDLE download)
{
    if (!download)
    {
        ERRE_SDK("invalidate download handle.");
        return -1;
    }

    RecordStream *rstream = (RecordStream *)download;

    return rstream->getPosition();
}

NET_API long 
c2_start_realREC(HANDLE view, const char *filepath)
{
    if (!view || !filepath)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->startREC(filepath);
}

NET_API void 
c2_stop_realREC(HANDLE view)
{
    if (!view)
    {
        LOGE_SDK("invalidate handle or parameter.");
    }

    Preview *pview = (Preview *)view;

    return pview->stopREC();
}

NET_API HANDLE 
c2_start_backREC(HANDLE login, const c2_preview_s *param, const char *filepath)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return NULL;
    }

    Cu* cu = (Cu *)(login);

    C2Preview view(*param);
    if (!view.validate())
    {
        return NULL;
    }

    return cu->startBackREC(view.toStr(), filepath);
}

NET_API void 
c2_stop_backREC(HANDLE login, HANDLE view)
{
    if (!view)
    {
        LOGE_SDK("invalidate handle or parameter.");
    }

    Cu* cu = (Cu *)(login);
    return cu->stopBackREC(view);
}

NET_API long 
c2_sync_time(HANDLE login, const c2_synct_s *sync)
{
    if (!login || !sync)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2Synct synct(*sync);
    if (!synct.validate())
    {
        return -1;
    }

    return cu->syncTime(synct.toStr());
}

NET_API int 
c2_matrix_switch(HANDLE login, const c2_matrix_s * param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2Matrix matrixParam(*param);

    if (!matrixParam.validate())
    {
        return -1;
    }

    return cu->sendCommand(Cu::R_MATRIX_SWITCH, matrixParam.toStr());
}

NET_API int
c2_decoder_switch(HANDLE login, const c2_decoder_param_s * param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

    C2DecoderParam decoderParam(*param);
    
    if (!decoderParam.validate())
    {
        return -1;
    }

    return cu->sendCommand(Cu::R_MATRIX_SWITCH,  decoderParam.toStr());
}
NET_API int  
c2_send_command(HANDLE login, char * json, int size)
{
    if (!login || !json)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);
    

    return cu->sendCommand(Cu::R_CUSTOM_COMMAND, string(json));
}

NET_API int  
c2_get_config(HANDLE login, char * _in_json, int _in_size, char* _out_json, int *_out_size)
{
    if (!login || !_in_json || !_out_json)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);

   int ret = cu->getConfig(_in_json, _out_json, _out_size);

   return ret;
}

NET_API int  
c2_set_face(HANDLE login, const c2_set_face_s * param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);
    C2SetFace face(*param);
    if (!face.validate())
    {
        return -1;
    }

    return cu->sendCommand(Cu::R_CUSTOM_COMMAND, face.toStr());
}
NET_API int
c2_lock_recordfile(HANDLE login, bool bLock, const c2_pb_name_s *param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Cu* cu = (Cu *)(login);
    SHL_Json::Value    value;



    value["Method"]  = "LockRecordFile";
    value["DevIp"]   = param->deviceHost;
    value["DevPort"] = param->devicePort;
    value["lock"] = bLock ? 1 : 0;
    value["fileName"] = param->filename;


    SHL_Json::FastWriter writer;
    string           json;
    json = writer.write(value);
    

    return cu->lockRecordFile(json);
}


NET_API int  c2_ptz_pattern(HANDLE login, const c2_ptz_pattern_s *param)
{
    if (!login || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }
    Cu* cu = (Cu *)(login);

    C2PtzPattern ptz(*param);
    if (!ptz.validate())
    {
        return -1;
    }

    return cu->ptzPatternCtrl(ptz.toStr());
}

NET_API int c2_set_displayregion(HANDLE view, const c2_display_region_s *param)
{

    if (!view || !param)
    {
        ERRE_SDK("invalidate handle or parameter.");
        return -1;
    }

    Preview *pview = (Preview *)view;

    return pview->setDisplayRegion(param->left, param->right, param->top, param->bottom);
}

NET_API int c2_set_server_status_cb(HANDLE login, c2_alarm_cb statucb, void *userdata)
{
    if (!login)
    {
        //not complite
        return 0;
    }
    else
    {
        Cu* cu = (Cu *)(login);
        cu->setServerStatusCb(statucb, userdata);
        return 0;
    }
}

#ifdef UNIX_DRAW_RECT
NET_API int  c2_set_sdk_param(const char* json)
{
    if (json == NULL)
    {
        return -1;
    }
    SHL_Json::Reader redaer;
    SHL_Json::Value rvalue;
    if(!redaer.parse(json, rvalue, false))
    {
        return -1;
    }

    AutoLock lock(g_sdkParamMtx);
    if (rvalue.isMember("rectShowType"))
    {
        g_rectShowType = rvalue["rectShowType"].asInt();
    }

    if (rvalue.isMember("roiShow"))
    {
        g_roiShow = rvalue["roiShow"].asInt();
    }
    return 0;
}
#endif