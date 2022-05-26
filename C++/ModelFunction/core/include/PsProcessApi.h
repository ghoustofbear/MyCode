#ifndef __PSPROCESSAPI_H__
#define __PSPROCESSAPI_H__

#include <stdint.h>
#ifdef PSPROCESS_STATICLIB
#  define PSPROCESS_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(PSPROCESS_EXPORTS)
#    define PSPROCESS_API  __declspec(dllexport)
#  else
#    define PSPROCESS_API  __declspec(dllimport)
#  endif
#else
#  define   PSPROCESS_API
#endif

#ifdef __cplusplus
extern "C" {
#endif


enum enPsType
{
	PS_PACK,
	PS_UNPACK
};


/**
* @brief 创建封装/解封装ps流对象
* @param [in] type 对象类型
* @return 返回值，成功返回对象指针，失败返回空
*/

PSPROCESS_API void* createpsinstance(enPsType type);
/**
* @brief 销毁ps流对象
* @param [in] obj createpsinstance 的返回值
* @return 返回值为空
*/
PSPROCESS_API void  destorypsinstance(void *obj);
/**
* @brief 流回调函数，PS封装时，仅前三个参数有效，PS解封装时，五个参数有效
* @param [out] data     数据流 
* @param [out] size     数据大小
* @param [out] userdata 用户数据
* @param [out] type     解封装时有效，'A'音频包 , 'p'视频P帧，'I'视频I帧 
* @param [out] pts      解封装时有效，时间戳
* @return 返回值为空
*/

typedef void(*pfnstreamdatacallback)(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime);
/**
* @brief 设置封装/解封装后数据流回调
* @param [in] obj createpsinstance 的返回值
* @param [in] fnstreamcb 数据流回调函数
* @param [in] puser 用户自定义对象
* @return 返回值为空
*/
PSPROCESS_API void  setpsstreamcallback(void *obj, pfnstreamdatacallback fnstreamcb, void *puser);
/**
* @brief 输入流数据
* @param [in] obj createpsinstance 的返回值
* @param [in] data 数据流
* @param [in] size 数据大小
* @param [in] type 帧类型, 'I':I帧 'P':P帧 'A': A帧 仅在封装时需要 
* @param [in] pts  时间戳  ，仅在封装时需要
* @return 返回值为空
*/
PSPROCESS_API void  inputstreamdata(void *obj, uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime);


/**
* @brief 设置时间戳使用标志
* @param [in] obj createpsinstance 的返回值
* @param [in] flag false表示使用内部时间戳，true表示使用外部时间戳
* @return 返回值为空
*/
PSPROCESS_API void  setPtsFlag(void *obj,bool flag);


#ifdef __cplusplus
}
#endif

#endif