#ifndef _UT_IVASDK_H
#define _UT_IVASDK_H

#ifdef _WIN32

#ifdef UTIVA_EXPORTS
#define UTIVA_API __declspec(dllexport)
#else
#define UTIVA_API __declspec(dllimport)
#endif
#define FUNCTAG __stdcall
#else
#define UTIVA_API 
#define FUNCTAG
#endif 

#include "typedef.h"
#include <string>

/** < 智能分析服务告警回调原型 */
typedef void (FUNCTAG * pfnAlarmCallback)(const char * strJson, void * pUser);

/** < 智能分析服务登录回调原型 */
typedef void (FUNCTAG * pfnLoginCallback)(const char * strJson, void * pUser);

typedef struct
{
    char           * cIvaIp;          /** < 智能分析服务IP */
    unsigned int     iIvaPort;        /** < 端口 */
    char           * cUserName;       /** < 用户名 */
    char           * cUserPwd;        /** < 用户密码 */
    bool             bAutoReconn;     /** < 是否自动重连 */
    bool             bTrackDesired;   /** < 是否订阅实时目标跟踪告警信息 */
    bool             bPTC_B;          /** < 后续是否以B扩展协议交互*/
    void           * pUser;           /** < 用户数据*/
    pfnAlarmCallback pfnAlarmCb;      /** < 告警回调: 实时跟踪、分析状态、分析结果 */
    pfnLoginCallback pfnLoginCb;      /** < 登录回调：状态、软件/数据库版本、用户权限、流媒体列表 */
}UTIVA_CONN_PARAM;                    /** < 智能分析服务连接参数 */

#ifdef __cplusplus
extern "C" {
#endif

/** < 登录/释放智能分析服务连接 */
UTIVA_API void* NET_IvaLogin(UTIVA_CONN_PARAM loginParam);

UTIVA_API void NET_IvaRelease(void* ivaSdk);

UTIVA_API void NET_IvaRequestTime(void* ivaSdk, bool required);

/** < 向智能分析服务发送Json命令请求：请求/响应json格式请参考SDK文档 */
UTIVA_API char* NET_IvaRequestCmd(void* ivaSdk, const char * strJson);

/** < Base64图片编解码、二进制图片文件存取工具 */
UTIVA_API bool Base64Encode(const char * input, int insz, char * output, int * outsz);

UTIVA_API bool Base64Decode(const char * input, int insz, char * output, int * outsz);

UTIVA_API bool ReadImage(const char * imFileName, std::string * imageData);

UTIVA_API bool WriteImage(const char * imFileName, std::string& imageData);

#ifdef __cplusplus
}
#endif
#endif
