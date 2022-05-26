/**
 * @file      msv_pu.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-04 10:31:03
 *
 * @note
 *  msv_pu.h defines
 */
#ifndef __MSV_PU_H__
#define __MSV_PU_H__
#include "pu.h"
#include "imatrix.h"
#include "idecoder.h"
#include "c2sdk.h"

class MsvPu : public Pu, public iMatrix, public iDecoder
{
    friend class MsvRealGu;
    friend class MsvRecordGu;
public:
    MsvPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~MsvPu();

    virtual int        start();

protected:
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    virtual int        ptzCtrl(int cmd, int channelNo,   int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);

    virtual int        queryRecord(int channelNo,  int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

    virtual int        syncTime(datetime_s &dt, const string &tzone);

    virtual void       onTimer();

protected:
    int                queryDevice(string addrcode);
    friend  void     * query(ThreadInfo *arg);
    friend  void       logincb(int code, void* userdata);

private:
    Thread             m_thread;
    HANDLE             m_handle;
};

#endif
