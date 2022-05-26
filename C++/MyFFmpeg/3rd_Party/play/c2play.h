/**
 * @file      c2play.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-07 11:11:16
 *
 * @note
 *  cross platform play sdk interface defines.
 */
#ifndef __C2PLAY_H__
#define __C2PLAY_H__

#ifndef DLL_EXPORT
#include "def.h"
//#include "type.h"
#endif

#ifdef PLAY_STATICLIB
#  define PLAY_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(PLAY_EXPORTS)
#    define PLAY_API  __declspec(dllexport)
#  else
#    define PLAY_API  __declspec(dllimport)
#  endif
#else
#  define   PLAY_API
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
PLAY_API long PLAY_set_error(const char* fmt, ...);

/**
 * @brief 获取错误
 *
 * @param [in]
 *
 * @return 错误字符串
 *
 * @see c2_set_error
 */

PLAY_API const char *PLAY_get_error();

typedef enum
{
    STREAM_REAL,                                           /**< 实时混合流 */
    STREAM_FILE_NET,                                       /**< 网络文件混合流 */
    STREAM_FILE_LOCAL,                                     /**< 本地文件混合流 */
    STREAM_AUDIO,                                          /**< 纯音频流 */
} STREAM_TYPE_E;

/**
 * @brief 创建播放实例
 *
 * @param [in] type 参考STREAM_TYPE_E 定义
 * @param [in] mode 0-按pts播放 1-按fps帧率播放
 *
 * @return 成功返回非NULL播放实例句柄, 失败返回NULL. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_destroy
 */
PLAY_API HANDLE PLAY_create(int type, int mode);

/**
 * @brief 销毁播放实例
 *
 * @param [in] hp 播放实例句柄
 *
 * @see PLAY_create
 */
PLAY_API void PLAY_destroy(HANDLE hp);

/**
 * @brief 打开本地录像文件,开始播放
 *
 * @param [in] hp       播放实例句柄
 * @param [in] filename 录像文件名
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_closefile
 */
PLAY_API long PLAY_openfile(HANDLE hp, const char *filename);

/**
 * @brief 关闭本地录像文件,停止播放
 *
 * @param [in] hp 播放实例句柄
 *
 * @see PLAY_create, PLAY_openfile
 */
PLAY_API void PLAY_closefile(HANDLE hp);

/**
 * 渲染类型定义
 */
typedef enum
{
    C2_RENDER_DDRAW,
    C2_RENDER_D3D,
    C2_RENDER_SDL,
    C2_RENDER_OPENGL,
    C2_RENDER_GLES2,
} c2_tag_render_e;

/**
* @brief  获取文件的开始/结束时间, 仅限播放本地文件(STREAM_FILE_LOCAL)使用
*
* @param [in] hp            播放实例句柄
* @param [out] start_time   文件开始时间
* @param [out] end_time     文件结束时间
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
*
*/
PLAY_API long PLAY_getFileTime(HANDLE hp, datetime_s* start_time, datetime_s* end_time);

/**
 * @brief  添加播放窗口
 *
 * @param [in] hp    播放实例句柄
 * @param [in] wnd   播放窗口
 * @param [in] rtype 渲染类型, 参考RENDER_DDRAW等
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_addwnd(HANDLE hp, HWND wnd, int rtype);


/**
 * @brief  删除播放窗口
 *
 * @param [in] hp    播放实例句柄
 * @param [in] wnd   播放窗口
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_addwnd
 */
PLAY_API long PLAY_delwnd(HANDLE hp, HWND wnd);


/**
 * @brief  输入混合流数据
 *
 * @param [in] hp   播放实例句柄
 * @param [in] data 混合流数据,包括私有头, 参考frame.h/c2_frame_s定义
 * @param [in] size 混合流数据长度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, c2_frame_s
 */
PLAY_API long PLAY_inputdata(HANDLE hp, void *data, int size, bool block);

/**
 * @brief 状态回调
 *
 * @param [out] code     状态码 参考CODE_STATUS_E定义
 * @param [out] user_data 用户数据
 *
 * @return void
 *
 * @note
 *  在回调函数内部调用PLAY_get_error获取具体错误信息
 */
typedef void (*c2_status_cb)(int code, void* user_data);

/**
 * @brief  开始播放
 *
 * @param [in] hp        播放实例句柄
 * @param [in] status_cb 状态回调
 * @param [in] user_data 用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_stop
 */
PLAY_API long PLAY_start(HANDLE hp, c2_status_cb status_cb, void *user_data);

/**
 * @brief  停止播放
 *
 * @param [in] hp  播放实例句柄
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_start
 */
PLAY_API long PLAY_stop(HANDLE hp);


/**
* @brief  打开声音
*
* @param [in] hp  播放实例句柄
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
*
* @see PLAY_create, PLAY_start
*/
PLAY_API long PLAY_openSound(HANDLE hp);

/**
* @brief  关闭声音
*
* @param [in] hp  播放实例句柄
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
*
* @see PLAY_create, PLAY_start
*/
PLAY_API long PLAY_closeSound(HANDLE hp);

/**
* @brief  设置音量
*
* @param [in] hp    播放实例句柄
* @param [in] vol   音量值 (0-100)
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
*
* @see PLAY_create, PLAY_start
*/
PLAY_API long PLAY_setVolume(HANDLE hp, int vol);



/**
 * @brief  暂停/继续播放
 *
 * @param [in] hp    播放实例句柄
 * @param [in] pause true: 暂停 false: 继续
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_start
 */
PLAY_API long PLAY_pause(HANDLE hp, bool pause);

/**
 * @brief 快进
 *
 * @param [in]  hp    播放实例句柄
 * @param [out] speed 当前播放速度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_slow, PLAY_pause
 */
PLAY_API long PLAY_fast(HANDLE hp, int *speed = NULL);

/**
 * @brief 慢进
 *
 * @param [in]  hp    播放实例句柄
 * @param [out] speed 当前播放速度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create, PLAY_fast, PLAY_pause
 */
PLAY_API long PLAY_slow(HANDLE hp, int *speed = NULL);

/**
 * @brief 抓拍图片
 *
 * @param [in] hp   播放实例句柄
 * @param [in] path 图像保存路径
 * @param [in] type 0:BMP 1:JPEG
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_capture(HANDLE hp, const char *path, int type);

/**
 * @brief 抓拍图像缓存
 *
 * @param [in]     hp   播放实例句柄
 * @param [in]     pic  图像缓存,由用户分配
 * @param [in,out] size 图像缓存长度，图像实际长度
 * @param [out]    w    图像宽度
 * @param [out]    h    图像高度
 * @param [in]     type 0:RGB24 1:YUV420 2:BMP 3:JPEG
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_capture_buf(HANDLE hp, char *pic, int *size, int *w, int *h, int type);

/**
 * @brief 获取当前播放剩余帧数
 *
 * @param [in] hp 播放实例句柄
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_remain_buffer(HANDLE hp);

/**
 * @brief 重置当前播放缓冲
 *
 * @param [in] hp 播放实例句柄
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_reset_buffer(HANDLE hp);

/**
 * @brief 获取当前播放绝对时间
 *
 * @param [in]  hp   播放实例句柄
 * @param [out] time 当前播放时间
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_current_time(HANDLE hp, datetime_s *time);

/**
 * @brief 缓冲区最大/最小阀值触发回调
 *
 * @param [out] type     0:min threshold, 1:max threshold
 * @param [out] fcnt     最大/最小阀值帧数
 * @param [out] userdata 用户数据
 *
 * @return void
 *
 * @note
 *  在回调函数内部调用c2_get_error获取具体错误信息
 */
typedef void (*c2_threshold_cb)(int type, int fcnt, void* userdata);

/**
 * @brief 设置缓冲区最大/最小阀值
 *
 * @param [in] hp           播放实例句柄
 * @param [in] type         0:min threshold, 1:max threshold
 * @param [in] fcnt         最大/最小阀值帧数
 * @param [in] threshold_cb 阀值触发回调函数
 * @param [in] userdata     用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 */
PLAY_API long PLAY_set_threshold(HANDLE hp, int type, int fcnt, c2_threshold_cb threshold_cb, void *userdata);

/**
 * @brief 改变播放窗口大小
 *
 * @param [in] hp     播放实例句柄,
 * @param [in] wnd    窗口句柄
 * @param [in] width  窗口宽度
 * @param [in] height 窗口高k度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 *  note: Linux下有效
 */
PLAY_API long PLAY_change_wndsize(HANDLE hp, HWND wnd, int width, int height);

/**
 * @brief 改变播放窗口位置
 *
 * @param [in] hp     播放实例句柄,
 * @param [in] wnd    窗口句柄
 * @param [in] width  窗口x
 * @param [in] height 窗口y
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_create
 *  note: Linux下有敄1??7 */
PLAY_API long PLAY_change_position(HANDLE hp, HWND wnd, int left, int top);


/**
 * @brief 设置当前播放位置
 *
 * @param [in] hp  播放实例句柄
 * @param [in] pos 播放位置 0~100
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see PLAY_openfile, PLAY_getpos
 */
PLAY_API long PLAY_setpos(HANDLE hp, int pos);

/**
 * @brief 获取当前播放进度
 *
 * @param [in] hp 播放实例句柄
 *
 * @return 成功返回当前播放进度,范围0~100. 失败返回-1. 获取错误信息调用PLAY_get_error.
 *
 * @see PLAY_openfile, PLAY_setpos
 */
PLAY_API long PLAY_getpos(HANDLE hp);

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
 * @param [out] wnd      画面渲染窗口
 * @param [out] w        图像宽度
 * @param [out] h        图像高度
 * @param [out] userdata 用户数据
 */
typedef void (*c2_draw_cb)(HDC hdc, HWND wnd, int w, int h, void *userdata);

/**
 * @brief  设置绘图回调函数
 *
 * @param [in] hp       播放实例句柄
 * @param [in] wnd      画面渲染窗口
 * @param [in] draw_cb  绘图回调函数入口
 * @param [in] userdata 用户数据
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
 *
 * @note
 */
PLAY_API long PLAY_set_drawcb(HANDLE hp, HWND wnd, c2_draw_cb draw_cb, void *userdata);

/**
 * @brief  设置解码后数据回调函数定义
 *
 * @param [in] hp        播放实例句柄
 * @param [in] decode_cb 解码后数据回调函数入口
 * @param [in] userdata  用户数据
 *
 * @note
 */
PLAY_API long PLAY_set_decodecb(HANDLE hp, c2_decode_cb decode_cb, void *userdata);

#ifdef __cplusplus
}
#endif

#endif
