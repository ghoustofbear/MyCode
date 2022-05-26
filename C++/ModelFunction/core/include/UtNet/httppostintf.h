#ifndef _HTTP_POST_INTF_H_
#define _HTTP_POST_INTF_H_

#ifndef HTTPPOSTLIB_EXPORTS
#define HTTP_API extern _declspec(dllimport)
#else
#define HTTP_API extern _declspec(dllexport)
#endif

#if __cplusplus
extern "C" {
#endif

HTTP_API void InitHttp();

HTTP_API char * HttpPost(const char * url, const char * body, const char * user , const char * password, int timeout);


#if __cplusplus
};
#endif

#endif
