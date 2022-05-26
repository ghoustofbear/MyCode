#ifndef __GB28181_SUPERIOR_H__
#define __GB28181_SUPERIOR_H__
#include "superior.h"



class GB28181Superior :
    public Superior
{
public:
    enum T_RESOURCE//query resource type
    {
        T_CATALOG_BUSINESS,
        T_CATALOG_DISTRICT,
        T_PRESET,
        T_RECORD,
    };
    struct Param_T;
    struct ParamSource_T;

public:
    GB28181Superior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
        int httpPort, int expires, string ifupIp, int bregister, int ivauslocal);

    virtual ~GB28181Superior();

    virtual int        start();
    

    //shl
    virtual int        on_RecordInfo(const string &json, string &xml);
    virtual int        push_mediaStatus(const string &xml);

    virtual int        on_resource(const string &json, string &xml);
    virtual int        push_resource();
    virtual int        keepaliveLoop();
    virtual void       onAlarm(string &json, void *data, uint32_t size);
protected:
    virtual void       timerdriver();
    
    virtual int        on_CatalogQuery_Business(const string json);
    virtual int        on_PresetQuery(const string json);
    virtual int        on_CatalogQuery_District(const string json);

    
    int                on_RecordInfoEx(const string json, string &xml);
protected:
    int                 CatalogChange_Business();

protected:
    static void      * threadProcEx(ThreadInfo *arg);

};


#endif //!__GB28181_SUPERIOR_H__