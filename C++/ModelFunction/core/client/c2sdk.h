/**
 * @file      c2sdk.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:32:59
 *
 * @note
 *  cross platform client sdk interface defines.
 */
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
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef void*                HANDLE;
typedef unsigned long        HWND;
typedef void*                HDC;
#endif


#define MAX_HOST_LEN	     (128)
#define MAX_NAME_LEN	     (128)
#define MAX_ADDR_LEN         (24)

typedef struct
{
    char         * key;
    char         * value;
} c2_ex_param_s;

typedef struct
{
    char         * servIp;                                 /**< 连接服务器IP */
    int            servPort;                               /**< 连接服务器端口 */
    char         * username;                               /**< 登录用户名 */
    char         * password;                               /**< 登录密码 */
    bool           subscribe;                              /**< 默认是否订阅报警 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_login_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    int            streamType;                             /**< 0-大码流, 1-从码流 */

    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            transport;                              /**< IPPROTO_TCP/IPPROTO_UDP, transport */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_preview_s;


typedef struct
{
    int            type;                                   /**< 设备类型, 参考国网B协议设备类型定义值 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_devinfo_s;

typedef struct
{
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_channel_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            speed;                                  /**< 速度 0~10 */
    int            preset;                                 /**< 预制位 0~128 */
    bool           stop;                                   /**< 是否停止 false-start, true-stop */

    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_ptz_s;

typedef struct
{
    char         * addrcode        ;                       /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    int            channelNo;                              /**< 通道号 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_preset_s;

typedef struct
{
    char         * name;                                   /**< 预置位名称 */
    int            index;                                  /**< 预置位序列号 */
    int            type;                                   /**< 预置位类型 */
    int            isIVS;                                  /**< 是否开启智能分析，0-否 */
} c2_preset_s;

typedef struct
{
    char         * addrcode;                               /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    int            channelNo;                              /**< 通道号 */

    int            presetNum;                              /**< 预置位个数 */
    c2_preset_s  * presetList;                             /**< 预置列表 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_set_preset_s;


#ifndef DATETIME_S
#define DATETIME_S
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
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            type;                                   /**< 0-所有录像 1-计划录像 2 手动录像 3-报警录像 */

    datetime_s     start;                                  /**< 查找开始时间 */
    datetime_s     end;                                    /**< 查找结束时间 */

    int            fromIndex;                              /**< 期望返回的开始记录序号, 从1开始 */
    int            toIndex;                                /**< 期望返回的结束记录序号(数) */

    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_record_s;

typedef struct
{
    char         * filename;                               /**< 录像文件名 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_pb_name_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            type;                                   /**< 0-所有录像 1-计划录像 2 手动录像 3-报警录像 */

    datetime_s     start;                                  /**< 回放开始时间 */
    datetime_s     end;                                    /**< 回放结束时间 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_pb_time_s;

typedef struct
{
    char         * savepath;                               /**< 录像保存路径, 需要包含文件名，不需要带后缀，例如：/fff/ddd/sss/filename  */
    char         * filename;                               /**< 录像文件名，从c2_query_record_v1,c2_query_record_v2接口中获得 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_download_s;

typedef struct
{
    char         * savepath;                               /**< 录像保存路径 */

    datetime_s     start;                                  /**< 下载开始时间 */
    datetime_s     end;                                    /**< 下载结束时间 */

    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_download_time_s;

typedef struct
{
    datetime_s     time;                                   /**< 同步时间值 */
    char         * zone;                                   /**< 时区, Asia/Shanghai */
    bool           setLocal;                               /**< 是否设置本地时间 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_synct_s;

typedef struct
{
    int            mode;                                   /**显示模式，NET_SHOW_MODE_E, 1, 4, 9 ,16*/
    int            channelNo;                              /**需要解码显示的通道，一位对应一个通道*/
    int            outPort;                                /**显示输出端口，从1开始 */
    char         * deviceHost;                             /**设备ip地址 */
    int            devicePort;                             /**设备端口 */

    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */

}c2_decoder_param_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */
    
}c2_dutk_chn;

typedef struct
{
    int            obtainType;                             /**< 0-IP/PORT, 1-IP/PORT 2-地址码, 3-地址码 */
    int            chnNum;                                 /**< 通道个数 */
    c2_dutk_chn  * chnList;                                /**< 通道列表 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_dualtalk_dev_s;


typedef struct
{
    int            inPort;                                 /**< 矩阵输入端口号 */
    int            outPort;                                /**< 矩阵输出端口号 */
    char         * deviceHost;                             /**< 设备IP地址 */
    int            devicePort;                             /**< 设备端口号 */
    char         * subServHost;                            /**< 二级流媒体服务器地址 */
    int            subServPort;                            /**< 二级流媒体端口号 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
}c2_matrix_s;

typedef struct
{
    int            opera;                                  /**< 操作类型，0：添加人脸信息 1：删除人脸信息 */

    int            picSize;                                /**< 图片大小（未使用，图片大小不得超过1.5M） */
    char         * picPath;                                /**< 图片路径 */
    char         * name;                                   /**< 人脸名称 */
    char         * Id;                                     /**< 人脸ID，不能重复 */

    int            channelNo;                              /**< 通道号，一般为 0 */
    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_set_face_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            patternCmd;                             /**< 云台轨迹操作命令码,详见下面定义*/
    int            patternID;                              /**< 云台轨迹ID(删除所有轨迹时无效)*/

    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_ptz_pattern_s;

typedef struct
{
    float          left;
    float          right;
    float          top;
    float          bottom;
    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_display_region_s;

#endif

typedef struct
{
    char           id[MAX_NAME_LEN];                       /**< 服务器id号 */
    char           version[MAX_NAME_LEN];                  /**< 服务器软件版本 */
    char           msvcode[MAX_ADDR_LEN];                  /**< 服务器地址码 */
    char           msvname[MAX_ADDR_LEN];                  /**< 服务器名称 */
    char           usercode[MAX_ADDR_LEN];                 /**< 登录用户地址码 */

    uint64_t       alarmBitmap;                            /**< 报警推送订阅位图 */

    uint64_t       mgrBitmap;                              /**< 管理权限位图 */
    uint64_t       optBitmap;                              /**< 操作权限位图 */
    int            mgrLevel;                               /**< 管理权限等级 */
    int            optLevel;                               /**< 操作权限等级 */

    int            deviceNum;                              /**< 下级设备数 */

    bool           allowRelogin;                           /**< 允许同用户重复登录 */
    bool           suportTalk;                             /**< 是否支持对讲 */
    bool           suportPriv;                             /**< 是否支持老协议 */
} c2_servinfo_s;

typedef struct
{
    char           id[MAX_NAME_LEN];                       /**< 设备id号 */
    char           addrcode[MAX_ADDR_LEN];                 /**< 地址码 */
    char           parent[MAX_ADDR_LEN];                   /**< 父设备地址码 */
    char           name[MAX_NAME_LEN];                     /**< 服务器名称 */
    char           host[MAX_HOST_LEN];                     /**< 设备host */

    int            port;                                   /**< 设备端口 */
    int            type;                                   /**< 0:本地流媒体 1:本地设备 2:远程流媒体设备 3:通道 */
    int            channelNo;                              /**< 通道号 */
    int            channelNum;                             /**< 通道数 */

    int            protoType;                              /**< 协议类型 */
    int            streamType;                             /**< 0-大码流, 1-从码流 */
    int            status;                                 /**< 0:不在线 1:在线 */
	int            devtype;                                /**< 352:dvr 353:ipc 360:nvr */
	int            manutype;                               /**< 0:hik 1:dahua 2:ut 3:tidy */
} c2_devinfo_s;

typedef struct
{
    char           filename[MAX_NAME_LEN];                 /**< 录像文件名 */
    int            size;                                   /**< 录像文件长度 */
    int            channelNo;                              /**< 通道号 */
    int            type;                                   /**< 1-计划录像 2 手动录像 3-报警录像 */
    int            index;
    //const char*            token;    /**< 当前记录序号, 从1开始 */
    //const char*        rtspUrl;
    datetime_s     start;                                  /**< 录像开始时间 */
    datetime_s     end;                                    /**< 录像结束时间 */
} c2_record_s;

typedef struct  
{
    char           name[MAX_NAME_LEN];                    /**< 名称 */
    char           addrcode[MAX_ADDR_LEN];                /**< 地址码 */
    int            index;                                 /**< 序号 */
    int            status;                                /**< 告警状态 */
    int            type;                                  /**< 告警类型 */
    char           beginTime[MAX_NAME_LEN];               /**< 告警开始时间 */
}c2_alarm_s;

typedef struct
{
	char user[MAX_NAME_LEN];
	char password[MAX_NAME_LEN];
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


/**
* @brief 初始化
*
* @param [in] jvm2nativePath jvm虚拟机路径映射到native路径，方便SDK操作jvm中的文件系统, eg:"/data/data/com.utc2/app_libs/";
*
* @note
*  windows,linux不需要填写，Android必填
*
* @return 0：成功 other:失败
*
*/
NET_API long   c2_init(const char* jvm2nativePath = "");

/**
 * @brief 去初始化
 *
 */
NET_API void   c2_uninit();

/**
 * @brief 获取sdk版本号
 *
 */
NET_API char * c2_version();

/**
* @brief 获取sdk版本号
*
*/
NET_API char * c2_buildPTS();

/**
 * @brief 设置错误,线程安全
 *
 * @param [in] fmt
 * @param [in] ...
 *
 * @return OK?
 *
 * @see c2_get_error();
 */
NET_API long c2_set_error(const char* fmt, ...);

/**
 * @brief 获取错误
 *
 * @param [in]
 *
 * @return 错误字符串
 *
 * @see c2_set_error
 */
NET_API const char *c2_get_error();

/**
 * @登录状态码
 */
enum
{
    CODE_LOGIN_SUCCESS   = 0,   /**< 登录成功 */
    CODE_LOGIN_FAIL      = 1,   /**< 登录失败 */
    CODE_LOGIN_CONNFAIL  = 2,   /**< 连接失败 */
    CODE_LOGIN_LOGINED   = 3,   /**< 该用户已经登录且不可重复登录 */
    CODE_LOGIN_NOUSER    = 4,   /**< 用户名不存在 */
    CODE_LOGIN_PSWDERR   = 5,   /**< 密码错误 */
    CODE_LOGIN_NORIGHT   = 6,   /**< 没有登录权限 */
    CODE_LOGIN_SWITCHOST = 7,   /**< 切换服务器 */
};

/**
 * @brief  登录状态回调
 *
 * @param [out] code     状态码 参考CODE_LOGIN_E定义
 * @param [out] userdata 用户数据
 *
 * @return void
 *
 * @note
 *  在回调函数内部调用c2_get_error获取具体错误信息
 */
typedef void (*c2_login_cb)(int code, void* userdata);

/**
 * @brief  登录
 *
 * @param [in] param    登录参数，参考c2_login_s
 * @param [in] loginCB  登录状态回调
 * @param [in] userdata 用户数据
 * @param [in] protocol 协议类型，
 *                      0 - HTTP
 *                      1 - PRIVATE
 *                      默认为0
 * 扩展参数： key       value             mean
 *        disableMD5     yes           不是用md5对密码进行加密
 *
 *
 * @return 成功返回登录句柄, 失败返回NULL, 获取错误信息调用c2_get_error.
 */
NET_API HANDLE c2_login(const c2_login_s *param, c2_login_cb loginCB, void *userdata, int protocol = 0);

/**
 * @brief  登出
 *
 * @param [in] login  c2_login返回值
 *
 * @return void
 *
 * @see    c2_login
 */
NET_API void c2_logout(HANDLE login);

/**
 * @brief  是否已经登录上
 *
 * @param [in] login  c2_login返回值
 *
 * @return true, false
 *
 * @see    c2_login
 */
NET_API bool c2_islogin(HANDLE login);

/**
 * @brief  添加一个登录备服务器地址
 *
 * @param [in] login c2_login返回值
 * @param [in] ip    备服务器IP
 * @param [in] port  备服务器端口
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 */
NET_API long c2_add_standby(HANDLE login, const char *ip, int port);

/**
 * @brief  删除一个登录备服务器地址
 *
 * @param [in] login c2_login返回值
 * @param [in] ip    备服务器IP
 * @param [in] port  备服务器端口
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 */
NET_API long c2_del_standby(HANDLE login, const char *ip, int port);

/**
 * @brief  获取登录流媒体服务器信息
 *
 * @param [in] login c2_login返回值
 * @param [in] info  流媒体服务器信息
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see    c2_login
 */
NET_API long c2_get_servinfo(HANDLE login, c2_servinfo_s *info);

/**
* @brief  用户信息回调函数定义
*
* @param [out] info     单个用户信息结构
* @param [out] userdata 用户数据
*
* @return void
*
*/
typedef void(*c2_userinfo_cb)(c2_user_info_s *info, void* userdata);

/**
* @brief 获取用户列表信息, 回调方式
*
* @param [in] login    c2_login返回值
* @param [in] userInfoCb  单个用户信息回调
* @param [in] userdata 用户数据
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_login
*/
NET_API long c2_query_user_v1(HANDLE login, c2_userinfo_cb userInfoCb, void *userdata);

/**
 * @brief  单个设备信息回调函数定义
 *
 * @param [out] info     单个设备信息结构
 * @param [out] userdata 用户数据
 *
 * @return void
 *
 */
typedef void (*c2_devinfo_cb)(c2_devinfo_s *info, void* userdata);

/**
 * @brief 获取给定地址码或IP和端口对应的设备下设备(通道)列表信息, 回调方式
 *
 * @param [in] login    c2_login返回值
 * @param [in] param    设备列表查找参数,参考c2_query_devinfo_s
 * @param [in] loginCB  单个设备信息回调
 * @param [in] userdata 用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_query_device_v1(HANDLE login, const c2_query_devinfo_s *param, c2_devinfo_cb devinfoCB, void *userdata);

/**
 * @brief 获取给定地址码或IP和端口对应的设备下设备(通道)列表信息, json方式
 *
 * @param [in] login    c2_login返回值
 * @param [in] param    设备列表查找参数,参考c2_query_devinfo_s
 * @param [in,out] json 设备列表信息,空间由调用者分配
 * @param [in,out] size 设备列表息长度
 *
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_query_device_v2(HANDLE login, const c2_query_devinfo_s *param, char *json, int *size);


/**
 * @brief 获取给定地址码或IP和端口通道号对应的通道信息, json方式
 *
 * @param [in] login    c2_login返回值
 * @param [in] param    设备列表查找参数,c2_query_channel_s
 * @param [in,out] json 通道信息,空间由调用者分配
 * @param [in,out] size 通道信息长度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_query_channel_status(HANDLE login, const c2_query_channel_s *param, char *json, int *size);


typedef enum
{
    ALARM_UNKNOW,
    ALARM_DEV_STATUS      = 1,                             /**< 设备状态报警, 1-上线 0-下线 */
    ALARM_DEV_IO,                                          /**< 设备I/O探头报警 */
    ALARM_DEV_HDD,                                         /**< 设备硬盘报警 */
    ALARM_DEV_VLOSS,                                       /**< 设备视频丢失报警 */
    ALARM_DEV_MD          = 5,                             /**< 设备移动侦测报警 */
    ALARM_DEV_VCOVER,                                      /**< 设备视频遮挡报警 */
    ALARM_DEV_INVADE,                                      /**< 设备非法入侵报警 */
    ALARM_DEV_MANUAL,                                      /**< 手持设备手动报警 */
    ALARM_DEV_GPS,                                         /**< 手持设备GPS报警 */
    ALARM_DEV_CALL        = 10,                            /**< 门口机呼叫报警用 */
    ALARM_DEV_END,                                         /**< 分隔标识, 新定义设备报警类型在这之前添加 */

    ALARM_AI_TRIPWIRE     = 20,                            /**< 拌线 */
    ALARM_AI_PERIMETER,                                    /**< 周界 */
    ALARM_AI_ITEMLEFT,                                     /**< 物品遗留丢失 */
    ALARM_AI_PLATE,                                        /**< 车牌检测 */
    ALARM_AI_HOVER,                                        /**< 徘徊检测 */
    ALARM_AI_ANTITHEFT    = 25,                            /**< 防盗检测 */
    ALARM_AI_OFFJOB,                                       /**< 脱岗检测 */
    ALARM_AI_MAINTENAREA,                                  /**< 检修区域检测 */
    ALARM_AI_PRESSPLATE,                                   /**< 压板投退状态检测 */
    ALARM_AI_INDICATOR,                                    /**< 分合指示牌检测*/
    ALARM_AI_DISCONNECTOR = 30,                            /**< 隔离开关(触头)状态检测 */
    ALARM_AI_LIGHT,                                        /**< 指示灯状态检测  */
    ALARM_AI_HELMET,                                       /**< 安全帽检测 */

    ALARM_AI_YX_END,                                       /**< 分隔标识, 新遥信类型在这之前添加 */

    ALARM_AI_LEDDIGIT     = 40,                            /**< 表计读数检测(七段数码管) */
    ALARM_AI_METER,                                        /**< 表计读数检测(刻度盘) */
    ALARM_AI_NUMPEOPLE,                                    /**< 人数统计 */
    ALARM_AI_THERMOMETRY,                                  /**< 测温是 */

    ALARM_AI_YC_END,                                       /**< 分隔标识, 新遥测类型在这之前添加 */

    ALARM_PUSH_SYSTEMINFO = 50,                            /**< 系统运行参数 */
    ALARM_PUSH_LINKAGE,                                    /**< 五防联动上传 */
    ALARM_PUSH_LINKAGE_TRANSMIT,                           /**< 客户端联动转发 */
    ALARM_PUSH_VI,                                         /**< 智能分析巡警结果报警开始 */
    
    ALARM_PUSH_END,                                        /**< 分隔标识, 新定义推送类型在这之前添加 */

    ALARM_AI_SIGNAL_LOST = 60,                             /**< 视频信号丢失 */
    ALARM_AI_SIGNAL_CONTRASTLOW,                                 /**< 对比度低 */
    ALARM_AI_SIGNAL_FUZZY,                                 /**< 图像模糊 */
    ALARM_AI_SIGNAL_OVERORUNDERBRIGHT,                            /**< 图像过亮\过暗 */
    ALARM_AI_SIGNAL_COLORCAST,                             /**< 图像偏色 */
    ALARM_AI_SIGNAL_NOISEJAMMING,                          /**< 噪声干扰 */
    ALARM_AI_SIGNAL_STRIPENOISE,                           /**< 条纹干扰 */
    ALARM_AI_SIGNAL_BLACKWHITEIMAGE,                       /**< 黑白图像 */
    ALARM_AI_SIGNAL_VIDEOOCCLUSION,                        /**< 视频遮挡 */
    ALARM_AI_SIGNAL_IMAGEFREEZE,                           /**< 画面冻结 */
    ALARM_AI_SIGNAL_VIDEOUPHEAVAL,                         /**< 视频剧变 */
    ALARM_AI_SIGNAL_VIDEOSHAKE,                            /**< 视频抖动 */
    ALARM_AI_SIGNAL_SCENECHANGE,                           /**< 场景变更 */
    ALARM_AI_SIGNAL_PTZ_OUTOFCONTROL,                      /**< 云台失控 */
    ALARM_AI_VIDEO_QUALITY_END,                            /**< 分隔标识 */

    ALARM_PB_EXCEEDHEIGHT,                                 /**< 超过限高 */
    ALARM_PERSION_TOGETHER,                                /**< 人员聚集 */
    ALARM_PERMETER_PERSON,                                 /**< 周界监控-人 */
    ALARM_PERMETER_CAR,                                    /**< 周界监控-车 */
    ALARM_PERMETER_NOCAR,                                  /**< 周界监控-非机动车 */
    ALARM_PERMETER_OBJECT,                                 /**< 周界监控-物 */
    ALARM_CAR_NOIN,                                        /**< 车辆管控-禁入区域 */
    ALARM_CAR_NOSTOP,                                      /**< 车辆管控-禁停区域 */
    ALARM_PB_FALLDOWN,                                     /**< 跌倒 */
    ALARM_PB_DAJIA,                                        /**< 打架 */
    ALARM_PB_SMOKE,                                        /**< 吸烟 */
    ALARM_FIRE_CONTROL,                                    /**< 烟火 */
    ALARM_TEMP_AREA,                                       /**< 区域测温 */
    ALARM_TEMP_DIFF,                                       /**< 温差检测 */

    ALARM_TEMP_BODONG,                                     /**< 温差波动 */
    ALARM_WAIGUAN_MINPAI,                                  /**< 外观检测-铭牌 */
    ALARM_WAIGUAN_JUEYUAN,                                 /**< 外观检测-绝缘子 */
    ALARM_WAIGUAN_TAOGUAN,                                 /**< 外观检测-套管 */
    ALARM_WAIGUAN_BIANYABT,                                /**< 外观检测-变压器本体 */
    ALARM_WAIGUAN_BILEIZHENG,                              /**< 外观检测-避雷针 */
    ALARM_WAIGUAN_PATI,                                    /**< 外观检测-爬梯 */
    ALARM_WAIGUAN_QIANGTI,                                 /**< 外观检测-墙体 */
    ALARM_WAIGUAN_GANTANG,                                 /**< 外观检测-杆塔 */

    ALARM_FACE_DETECTTION,                                 /**< 人脸检测 */
    ALARM_FACE_IDCARD_DETECTTION,                          /**< 人证合一检测 */
    ALARM_GATE_STATUS,                                     /**< 道闸状态 *//** 202006011 zhangshenyu add**/
    ALARM_CAPTURE_PIC,                                     /**< 图像抓拍 */

    ALARM_SYSTEM_STATUS,                                   /**< 系统运行状态, add by shl  */
    ALARM_PTZ_CONTROL,                                     /**< 云台控制状态, add by shl */

    ALARM_GATEWAY_STATUS,                                  /**< 人员通道摆闸状态 *//** 20201112 quxiaohui add**/
    ALARM_THERMOMETRY_ALARM,                               /**< 红外摄像机的温度告警 */
    ALARM_STARTED_IVA,                                     /**< 智能分析已启动 */
	ALARM_PTZ_CONTROL_ALARM,                               /**< 云台控制告警 */
} ALARM_TYPE_E;

/**
 * @brief  报警数据回调
 *
 * @param [out] json  报警数据 情形1 - {"version": "1.0", "time": "2016-09-09 10:39:40", "deviceName": "HK-DVR", "deviceHost": "192.168.60.34", "devicePort": 8000,"addrcode":"362110000003600002","level":,alarms": [
 *                                     {"type": 1, "code", "channelNo": 3}]}
 *
 *                             情形2 - 同一时刻触发多个报警，如智能分析同一张图片可能会有多种类型报警
 *                                     {"version": "1.0", "time": "2016-09-09 10:39:40", "deviceName": "HK-DVR", "deviceHost": "192.168.60.34", "devicePort": 8000,"addrcode":"362110000003600002","level":,"alarms": [
 *                                     {"type": 20, "channelNo": 1, "points": ["p1", "p2": ] },
 *                                     {"type": 21, "channelNo": 1, "points": ["p1", "p2":, "p3":, "p4": } ]}
 *
 * @param [out] size  报警数据长度
 * @param [out] userdata 用户数据
 *
 * @return 0
 */
typedef int (*c2_alarm_cb)(const char* json, int size, void* userdata);

/**
 * @brief 报警订阅
 *
 * @param [in] login      c2_login返回值
 * @param [in] typeBitMap 报警订阅类型位图,每位定义参考ALARM_TYPE_E
 * @param [in] alarmCB    报警推送回调函数入口
 * @param [in] userdata   用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_subscribe(HANDLE login, uint64_t typeBitMap, c2_alarm_cb alarmCB, void *userdata);

/**
  *@brief 预览状态码
  */
enum
{
    CODE_SUCCESS     = 0,       /**< 预览成功 */
    CODE_STREAM_FAIL = 1,       /**< 取流失败 */
    CODE_PLAY_SUCCESS= 2,       /**< 解码成功 */
    CODE_PLAY_FAIL   = 3,       /**< 解码失败 */
    CODE_STREAM_END  = 4,       /**< 取流结束 */
    CODE_PLAY_END    = 5,       /**< 播放结束 */
    CODE_CAP_FAIL    = 6,       /**< 抓图失败 */
    CODE_REC_FAIL    = 7,       /**< 录像失败 */
    CODE_TALK_SUCC   = 8,       /**< 对讲成功 */
    CODE_TALK_FAIL   = 9,       /**< 对讲失败 */
};

/**
 * @brief 状态回调
 *
 * @param [out] code     状态码 参考CODE_STATUS_E定义
 * @param [out] userdata 用户数据
 *
 * @return void
 *
 * @note
 *  在回调函数内部调用c2_get_error获取具体错误信息
 */
typedef void (*c2_status_cb)(int code, void* userdata);

/**
 * @brief  码流数据回调函数定义
 *
 * @param [out] wnd      当前的预览/回放句柄
 * @param [out] data     存放数据的缓冲区指针, ut_frame_s+音视频数据
 * @param [out] size     缓冲区大小
 * @param [out] userdata 用户数据
 *
 * @return false, 通知内部,播放缓存已满
 */
typedef bool (*c2_stream_cb)(HWND wnd, void *data, uint32_t size, void *userdata);

/**
 * @brief 启动某台设备某个通道的视频预览
 *
 * @param [in] login      c2_login返回值
 * @param [in] param      预览参数,参考c2_preview_s定义
 * @param [in] streamCB   实时视频数据回调函数入口
 * @param [in] streamData 实时视频数据回调用户数据
 * @param [in] statusCB   状态回调
 * @param [in] statusData 状态回调用户数据
 *
 * @return 成功返回预览句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
 *
 * @see c2_stop_preview, c2_login
 */
NET_API HANDLE c2_start_preview(HANDLE login, const c2_preview_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);


/**
* @brief 给预览额外添加一个播放窗口
*
* @param [in] view       预览句柄, c2_start_preview返回值
* @param [in] wnd        窗口句柄
*
* @return 成功返回0, 失败返回. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview, c2_del_preview_surface
*/
NET_API int c2_add_preview_surface(HANDLE view, HWND wnd);


/**
* @brief 删除预览窗口
*
* @param [in] view       预览句柄, c2_start_preview返回值
* @param [in] wnd        窗口句柄
*
* @return 成功返回0, 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview, c2_add_preview_surface
*/
NET_API int c2_del_preview_surface(HANDLE view, HWND wnd);


/**
* @brief 开始对讲, 不再使用
*
* @param [in] view   预览句柄, c2_start_preview返回值
*
* @see
*/
NET_API int c2_start_talk(HANDLE view);

/**
* @brief 关闭对讲，不再使用
*
* @param [in] view   预览句柄, c2_start_preview返回值
*
* @see
*/
NET_API int c2_stop_talk(HANDLE view);

/**
* @brief 获取预览状态，对讲，抓图状态
*
* @param [in] view   预览句柄, c2_start_preview返回值
*
* @see
*/
NET_API int c2_preview_status(HANDLE view);

/**
 * @brief  停止某台设备某个通道的视频预览
 *
 * @param [in] login 登录句柄, c2_login返回值
 * @param [in] view  预览句柄, c2_start_preview返回值
 *
 * @see c2_start_preview, c2_login
 */
NET_API void c2_stop_preview(HANDLE login, HANDLE view);


/**
* @brief  获取该通道是否关联轨道车设备
*
* @param [in] view  预览句柄, c2_start_preview返回值
* @param [in] param 设备信息
* @param [out] json 关联设备信息,空间由调用者分配
* @param [in, out] size 调用时传入分配空间的大小，调用完成后返回实际json的长度
* @return 0:没有关联轨道车设备  1:关联轨道车设备
*
* @see c2_start_preview, c2_login
*/
NET_API int c2_query_chn_relation_dev(HANDLE view, char* json, int *size);

#ifdef UNIX_DRAW_RECT
/**
* @brief  控制绘图显示
* @param [in] view  预览句柄, c2_start_preview返回值
* @param [in] json  控制命令
*                   (1)清空图形数据：{"command":"clearROI"}
* @see c2_start_preview
*/
NET_API int c2_ctrl_drawroi(HANDLE view, char* json);
#endif

/**
 * @brief 改变预览播放窗口大小
 *
 * @param [in] view   预览句柄, c2_start_preview返回值
 * @param [in] width  窗口宽度
 * @param [in] height 窗口高度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_start_preview,
 *
 * @note
 *   Linux下才起作用
 */
NET_API long c2_change_wndsize(HANDLE view, int width, int height);

/**
 * @brief 改变预览播放窗口位置
 *
 * @param [in] view   预览句柄, c2_start_preview返回值
 * @param [in] x  窗口宽度
 * @param [in] y  窗口高度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_start_preview,
 *
 * @note
 *   Linux下才起作用
 */
NET_API long c2_change_wndpos(HANDLE view, int x, int y);

/**
 * @brief 打开预览声音, 默认关闭
 *
 * @param [in] view   预览句柄, c2_start_preview返回值
 *
 * @see c2_start_preview, c2_colsesound
 */
NET_API int c2_opensound(HANDLE view);

/**
 * @brief 关闭预览声音
 *
 * @param [in] view   预览句柄, c2_start_preview返回值
 *
 * @see c2_start_preview, c2_opensound
 */
NET_API int c2_colsesound(HANDLE view);

/**
 * @brief 设置预览播放声音大小
 *
 * @param [in] view   预览句柄, c2_start_preview返回值
 * @param [in] volume 声音大小, 0~10
 *
 * @see c2_start_preview,
 */
NET_API void c2_set_volume(HANDLE view, int volume);

/**
 * @brief  实时预览抓图
 *
 * @param [in] view     c2_start_preview返回值
 * @param [in] filepath 抓拍图像保存路径
 * @param [in] type     抓拍类型0-BMP 1-JPEG
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @note
 */
NET_API long c2_capture(HANDLE view, const char *filepath, int type);

/**
 * @brief  开始实时录像
 *
 * @param [in] view     c2_start_preview返回值
 * @param [in] filepath 录像保存路径
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @note
 */
NET_API long c2_start_realREC(HANDLE view, const char *filepath);

/**
 * @brief  停止实时录像
 *
 * @param [in] view     c2_start_preview返回值
 *
 * @see c2_start_preview, c2_stop_realREC
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 */
NET_API void c2_stop_realREC(HANDLE view);

/**
* @brief 启动某台设备某个通道的后台录像
*
* @param [in] login      c2_login返回值
* @param [in] param      预览参数,参考c2_preview_s定义
* @param [in] filepath   录像文件保存路径
*
* @return 成功返回后台录像句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
*
* @see c2_stop_backREC, c2_login
*/
NET_API HANDLE c2_start_backREC(HANDLE login, const c2_preview_s *param, const char *filepath);


/**
* @brief  停止后台录像
*
* @param [in] view     c2_start_backREC返回值
*
* @see c2_start_backREC
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*/
NET_API void c2_stop_backREC(HANDLE login, HANDLE view);

/**
 * @brief  解码后数据回调函数定义
 *
 * @param [out] type     yuv类型 0-i420 1-yv12 2-rgb32
 * @param [out] w        图像宽度
 * @param [out] h        图像高度
 * @param [out] yuv      yuv数据缓冲区指针
 * @param [out] size     缓冲区大小
 * @param [out] userdata 用户数据
 */
typedef void (*c2_decode_cb)(int type, int w, int h, void *yuv, int size, void *userdata);

/**
 * @brief  绘图回调函数定义
 *
 * @param [out] hdc      当前绘图DC
 * @param [out] w        图像宽度
 * @param [out] h        图像高度
 * @param [out] userdata 用户数据
 */
typedef void (*c2_draw_cb)(HDC hdc, HWND wnd, int w, int h, void *userdata);

/**
 * @brief  设置绘图回调函数
 *
 * @param [in] view     c2_start_preview返回值
 * @param [in] drawCB   绘图回调函数入口
 * @param [in] userdata 用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @note
 */
NET_API long c2_set_drawcb(HANDLE view, c2_draw_cb drawCB, void *userdata);

/**
 * @brief  设置解码后数据回调函数定义
 *
 * @param [in] view     c2_start_preview返回值
 * @param [in] decodeCB 解码后数据回调函数入口
 * @param [in] userdata 用户数据
 *
 *
 * @note
 */
NET_API long c2_set_decodecb(HANDLE view, c2_decode_cb decodeCB, void *userdata);

typedef enum
{
    PTZ_CMD_NONE,
    PTZ_CMD_UP       = 1,
    PTZ_CMD_DOWN,
    PTZ_CMD_LEFT,
    PTZ_CMD_RIGHT,
    PTZ_CMD_UPLEFT   = 5,
    PTZ_CMD_UPRIGHT,
    PTZ_CMD_DOWNLEFT,
    PTZ_CMD_DOWNRIGHT,
    PTZ_CMD_ZOOMIN,
    PTZ_CMD_ZOOMOUT  = 10,
    PTZ_CMD_FOCUSNEAR,
    PTZ_CMD_FOCUSFAR,
    PTZ_CMD_IRISOPEN,
    PTZ_CMD_IRISCLOSE,
    PTZ_CMD_AUTOSCAN = 15,
    PTZ_CMD_SETPRESET,
    PTZ_CMD_CLEARPRESET,
    PTZ_CMD_CALLPRESET,
    PTZ_CMD_RAIN_ON,                       //雨刷
    PTZ_CMD_RAIN_OFF = 20,
    PTZ_CMD_AUXLIGHT_ON,                   //辅助灯光   
    PTZ_CMD_AUXLIGHT_OFF,
    PTZ_CMD_END,
} PTZ_CMD_E;

/**
 * @brief 云台控制，与预览通道是否打开无关
 *
 * @param [in] login  c2_login返回值
 * @param [in] cmd    云台控制命令,参考PTZ_CMD_E定义
 * @param [in] param  云台控制参数,参考c2_ptz_s定义
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_ptz_control(HANDLE login, int cmd, const c2_ptz_s *param);


/**
* @brief  预置位列表信息回调函数定义()
*
*/
typedef void(*c2_preset_cb)(c2_preset_s *preset, void *pUser);

/**
* @brief  获取设备通道预置位信息(新规约)
*
* @param [in] hConn  已经建立连接的句柄,c2_login返回值
* @param [in] pParam 获取设备通道信息，参考c2_query_preset_s
*
* @see
*/
NET_API long c2_get_chn_preset_v1(HANDLE login, const c2_query_preset_s *param, c2_preset_cb presetCB, void *userdata);

/**
* @brief  获取设备通道预置位信息(JSON)
*
* @param [in]  login  已经建立连接的句柄,c2_login返回值
* @param [in]  pParam 获取设备通道信息，参考c2_query_preset_s
* @param [out] json   预置位列表信息,空间由调用者分配
  @param [in, out] size 调用时传入分配空间的大小，调用完成后返回实际json的长度
* @see
*/
NET_API long c2_get_chn_preset_v2(HANDLE login, const c2_query_preset_s *param, char* json, int *size);


/**
* @brief  设置/删除设备通道预置位信息(新规约)
*
* @param [in] hConn  已经建立连接的句柄,c2_login返回值
* @param [in] pParam 获取设备通道信息，参考c2_query_preset_s
*
* @note 当c2_set_preset_s.c2_preset_s.name 为空时表示删除该预置位信息
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see
*/
NET_API long c2_set_chn_preset(HANDLE login, const c2_set_preset_s *param);

//NET_API long c2_set_chn_preset_v1(HANDLE login, const c2_set_preset_s *param);

/**
 * @brief  单条录像记录回调函数定义
 *
 * @param [out] record    单条录像记录结构
 * @param [out] realNum   总共实际记录数
 * @param [out] fromIndex 记录开始序号, 从1开始
 * @param [out] toIndex   记录结束序号
 * @param [out] userdata  用户数据
 *
 * @return void
 *
 */
typedef void (*c2_record_cb)(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata);

/**
 * @brief 录像查找, 回调方式
 *
 * @param [in] login    c2_login返回值
 * @param [in] param    设备列表查找参数,参考c2_query_record_s
 * @param [in] recordCB 单个录像记录信息回调
 * @param [in] userdata 用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_query_record_v1(HANDLE login, const c2_query_record_s *param, c2_record_cb recordCB, void *userdata);

/**
 * @brief 录像查找, josn方式
 *
 * @param [in]     login c2_login返回值
 * @param [in]     param 设备列表查找参数,参考c2_query_record_s
 * @param [in,out] json  设备列表信息,空间由调用者分配
 * @param [in,out] size  设备列表息长度
 *
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_query_record_v2(HANDLE login, const c2_query_record_s *param, char *json, int *size);

/**
 * @brief 按文件名远程回放
 *
 * @param [in] login      c2_login返回值
 * @param [in] param      预览参数,参考c2_pb_name_s定义
 * @param [in] streamCB   回放视频数据回调函数入口
 * @param [in] streamData 回放视频数据回调用户数据
 * @param [in] statusCB   状态回调
 * @param [in] statusData 状态回调用户数据
 *
 * @return 成功返回预览句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
 *
 * @see c2_playback_bytime, c2_stop_playback, c2_login
 */
NET_API HANDLE c2_playback_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                  c2_status_cb statusCB, void *statusData);

/**
* @brief 按文件名远程回放, 已删除
*
* @param [in] login      c2_login返回值
* @param [in] param      预览参数,参考c2_pb_name_s定义
* @param [in] streamCB   回放视频数据回调函数入口
* @param [in] streamData 回放视频数据回调用户数据
* @param [in] statusCB   状态回调
* @param [in] statusData 状态回调用户数据
*
* @return 成功返回预览句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
*
* @see c2_playback_bytime, c2_stop_playback, c2_login
*/
NET_API HANDLE c2_playbackreverse_byname(HANDLE login, const c2_pb_name_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
    c2_status_cb statusCB, void *statusData);
/**
 * @brief 按时间段远程回放
 *
 * @param [in] login      c2_login返回值
 * @param [in] param      时间回放参数,参考c2_pb_time_s定义
 * @param [in] streamCB   回放视频数据回调函数入口
 * @param [in] streamData 回放视频数据回调用户数据
 * @param [in] statusCB   状态回调
 * @param [in] statusData 状态回调用户数据
 *
 * @return 成功返回预览句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
 *
 * @see c2_playback_byname, c2_stop_playback, c2_login
 */
NET_API HANDLE c2_playback_bytime(HANDLE login, const c2_pb_time_s *param, HWND wnd, c2_stream_cb streamCB, void *streamData,
                                  c2_status_cb statusCB, void *statusData);

/**
* @brief 给回放额外添加一个播放窗口
*
* @param [in] view       预览句柄, c2_start_preview返回值
* @param [in] wnd        窗口句柄
*
* @return 成功返回0, 失败返回. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview, c2_del_preview_surface
*/
NET_API int c2_add_playback_surface(HANDLE playback, HWND wnd);


/**
* @brief 删除回放窗口
*
* @param [in] view       预览句柄, c2_start_preview返回值
* @param [in] wnd        窗口句柄
*
* @return 成功返回0, 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview, c2_add_preview_surface
*/
NET_API int c2_del_playback_surface(HANDLE playback, HWND wnd);


/**
* @brief  设置录像回放解码后数据回调函数定义
*
* @param [in] pb     c2_playback_byname或c2_playback_bytime返回值
* @param [in] decodeCB 解码后数据回调函数入口
* @param [in] userdata 用户数据
*
*
* @note
*/
NET_API long c2_set_pb_decodecb(HANDLE pb, c2_decode_cb decodeCB, void *userdata);


/**
 * @brief  停止某台设备某个通道的远程回放
 *
 * @param [in] login    登录句柄, c2_login返回值
 * @param [in] playback 远程回放句柄, c2_playback_byname, c2_playback_bytime返回值
 *
 * @see    c2_playback_byname, c2_playback_bytime, c2_login
 */
NET_API void c2_stop_playback(HANDLE login, HANDLE playback);

enum
{
    PLAY_NONE,
    PLAY_START,		                                       /**< 开始播放 */
    PLAY_STOP,		                                       /**< 停止播放 */
    PLAY_PAUSE,		                                       /**< 暂停播放 */
    PLAY_RESTART,	                                       /**< 恢复播放,在暂停后调用将恢复暂停前的速度播放 */
    PLAY_FAST,	                                           /**< 快放 */
    PLAY_SLOW,		                                       /**< 慢放 */
    PLAY_NORMAL,		                                   /**< 正常速度 */
    PLAY_STEP,                                             /**< 单帧播放 */
    PLAY_OPENAUDIO,	                                       /**< 打开声音 */
    PLAY_CLOSEAUDIO,	                                   /**< 关闭声音 */
    PLAY_STARTREPOS,                                       /**< 进度条定位开始, (按下进度条调用) */
    PLAY_SETPOS	,	                                       /**< 按进度条位置定位(0~100), (拖动后松开进度条调用) */
    PLAY_SETTIME,                                          /**< 按绝对时间定位, (拖动后松开进度条调用) */
    PLAY_REVERSE,                                          /**< 倒放 */
    PLAY_FORWARD,                                          /**< 正放 */ 
    PLAY_SETLABEL,                                         /**< 打标签 */
    PLAY_SETVOLUME,                                        /**< 设置音量 */
};

/**
 * @brief 远程回放控制
 *
 * @param [in] playback 远程回放句柄, c2_playback_byname, c2_playback_bytime返回值
 * @param [in] code     回放控制命令码
 * @param [in, out] val 回放控制参数
 * @param [in] size     回放控制参数大小
 *
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_playback_control(HANDLE playback, int code, void *val, int valSize);

/**
* @brief 远程回放控制V1
*
* @param [in] playback 远程回放句柄, c2_playback_byname, c2_playback_bytime返回值
* @param [in] code     回放控制命令码
* @param [in] value    回放进度:1~99,当code为设置进度时有效
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_login
*/
//NET_API long c2_playback_controlV1(HANDLE playback, int code, int value);
NET_API long c2_playback_controlV1(HANDLE playback, int code, long value);


/**
* @brief  回放抓图
*
* @param [in] view     c2_playback_bytime或c2_playback_byname返回值
* @param [in] filepath 抓拍图像保存路径
* @param [in] type     抓拍类型0-BMP 1-JPEG
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @note
*/
NET_API long c2_playback_capture(HANDLE view, const char *filepath, int type);

/**
 * @brief 获取当前回放播放绝对时间
 *
 * @param [in] playback 远程回放句柄, c2_playback_byname, c2_playback_bytime返回值
 * @param [out] time    播放绝对时间
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_playback_byname, c2_playback_bytime, c2_get_playbackpos
 */
NET_API long c2_get_playbacktime(HANDLE playback, datetime_s *time);

/**
 * @brief  返回当前播放进度位置
 *
 * @param [in] playback 远程回放句柄, c2_playback_byname, c2_playback_bytime返回值
 * @param [in] start    回放开始时间, 用来精确计算播放进度
 * @param [in] end      回放结束时间, 用来精确计算播放进度
 *
 * @return 成功返回当前播放位置(0~100), 否则返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_playback_byname, c2_playback_bytime, c2_playback_control
 *
 * @note
 *  建议调用者采用c2_get_playbacktime自己计算播放进度
 */
NET_API long c2_get_playbackpos(HANDLE playback, datetime_s *start = 0, datetime_s *end = 0);


/**
* @brief 改变回放窗口大小
*
* @param [in] playback   回放句柄, c2_playback_bytime或c2_playback_byname返回值
* @param [in] width  窗口宽度
* @param [in] height 窗口高度
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview,
*
* @note
*   Linux下才起作用
*/
NET_API long c2_playback_change_wndsize(HANDLE playback, int width, int height);

/**
* @brief 改变回放窗口位置
*
* @param [in] playback   回放句柄, c2_playback_bytime或c2_playback_byname返回值
* @param [in] x  窗口x坐标
* @param [in] y  窗口y坐标
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_start_preview,
*
* @note
*   Linux下才起作用
*/
NET_API long c2_playback_change_wndpos(HANDLE playback, int x, int y);

/**
* @brief 打开回放声音, 默认关闭
*
* @param [in] view   回放句柄, c2_playback_byname 或 c2_playback_bytime返回值
*
* @see c2_pb_colsesound
*/
NET_API int c2_pb_opensound(HANDLE view);

/**
* @brief 关闭回放声音
*
* @param [in] view   预览句柄, c2_playback_byname 或 c2_playback_bytime返回值
*
* @see c2_pb_opensound
*/
NET_API int c2_pb_colsesound(HANDLE view);


/**
* @brief 设置音量
*
* @param [in] view   预览句柄, c2_playback_byname 或 c2_playback_bytime返回值
* @param [in] vol    音量值(0 - 10)
* @see c2_pb_opensound
*/
NET_API int c2_pb_setvolume(HANDLE view, int vol);

/**
 * @brief 远程录像下载(按文件名方式)
 *
 * @param [in] login      c2_login返回值
 * @param [in] param      下载参数,参考c2_download_s定义
 * @param [in] streamCB   下载码流数据回调函数入口
 * @param [in] streamData 下载码流数据回调用户数据
 * @param [in] statusCB   状态回调
 * @param [in] statusData 状态回调用户数据
 *
 * @return 成功返回下载句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
 *
 * @see c2_stop_download, c2_login
 */
NET_API HANDLE c2_start_download(HANDLE login, const c2_download_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

/**
* @brief 远程录像下载(按时间方式下载)
*
* @param [in] login      c2_login返回值
* @param [in] param      下载参数,参考c2_download_s定义
* @param [in] streamCB   下载码流数据回调函数入口
* @param [in] streamData 下载码流数据回调用户数据
* @param [in] statusCB   状态回调
* @param [in] statusData 状态回调用户数据
*
* @return 成功返回下载句柄, 失败返回NULL. 获取错误信息调用c2_get_error.
*
* @see c2_stop_download, c2_login
*/
NET_API HANDLE c2_start_downloadByTime(HANDLE login, const c2_download_time_s *param, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData);

/**
 * @brief  停止某台设备某个通道的远程下载
 *
 * @param [in] login    登录句柄, c2_login返回值
 * @param [in] download 远程下载句柄, c2_start_download返回值
 *
 * @see    c2_start_download, c2_login
 */
NET_API void c2_stop_download(HANDLE login, HANDLE download);

/**
 * @brief  返回当前播放进度位置
 *
 * @param [in] playback 远程回放句柄, c2_start_download返回值
 *
 * @return 成功返回当前播放位置(0~100), 否则返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_start_download
 */
NET_API long c2_get_downloadpos(HANDLE playback);

/**
 * @brief 时间同步
 *
 * @param [in] login  c2_login返回值
 * @param [in] synct  时间同步参数,参考c2_synct_s定义
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @see c2_login
 */
NET_API long c2_sync_time(HANDLE login, const c2_synct_s *synct);


/**
* @brief 初始化设备对讲
*
* @param [in] login  c2_login返回值
*
* @return 成功返回对讲句柄. 开始对讲和发送数据都会用到该句柄， 失败返回NULL. 获取错误信息调用c2_get_error.
*
* @see c2_uninit_dualtalk
*/
NET_API HANDLE c2_init_dualtalk(int flag);

/**
* @brief 去初始化设备对讲，结束对讲需调用该接口释放资源，断开连接
*
* @param [in] login  c2_login返回值
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_init_dualtalk
*/
NET_API int c2_uninit_dualtalk(HANDLE dutk);


/**
*@brief 对讲状态码
*/
enum
{
    CODE_TALK_SUCCESS = 0,       /**< 对讲成功 */
    CODE_TALK_FAILED = 1,        /**< 连接服务器失败 */
    CODE_TALK_BUSY = 2           /**< 设备占线 */
};


/**
* @brief 开始设备对讲
*
* @param [in] dutk   c2_init_dualtalk返回值
* @param [in] param  对讲设备参数，目前只支持单个设备对讲
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_stop_dev_dualtalk
*/
NET_API HANDLE c2_start_dev_dualtalk(HANDLE dutk, HANDLE view, c2_status_cb statusCB, void* userdata);

/**
* @brief 停止与某个设备对讲
*
* @param [in] login  c2_init_dualtalk返回值
* @param [in] param  对讲设备参数，目前只支持单个设备对讲
*
* @return 成功返回非NULL. 失败返回NULL. 获取错误信息调用c2_get_error.
*
* @see c2_login
*/
NET_API int c2_stop_dev_dualtalk(HANDLE dutk, HANDLE view);

/**
* @brief 发送音频数据
*
* @param [in] dutk       c2_init_dualtalk返回值
* @param [in] data       裸音频数据
* @param [in] size       音频数据大小
* @param [in] encode     音频数据编码， 4：G711A, 5：G711U, 6：G722,
* @param [in] sps        采样率 一般为8000
* @param [in] channels   声道数，一般为1
* @param [in] bps        码率
* @param [in] pts        音频时间戳
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_send_video
*/
NET_API int c2_send_audio(HANDLE dutk, void* data, int size, int encode, int sps, int channels, int bps, uint64_t pts);

/**
* @brief 发送视频数据，音频对讲忽略该接口
*
* @param [in] dutk       c2_init_dualtalk返回值
* @param [in] data       裸视频数据
* @param [in] size       视频数据大小
* @param [in] w          视频宽度
* @param [in] h          视频高度
* @param [in] encode     视频频数据编码，1：H264, 2：HEVC， 3：JPEG
* @param [in] fps        帧率
* @param [in] pts        视频时间戳
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @see c2_send_audio
*/
NET_API int c2_send_video(HANDLE dutk, void* data, int size, int w, int h, int type, int encode, int fps, uint64_t pts);



/**
* @brief  矩阵切换
*
* @param [in] login  c2_login返回值
* @param [in] pparam 请求矩阵切换相关参数配置，参考NET_MATRIX_PARAM_S
*
* @return 成功返回0, 否则返回非0, 获取错误码调用c2_get_error
*
* @see    
*/
NET_API int c2_matrix_switch(HANDLE login, const c2_matrix_s * pParam);

/**
* @brief  解码器显示切换
*
* @param [in] hConn  已经建立连接的句柄,c2_login返回值
* @param [in] pparam 请求矩阵切换相关参数配置，参考NET_DECODER_PARAM_S
*
* @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
*
* @see    NET_ConnStart
*/
NET_API int c2_decoder_switch(HANDLE login, const c2_decoder_param_s * param);

/**
* @brief 下发命令到流媒体(json格式)
* @param [in] login 已经建立连接的句柄,c2_login返回值
* @param [in] json 数据内容
* @param [in] size 数据大小
* @return 返回值，成功0，失败非0

* @demo
* 解码器设置json字符串
{"DevIp":"192.168.50.76","DevPort":8000,"DisPort":2,"Item":[{"Chn":33,"Ip":"192.168.60.235","Port":8000}],"ItemCount":1,"Method":"WindowDecoder"}
讯维矩阵json字符串
{"DevIp":"192.168.50.76","DevPort":8000,"OutPort":2,,"InPort":1,"Method":"SwitchPort"}
* 智能分析
{
    "DevIp":"192.168.50.76",
    "DevPort":8000,
    "devIp":"192.168.80.235",
    "chnNo":33,
    "presetNo":1,
    "Method":"startIva"
}

 {"DevIp":"192.168.50.76","DevPort":8000,"devIp":2,,"chnNo":1, "presetNo":1, "Method":"stopIva"}

* 告警抑制
{"DevIp":"192.168.50.76","DevPort":8000,"devIp":2,,"chnNo":1, "presetNo":1, "prohibitTime", 10(min), "Method":"prohibitAlarm"}

*/
NET_API int  c2_send_command(HANDLE login, char * json, int size);


/**
* @brief 从流媒体获取配置信息(json格式)
* @param [in] login 已经建立连接的句柄,c2_login返回值
* @param [in] _in_json 数据内容
* @param [in] _in_size 数据大小
* @param [out] _out_json 数据内容
* @param [out] _out_size 数据大小
* @return 返回值，成功0，失败非0
*
* @demo
* 获取视频遮挡参数
{"DevIp":"192.168.50.76","DevPort":8000,"DisPort":2,"Item":[{"Chn":33,"Ip":"192.168.60.235","Port":8000}],"ItemCount":1,"Method":"WindowDecoder"}
* 智能分析总招
{"DevIp":"192.168.50.76","DevPort":8000,"Method":"callAll"}
*/

NET_API int  c2_get_config(HANDLE login, char * _in_json, int _in_size, char* _out_json, int *_out_size);

/**
* @brief 上传人脸照片,修改或删除
* @param [in] login 已经建立连接的句柄,c2_login返回值
* @param [in] param 人脸图片信息
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_set_face(HANDLE login, const c2_set_face_s * param);

/**
* @brief 锁定/解锁录像文件
* @param [in] login 已经建立连接的句柄,c2_login返回值
* @param [in] param 文件信息
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_lock_recordfile(HANDLE login, bool bLock, const c2_pb_name_s *param);

/**
* @brief 云台花样扫描
* @param [in] login 已经建立连接的句柄,c2_login返回值
* @param [in] param 花样扫描结构
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_ptz_pattern(HANDLE login, const c2_ptz_pattern_s *param);

/**
* @brief 电子放大区域（播放库~I
* @param [in] view 预览句柄, c2_start_preview返回|<
* @param [in] param 电子放大结构
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_set_displayregion(HANDLE view, const c2_display_region_s *param);

/**
* @brief 设置系统状态回调
* @param [in] login 当login为NULL时表示设置本机系统状态回调,当login为c2_login返回值时表示设置流媒体服务系统状态回调函数
* @param [in] statucb, userdata 状态回调函数
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_set_server_status_cb(HANDLE login, c2_alarm_cb statucb, void* userdata);

#ifdef UNIX_DRAW_RECT
/**
* @brief 设置sdk参数
* @param [in] json 
    eg. {"roiShow":0}、{"rectShowType":0}、{"roiShow":0,"rectShowType":0}
    roiShow 0：显示智能分析区域框，1：不显示
    rectShowType 0：显示外接矩形，1：显示原始矩形，2：都显示
* @return 返回值，成功0，失败非0, 获取错误码调用c2_get_error
*/
NET_API int  c2_set_sdk_param(const char* json);
#endif


#ifdef __cplusplus
}
#endif

#endif
