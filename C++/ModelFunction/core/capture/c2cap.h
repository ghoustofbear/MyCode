/**
 * @file      c2cap.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-13 08:46:01
 *
 * @note
 *  c2cap.h defines
 */
#ifndef __C2CAP_H__
#define __C2CAP_H__

#ifndef DLL_EXPORT
#include "def.h"
#endif

#ifdef CAP_STATICLIB
#  define CAP_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(CAP_EXPORTS)
#    define CAP_API  __declspec(dllexport)
#  else
#    define CAP_API  __declspec(dllimport)
#  endif
#else
#  define   CAP_API
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


#endif

#ifdef __cplusplus
extern "C" {
#endif

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
CAP_API long CAP_set_error(const char* fmt, ...);

/**
 * @brief 获取错误
 *
 * @param [in]
 *
 * @return 错误字符串
 *
 * @see c2_set_error
 */

CAP_API const char *CAP_get_error();

/**
 * @brief 创建播放实例
 *
 * @param [in] type 参考STREAM_TYPE_E 定义
 * @param [in] mode 0-按pts播放 1-按fps帧率播放
 *
 * @return 成功返回非NULL播放实例句柄, 失败返回NULL. 获取错误信息调用CAP_get_error.
 *
 * @see CAP_destroy
 */
//CAP_API HANDLE CAP_create(int type, int mode);

/**
 * @brief 销毁播放实例
 *
 * @param [in] hp 播放实例句柄
 *
 * @see CAP_create
 */
//CAP_API void CAP_destroy(HANDLE hp);

/**
 * 音视频采集与编码相关
 */

typedef struct
{
    int            type;                                   /**< 0-ddshow, 1-vfw, 2-video4linux2, 3-avfoundation, 4-gdigrab, 5-wave */
    int            index;                                  /**< 设备序号*/
    char           name[128];
} cap_device_s;

typedef struct
{
    int            type;                                   /**< 0-h264 1-h265 */
    int            width;
    int            height;
    int            gop;
    int            fps;
    int            bps;
    int            mode;                                   /**< 0-baseline, 1-main */
} cap_video_s;

/**
 * @brief 编码视频数据回调
 *
 * @param [out] type     0-RGB24数据 1-YUV420数据 2-H.264数据 3-H.265数据
 * @param [out] data     数据缓冲区
 * @param [out] size     数据长度
 * @param [out] pts      时间戳
 * @param [out] fps      实际帧率
 * @param [out] userdata 用户数据
 *
 * @return 成功返回0, 失败-1.
 */
typedef void (*cap_video_cb)(int type, void *data, int size, uint64_t pts, int fps, void *userdata);

/**
 * @brief 设备枚举查找
 *
 * @param [in] count   设备个数
 * @param [in] devices 枚举设备数组，空间由用户创建
 *
 * @note
 */
CAP_API long CAP_enum_device(int *count, cap_device_s *devices);

/**
 * @brief 打开摄像头并开始采集数据
 *
 * @param [in] device   枚举到的设备cap_video_s
 * @param [in] hwnd     本地显示窗口
 * @param [in] mode     数据回调类型
 *                       bit0-RGB
 *                       bit1-YUV420
 *                       bit2-H.264帧数据
 *                       bit3-H.265帧数据
 * @param [in] encode   编码参数
 * @param [in] userdata 用户数据
 *
 * @return 成功返回非NULL采集实例句柄, 失败返回NULL. 获取错误信息调用CAP_get_error.
 */
CAP_API HANDLE CAP_video_start(cap_device_s *device, HWND wnd, cap_video_s *encode, uint32_t mode, cap_video_cb video_cb, void *userdata);

/**
 * @brief 关闭摄像头停止抓取数据
 *
 * @param [in] device   枚举到的设备cap_video_s
 *
 * @return 成功返回0, 失败-1.
 */
CAP_API long CAP_video_stop(HANDLE cap);


typedef struct
{
    int            type;                                   /**< 0-pcm 1-g711a 2-g711u 3-g722 4-aac */
    int            sps;                                    /**< samples in per second */
    int            channels;                               /**< samples channels */
    int            bps;                                    /**< bits in per sample */
    int            ssize;                                  /**< size in per sample */
} cap_audio_s;

/**
 * @brief 编码音频数据回调
 *
 * @param [out] type     0-pcm 1-g711a 2-g711u 3-g722 4-aac
 * @param [out] data     数据缓冲区
 * @param [out] size     数据长度
 * @param [out] pts      时间戳
 * @param [out] userdata 用户数据
 *
 * @return 成功返回0, 失败-1.
 */
typedef void (*cap_audio_cb)(int type, void *data, int size, uint64_t pts, void *userdata);

/**
 * @brief 打开麦克风并开始抓取数据
 *
 * @param [in] device   枚举到的设备cap_audio_s
 * @param [in] mode     数据回调类型
 *                       bit0-PCM
 *                       bit1-g711A
 *                       bit2-g711u
 *                       bit3-g722
 *                       bit4-aac
 * @param [in] encode   编码参数
 * @param [in] userdata 用户数据
 *
 * @return 成功返回非NULL采集实例句柄, 失败返回NULL. 获取错误信息调用CAP_get_error.
 */
CAP_API HANDLE CAP_start_audio(cap_device_s *device, cap_audio_s *encode, uint32_t mode, cap_audio_cb video_cb, void *userdata);

/**
 * @brief 关闭麦克风停止抓取数据
 *
 * @param [in] device   枚举到的设备cap_video_s
 *
 * @return 成功返回0, 失败-1.
 */
CAP_API long CAP_stop_audio(HANDLE cap);

#ifdef __cplusplus
}
#endif

#endif
