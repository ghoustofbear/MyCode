#include "dahua_gu.h"
#include "dahua_pu.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"

#ifdef __linux__
void CBK DahuaRealGu::RealDataCallBack( 
    LLONG      lRealHandle, 
    DWORD       dwDataType,
    BYTE      * pData,
    DWORD       dwDataSize,
    LONG        dwParam,
    long int    dwUser)

#else
void CBK DahuaRealGu::RealDataCallBack( 
    LLONG   lRealHandle, 
    DWORD   dwDataType, 
    BYTE    * pData,
    DWORD   dwDataSize, 
    LONG    dwParam,
    LDWORD   dwUser)
#endif
{
    ASSERT(dwUser);
    DahuaRealGu   *gu = (DahuaRealGu *)dwUser;

    if (dwDataType == EM_REAL_DATA_TYPE_PRIVATE)
    {
        
        //大华私有数据
        if (gu->m_streamProtoType != 0)
        {
            return;
        }
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);
        gu->notifyPrivStream(pData, dwDataSize, ENCODE_H264, TAG_DAHUA);
    }
    else if (dwDataType == NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_GBPS)
    {
        //国标ps流
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);
        gu->notifyPrivStream(pData, dwDataSize, ENCODE_H264, TAG_STD_PS);
    }
    else if (dwDataType == NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_H264)
    {

        //H264流
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);

        if (gu->m_streamProtoType != 1)
        {
            return;
        }

        gu->notifyVideoStream(pData, dwDataSize, ENCODE_H264, 'I', 1920, 1080, 25, sys->currentTimeMillis());
    }
    else
    {
        return;
    }

    gu->m_lastpts = sys->currentTimeMillis();
    gu->traffic(dwDataSize);
}


DahuaRealGu::DahuaRealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    m_realHandl = -1;
    m_bufstream = new char[4*1024*1024];
    m_pos = 0;
}

DahuaRealGu::DahuaRealGu(Pu *pu, string addrcode, int streamType, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
    m_realHandl = -1;
    m_bufstream = new char[4*1024*1024];
    m_pos = 0;
}

DahuaRealGu::~DahuaRealGu()
{
    if (NULL != m_bufstream)
    {
        delete [] m_bufstream;
        m_bufstream = NULL;
    }
}


int
DahuaRealGu::start(int offset)
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());
        return -1;
    }
    
#if 0

    if (0 == m_streamType)
    {
        m_realHandl = CLIENT_RealPlayEx(((DahuaPu *)m_pu)->getUserID(), m_channelNo, 0, DH_RType_Realplay_0);
    }
    else if (1 == m_streamType)
    {
        m_realHandl = CLIENT_RealPlayEx(((DahuaPu *)m_pu)->getUserID(), m_channelNo, 0, DH_RType_Realplay_1);
    }
    
    if (m_realHandl <= 0)
    {
        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
        ERRE_MSV("realhandle:%ld, CLIENT_RealPlayEx ip:%s, chn: %d, streamtype:%d, failed reason: %d ",
                 m_realHandl, m_pu->ip().c_str(), m_channelNo, m_streamType, nErrorNum);
        return -1;
    }
    
#ifdef __linux__
    LDWORD dwUser = (long int)this;
#else
    LDWORD dwUser = (LDWORD)this;
#endif

    CLIENT_SetRealDataCallBackEx(m_realHandl, RealDataCallBack, (LDWORD)this,  0x1f);

#endif
    NET_IN_REALPLAY_BY_DATA_TYPE stIn = { sizeof(stIn) };
    NET_OUT_REALPLAY_BY_DATA_TYPE stOut = { sizeof(stOut) };

    stIn.cbRealData = RealDataCallBack;

    stIn.nChannelID = m_channelNo;
    stIn.hWnd = NULL;//GetSelfWindow();
    stIn.dwUser = (LDWORD)this;


    if (0 == m_streamType)
    {
        stIn.rType = DH_RType_Realplay_0;
    }
    else
    {
        stIn.rType = DH_RType_Realplay_1;
    }
    if (m_streamProtoType == 0)
    {
        stIn.emDataType = EM_REAL_DATA_TYPE_PRIVATE;
    }
    else
    {
        stIn.emDataType = EM_REAL_DATA_TYPE_H264;
    }
    
    m_realHandl = CLIENT_RealPlayByDataType(((DahuaPu *)m_pu)->getUserID(), &stIn, &stOut, 5000);
    if (m_realHandl)
    {
        LOGD_MSV("gu create success455\n");       
    }
    else
    {
        return -1;
    }

    realGuMgr->addGu(this);

    return 0;
}

void
DahuaRealGu::stop()
{
    realGuMgr->delGu(this);
    if (-1 != m_realHandl)
    {
        ERRE_MSV("free dahua gu");
        CLIENT_StopRealPlayEx(m_realHandl);
        m_realHandl = -1;
    }
    LOGD_MSV("gu create success\n");
}

void
DahuaRealGu::timerdriver()
{
    Gu::onTimer();
}

