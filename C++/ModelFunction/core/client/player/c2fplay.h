/**
 * @file      c2fplay.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-02-05 15:17:38
 *   
 * @note
 *  c2fplay.h defines 
 */
#ifndef __C2FPLAY_H__
#define __C2FPLAY_H__

#ifdef FPLAY_STATICLIB
#  define FPLY_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(FPLY_EXPORTS)
#    define FPLY_API  __declspec(dllexport)
#  else
#    define FPLY_API  __declspec(dllimport)
#  endif
#else
#  define   FPLY_API
#endif

//#if defined(DLL_EXPORT)
//#include <stdint.h>
//
//#ifdef _WIN32
//#include <windows.h>
//#else
//typedef void*                HANDLE;
//typedef unsigned long        HWND;
//typedef void*                HDC;
//#endif
//
//#endif

#include "playerlistener.h"

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
FPLY_API long FPLY_set_error(const char* fmt, ...);

/**
 * @brief 获取错误
 *
 * @param [in]
 *
 * @return 错误字符串
 *
 * @see c2_set_error
 */
FPLY_API const char *FPLY_get_error();

/**
 * 渲染类型定义
 */
typedef enum
{
    C2F_RENDER_DDRAW,
    C2F_RENDER_D3D,
    C2F_RENDER_SDL,
    C2F_RENDER_OPENGL
} tag_render_e;

/**
 * @brief 打开本地录像文件,开始播放
 *
 * @param [in] wnd      播放窗口
 * @param [in] filename 录像文件名
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_closefile
 */
FPLY_API HANDLE FPLY_openfile(HWND wnd, const char *filename);

/**
 * @brief 关闭本地录像文件,停止播放
 *
 * @param [in] fp 播放实例句柄
 *
 * @see FPLY_openfile, FPLY_openfile
 */
FPLY_API void FPLY_closefile(HANDLE fp);

/**
* @brief 设置码流监听对象
*
* @param [in] fp 播放实例句柄
* 
* @param [in] listener 码流监听对象
*/
FPLY_API void FPLY_setPlayerListener(HANDLE fp, PlayerListener* listener);


/**
 * @brief  添加播放窗口
 *
 * @param [in] fp    播放实例句柄
 * @param [in] wnd   播放窗口
 * @param [in] rtype 渲染类型, 参考RENDER_DDRAW等
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile
 */
FPLY_API long FPLY_addwnd(HANDLE fp, HWND wnd, int rtype);

/**
 * @brief  删除播放窗口
 *
 * @param [in] fp    播放实例句柄
 * @param [in] wnd   播放窗口
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_addwnd
 */
FPLY_API long FPLY_delwnd(HANDLE fp, HWND wnd);

/**
 * @brief  暂停/继续播放
 *
 * @param [in] fp    播放实例句柄
 * @param [in] pause true: 暂停 false: 继续
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_start
 */
FPLY_API long FPLY_pause(HANDLE fp, bool pause);

/**
 * @brief 快进
 *
 * @param [in]  fp    播放实例句柄
 * @param [out] speed 当前播放速度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_slow, FPLY_pause
 */
FPLY_API long FPLY_fast(HANDLE fp, int *speed = nullptr);

/**
 * @brief 慢进
 *
 * @param [in]  fp    播放实例句柄
 * @param [out] speed 当前播放速度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_fast, FPLY_pause
 */
FPLY_API long FPLY_slow(HANDLE fp, int *speed = nullptr);

/**
 * @brief 恢复正常速度播放 
 *
 * @param [in]  fp    播放实例句柄
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_fast, FPLY_pause, FPLY_slow
 */
FPLY_API long FPLY_normal(HANDLE fp);

/**
* @brief 打开声音
*
* @param [in]  fp    播放实例句柄
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
*
* @see PLAY_create, PLAY_start
*/
FPLY_API long FPLY_openAudio(HANDLE fp);

/**
 * @brief 关闭声音 
 *
 * @param [in]  fp    播放实例句柄
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see PLAY_create, PLAY_start
 */
FPLY_API long FPLY_closeAudio(HANDLE fp);

/**
* @brief  设置音量
*
* @param [in] fp    播放实例句柄
* @param [in] vol   音量值 (0-100)
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用PLAY_get_error.
*
* @see PLAY_create, PLAY_start
*/
FPLY_API long FPLY_setVolume(HANDLE hp, int vol);

/**
 * @brief 抓拍图片
 *
 * @param [in] fp   播放实例句柄
 * @param [in] path 图像保存路径
 * @param [in] type 0:BMP 1:JPEG
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile
 */
FPLY_API long FPLY_capture(HANDLE fp, const char *path, int type);

/**
 * @brief 抓拍图像缓存
 *
 * @param [in]     fp   播放实例句柄
 * @param [in]     pic  图像缓存,由用户分配
 * @param [in,out] size 图像缓存长度，图像实际长度
 * @param [out]    w    图像宽度
 * @param [out]    h    图像高度
 * @param [in]     type 0:RGB24 1:YUV420 2:BMP 3:JPEG
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile
 */
FPLY_API long FPLY_capture_buf(HANDLE fp, char *pic, int *size, int *w, int *h, int type);

/**
 * @brief 设置当前播放位置
 *
 * @param [in] fp  播放实例句柄
 * @param [in] pos 播放位置 0~100
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_getpos
 */
FPLY_API long FPLY_setpos(HANDLE fp, int pos);

/**
 * @brief 获取当前播放进度
 *
 * @param [in] fp 播放实例句柄
 *
 * @return 成功返回当前播放进度,范围0~100. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile, FPLY_setpos
 */
FPLY_API long FPLY_getpos(HANDLE fp);

/**
 * @brief 改变播放窗口大小
 *
 * @param [in] fp     播放实例句柄,
 * @param [in] wnd    窗口句柄
 * @param [in] width  窗口宽度
 * @param [in] height 窗口高k度
 *
 * @return 成功返回0. 失败返回-1. 获取错误信息调用FPLY_get_error.
 *
 * @see FPLY_openfile
 *  note: Linux下有效
 */
FPLY_API long FPLY_change_wndsize(HANDLE fp, HWND wnd, int width, int height);


FPLY_API long FPLY_change_wndpos(HANDLE fp, HWND wnd, int width, int height);

/**
* @brief  绘图回调使能
*
* @param [in] fp       FPLY_openfile返回值
* @param [in] enable   是否启用绘图回调
*
* @return 成功返回0. 失败返回-1. 获取错误信息调用c2_get_error.
*
* @note
*/
FPLY_API long FPLY_enable_drawcb(HANDLE fp, bool enable);

/**
* @brief  解码回调使能
*
* @param [in] fp       FPLY_openfile返回值
* @param [in] enable  是否启用解码回调
*
*
* @note
*/
FPLY_API long FPLY_enable_decodecb(HANDLE fp, bool enable);

#ifdef __cplusplus
}
#endif

#endif
