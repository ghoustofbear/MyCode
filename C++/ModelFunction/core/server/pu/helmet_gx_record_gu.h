#ifndef __HELMET_GX_RECORD_GU_H__
#define __HELMET_GX_RECORD_GU_H__

#include "gu.h"
#include "3rd/helmet/iCVSC.h"
#include "3rd/helmet/iCVSC_base.h"
#include "thread.h"

class HelmetGXRecordGu : public RecordGu 
{
public:
	HelmetGXRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode);
	HelmetGXRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode);
	HelmetGXRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode);
	HelmetGXRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode);
    virtual ~HelmetGXRecordGu();



protected:
    virtual int        start(int offset);
    virtual void       stop();

    virtual int        pause();
    virtual int        continuex();

    virtual int        startRepos();
    virtual int        setPos(int pos);
    virtual int        setTime(datetime_s &t);

    virtual void       onTimer();
    static  void     * threadEntry(ThreadInfo *arg);
    void               VideoGatherLoop();

    void               RealDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType);
    void               RawDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType, uint32_t nKeyFrmFlag);

private:
    //int                LiveStart();
    //int                LiveStop();


//
//    virtual S32 RequestIFrame();
//    virtual S32 PtzCtrl(S32 s32Cmd, S32 s32Speed, S32 s32Stop);
//





private:
    bool       m_bGetFrame;
    bool      m_bStop;

    IC_STR     m_szVodID;
    IC_HANDLE  m_lRealHandle;
    Thread     m_thread;

};

#endif
