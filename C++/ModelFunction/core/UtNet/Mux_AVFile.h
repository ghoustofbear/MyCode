#ifndef __MUX_AVFILE_H__
#define __MUX_AVFILE_H__

#define MUXAVF_EXPORTS

#ifdef _WIN32
#ifdef   MUXAVF_EXPORTS
#define  MUXAVF_API __declspec(dllexport)
#else
#define  MUXAVF_API __declspec(dllimport)
#endif
#else
#define  MUXAVF_API
#endif
#include <math.h>
#include <stdlib.h>
//#include "def.h"

#ifndef _MUX_DTAT_TYPE_
#define _MUX_DTAT_TYPE_
enum MUX_DTAT_TYPE{
    MUX_DATA_REMOTE,    //海康音视频ps流
    MUX_DATA_LOCAL,     //本地音频
};
#endif


#ifndef _MUX_CALL_TYPE_
#define _MUX_CALL_TYPE_
enum MUX_CALL_TYPE{
    MUX_CALL_START,     //开始录像
    MUX_CALL_END,       //结束录像
};
#endif //__MUX_AVFILE_H__

/**
* @brief 封装文件回调
* @param strFilename：文件名（带路径）
* @param type：回调类型
* @param pUser：用户数据
*
* @author zhangshenyu
* @date 2020415
*/
typedef void(*muxavf_callback)(char *strFilename, MUX_CALL_TYPE type, void* pUser);

/**
* @brief 创建封装文件
* @param [in] strFilePath  文件路径 例：E:/，trFilepath不得为NULL或长度超过1024，否则默认为“./”
* @param [in] nSubpackageTime 分包时长：min，不得小于5min，否则按5min计算；分包后命名规则：“test-1.avi”、...“test-n.avi”，n表示除“test.avi”外的第n个分包
* @param [in] callback  文件创建、关闭回调 
* @param [in] pUser  用户数据 
* @see    成功返回封装文件句柄,失败返回NULL
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void *MuxAVF_creat(char *strFilePath="./", int nSubpackageTime=60, muxavf_callback callback=NULL, void *pUser=NULL);

/**
* @brief 填入数据
* @param [in] hMux  已经建立封装文件的句柄,MuxAVF_creat返回值
* @param [in] data	音视频帧数据,海康：PS流，本地视频格式：G711u
* @param [in] nLen  数据长度
* @param [in] type  数据类型
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void MuxAVF_inputData(void *hMux, void *data, int nLen, MUX_DTAT_TYPE type);

/**
* @brief  关闭封装文件
* @param [in] hMux  已经建立封装文件的句柄,MuxAVF_creat返回值
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void MuxAVF_close(void *hMux);

/**
* @brief  获取最后的错误信息
* @param [in] hMux  已经建立封装文件的句柄,MuxAVF_creat返回值
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API const char *MuxAVF_getLastErr(void *hMux);


#endif