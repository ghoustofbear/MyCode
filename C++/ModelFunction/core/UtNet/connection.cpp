#include "connection.h"
#include "previewc1.h"
#include "dualtalk.h"
#include "json/json.h"
#include "playbackc1.h"
#include "downloadc1.h"
#include "download_bytime.h"
#include "plaympeg4.h"



#define ENABLE_CONNECTION_DEBUG
#ifdef  ENABLE_CONNECTION_DEBUG
#ifdef _WIN32
#define CONNECTION_DEBUG             printf
#else
#define CONNECTION_DEBUG(MSG...)     DEBUG("CONNECTION", MSG)
#endif /* end _WIN32 */
#else
#define CONNECTION_DEBUG(MSG...)
#endif /* end ENABLE_CONNECTION_DEBUG */

Connection::Connection()
    :BasePlatformConn()
{
    m_bExit = FALSE;
    m_bLogined = FALSE;
    m_trdMain  = 0;

    m_fConnCallback      = NULL;
    m_fAlarmCallback     = NULL;
    m_fWFLinkageCallback = NULL;

    m_pDualtalk          = NULL;
    m_bInitDTalk         = FALSE;
    m_pDualTalkEx        = NULL;
};

Connection::~Connection() {};


S32 Connection::StartMainMsgLoop()
{
    THREAD_ARG_S *pThreadArg = new THREAD_ARG_S;

    pThreadArg->pClass = this;
    pThreadArg->pArg   = NULL;
    if (CreateJoinableThread(ThreadEntry, pThreadArg, &m_trdMain) < 0)
    {
        delete pThreadArg;
        return -1;
    }

    return 0;
}

BOOL Connection::Start(const NET_CONN_PARAM_S* pParam)
{
    m_s32PviewCliNum = 0;

    if (m_szUsername.size() >= MAX_NAME_LEN
            || m_szPassword.size() >= MAX_NAME_LEN)
    {
        return FALSE;
    }
    m_szIP           = pParam->s8Ip;
    m_u16Port        = pParam->u16Port;
    m_szUsername     = pParam->ps8Username;
    m_szPassword     = pParam->ps8Password;
    m_szName         = pParam->ps8Name;
    m_szClientIp     = pParam->ps8ClientIp;
    m_u16CTalkPort   = pParam->u16ClientTalkPort;
    m_hWnd           = pParam->hWnd;
    m_u32Msg         = pParam->u32Msg;
    m_pUser          = pParam->pUser;
    m_bAutoReconn    = pParam->bAutoReconn;
    m_bAlarmDesired  = pParam->bAlarmDesired;
    m_fAlarmCallback = pParam->fAlarmCallback;
    m_fConnCallback  = pParam->fConnCallback;

    memset(&m_stLoginRsp, 0, sizeof(m_stLoginRsp));
    m_s32LoginRspSize = sizeof(m_stLoginRsp.stRsp);

	AutoLock lock(m_lsHost.m_Mutex);
	strcpy(m_host.ip, pParam->s8Ip);
	m_host.port = pParam->u16Port;
	m_lsHost.push_back(m_host);

    StartMainMsgLoop();

    return TRUE;
}

VOID Connection::Stop()
{

    /*  等待所有预览线程退出 */
    {
        AutoLock lock(m_mtxPView);
        for (size_t i = 0; i < m_vcPViews.size(); ++i)
        {
            m_vcPViews[i]->StopAll();
            delete m_vcPViews[i];
        }
        m_vcPViews.erase(m_vcPViews.begin(), m_vcPViews.end());
    }

    /*  对讲线程退出 */
    if (m_pDualtalk)
    {
        m_pDualtalk->Stop();
        delete m_pDualtalk;
        m_pDualtalk = NULL;
    }

    /* 主线程最后退出 */
    m_bAutoReconn = FALSE;
    m_bExit       = TRUE;
    m_bLogined    = FALSE;
    m_MainSock.KillSocket();
    ThreadJoin(m_trdMain);

    ASSERT(m_s32PviewCliNum == 0);
}

VOID  Connection::BeginTransaction()
{
}

VOID  Connection::EndTransaction()
{
    //mTransactionEvent.Reset();
    m_MainSock.KillSocket();
    //mMsgTransactionQueue.Clear();

    {
#if 0
        AutoLock lock(m_mtxPView);
        for (size_t i = 0; i < m_vcPViews.size(); ++i)
        {
            m_vcPViews[i]->Stop();
        }
#endif
    }

    AutoLock lock(m_mtxTrans);
    m_MainSock.Close();
}

VOID Connection::MainMsgLoop()
{
    do
    {
        m_bLogined = FALSE;
        if (!Connect() || !Login())
        {
            if (m_bAutoReconn)
            {
                Msleep(500);
               
				SwitchHost();
            }
            continue;
        }

        BeginTransaction();
        m_bLogined = TRUE;
        LOGIN_STATUS(MSG_LOGIN_SUCCESS);

        int   timeoutNum = 0;
        time_t tStart    = time(NULL);
        while (!m_bExit)
        {
            BOOL timeout = FALSE;
            if (m_MainSock.Select(FALSE, 5000, &timeout))
            {
                MsgBuf msgBuf;
                if (RecvMsg(m_MainSock, msgBuf))
                {
                    timeoutNum = 0;
                    switch (msgBuf.GetMsgType())
                    {
                    case MSG_HEARTBEAT:
                    case MSG_UDP_PREVIEW_STOP:
                        break;

                    case MSG_ALARM_REPORT:
                        ProcessAlarmReport(msgBuf);
                        CONNECTION_DEBUG("AlarmReport: msgsize = %d, msgsubtype = %d\n", msgBuf.u32MsgSize, msgBuf.u16MsgSubType);
                        break;

                    default :
                        CONNECTION_DEBUG("msgsize=%d,msgsubtype = %d,msgextsize=%d", msgBuf.u32MsgSize, msgBuf.u16MsgSubType, msgBuf.u6ExtSize);
                        m_MsgTransQueue.Rsp(msgBuf);
                        break;
                    }

                    /**
                     *	如果服务器一直在发信息（如一直上报报警信息），
                     *  这时timeout很长时间都不会变true，因而发不出心跳，
                     *  导致服务器会超时断开
                     */
                    time_t tEclipse = time(NULL);
                    S64 temppp = tEclipse - tStart;

                    if (MSG_ALARM_REPORT == msgBuf.GetMsgType() &&
                        abs(temppp) > 5)
                    {
                        timeout = TRUE;
                        tStart  = tEclipse;
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if (timeout)
            {
                MsgHeadWrap heartbeat(MSG_HEARTBEAT);
                heartbeat.u2Version = m_u32MsgVersion;
                if (++timeoutNum < 3 && SendMsg(m_MainSock, heartbeat))
                {
                    CONNECTION_DEBUG("Send heatbeat\n");
                    continue;
                }
            }

            break;
        }

        EndTransaction();
        LOGIN_STATUS(MSG_CONN_CLOSED);

		SwitchHost();
    }while (m_bAutoReconn);

    //mConnectionDownEvent.Set();
}

BOOL Connection::Connect()
{
    LOGIN_STATUS(MSG_CONN_START);
    if (m_MainSock.Socket(TRUE) \
            && m_MainSock.SetSendTimeOut(SEND_TIMEOUT) \
            && m_MainSock.SetRecvTimeOut(RECV_TIMEOUT) \
           // && m_MainSock.Connect((S8 *)m_szIP.c_str(), m_u16Port, CONN_TIMEOUT, TRUE))
		   && m_MainSock.Connect(m_host.ip, m_host.port, CONN_TIMEOUT, TRUE))
    {
        LOGIN_STATUS(MSG_CONN_SUCCESS);
        return TRUE;
    }

    LOGIN_STATUS(MSG_CONN_FAIL);

    m_MainSock.Close();

    return FALSE;
}

BOOL Connection::Login()
{
    if (0)
    {
        m_u32MsgVersion = MsgHeadV2;
    }
    else
    {
        m_u32MsgVersion = MsgHeadV1;
    }

    LOGIN_REQ_S stReq;
    memset(&stReq, 0, sizeof(stReq));
    strcpy(stReq.s8Username, m_szUsername.c_str());
    strcpy(stReq.s8Password, m_szPassword.c_str());
    stReq.bAlarmDesired = m_bAlarmDesired;
    //liuwei 20140311 增加客户端的IP，名称，对讲端口
    INET_PTON(m_szClientIp.c_str(), stReq.u8ClientIp);
    stReq.u16ClientTalkPort = m_u16CTalkPort;
    strcpy(stReq.s8Name, m_szName.c_str());

    MsgHeadWrap msgHead(MSG_LOGIN, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    MsgBuf buf;

    LOGIN_STATUS(MSG_LOGIN_REQUEST);

    if (SendMsg(m_MainSock, msgHead, &stReq)
            && RecvMsg(m_MainSock, buf)
            && buf.ValidRsp(msgHead))
    {
        if (buf.IsSuccess())
        {
            if (buf.GetDataLen() > (S32)sizeof(m_stLoginRsp))
            {
                CONNECTION_DEBUG("LoginRsp too long, net.dll should be rebuild\n");
            }
            else
            {
                //保存消息头的版本号
                m_u32MsgVersion = buf.u2Version;
                {
                    AutoLock lock(m_mtxLogin);
                    memcpy(&m_stLoginRsp, buf.GetData(), buf.GetDataLen());
                    m_s32LoginRspSize = buf.GetDataLen();
                }
//
// 						if ((!mAlarmRptDesired) != (!stReq.AlarmReportDesired))
// 							AlarmSubscribe(mAlarmRptDesired);
//
                return TRUE;
            }
        }


        if (buf.GetAckFlag() == ACK_LOGIN_HAS_LOGINED)
        {
            LOGIN_STATUS(MSG_LOGIN_USERLOGINED);
        }
        else if (buf.GetAckFlag() == ACK_LOGIN_NO_USERNAME)
        {
            LOGIN_STATUS(MSG_LOGIN_NOUSERNAME);
        }
        else if (buf.GetAckFlag() == ACK_LOGIN_AUTH_FAIL)
        {
            LOGIN_STATUS(MSG_LOGIN_PASSWORDERROR);
        }
        else if (buf.GetAckFlag() == ACK_LOGIN_PERMDENY)
        {
            LOGIN_STATUS(MSG_LOGIN_NORIGHT);
        }
        else if (buf.GetAckFlag() == ACK_LOGIN_AAA_AUTH_FAIL)
        {
            LOGIN_STATUS(MSG_LOGIN_AAA_AUTH_FAIL);
        }

        if (m_bAutoReconn)
        {
            Msleep(1000);
        }
    }
    else
    {
        LOGIN_STATUS(MSG_LOGIN_FAIL);
    }

    m_MainSock.Close();

    return FALSE;
}

BOOL Connection::GetLoginRsp(LOGIN_RSP_S *pRsp)
{
    ASSERT(pRsp);

    if (!m_bLogined)
    {
        return FALSE;
    }

    AutoLock lock(m_mtxLogin);
    memcpy(pRsp, &m_stLoginRsp, sizeof(LOGIN_RSP_S));

    return TRUE;
}

BOOL Connection::GetUserList(const NET_USER_PARAM_S *pParam)
{
    ASSERT(pParam);

    MsgBuf buf;

    if (!QueryParam(buf, MSG_PARAM_USERLIST, NULL, 0))
    {
        return FALSE;
    }

    ListDataParser<USERINFO_RSP_S> parser(buf);
    UINT num = parser.GetListNum();
    UINT i = 0;
    for (i = 0; i < num; ++i)
    {
        USERINFO_RSP_S *pRsp = parser.GetAt(i);

        if (pParam->fUserListCallback)
        {
            pParam->fUserListCallback(pRsp, pParam->pUser);
        }
    }

    return TRUE;
}

BOOL Connection::GetDeviceList(const NET_DEVICE_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8AddrCode);
    ASSERT(pParam->ps8DeviceIp);

    DEVINFO_REQ_S stReq;

    memset(&stReq, 0, sizeof(stReq));

    if (pParam->ps8AddrCode)
    {
        strncpy(stReq.s8AddrCode, pParam->ps8AddrCode, MAX_ADDR_LEN);
    }
    if (pParam->ps8DeviceName)
    {
        strncpy(stReq.s8DeviceName, pParam->ps8DeviceName, MAX_ADDR_LEN);
    }

    if (pParam->ps8DeviceIp)
    {
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    }

    stReq.u16DevicePort = pParam->u16DevicePort;

    MsgBuf buf;
    if (!QueryParam(buf, MSG_PARAM_DEVLIST, &stReq, sizeof(stReq)))
    {
        return FALSE;
    }

    ListDataParser<DEVINFO_RSP_S> parser(buf);
    UINT num = parser.GetListNum();

    if (0 == num)
    {
        return FALSE;
    }

    UINT i = 0;
    for (i = 0; i < num; ++i)
    {
        DEVINFO_RSP_S *pRsp = parser.GetAt(i);

        if (pParam->fDeviceListCallback)
        {
            pParam->fDeviceListCallback(pRsp, pParam->pUser);
        }
    }

    return TRUE;
}

BOOL Connection::QueryParam(MsgBuf &buf, U32 u32SubType, VOID *pReq, U32 u32Size)
{
    MsgHeadWrap msgHead(MSG_QUERY_PARAM, u32SubType, u32Size);
    msgHead.u2Version = m_u32MsgVersion;

    return Transaction(msgHead, pReq, buf);
}

VOID Connection::ProcessAlarmReport(MsgBuf &buf)
{
    U16 u16SubMsgType = buf.GetMsgSubType();
    U16 u16AlarmType  = ALARM_IO;

    ALARMER_S *pAlarmer = (ALARMER_S *)buf.GetData();

    if (MSG_ALARM_WFLINGAGE == u16SubMsgType && m_fWFLinkageCallback)
    {
        U32 u32OperType = *((U32 *)((S8 *)buf.GetData() + sizeof(ALARMER_S)));
        LINKAGE_RPT_S *pstLinkageRpt = (LINKAGE_RPT_S *)((S8 *)buf.GetData() + sizeof(ALARMER_S) + 4);

        m_fWFLinkageCallback(
            pAlarmer,
            u32OperType,
            pstLinkageRpt,
            buf.GetDataLen() - 4 - sizeof(ALARMER_S),
            m_pWFUser);
    }
    else if (m_bAlarmDesired && m_fAlarmCallback)
    {
        switch (u16SubMsgType)
        {

        case MSG_ALARM_IO:
            u16AlarmType = ALARM_IO;
            break;
        case MSG_ALARM_MD:
            u16AlarmType = ALARM_MD;
            break;
        case MSG_ALARM_VLOSS:
            u16AlarmType = ALARM_VLOSS;
            break;
        case MSG_ALARM_HDD:
            u16AlarmType = ALARM_HDD;
            break;
        case MSG_ALARM_DEV_STATE:
            u16AlarmType = REPORT_DEV_STAT;
            break;
        case  MSG_ALARM_LINK_TRANS:
            u16AlarmType = LINKAGE_TRANSMIT;
            break;
        case  MSG_ALARM_AI:
            u16AlarmType = ALARM_AI;
            break;
        case MSG_ALARM_SYSINFO:
            u16AlarmType = SYSTEM_INFO_UPLOAD;
            break;
        case MSG_ALARM_MANUAL:
            u16AlarmType = ALARM_MANUAL;
            break;
        case MSG_ALARM_GPS:
            u16AlarmType = ALARM_GPS;
            break;
        case MSG_ALARM_ACCESS:
            u16AlarmType = ALARM_ACCESS;
            break;
		case MSG_ALARM_CALL:
			u16AlarmType = ALARM_CALL_STATUS;
            break;
		case MSG_ALARM_PTZ:
			u16AlarmType = ALARM_PTZ_CALL;
            break;
		case MSG_ALARM_IVASTATE:
			u16AlarmType = ALARM_IVA_STATE;
            break;
		case MSG_ALARM_CAMSTATE:
			u16AlarmType = ALARM_CAMERA_STATE;
			break;
        default:
            break;
        }
        m_fAlarmCallback(pAlarmer, u16AlarmType, (U8 *)buf.GetData() + sizeof(ALARMER_S), buf.GetDataLen() - sizeof(ALARMER_S), m_pUser);
    }
}

HANDLE Connection::StartPreview(const NET_PVIEW_PARAM_S *pParam)
{
    ASSERT(pParam);

    if (!pParam->ps8FrontAddrCode)
    {
        ASSERT(pParam->ps8DeviceIp);
    }

    AutoLock lock(m_mtxPView);

    Preview *pPView  = NULL;
    BOOL bIsNewPView = FALSE;

    size_t i = 0;
    for (i = 0; i < m_vcPViews.size(); ++i)
    {
        if (m_vcPViews[i]->IsSameStream(pParam))
        {
            pPView = m_vcPViews[i];
            break;
        }
    }

    if (!pPView)
    {
        pPView = new Preview(this);
        if (!pPView)
        {
            return NULL;
        }

        bIsNewPView = TRUE;
    }

    PreviewClient *pPViewClient = pPView->Start(pParam);
    ASSERT(pPViewClient);

    if (bIsNewPView)
    {
        if (pPViewClient == NULL)
        {
            delete pPView;
        }
        else
        {
            m_vcPViews.push_back(pPView);
        }
    }
    else
    {
        if (pPViewClient)
        {
            IFRAME_REQ_S stReq = {0};

            stReq.u32ChnNO = pParam->u32ChnNO;
            stReq.u8Type   = pParam->u8Type;
            stReq.u16DevicePort  = pParam->u16DevicePort;
            stReq.u16SubServPort = pParam->u16SubServPort;
            INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
            INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

            MsgHeadWrap msgHead(MSG_REQ_IFRAME, MSG_NO_SUBTYPE, sizeof(stReq));
            msgHead.u2Version = m_u32MsgVersion;
            SendReq(msgHead, &stReq);
        }
    }

    if (pPViewClient != NULL)
    {
        ++m_s32PviewCliNum;
    }

    return pPViewClient;

}

VOID Connection::StopPreview(HANDLE *hPViewCilent)
{
    AutoLock lock(m_mtxPView);
    if (*hPViewCilent == NULL)
    {
        return ;
    }
    

    PreviewClient *pPViewClient = (PreviewClient*)(*hPViewCilent);
    Preview    *pPView          = (Preview*)pPViewClient->pPView;

    if (pPView->Stop(pPViewClient))
    {
        vector<Preview *>::iterator i = find(m_vcPViews.begin(), m_vcPViews.end(), pPView);
        ASSERT(i != m_vcPViews.end());
        if (i == m_vcPViews.end())
        {
            return;
        }

        m_vcPViews.erase(i);
        delete pPView;
        *hPViewCilent = NULL;
    }

    --m_s32PviewCliNum;
}

BOOL Connection::RequestIFrame(const NET_REQI_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8DeviceIp);

    IFRAME_REQ_S stReq = {0};

    stReq.u32ChnNO = pParam->u32ChnNO;
    stReq.u8Type   = pParam->u8Type;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_REQ_IFRAME, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::PreviewSwitch(const NET_SWITCH_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8DeviceIp);

    SWITCH_REQ_S stReq = {0};

    stReq.u32Channel = pParam->u32Channel;
    stReq.u32Mode    = pParam->u32Mode;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_PVIEW_SWITCH, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::MatrixSwitch(const NET_MATRIX_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8DeviceIp);

    MATRIX_REQ_S stReq = {0};

    stReq.u32InPort  = pParam->u32InPort;
    stReq.u32OutPort = pParam->u32OutPort;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_MATRIX_SWITCH, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::DecoderSwitch(const NET_DECODER_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8DeviceIp);

    DECODER_REQ_S stReq = {0};

    stReq.u32Mode    = pParam->u32Mode;
    stReq.u32Channel = pParam->u32Channel;
    stReq.u32OutPort = pParam->u32OutPort;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_MATRIX_SWITCH, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->ps8DeviceIp);

    DEC_DISPLAY_REQ_S stReq = {0};

    stReq.u32Mode    = pParam->u32Mode;
    stReq.u32OutPort = pParam->u32OutPort;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_DEC_SETDISPLAY, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::DecoderStartDynamic(
    U8  * pu8DeviceIp,
    U16   u16DevicePort,
    U8  * pu8SubServIp,
    U16   u16SubServPort,
    S32   s32DecChnNo,
    S32   s32DisOutPort,
    S32   s32DisWndNo,
    const NET_DEC_CHN_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pu8DeviceIp);
    ASSERT(pu8SubServIp);

    DEC_DYNAMIC_REQ_S stReq;

    memset(&stReq, 0, sizeof(stReq));

    if (pParam->bValidStream)
    {
        stReq.bValidStream = TRUE;
        strncpy(stReq.stDecChnCfg.stStreamCfg.s8Url, pParam->stStreamCfg.ps8Url, MAX_URL_LEN);
        stReq.stDecChnCfg.stStreamCfg.u16Port          = pParam->stStreamCfg.u16Port;
        stReq.stDecChnCfg.stStreamCfg.u32TransProtocol = pParam->stStreamCfg.u32TransProtocol;
        strncpy(stReq.stDecChnCfg.stStreamCfg.s8Username, pParam->stStreamCfg.ps8Username, MAX_NAME_LEN);
        strncpy(stReq.stDecChnCfg.stStreamCfg.s8Password, pParam->stStreamCfg.ps8Password, MAX_NAME_LEN);
    }
    else
    {
        INET_PTON(pParam->stDvrCfg.ps8Ip, stReq.stDecChnCfg.stDvrCfg.u8Ip);

        stReq.stDecChnCfg.stDvrCfg.u16Port          = pParam->stDvrCfg.u16Port;
        stReq.stDecChnCfg.stDvrCfg.u32ChnNo         = pParam->stDvrCfg.u32ChnNo;
        stReq.stDecChnCfg.stDvrCfg.u32TransProtocol = pParam->stDvrCfg.u32TransProtocol;
        stReq.stDecChnCfg.stDvrCfg.u32TransMode     = pParam->stDvrCfg.u32TransMode;
        stReq.stDecChnCfg.stDvrCfg.u32ProtoType     = pParam->stDvrCfg.u32ProtoType;

        strncpy(stReq.stDecChnCfg.stDvrCfg.s8Username, pParam->stDvrCfg.ps8Username, MAX_NAME_LEN);
        strncpy(stReq.stDecChnCfg.stDvrCfg.s8Password, pParam->stDvrCfg.ps8Password, MAX_NAME_LEN);
    }

    stReq.s32DecChnNo    = s32DecChnNo;
    stReq.s32DisWndNo    = s32DisWndNo;
    stReq.s32DisOutPort  = s32DisOutPort;

    stReq.u16DevicePort  = u16DevicePort;
    stReq.u16SubServPort = u16SubServPort;

    memcpy(stReq.u8DeviceIp, pu8DeviceIp, MAX_IPV4_LEN);
    memcpy(stReq.u8SubServIp, pu8SubServIp, MAX_IPV4_LEN);

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_DEC_START_DYNAMIC, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::DecoderStopDynamic(
    U8 * pu8DeviceIp,
    U16 u16DevicePort,
    U8 * pu8SubServIp,
    U16 u16SubServPort,
    S32 s32DisOutPort)
{
    DEC_DYNAMIC_STOP_REQ_S stReq;
    stReq.s32DisOutPort = s32DisOutPort;
    memcpy(stReq.u8DeviceIp, pu8DeviceIp, MAX_IPV4_LEN);
    memcpy(stReq.u8SubServIp, pu8SubServIp, MAX_IPV4_LEN);

    stReq.u16DevicePort  = u16DevicePort;
    stReq.u16SubServPort = u16SubServPort;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_DEC_STOP_DYNAMIC, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SendReq(const MsgHeadWrap &msgHead, VOID *reqData)
{
    AutoLock lock(m_mtxTrans);
    //return mTransactionEvent.Wait(0) && SendMsg(mSock, msgHead, reqData);
    return SendMsg(m_MainSock, msgHead, reqData);
}

VOID *Connection::ThreadEntry(VOID *pArg)
{
    ASSERT(pArg);
    THREAD_ARG_S *pThreadArg = (THREAD_ARG_S *)pArg;
    Connection *pConn = (Connection *)pThreadArg->pClass;

    pConn->MainMsgLoop();

    delete pThreadArg;

    return NULL;
}

BOOL Connection::PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
    ASSERT(pParam);

    PTZ_CTRL_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        stReq.u8ReqType = 0;
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNO;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }

    stReq.u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;
    stReq.stNormal       = pParam->stNormal;
    stReq.stCruise       = pParam->stCruise;
    stReq.stPreset       = pParam->stPreset;

    if (stReq.stNormal.u32Speed > 10 ||
            stReq.stNormal.u32Speed < 0)
    {
        stReq.stNormal.u32Speed = 5;
    }

    WORD  dwMsg = MSG_NO_SUBTYPE;
    switch (u32Cmd)
    {
    case PTZ_CMD_UP:
        dwMsg = MSG_PTZ_UP;
        break;
    case PTZ_CMD_DOWN:
        dwMsg = MSG_PTZ_DOWN;
        break;
    case PTZ_CMD_LEFT:
        dwMsg = MSG_PTZ_LEFT;
        break;
    case PTZ_CMD_RIGHT:
        dwMsg = MSG_PTZ_RIGHT;
        break;
    case PTZ_CMD_UPLEFT:
        dwMsg = MSG_PTZ_UPLEFT;
        break;
    case PTZ_CMD_UPRIGHT:
        dwMsg = MSG_PTZ_UPRIGHT;
        break;
    case PTZ_CMD_DOWNLEFT:
        dwMsg = MSG_PTZ_DOWNLEFT;
        break;
    case PTZ_CMD_DOWNRIGHT:
        dwMsg = MSG_PTZ_DOWNRIGHT;
        break;
    case PTZ_CMD_ZOOMIN:
        dwMsg = MSG_PTZ_ZOOMIN;
        break;
    case PTZ_CMD_ZOOMOUT:
        dwMsg = MSG_PTZ_ZOOMOUT;
        break;
    case PTZ_CMD_FOCUSNEAR:
        dwMsg = MSG_PTZ_FOCUSNEAR;
        break;
    case PTZ_CMD_FOCUSFAR:
        dwMsg = MSG_PTZ_FOCUSFAR;
        break;
    case PTZ_CMD_IRISOPEN:
        dwMsg = MSG_PTZ_IRISOPEN;
        break;
    case PTZ_CMD_IRISCLOSE:
        dwMsg = MSG_PTZ_IRISCLOSE;
        break;
    case PTZ_CMD_AUTOSCAN:
        dwMsg = MSG_PTZ_AUTOSCAN;
        break;
    case PTZ_CMD_CRUISE:
        dwMsg = MSG_PTZ_CRUISE;
        break;
    case PTZ_CMD_SETPRESET:
        dwMsg = MSG_PTZ_SETPRESET;
        break;
    case PTZ_CMD_CLEARPRESET:
        dwMsg = MSG_PTZ_CLEARPRESET;
        break;
    case PTZ_CMD_CALLPRESET:
        dwMsg = MSG_PTZ_CALLPRESET;
        break;
    case PTZ_CMD_RAIN_ON:
        dwMsg = MSG_PTZ_RAINON;
        break;
    case PTZ_CMD_RAIN_OFF:
        dwMsg = MSG_PTZ_RAINOFF;
        break;
    default:
        return FALSE;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_PTZ_CONTROL, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    CONNECTION_DEBUG("PTZ MsgType=%u, MsgSubType=%u, flag=%u\n", msgHead.u16MsgType, msgHead.u16MsgSubType, pParam->stNormal.u32Flag);

    /* 既要返回快，又要判断权限。时间太小会马上超时，建议大于500 */
    if (!Transaction(msgHead, &stReq, buf, 500) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
    ASSERT(pParam);

    PTZ_CTRL_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        stReq.u8ReqType = 0;
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNO;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }

    stReq.u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;
    stReq.stNormal       = pParam->stNormal;
    stReq.stCruise       = pParam->stCruise;
    stReq.stPreset       = pParam->stPreset;

    if (stReq.stNormal.u32Speed > 10 ||
            stReq.stNormal.u32Speed < 0)
    {
        stReq.stNormal.u32Speed = 5;
    }

    WORD  dwMsg = MSG_NO_SUBTYPE;
    switch (u32Cmd)
    {
    case PTZ_CMD_UP:
        dwMsg = MSG_PTZ_UP;
        break;
    case PTZ_CMD_DOWN:
        dwMsg = MSG_PTZ_DOWN;
        break;
    case PTZ_CMD_LEFT:
        dwMsg = MSG_PTZ_LEFT;
        break;
    case PTZ_CMD_RIGHT:
        dwMsg = MSG_PTZ_RIGHT;
        break;
    case PTZ_CMD_UPLEFT:
        dwMsg = MSG_PTZ_UPLEFT;
        break;
    case PTZ_CMD_UPRIGHT:
        dwMsg = MSG_PTZ_UPRIGHT;
        break;
    case PTZ_CMD_DOWNLEFT:
        dwMsg = MSG_PTZ_DOWNLEFT;
        break;
    case PTZ_CMD_DOWNRIGHT:
        dwMsg = MSG_PTZ_DOWNRIGHT;
        break;
    case PTZ_CMD_ZOOMIN:
        dwMsg = MSG_PTZ_ZOOMIN;
        break;
    case PTZ_CMD_ZOOMOUT:
        dwMsg = MSG_PTZ_ZOOMOUT;
        break;
    case PTZ_CMD_FOCUSNEAR:
        dwMsg = MSG_PTZ_FOCUSNEAR;
        break;
    case PTZ_CMD_FOCUSFAR:
        dwMsg = MSG_PTZ_FOCUSFAR;
        break;
    case PTZ_CMD_IRISOPEN:
        dwMsg = MSG_PTZ_IRISOPEN;
        break;
    case PTZ_CMD_IRISCLOSE:
        dwMsg = MSG_PTZ_IRISCLOSE;
        break;
    case PTZ_CMD_AUTOSCAN:
        dwMsg = MSG_PTZ_AUTOSCAN;
        break;
    case PTZ_CMD_CRUISE:
        dwMsg = MSG_PTZ_CRUISE;
        break;
    case PTZ_CMD_SETPRESET:
        dwMsg = MSG_PTZ_SETPRESET;
        break;
    case PTZ_CMD_CLEARPRESET:
        dwMsg = MSG_PTZ_CLEARPRESET;
        break;
    case PTZ_CMD_CALLPRESET:
        dwMsg = MSG_PTZ_CALLPRESET;
        break;
    case PTZ_CMD_RAIN_ON:
        dwMsg = MSG_PTZ_RAINON;
        break;
    case PTZ_CMD_RAIN_OFF:
        dwMsg = MSG_PTZ_RAINOFF;
        break;
    default:
        return FALSE;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_PTZ_CONTROL_EX, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    CONNECTION_DEBUG("PTZ MsgType=%u, MsgSubType=%u, flag=%u\n", msgHead.u16MsgType, msgHead.u16MsgSubType, pParam->stNormal.u32Flag);

    /* 既要返回快，又要判断权限。时间太小会马上超时，建议大于500 */
    if (!Transaction(msgHead, &stReq, buf, 500) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::QueryRecord(const NET_QRECORD_PARAM_S *pParam)
{
    ASSERT(pParam);

    QUERY_RECORD_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        stReq.u8ReqType = 0;
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNO;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }

    stReq.u32Type        = pParam->u32Type;
    stReq.stBegin        = pParam->stBegin;
    stReq.stEnd          = pParam->stEnd;

    WORD  dwMsg = MSG_NO_SUBTYPE;
    if (QRECORD_CALENDAR == pParam->u32QType)
    {
        dwMsg = MSG_RECORD_CALENDAR;
    }
    else if (QRECORD_DETAIL == pParam->u32QType)
    {
        dwMsg = MSG_RECORD_DETAIL;
    }
    else if (QRECORD_DAY_HALFHOUR == pParam->u32QType)
    {
        dwMsg = MSG_RECORD_DAYHALFHOUR;
    }
    else
    {
        return FALSE;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_QUERY_RECORD, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf, 15000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    //RecordPlayStatus("%s end\n", __FUNCTION__);

    if (dwMsg == MSG_RECORD_DETAIL)
    {
        ListDataParser<REC_DETAIL_RSP_S> parser(buf);
        UINT num = parser.GetListNum();
        UINT i = 0;
        for (i = 0; i < num; ++i)
        {
            REC_DETAIL_RSP_S *pRsp = parser.GetAt(i);
            if (pParam->fDetailCallback != NULL)
            {
                pParam->fDetailCallback(pRsp, pParam->pUser);
            }
        }
    }
    else if (dwMsg == MSG_RECORD_CALENDAR)
    {
        ListDataParser<REC_CALENDAR_RSP_S> parser(buf);
        UINT num = parser.GetListNum();
        UINT i = 0;
        for (i = 0; i < num; ++i)
        {
            REC_CALENDAR_RSP_S *pRsp = parser.GetAt(i);
            CONNECTION_DEBUG("%u.%u.%u\n", pRsp->stDay.u32Year, pRsp->stDay.u32Month, pRsp->stDay.u32Day);

            if (pParam->fCalendarCallback != NULL)
            {
                pParam->fCalendarCallback(pRsp, pParam->pUser);
            }
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam)
{
    ASSERT(pParam);

    QUERY_RECORD_REQ_EX_S stReq = {0};

    if (pParam->pRecordReq->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->pRecordReq->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        stReq.u8ReqType = 0;
        ASSERT(pParam->pRecordReq->ps8DeviceIp);
        if (!pParam->pRecordReq->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->pRecordReq->u32ChnNO;
        stReq.u16DevicePort  = pParam->pRecordReq->u16DevicePort;
        stReq.u16SubServPort = pParam->pRecordReq->u16SubServPort;
        INET_PTON(pParam->pRecordReq->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->pRecordReq->ps8SubServIp, stReq.u8SubServIp);
    }

    stReq.u32Type        = pParam->pRecordReq->u32Type;
    stReq.stBegin        = pParam->pRecordReq->stBegin;
    stReq.stEnd          = pParam->pRecordReq->stEnd;

    stReq.u32TriggerType = pParam->u32TriggerType;
    stReq.u32TriggerPort = pParam->u32TriggerPort;

    WORD  dwMsg = MSG_NO_SUBTYPE;
    if (QRECORD_CALENDAR == pParam->pRecordReq->u32QType)
    {
        dwMsg = MSG_RECORD_CALENDAR;
    }
    else if (QRECORD_DETAIL == pParam->pRecordReq->u32QType)
    {
        dwMsg = MSG_RECORD_DETAIL;
    }
    else if (QRECORD_DAY_HALFHOUR == pParam->pRecordReq->u32QType)
    {
        dwMsg = MSG_RECORD_DAYHALFHOUR;
    }
    else
    {
        return FALSE;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_QUERY_RECORD_EX, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    //RecordPlayStatus("%s end\n", __FUNCTION__);

    if (dwMsg == MSG_RECORD_DETAIL)
    {
        ListDataParser<REC_DETAIL_RSP_S> parser(buf);
        UINT num = parser.GetListNum();
        UINT i = 0;
        for (i = 0; i < num; ++i)
        {
            REC_DETAIL_RSP_S *pRsp = parser.GetAt(i);
            if (pParam->pRecordReq->fDetailCallback != NULL)
            {
                pParam->pRecordReq->fDetailCallback(pRsp, pParam->pRecordReq->pUser);
            }
        }
    }
    else if (dwMsg == MSG_RECORD_CALENDAR)
    {
        ListDataParser<REC_CALENDAR_RSP_S> parser(buf);
        UINT num = parser.GetListNum();
        UINT i = 0;
        for (i = 0; i < num; ++i)
        {
            REC_CALENDAR_RSP_S *pRsp = parser.GetAt(i);
            CONNECTION_DEBUG("%u.%u.%u\n", pRsp->stDay.u32Year, pRsp->stDay.u32Month, pRsp->stDay.u32Day);

            if (pParam->pRecordReq->fCalendarCallback != NULL)
            {
                pParam->pRecordReq->fCalendarCallback(pRsp, pParam->pRecordReq->pUser);
            }
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser)
{
    m_fWFLinkageCallback = fWFLinkageCallback;
    m_pWFUser = (VOID *)pUser;

    return TRUE;
}

BOOL Connection::GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser)
{
    WORD  dwMsg = MSG_NO_SUBTYPE;
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_GET_CLIENT_LIST, dwMsg, 0);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, NULL, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    ListDataParser<CLIENT_INFO_RSP_S> parser(buf);
    UINT num = parser.GetListNum();
    UINT i = 0;;
    for (i = 0; i < num; ++i)
    {
        CLIENT_INFO_RSP_S *pRsp = parser.GetAt(i);

        if (fGetClientCallback != NULL)
        {
            fGetClientCallback(pRsp, pUser);
        }
    }

    return TRUE;
}

BOOL Connection::SynchrTime(const NET_SYNTIME_PARAM_S *pParam)
{
    SYNCHR_TIME_REQ_S stReq = {0};

    stReq.u32UserKey = m_stLoginRsp.stRsp.u32UserKey;
    stReq.u8TimeZone = pParam->u8TimeZone;
    stReq.stTime     = pParam->stTime;
    stReq.u8IsSetLoaclTime = pParam->u8IsSetLoaclTime;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SYNCHR_TIME, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::QueryTime(DATETIME_S *pDateTime)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_QUERY_TIME, MSG_NO_SUBTYPE, 0);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, NULL, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    *pDateTime = *((DATETIME_S *)buf.GetData());

    return TRUE;
}


CHN_INFO_S * Connection::GetDeviceData(U32 *u32DevNum)
{
    if (!m_bLogined)
    {
        return FALSE;
    }

    *u32DevNum = m_stLoginRsp.stRsp.u32DeviceNum;

    return (CHN_INFO_S*)m_stLoginRsp.Reserved;
}

BOOL Connection::GetDeviceState(U8 u8Ip[], U16 u16Port)
{
    BOOL bState = FALSE;

    if (!m_bLogined)
    {
        return bState;
    }

    U32 u32Size = m_s32LoginRspSize - sizeof(LOGIN_RSP_S);
    CHN_INFO_S *pDevInfo = (CHN_INFO_S*)m_stLoginRsp.Reserved;
    U32 uCount = u32Size / sizeof(CHN_INFO_S);
    U32 u32Index = 0;
    for (; u32Index < uCount; u32Index++)
    {
        if (pDevInfo[u32Index].u8IPAddr[0] == u8Ip[0]
                && pDevInfo[u32Index].u8IPAddr[1] == u8Ip[1]
                && pDevInfo[u32Index].u8IPAddr[2] == u8Ip[2]
                && pDevInfo[u32Index].u8IPAddr[3] == u8Ip[3]
                && pDevInfo[u32Index].u16Port == u16Port)
        {
            bState = pDevInfo[u32Index].u8IsOnline ? TRUE : FALSE;
            break;
        }
    }
    return bState;
}

BOOL Connection::QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser)
{
    WORD  dwMsg = MSG_NO_SUBTYPE;
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_QUERY_RESOURCE, dwMsg, 0);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, NULL, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    ListDataParser<RESOURCE_RSP_S> parser(buf);
    UINT num = parser.GetListNum();
    UINT i = 0;;
    for (i = 0; i < num; ++i)
    {
        RESOURCE_RSP_S *pRsp = parser.GetAt(i);

        if (fResourceCallback)
        {
            fResourceCallback(pRsp, pUser);
        }
    }

    return TRUE;
}

HANDLE Connection::InitDualtalk(const NET_DTALK_PARAM_S *pParam)
{
    if (!m_bInitDTalk)
    {
        m_pDualtalk = new Dualtalk(this);
    }

    m_pDualtalk->SetDTalkParam(
        pParam->u32CollectFlag,
        pParam->hLocalWnd,
        pParam->hRemoteWnd,
        pParam->hMsgWnd,
        pParam->fDTalkMsgCallback,
        pParam->fDTalkDataCallback,
        pParam->pUser);
    if (m_pDualtalk->Start() < 0)
    {
        return NULL;
    }

    m_bInitDTalk = TRUE;
    return m_pDualtalk;
}

VOID Connection::UnInitDualtalk(HANDLE hDTalk)
{
    if (hDTalk)
    {
        ASSERT(hDTalk == m_pDualtalk);
    }

    if (!m_pDualtalk)
    {
        return;
    }

    m_pDualtalk->Stop();

    delete m_pDualtalk;
    m_pDualtalk  = NULL;
    m_bInitDTalk = FALSE;
}

BOOL  Connection::LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_CLIENT_LINKAGE, MSG_NO_SUBTYPE, sizeof(LINKAGE_TRANS_RPT_S));
    msgHead.u2Version = m_u32MsgVersion;
    LINKAGE_TRANS_RPT_S linkTrans = {0};
    memcpy(&linkTrans, pLinkTrans, sizeof(LINKAGE_TRANS_RPT_S));
    if (!Transaction(msgHead, &linkTrans, buf, 10) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}


BOOL Connection::RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
    ASSERT(pParam);

    PTZ_CTRL_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        stReq.u8ReqType = 0;
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNO;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }

    stReq.u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;
    stReq.stNormal       = pParam->stNormal;
    stReq.stCruise       = pParam->stCruise;
    stReq.stPreset       = pParam->stPreset;

    WORD  dwMsg = MSG_NO_SUBTYPE;
    switch (u32Cmd)
    {
    case PTZ_CMD_UP:
        dwMsg = MSG_PTZ_UP;
        break;
    case PTZ_CMD_DOWN:
        dwMsg = MSG_PTZ_DOWN;
        break;
    case PTZ_CMD_LEFT:
        dwMsg = MSG_PTZ_LEFT;
        break;
    case PTZ_CMD_RIGHT:
        dwMsg = MSG_PTZ_RIGHT;
        break;
    case PTZ_CMD_UPLEFT:
        dwMsg = MSG_PTZ_UPLEFT;
        break;
    case PTZ_CMD_UPRIGHT:
        dwMsg = MSG_PTZ_UPRIGHT;
        break;
    case PTZ_CMD_DOWNLEFT:
        dwMsg = MSG_PTZ_DOWNLEFT;
        break;
    case PTZ_CMD_DOWNRIGHT:
        dwMsg = MSG_PTZ_DOWNRIGHT;
        break;
    case PTZ_CMD_ZOOMIN:
        dwMsg = MSG_PTZ_ZOOMIN;
        break;
    case PTZ_CMD_ZOOMOUT:
        dwMsg = MSG_PTZ_ZOOMOUT;
        break;
    case PTZ_CMD_FOCUSNEAR:
        dwMsg = MSG_PTZ_FOCUSNEAR;
        break;
    case PTZ_CMD_FOCUSFAR:
        dwMsg = MSG_PTZ_FOCUSFAR;
        break;
    case PTZ_CMD_IRISOPEN:
        dwMsg = MSG_PTZ_IRISOPEN;
        break;
    case PTZ_CMD_IRISCLOSE:
        dwMsg = MSG_PTZ_IRISCLOSE;
        break;
    case PTZ_CMD_AUTOSCAN:
        dwMsg = MSG_PTZ_AUTOSCAN;
        break;
    case PTZ_CMD_CRUISE:
        dwMsg = MSG_PTZ_CRUISE;
        break;
    case PTZ_CMD_SETPRESET:
        dwMsg = MSG_PTZ_SETPRESET;
        break;
    case PTZ_CMD_CLEARPRESET:
        dwMsg = MSG_PTZ_CLEARPRESET;
        break;
    case PTZ_CMD_CALLPRESET:
        dwMsg = MSG_PTZ_CALLPRESET;
        break;
    case PTZ_CMD_RAIN_ON:
        dwMsg = MSG_PTZ_RAINON;
        break;
    case PTZ_CMD_RAIN_OFF:
        dwMsg = MSG_PTZ_RAINOFF;
        break;
    default:
        break;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_CAR_CONTROL, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    CONNECTION_DEBUG("Rail car MsgType=%u, MsgSubType=%u, flag=%u\n", msgHead.u16MsgType, msgHead.u16MsgSubType, pParam->stNormal.u32Flag);

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart)
{
    CONTROL_RECORD_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }

    ASSERT(pParam->ps8DeviceIp);
    if (!pParam->ps8DeviceIp)
    {
        return FALSE;
    }

    stReq.bStart         = bStart;
    stReq.u32ChnNO       = pParam->u32ChnNO;
    stReq.u16DevicePort  = pParam->u16DevicePort;
    stReq.u16SubServPort = pParam->u16SubServPort;
    INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
    INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);

    WORD  dwMsg = MSG_NO_SUBTYPE;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_CONTROL_RECORD, dwMsg, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    CONNECTION_DEBUG("Control Record MsgType=%u, MsgSubType=%u, flag=%u\n", msgHead.u16MsgType, msgHead.u16MsgSubType, bStart);

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SetParameter(const NET_PARAMSET_PARAM_S *pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->pParamData);

    MsgBuf buf;

    WORD subMsgType;
    if (pParam->u32Type == PARAM_TYPE_DV)
    {
        subMsgType = MSG_PARAM_DV;
    }
    else if (pParam->u32Type == PARAM_TYPE_CRUISE)
    {
        subMsgType = MSG_PARAM_TOUR;
    }
    else if (pParam->u32Type == PARAM_TYPE_DV_EX)
    {
        subMsgType = MSG_PARAM_DV_EX;
    }
    else if (pParam->u32Type == PARAM_TYPE_DV_EX0815)
    {
        subMsgType = MSG_PARAM_DV_EX0815;
    }

    MsgHeadWrap msgHead(MSG_SET_PARAM, subMsgType, pParam->u32ParamSize);
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, pParam->pParamData, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SetParameterEx(const S8 * pParam)
{
    U32 u32Len = strlen(pParam);
    if (0 == u32Len)
    {
        return FALSE;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_PARAM_EX, 0, u32Len);
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, pParam, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SetDTalkState(U32 u32DTalkState)
{

    WORD  dwMsg = MSG_NO_SUBTYPE;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_DTALK_STATE, dwMsg, sizeof(u32DTalkState));
    msgHead.u2Version = m_u32MsgVersion;
    CONNECTION_DEBUG("Set DTalk State MsgType=%u, MsgSubType=%u, flag=%u\n", msgHead.u16MsgType, msgHead.u16MsgSubType, u32DTalkState);

    if (!Transaction(msgHead, &u32DTalkState, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::GetChnPreset(const NET_GETPRESET_PARAM_S *pParam)
{
    ASSERT(pParam);

    GET_PRESET_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNo;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }
    stReq.u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_GET_PRESET, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    ListDataParser<GET_PRESET_RSP_S> parser(buf);
    UINT num = parser.GetListNum();
    UINT i = 0;
    for (i = 0; i < num; ++i)
    {
        GET_PRESET_RSP_S *pRsp = parser.GetAt(i);

        if (pParam->fnPresetListCallback != NULL)
        {
            pParam->fnPresetListCallback(pRsp->u32PresetNo, pRsp->s8PresetName, pParam->pUser);
        }
    }

    return TRUE;
}

BOOL Connection::SetChnPreset(const NET_SETPRESET_PARAM_S *pParam)
{
    ASSERT(pParam);

    S32 s32ReqSize = sizeof(GET_PRESET_REQ_S) + sizeof(GET_PRESET_RSP_S) * pParam->u32PresetNum;
    S8 *pReqBuf = new S8[s32ReqSize];

    GET_PRESET_REQ_S *pstReq    = (GET_PRESET_REQ_S *)pReqBuf;
    GET_PRESET_RSP_S *pstPreset = (GET_PRESET_RSP_S *)(pReqBuf + sizeof(GET_PRESET_REQ_S));

    if (pParam->ps8FrontAddrCode)
    {
        pstReq->u8ReqType = 1;
        strncpy(pstReq->s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            delete []pReqBuf;
            return FALSE;
        }

        pstReq->u8ReqType      = 0;
        pstReq->u32ChnNO       = pParam->u32ChnNo;
        pstReq->u16DevicePort  = pParam->u16DevicePort;
        pstReq->u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  pstReq->u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, pstReq->u8SubServIp);
    }

    pstReq->u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;

    S32 s32I = 0;
    for (s32I = 0; s32I < pParam->u32PresetNum; s32I++)
    {
        pstPreset->u32StSize   = sizeof(GET_PRESET_RSP_S);
        pstPreset->u32PresetNo = pParam->pstPresetList[s32I].u32PresetNo;
        strncpy(pstPreset->s8PresetName, pParam->pstPresetList[s32I].ps8PresetName, MAX_NAME_LEN);
        pstPreset++;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_PRESET, MSG_NO_SUBTYPE, s32ReqSize);
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, pReqBuf, buf) || !buf.IsSuccess())
    {
        delete []pReqBuf;
        return FALSE;
    }

    delete []pReqBuf;
    return TRUE;
}

BOOL Connection::GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam)
{
    ASSERT(pParam);

    GET_PRESET_REQ_S stReq = {0};

    if (pParam->ps8FrontAddrCode)
    {
        stReq.u8ReqType = 1;
        strncpy(stReq.s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            return FALSE;
        }

        stReq.u32ChnNO       = pParam->u32ChnNo;
        stReq.u16DevicePort  = pParam->u16DevicePort;
        stReq.u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  stReq.u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, stReq.u8SubServIp);
    }
    stReq.u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_GET_PRESET_EX, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, &stReq, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    ListDataParser<GET_PRESET_RSP_S_EX> parser(buf);
    UINT num = parser.GetListNum();
    UINT i = 0;
    for (i = 0; i < num; ++i)
    {
        GET_PRESET_RSP_S_EX *pRsp = parser.GetAt(i);

        if (pParam->fnPresetListCallback != NULL)
        {
            pParam->fnPresetListCallback(pRsp->u32PresetNo, pRsp->s8PresetName, pRsp->u32PresetType, pParam->pUser);
        }
    }

    return TRUE;
}

BOOL Connection::SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam)
{
    ASSERT(pParam);

    S32 s32ReqSize = sizeof(GET_PRESET_REQ_S) + sizeof(GET_PRESET_RSP_S_EX) * pParam->u32PresetNum;
    S8 *pReqBuf = new S8[s32ReqSize];

    GET_PRESET_REQ_S    *pstReq    = (GET_PRESET_REQ_S *)pReqBuf;
    GET_PRESET_RSP_S_EX *pstPreset = (GET_PRESET_RSP_S_EX *)(pReqBuf + sizeof(GET_PRESET_REQ_S));

    if (pParam->ps8FrontAddrCode)
    {
        pstReq->u8ReqType = 1;
        strncpy(pstReq->s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
    }
    else
    {
        ASSERT(pParam->ps8DeviceIp);
        if (!pParam->ps8DeviceIp)
        {
            delete []pReqBuf;
            return FALSE;
        }

        pstReq->u8ReqType      = 0;
        pstReq->u32ChnNO       = pParam->u32ChnNo;
        pstReq->u16DevicePort  = pParam->u16DevicePort;
        pstReq->u16SubServPort = pParam->u16SubServPort;
        INET_PTON(pParam->ps8DeviceIp,  pstReq->u8DeviceIp);
        INET_PTON(pParam->ps8SubServIp, pstReq->u8SubServIp);
    }

    pstReq->u32UserKey     = m_stLoginRsp.stRsp.u32UserKey;

    S32 s32I = 0;
    for (s32I = 0; s32I < pParam->u32PresetNum; s32I++)
    {
        pstPreset->u32StSize    = sizeof(GET_PRESET_RSP_S);
        pstPreset->u32PresetNo  = pParam->pstPresetList[s32I].u32PresetNo;
        strncpy(pstPreset->s8PresetName, pParam->pstPresetList[s32I].ps8PresetName, MAX_NAME_LEN);
        pstPreset->u32PresetType = pParam->pstPresetList[s32I].u32PresetType;
        pstPreset++;
    }

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_PRESET_EX, MSG_NO_SUBTYPE, s32ReqSize);
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, pReqBuf, buf) || !buf.IsSuccess())
    {
        delete []pReqBuf;
        return FALSE;
    }

    delete []pReqBuf;
    return TRUE;
}

#ifdef _WIN32
VOID Connection::DrawPreset(const NET_PRESET_DRAW * pParam)
{
    PreviewClient *pPViewClient = (PreviewClient*)(*(pParam->hPView));
    Preview    *pPView          = (Preview*)pPViewClient->pPView;
    if (pPView == NULL)
    {
        return;
    }

    pPView->DrawPreset(pParam->ps8PresetName, pParam->u8WndNum, pParam->u16Left, pParam->u16Top);
}

BOOL Connection::SetRegDraw(const NET_REGDRAW_PARAM * pParam)
{
    PreviewClient *pPViewClient = (PreviewClient*)(*(pParam->hPView));
    Preview    *pPView          = (Preview*)pPViewClient->pPView;
    if (pPView == NULL)
    {
        return FALSE;
    }

    pPView->SetRegDraw(pParam->pfnDraw, pParam->hUser);

    return TRUE;
}

BOOL Connection::SetDecDataCallBack(const NET_DECFRAME_PARAM * pParam)
{
    PreviewClient *pPViewClient = (PreviewClient*)(*(pParam->hPView));
    Preview    *pPView          = (Preview*)pPViewClient->pPView;
    if (pPView == NULL)
    {
        return FALSE;
    }

    pPView->SetDecDataCallBack(pParam->pfnDecCb, pParam->hUser);

    return TRUE;
}
#endif

BOOL Connection::StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
{
    IVA_ANALYSE_PARAM  stReq = {0};

    U8 u8IvaIp[MAX_IPV4_LEN] = {0};
    INET_PTON(pParam->ps8IvaIp, u8IvaIp);
    memcpy(stReq.u8IvaIp, u8IvaIp, MAX_IPV4_LEN);

    U8 u8DevIp[MAX_IPV4_LEN] = {0};
    INET_PTON(pParam->ps8DevIp, u8DevIp);
    memcpy(stReq.u8DeviceIp, u8DevIp, MAX_IPV4_LEN);

    stReq.u16IvaPort    = pParam->u16IvaPort;
    stReq.u16DevicePort = pParam->u16DevPort;
    stReq.u32ChnNO      = pParam->u8DevChn;
    stReq.u8DevPreset   = pParam->u8DevPreset;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_START_IVAANALYSE, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam)
{
    IVA_ANALYSE_PARAM  stReq = {0};

    U8 u8IvaIp[MAX_IPV4_LEN] = {0};
    INET_PTON(pParam->ps8IvaIp, u8IvaIp);
    memcpy(stReq.u8IvaIp, u8IvaIp, MAX_IPV4_LEN);

    U8 u8DevIp[MAX_IPV4_LEN] = {0};
    INET_PTON(pParam->ps8DevIp, u8DevIp);
    memcpy(stReq.u8DeviceIp, u8DevIp, MAX_IPV4_LEN);

    stReq.u16IvaPort    = pParam->u16IvaPort;
    stReq.u16DevicePort = pParam->u16DevPort;
    stReq.u32ChnNO      = pParam->u8DevChn;
    stReq.u8DevPreset   = pParam->u8DevPreset;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_STOP_IVAANALYSE, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam)
{
    if (0 == pParam->u32RuleListNum)
    {
        return FALSE;
    }

    U32  u32MsgSize = pParam->u32RuleListNum * sizeof(TERM_RULE_INFO);
    U8 * pu8Msg = new U8[u32MsgSize];

    do
    {
        if (NULL == pu8Msg)
        {
            break;
        }

        TERM_RULE_INFO * pTermRuleList     = (TERM_RULE_INFO *)pu8Msg;
        NET_THERM_RULE_INFO * pTermRuleInfo = (NET_THERM_RULE_INFO *)pParam->pThermRuleList;

        for (S32 s32I = 0; s32I < pParam->u32RuleListNum; ++s32I)
        {
            strncpy(pTermRuleList->s8RuleName, pTermRuleInfo[s32I].ps8RuleName, MAX_NAME_LEN);
            INET_PTON(pTermRuleInfo[s32I].ps8DevIp, pTermRuleList->u8DevIp);
            pTermRuleList->u16DevPort     = pTermRuleInfo[s32I].u16DevPort;
            pTermRuleList->u8ChnNo        = pTermRuleInfo[s32I].u8ChnNo;
            pTermRuleList->u8PresetNo     = pTermRuleInfo[s32I].u8PresetNo;
            pTermRuleList->u8RuleId       = pTermRuleInfo[s32I].u8RuleId;
            pTermRuleList->u8Enable       = pTermRuleInfo[s32I].u8Enable;
            pTermRuleList->fEmissivity    = pTermRuleInfo[s32I].fEmissivity;
            pTermRuleList->fDistance      = pTermRuleInfo[s32I].fDistance;
            pTermRuleList->fReflexValue   = pTermRuleInfo[s32I].fReflexValue;
            pTermRuleList->stFrom.fxRatio = pTermRuleInfo[s32I].stFrom.fxRatio;
            pTermRuleList->stFrom.fyRatio = pTermRuleInfo[s32I].stFrom.fyRatio;
            pTermRuleList->stTo.fxRatio   = pTermRuleInfo[s32I].stTo.fxRatio;
            pTermRuleList->stTo.fyRatio   = pTermRuleInfo[s32I].stTo.fyRatio;

            pTermRuleList++;
        }

        MsgHeadWrap msgHead;
        msgHead.SetMsgHead(MSG_SET_TERM_RULE, MSG_NO_SUBTYPE, u32MsgSize);
        msgHead.u2Version = m_u32MsgVersion;

        BOOL bRet = SendReq(msgHead, pu8Msg);
        if (!bRet)
        {
            break;
        }

        delete [] pu8Msg;

        return TRUE;
    }
    while (0);

    delete [] pu8Msg;

    return FALSE;
}

BOOL Connection::SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam)
{
    TERM_SHOWMODE_PARAM  stReq = {0};

    U8 u8DevIp[MAX_IPV4_LEN] = {0};
    INET_PTON(pParam->ps8DevIp, u8DevIp);
    memcpy(stReq.u8DevIp, u8DevIp, MAX_IPV4_LEN);

    stReq.u16DevPort = pParam->u16DevPort;
    stReq.u32ChnNo   = pParam->u8ChnNo;
    stReq.u8TermMode = pParam->mode;

    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_TERM_MODE, MSG_NO_SUBTYPE, sizeof(stReq));
    msgHead.u2Version = m_u32MsgVersion;

    if (!Transaction(msgHead, &stReq, buf) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}


BOOL Connection::GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_GET_PRESET_JSON, MSG_NO_SUBTYPE, u32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8PresetReq, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    *ps8PresetInfo = (S8 *)buf.GetData();

    return TRUE;
}

BOOL Connection::SendHSResource(const S8 * ps8Resource, S32 s32Size)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_SET_HS_RESOURCE, MSG_NO_SUBTYPE, s32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8Resource, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_DO_PRESET_JSON, MSG_NO_SUBTYPE, s32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8PresetInfo, buf, 8000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::SendCommand(const S8 * ps8info, S32 s32Size)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_DO_SEND_COMMAND, MSG_NO_SUBTYPE, s32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8info, buf, 5000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Connection::GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo)
{
    MsgBuf buf;
    MsgHeadWrap msgHead(MSG_CALL_IVAALARM, MSG_NO_SUBTYPE, u32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8Req, buf, 5000) || !buf.IsSuccess())
    {
        return FALSE;
    }

    *ps8AlarmInfo = (S8 *)buf.GetData();

    return TRUE;
}

BOOL Connection::GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList)
{
	MsgBuf buf;
    MsgHeadWrap msgHead(MSG_GET_IVACRUISELIST, MSG_NO_SUBTYPE, u32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8Req, buf, 5000) || !buf.IsSuccess())
    {
        return FALSE;
    }
	
    *ps8IvaCruiseList = (S8 *)buf.GetData();

	return TRUE;
}

BOOL Connection::ControlIvaCruise(const S8 * ps8Req, U32 u32Size)
{
	MsgBuf buf;
    MsgHeadWrap msgHead(MSG_CONTROL_IVACRUISE, MSG_NO_SUBTYPE, u32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8Req, buf, 5000) || !buf.IsSuccess())
    {
        return FALSE;
    }

	return TRUE;
}

BOOL Connection::QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList)
{
	MsgBuf buf;
    MsgHeadWrap msgHead(MSG_QUERY_IVALIST, MSG_NO_SUBTYPE, u32Size);
    msgHead.u2Version = m_u32MsgVersion;
    if (!Transaction(msgHead, ps8Req, buf, 5000) || !buf.IsSuccess())
    {
        return FALSE;
    }

	*ps8IvaList = (S8 *)buf.GetData();
	
	return TRUE;
}

BOOL Connection::AddStandby(const S8 *ip, U32 port)
{
	AutoLock lock(m_lsHost.m_Mutex);
	Host host;
	strcpy(host.ip, ip);
	host.port = port;

	TList<Host>::iterator it = find(m_lsHost.begin(),m_lsHost.end(),host);
	if (it == m_lsHost.end())
	{
		m_lsHost.push_back(host);
	}
	
	return TRUE;
}

BOOL Connection::DelStandby(const S8 *ip, U32 port)
{
	AutoLock lock(m_lsHost.m_Mutex);
	Host host;
	strcpy(host.ip, ip);
	host.port = port;
	
	TList<Host>::iterator it = find(m_lsHost.begin(),m_lsHost.end(),host);
	if (it != m_lsHost.end())
	{
		m_lsHost.erase(it);
	}
	
	return TRUE;
}

void Connection::SwitchHost()
{
    AutoLock lock(m_lsHost.m_Mutex);
	if (0==m_lsHost.size())
	{
       return;
	}

    TList<Host>::iterator it = find(m_lsHost.begin(),m_lsHost.end(),m_host);
	if (it == m_lsHost.end())
	{
		m_host = *(m_lsHost.begin());	
	}
	else
	{
        ++it;
		if (it == m_lsHost.end())
		{
			m_host = *(m_lsHost.begin());
		}
		else
		{
			m_host = (*it);
		}
	}

	m_szIP = m_host.ip;
	m_u16Port = m_host.port;
}


HANDLE   Connection::StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam)
{
	RecordPlay *pRecPlay = new RecordPlay(this);
	if (pRecPlay)
	{
		if (pRecPlay->Start(pParam))
		{
			return pRecPlay;
		}
		
		delete pRecPlay;
	}  
    return NULL;
}

HANDLE   Connection::StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
	Download *pDownload = new Download(this);
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}   
    return NULL;
}


HANDLE   Connection::StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
	BaseDownloadByTime *pDownload = new DownloadByTime(this);
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}
    return NULL;   
}

HANDLE Connection::InitDevTalk(int audioType)
{
    if (NULL != m_pDualTalkEx)
    {
        return m_pDualTalkEx;	
    }

    m_pDualTalkEx = new DualtalkEx(this, audioType);

    if (NULL == m_pDualTalkEx)
    {	    
        return NULL;
    }

    S32 s32Ret = m_pDualTalkEx->Start();
    if (s32Ret < 0)
    {
	    delete m_pDualTalkEx;
	    m_pDualTalkEx = NULL;
        return NULL;
    }

    return m_pDualTalkEx;			
}


/* 获取对讲通道*/
int Connection::GetTalkChannel()
{
    if (m_pDualTalkEx == NULL)
    {
        return -1;
    }
    return m_pDualTalkEx->GetTalkChannel();
}