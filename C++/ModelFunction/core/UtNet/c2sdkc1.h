#ifndef __C2SDK_H__
#define __C2SDK_H__


#ifndef DLL_EXPORT
#include "c2types.h"
#endif

#ifdef NET_STATICLIB
#  define NET_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(NET_EXPORTS)
#    define NET_API  __declspec(dllexport)
#  else
#    define NET_API  __declspec(dllimport)
#  endif
#else
#  define   NET_API
#endif

#if defined(DLL_EXPORT)
//#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef void*                HANDLE;
typedef unsigned long        HWND;
typedef void*                HDC;
#endif


#define MAX_HOST_LEN	     (128)
#define MAX_NAME_LEN_C2	     (128)
#define MAX_ADDR_LEN         (24)

typedef struct
{
    char         * key;
    char         * value;
} c2_ex_param_s;

typedef struct
{
    char         * servIp;                                 
    int            servPort;                              
    char         * username;                              
    char         * password;                              
    bool           subscribe;                             

    int            exLen;                                 
    void         * exParam;                               
} c2_login_s;

typedef struct
{
    int            channelNo;                             
    int            obtainType;                           
    int            streamType;                            

    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            transport;                             

    int            exLen;                                 
    void         * exParam;                               
} c2_preview_s;


typedef struct
{
    int            type;                                  
    char         * deviceHost;                            
    int            devicePort;                            
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_query_devinfo_s;

typedef struct
{
    char         * deviceHost;                            
    int            devicePort;                            
    int            channelNo;                             
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_query_channel_s;

typedef struct
{
    int            channelNo;                             

    int            speed;                                 
    int            preset;                                
    bool           stop;                                  

    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_ptz_s;

typedef struct
{
    char         * addrcode        ;                     
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    int            channelNo;                             

    int            exLen;                                 
    void         * exParam;                               
} c2_query_preset_s;

typedef struct
{
    char         * name;                                  
    int            index;                                 
    int            type;                                  
    int            isIVS;                                 
} c2_preset_s;

typedef struct
{
    char         * addrcode;                             
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    int            channelNo;                             

    int            presetNum;                             
    c2_preset_s  * presetList;                            

    int            exLen;                                 
    void         * exParam;                               
} c2_set_preset_s;


#ifndef DATETIME_S_C2
#define DATETIME_S_C2
typedef struct
{
    int            year;
    int            month;
    int            day;
    int            hour;
    int            minute;
    int            second;
} datetime_s;
#endif

typedef struct
{
    int            channelNo;                             

    int            type;                                  

    datetime_s     start;                                 
    datetime_s     end;                                   

    int            fromIndex;                             
    int            toIndex;                               

    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_query_record_s;

typedef struct
{
    char         * filename;                              

    int            obtainType;                            
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_pb_name_s;

typedef struct
{
    int            channelNo;                             

    int            type;                                  

    datetime_s     start;                                 
    datetime_s     end;                                   

    int            obtainType;                            
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_pb_time_s;

typedef struct
{
    char         * savepath;                              
    char         * filename;                              

    int            obtainType;                            
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_download_s;

typedef struct
{
    char         * savepath;                              

    datetime_s     start;                                 
    datetime_s     end;                                   

    int            channelNo;                             
    int            obtainType;                            
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_download_time_s;

typedef struct
{
    datetime_s     time;                                  
    char         * zone;                                  
    bool           setLocal;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_synct_s;

typedef struct
{
    int            mode;                                  
    int            channelNo;                             
    int            outPort;                               
    char         * deviceHost;                            
    int            devicePort;                            

    char         * subServHost;                           
    int            subServPort;                           

}c2_decoder_param_s;

typedef struct
{
    int            channelNo;                             
    char         * deviceHost;                            
    int            devicePort;                            
    char         * addrcode;                              
    
}c2_dutk_chn;

typedef struct
{
    int            obtainType;                            
    int            chnNum;                                
    c2_dutk_chn  * chnList;                               

    int            exLen;                                 
    void         * exParam;                               
} c2_dualtalk_dev_s;


typedef struct
{
    int            inPort;                                
    int            outPort;                               
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           

    int            exLen;                                 
    void         * exParam;                               
}c2_matrix_s;

typedef struct
{
    int            opera;                                

    int            picSize;                               
    char         * picPath;                               
    char         * name;                                  
    char         * Id;                                    

    int            channelNo;                             
    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_set_face_s;

typedef struct
{
    int            channelNo;                             
    int            patternCmd;                            
    int            patternID;                             

    char         * deviceHost;                            
    int            devicePort;                            
    char         * subServHost;                           
    int            subServPort;                           
    char         * addrcode;                              

    int            exLen;                                 
    void         * exParam;                               
} c2_ptz_pattern_s;

typedef struct
{
    float          left;
    float          right;
    float          top;
    float          bottom;
    int            exLen;                                 
    void         * exParam;                               
} c2_display_region_s;

#endif

typedef struct
{
    char           id[MAX_NAME_LEN_C2];                   
    char           version[MAX_NAME_LEN_C2];              
    char           msvcode[MAX_ADDR_LEN];                 
    char           msvname[MAX_ADDR_LEN];                 
    char           usercode[MAX_ADDR_LEN];                

    uint64_t       alarmBitmap;                           

    uint64_t       mgrBitmap;                             
    uint64_t       optBitmap;                             
    int            mgrLevel;                              
    int            optLevel;                              

    int            deviceNum;                             

    bool           allowRelogin;                          
    bool           suportTalk;                            
    bool           suportPriv;                            
} c2_servinfo_s;

typedef struct
{
    char           id[MAX_NAME_LEN_C2];                   
    char           addrcode[MAX_ADDR_LEN];                
    char           parent[MAX_ADDR_LEN];                  
    char           name[MAX_NAME_LEN_C2];                 
    char           host[MAX_HOST_LEN];                    

    int            port;                                  
    int            type;                                  
    int            channelNo;                             
    int            channelNum;                            

    int            protoType;                             
    int            streamType;                            
    int            status;                                
	int            devtype;                               
	int            manutype;                              
} c2_devinfo_s;

typedef struct
{
    char           filename[MAX_NAME_LEN_C2];             
    int            size;                                  
    int            channelNo;                             
    int            type;                                  
    int            index;                                 

    datetime_s     start;                                 
    datetime_s     end;                                   
} c2_record_s;

typedef struct  
{
    char           name[MAX_NAME_LEN_C2];                 
    char           addrcode[MAX_ADDR_LEN];                
    int            index;                                 
    int            status;                                
    int            type;                                  
    char           beginTime[MAX_NAME_LEN_C2];            
}c2_alarm_s;

typedef struct
{
	char user[MAX_NAME_LEN_C2];
	char password[MAX_NAME_LEN_C2];
	uint8_t type;
	uint64_t rightBit;
	uint8_t previewBit;
	uint8_t playbackBit;
	uint8_t ctrlBit;
	uint8_t level;
}c2_user_info_s;

#ifdef __cplusplus
extern "C" {
#endif



NET_API long   c2_init(const char* jvm2nativePath = "");


NET_API void   c2_uninit();


NET_API char * c2_version();


NET_API char * c2_buildPTS();


NET_API long c2_set_error(const char* fmt, ...);


NET_API const char *c2_get_error();


enum
{
    CODE_LOGIN_SUCCESS   = 0,   
    CODE_LOGIN_FAIL      = 1,   
    CODE_LOGIN_CONNFAIL  = 2,   
    CODE_LOGIN_LOGINED   = 3,   
    CODE_LOGIN_NOUSER    = 4,   
    CODE_LOGIN_PSWDERR   = 5,   
    CODE_LOGIN_NORIGHT   = 6,   
    CODE_LOGIN_SWITCHOST = 7,   
};


typedef void (*c2_login_cb)(int code, void* userdata);


NET_API HANDLE c2_login(const c2_login_s *param, c2_login_cb loginCB, void *userdata, int protocol = 0);


NET_API void c2_logout(HANDLE login);


NET_API bool c2_islogin(HANDLE login);


NET_API long c2_add_standby(HANDLE login, const char *ip, int port);

NET_API long c2_del_standby(HANDLE login, const char *ip, int port);


NET_API long c2_get_servinfo(HANDLE login, c2_servinfo_s *info);


typedef void(*c2_userinfo_cb)(c2_user_info_s *info, void* userdata);


NET_API long c2_query_user_v1(HANDLE login, c2_userinfo_cb userInfoCb, void *userdata);


typedef void (*c2_devinfo_cb)(c2_devinfo_s *info, void* userdata);


NET_API long c2_query_device_v1(HANDLE login, const c2_query_devinfo_s *param, c2_devinfo_cb devinfoCB, void *userdata);


NET_API long c2_query_device_v2(HANDLE login, const c2_query_devinfo_s *param, char *json, int *size);


NET_API long c2_query_channel_status(HANDLE login, const c2_query_channel_s *param, char *json, int *size);


typedef enum
{
    ALARM_UNKNOW,
    ALARM_DEV_STATUS      = 1,                             
    ALARM_DEV_IO,                                          
    ALARM_DEV_HDD,                                         
    ALARM_DEV_VLOSS,                                       
    ALARM_DEV_MD          = 5,                             
    ALARM_DEV_VCOVER,                                      
    ALARM_DEV_INVADE,                                      
    ALARM_DEV_MANUAL,                                      
    ALARM_DEV_GPS,                                         
    ALARM_DEV_CALL        = 10,                            
    ALARM_DEV_END,                                         

    ALARM_AI_TRIPWIRE     = 20,                            
    ALARM_AI_PERIMETER,                                    
    ALARM_AI_ITEMLEFT,                                     
    ALARM_AI_PLATE,                                        
    ALARM_AI_HOVER,                                        
    ALARM_AI_ANTITHEFT    = 25,                            
    ALARM_AI_OFFJOB,                                       
    ALARM_AI_MAINTENAREA,                                  
    ALARM_AI_PRESSPLATE,                                   
    ALARM_AI_INDICATOR,                                    
    ALARM_AI_DISCONNECTOR = 30,                            
    ALARM_AI_LIGHT,                                        
    ALARM_AI_HELMET,                                       

    ALARM_AI_YX_END,                                       

    ALARM_AI_LEDDIGIT     = 40,                            
    ALARM_AI_METER,                                        
    ALARM_AI_NUMPEOPLE,                                    
    ALARM_AI_THERMOMETRY,                                  

    ALARM_AI_YC_END,                                       

    ALARM_PUSH_SYSTEMINFO = 50,                            
    ALARM_PUSH_LINKAGE,                                    
    ALARM_PUSH_LINKAGE_TRANSMIT,                           
    ALARM_PUSH_VI,                                         
    
    ALARM_PUSH_END,                                       

    ALARM_AI_SIGNAL_LOST = 60,                             
    ALARM_AI_SIGNAL_CONTRASTLOW,                           
    ALARM_AI_SIGNAL_FUZZY,                                 
    ALARM_AI_SIGNAL_OVERORUNDERBRIGHT,                     
    ALARM_AI_SIGNAL_COLORCAST,                             
    ALARM_AI_SIGNAL_NOISEJAMMING,                          
    ALARM_AI_SIGNAL_STRIPENOISE,                           
    ALARM_AI_SIGNAL_BLACKWHITEIMAGE,                       
    ALARM_AI_SIGNAL_VIDEOOCCLUSION,                        
    ALARM_AI_SIGNAL_IMAGEFREEZE,                           
    ALARM_AI_SIGNAL_VIDEOUPHEAVAL,                         
    ALARM_AI_SIGNAL_VIDEOSHAKE,                            
    ALARM_AI_SIGNAL_SCENECHANGE,                           
    ALARM_AI_SIGNAL_PTZ_OUTOFCONTROL,                      
    ALARM_AI_VIDEO_QUALITY_END,                            

    ALARM_PB_EXCEEDHEIGHT,                                 
    ALARM_PERSION_TOGETHER,                                
    ALARM_PERMETER_PERSON,                                 
    ALARM_PERMETER_CAR,                                    
    ALARM_PERMETER_NOCAR,                                  
    ALARM_PERMETER_OBJECT,                                 
    ALARM_CAR_NOIN,                                        
    ALARM_CAR_NOSTOP,                                      
    ALARM_PB_FALLDOWN,                                     
    ALARM_PB_DAJIA,                                        
    ALARM_PB_SMOKE,                                        
    ALARM_FIRE_CONTROL,                                    
    ALARM_TEMP_AREA,                                       
    ALARM_TEMP_DIFF,                                       

    ALARM_TEMP_BODONG,                                     
    ALARM_WAIGUAN_MINPAI,                                  
    ALARM_WAIGUAN_JUEYUAN,                                 
    ALARM_WAIGUAN_TAOGUAN,                                 
    ALARM_WAIGUAN_BIANYABT,                                
    ALARM_WAIGUAN_BILEIZHENG,                              
    ALARM_WAIGUAN_PATI,                                    
    ALARM_WAIGUAN_QIANGTI,                                 
    ALARM_WAIGUAN_GANTANG,                                 

    ALARM_FACE_DETECTTION,                                 
    ALARM_FACE_IDCARD_DETECTTION,                          
    ALARM_GATE_STATUS,                                     
    ALARM_CAPTURE_PIC,                                     

    ALARM_SYSTEM_STATUS,                                   
    ALARM_PTZ_CONTROL,                                     

    ALARM_GATEWAY_STATUS,                                  
    ALARM_THERMOMETRY_ALARM,                               
    ALARM_STARTED_IVA,                                     
	ALARM_PTZ_CONTROL_ALARM,                               
} ALARM_TYPE_E;


typedef int (*c2_alarm_cb)(const char* json, int size, void* userdata);


NET_API long c2_subscribe(HANDLE login, uint64_t typeBitMap, c2_alarm_cb alarmCB, void *userdata);


enum
{
    CODE_SUCCESS     = 0,      
    CODE_STREAM_FAIL = 1,      
    CODE_PLAY_SUCCESS= 2,      
    CODE_PLAY_FAIL   = 3,      
    CODE_STREAM_END  = 4,      
    CODE_PLAY_END    = 5,      
    CODE_CAP_FAIL    = 6,      
    CODE_REC_FAIL    = 7,      
    CODE_TALK_SUCC   = 8,      
    CODE_TALK_FAIL   = 9,      
};


typedef void (*c2_status_cb)(int code, void* userdata);


typedef bool (*c2_stream_cb)(HWND wnd, void *data, uint32_t size, void *userdata);


NET_API HANDLE c2_start_preview(HANDLE login, const c2_preview_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);



NET_API int c2_add_preview_surface(HANDLE view, HWND wnd);



NET_API int c2_del_preview_surface(HANDLE view, HWND wnd);



NET_API int c2_start_talk(HANDLE view);

NET_API int c2_stop_talk(HANDLE view);

NET_API int c2_preview_status(HANDLE view);

NET_API void c2_stop_preview(HANDLE login, HANDLE view);

NET_API int c2_query_chn_relation_dev(HANDLE view, char* json, int *size);

NET_API long c2_change_wndsize(HANDLE view, int width, int height);

NET_API long c2_change_wndpos(HANDLE view, int x, int y);

NET_API int c2_opensound(HANDLE view);

NET_API int c2_colsesound(HANDLE view);

NET_API void c2_set_volume(HANDLE view, int volume);

NET_API long c2_capture(HANDLE view, const char *filepath, int type);

NET_API long c2_start_realREC(HANDLE view, const char *filepath);

NET_API void c2_stop_realREC(HANDLE view);

NET_API HANDLE c2_start_backREC(HANDLE login, const c2_preview_s *param, const char *filepath);

NET_API void c2_stop_backREC(HANDLE login, HANDLE view);

typedef void (*c2_decode_cb)(int type, int w, int h, void *yuv, int size, void *userdata);

typedef void (*c2_draw_cb)(HDC hdc, HWND wnd, int w, int h, void *userdata);

NET_API long c2_set_drawcb(HANDLE view, c2_draw_cb drawCB, void *userdata);

NET_API long c2_set_decodecb(HANDLE view, c2_decode_cb decodeCB, void *userdata);

typedef enum
{
		PTZ_CMD_NONE_C2,
		PTZ_CMD_UP_C2       = 1,
		PTZ_CMD_DOWN_C2,
		PTZ_CMD_LEFT_C2,
		PTZ_CMD_RIGHT_C2,
		PTZ_CMD_UPLEFT_C2   = 5,
		PTZ_CMD_UPRIGHT_C2,
		PTZ_CMD_DOWNLEFT_C2,
		PTZ_CMD_DOWNRIGHT_C2,
		PTZ_CMD_ZOOMIN_C2,
		PTZ_CMD_ZOOMOUT_C2  = 10,
		PTZ_CMD_FOCUSNEAR_C2,
		PTZ_CMD_FOCUSFAR_C2,
		PTZ_CMD_IRISOPEN_C2,
		PTZ_CMD_IRISCLOSE_C2,
		PTZ_CMD_AUTOSCAN_C2 = 15,
		PTZ_CMD_SETPRESET_C2,
		PTZ_CMD_CLEARPRESET_C2,
		PTZ_CMD_CALLPRESET_C2,
		PTZ_CMD_RAIN_ON_C2,                       
		PTZ_CMD_RAIN_OFF_C2 = 20,
		PTZ_CMD_AUXLIGHT_ON_C2,                   
		PTZ_CMD_AUXLIGHT_OFF_C2,
		PTZ_CMD_END,
} PTZ_CMD_E;


NET_API long c2_ptz_control(HANDLE login, int cmd, const c2_ptz_s *param);

typedef void(*c2_preset_cb)(c2_preset_s *preset, void *pUser);


NET_API long c2_get_chn_preset_v1(HANDLE login, const c2_query_preset_s *param, c2_preset_cb presetCB, void *userdata);

NET_API long c2_get_chn_preset_v2(HANDLE login, const c2_query_preset_s *param, char* json, int *size);


NET_API long c2_set_chn_preset(HANDLE login, const c2_set_preset_s *param);

//NET_API long c2_set_chn_preset_v1(HANDLE login, const c2_set_preset_s *param);

typedef void (*c2_record_cb)(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata);


NET_API long c2_query_record_v1(HANDLE login, const c2_query_record_s *param, c2_record_cb recordCB, void *userdata);


NET_API long c2_query_record_v2(HANDLE login, const c2_query_record_s *param, char *json, int *size);

NET_API HANDLE c2_playback_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                  c2_status_cb statusCB, void *statusData);

NET_API HANDLE c2_playbackreverse_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
    c2_status_cb statusCB, void *statusData);

NET_API HANDLE c2_playback_bytime(HANDLE login, const c2_pb_time_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                  c2_status_cb statusCB, void *statusData);

NET_API int c2_add_playback_surface(HANDLE playback, HWND wnd);


NET_API int c2_del_playback_surface(HANDLE playback, HWND wnd);


NET_API long c2_set_pb_decodecb(HANDLE pb, c2_decode_cb decodeCB, void *userdata);


NET_API void c2_stop_playback(HANDLE login, HANDLE playback);

enum
{
    PLAY_NONE,
    PLAY_START,		                                      
    PLAY_STOP,		                                      
    PLAY_PAUSE,		                                      
    PLAY_RESTART,	                                      
    PLAY_FAST,	                                          
    PLAY_SLOW,		                                      
    PLAY_NORMAL,		                                  
    PLAY_STEP,                                            
    PLAY_OPENAUDIO,	                                      
    PLAY_CLOSEAUDIO,	                                  
    PLAY_STARTREPOS,                                      
    PLAY_SETPOS	,	                                      
    PLAY_SETTIME,                                         
    PLAY_REVERSE,                                         
    PLAY_FORWARD,                                         
    PLAY_SETLABEL,                                        
    PLAY_SETVOLUME,                                       
};

NET_API long c2_playback_control(HANDLE playback, int code, void *val, int valSize);

NET_API long c2_playback_capture(HANDLE view, const char *filepath, int type);

NET_API long c2_get_playbacktime(HANDLE playback, datetime_s *time);


NET_API long c2_get_playbackpos(HANDLE playback, datetime_s *start = 0, datetime_s *end = 0);



NET_API long c2_playback_change_wndsize(HANDLE playback, int width, int height);

NET_API long c2_playback_change_wndpos(HANDLE playback, int x, int y);


NET_API int c2_pb_opensound(HANDLE view);

NET_API int c2_pb_colsesound(HANDLE view);

NET_API int c2_pb_setvolume(HANDLE view, int vol);

NET_API HANDLE c2_start_download(HANDLE login, const c2_download_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

NET_API HANDLE c2_start_downloadByTime(HANDLE login, const c2_download_time_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

NET_API void c2_stop_download(HANDLE login, HANDLE download);

NET_API long c2_get_downloadpos(HANDLE playback);

NET_API long c2_sync_time(HANDLE login, const c2_synct_s *synct);


NET_API HANDLE c2_init_dualtalk(int flag);

NET_API int c2_uninit_dualtalk(HANDLE dutk);



enum
{
    CODE_TALK_SUCCESS = 0,    
    CODE_TALK_FAILED = 1,     
    CODE_TALK_BUSY = 2        
};


NET_API HANDLE c2_start_dev_dualtalk(HANDLE dutk, HANDLE view, c2_status_cb statusCB, void* userdata);

NET_API int c2_stop_dev_dualtalk(HANDLE dutk, HANDLE view);

NET_API int c2_send_audio(HANDLE dutk, void* data, int size, int encode, int sps, int channels, int bps, uint64_t pts);

NET_API int c2_send_video(HANDLE dutk, void* data, int size, int w, int h, int type, int encode, int fps, uint64_t pts);

NET_API int c2_matrix_switch(HANDLE login, const c2_matrix_s * pParam);

NET_API int c2_decoder_switch(HANDLE login, const c2_decoder_param_s * param);

NET_API int  c2_send_command(HANDLE login, char * json, int size);



NET_API int  c2_get_config(HANDLE login, char * _in_json, int _in_size, char* _out_json, int *_out_size);


NET_API int  c2_set_face(HANDLE login, const c2_set_face_s * param);


NET_API int  c2_lock_recordfile(HANDLE login, bool bLock, const c2_pb_name_s *param);


NET_API int  c2_ptz_pattern(HANDLE login, const c2_ptz_pattern_s *param);


NET_API int  c2_set_displayregion(HANDLE view, const c2_display_region_s *param);


NET_API int  c2_set_server_status_cb(HANDLE login, c2_alarm_cb statucb, void* userdata);

#ifdef __cplusplus
}
#endif

#endif
