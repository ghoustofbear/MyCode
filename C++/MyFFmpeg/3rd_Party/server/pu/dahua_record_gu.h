/**
 * @file      hik_record_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 15:08:13
 *
 * @note
 *  hik_record_gu.h defines
 */
#ifndef _DAHUA_RECORD_GU_H__
#define _DAHUA_RECORD_GU_H__
#include "gu.h"
#include "3rd/dahua/dhnetsdk.h"

#define CBK            CALLBACK 
class DahuaRecordGu : public RecordGu
{
public:
    DahuaRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode);
	DahuaRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode);

	DahuaRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode);
	DahuaRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);

    virtual ~DahuaRecordGu();

     int      play(string scale, string range);

protected:
	virtual int        start(int offset);
    virtual void       stop();

    virtual int        pause();
    virtual int        continuex();

    virtual int        startRepos();
    virtual int        setPos(int pos);
    virtual int        setTime(datetime_s &t);

    virtual void       onTimer();

protected:
    friend int   CALLBACK PlayBackDataCallBack(LLONG  lRealHandle, DWORD  dwDataType,BYTE  *pData,DWORD  dwDataSize,LDWORD  dwUser);
    friend void  CALLBACK DownLoadPosCallBack(LLONG  lPlayHandle, DWORD  dwTotalSize, DWORD  dwDownLoadSize, LDWORD dwUser);


#ifdef __linux__
    static int CBK    RecordDataCallBack(LLONG   lRealHandle,
        DWORD   dwDataType,
        BYTE    * pData,
        DWORD   dwDataSize,
        LDWORD   dwUser);
#else
    static int CBK    RecordDataCallBack(LLONG   lRealHandle,
        DWORD     dwDataType,
        BYTE      * pData,
        DWORD     dwDataSize,
        LDWORD     dwUser);
#endif


private:
    void               login();
    void               getFileTimeFromFile(string filename, NET_TIME& s, NET_TIME& e, int& c);

private:
    LLONG              m_recordID;

    unsigned long      m_totalSize;
    unsigned long      m_curSize;
    char             * m_bufstream;
    int                m_pos;
    string             m_scale;
    bool               m_bexit;
    LLONG              m_userId;
};
#endif
