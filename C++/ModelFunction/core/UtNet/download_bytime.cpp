#include "download_bytime.h"
#include "connection.h"
#include "excep.h"
#include "fs_rwc1.h"

DownloadByTime::DownloadByTime(Connection *pConn)
    :BaseDownloadByTime()
{
    m_pConn        = pConn;
    m_bExit        = FALSE;
    m_bAutoReconn  = TRUE;
    m_u32Pos       = 0;
    m_pSaveFile    = NULL;
    m_u32DownloadMode = 1;

    m_bRecord            = FALSE;
    m_pRecWriter         = NULL;
    m_bRecordSwitch      = FALSE;

	m_pWriteData   = NULL;
	m_u32WriteSize = 0;
}

BOOL DownloadByTime::Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S * pParam)
{
//	ASSERT(pParam->ps8DeviceIp);
//	ASSERT(pParam->ps8SubServIp);

    m_stRecDldPrarm   = *pParam;
    m_fStatusCallback = pParam->fStatusCallback;
    m_pUser           = pParam->pUser;

    m_bAutoReconn = TRUE;

    m_u32DownloadMode = 1;
    if (pParam->ps8FrontAddrCode != NULL)
    {
        m_strFrontAddrCode = pParam->ps8FrontAddrCode;
        m_u32DownloadMode  = 2;

        if (pParam->ps8FileName)
        {
            AutoLock lock(m_mtxRecord);
            if (m_pRecWriter)
            {
                delete m_pRecWriter;
                m_pRecWriter = NULL;
            }

            m_pRecWriter = new RecordWriter();
            if (!m_pRecWriter)
            {
                DOWNLOAD_STATUS(MSG_DOWNLOAD_STOREFAIL);
                return FALSE;
            }

            if (!m_pRecWriter->open(pParam->ps8FileName))
            {
                DOWNLOAD_STATUS(MSG_DOWNLOAD_STOREFAIL);
                delete m_pRecWriter;
                m_pRecWriter = NULL;
            }
            m_bRecord = TRUE;
        }
    }
    else
    {
        m_szDeviceIp       = pParam->ps8DeviceIp;
        m_szSubServIp      = pParam->ps8SubServIp;
        m_u16DevicePort    = pParam->u16DevicePort;
        m_u16SubServPort   = pParam->u16SubServPort;

        if (pParam->ps8FileName)
        {
            m_pSaveFile = fopen(pParam->ps8FileName, "wb");
            if (!m_pSaveFile)
            {
                DOWNLOAD_STATUS(MSG_DOWNLOAD_STOREFAIL);
                return  FALSE;
            }
        }
		
		m_pWriteData = new U8[MAX_FWRITE_SIZE];
    }

    CreateJoinableThread(RecDownloadLoop, this, &m_trdDownload);

    return  TRUE;
}

VOID DownloadByTime::Stop()
{
    m_bExit = TRUE;
    m_bAutoReconn = FALSE;
    m_sockDownload.KillSocket();

    if (2 == m_u32DownloadMode)
    {
        AutoLock lock(m_mtxRecord);
        if (m_bRecord)
        {
            if (m_pRecWriter)
            {
                delete m_pRecWriter;
                m_pRecWriter = NULL;
            }
            m_bRecord = FALSE;
        }
    }

    ThreadJoin(m_trdDownload);
	if (m_pWriteData)
	{
		delete []m_pWriteData;
		m_pWriteData = NULL;
	}
}

VOID *DownloadByTime::RecDownloadLoop(VOID *pArg)
{
    DownloadByTime *pDown = (DownloadByTime *)pArg;
    pDown->Recv();

    return 0;
}

VOID DownloadByTime::Recv()
{
    do
    {
        if (!m_pConn->IsLogined())
        {
            Msleep(100);
            continue;
        }

        REC_PLAY_REQ_S stReq;
        stReq.u32UserKey = m_pConn->GetUserKey();

        if (1 == m_u32DownloadMode)
        {
            stReq.u8PlayMode     = REC_PLAY_IP_TIME;
            stReq.stIpMode.stBegin        = m_stRecDldPrarm.stBegin;
            stReq.stIpMode.stEnd          = m_stRecDldPrarm.stEnd;
            stReq.stIpMode.u32ChnNO       = m_stRecDldPrarm.u32ChnNO;
            stReq.stIpMode.u32Type        = 0xFFFF;
            stReq.stIpMode.u16DevicePort  = m_stRecDldPrarm.u16DevicePort;
            stReq.stIpMode.u16SubServPort = m_stRecDldPrarm.u16SubServPort;
            INET_PTON(m_szDeviceIp.c_str(),  stReq.stIpMode.u8DeviceIp);
            INET_PTON(m_szSubServIp.c_str(), stReq.stIpMode.u8SubServIp);
            printf("open record %u:%u:%u\n", stReq.stIpMode.stBegin.u32Hour, stReq.stIpMode.stBegin.u32Minute, stReq.stIpMode.stBegin.u32Second);
        }
        else if (2 == m_u32DownloadMode)
        {
            stReq.u8PlayMode = REC_PLAY_ADDR_TIME;
            strncpy(stReq.stAddrMode.s8FrontAddrCode, m_strFrontAddrCode.c_str(), MAX_ADDR_LEN);

            stReq.stAddrMode.stBegin = m_stRecDldPrarm.stBegin;
            stReq.stAddrMode.stEnd   = m_stRecDldPrarm.stEnd;
        }

        MsgHeadWrap head(MSG_PLAYBACK, MSG_NO_SUBTYPE, sizeof(stReq));
        head.u2Version = m_pConn->m_u32MsgVersion;

        MsgBuf buf;

        BOOL bRet = m_pConn->IsLogined()
                    && m_sockDownload.Socket(TRUE)
                    && m_sockDownload.SetSendTimeOut(SEND_TIMEOUT)
                    && m_sockDownload.SetRecvTimeOut(RECV_TIMEOUT)
                    && m_sockDownload.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE)
                    && ::SendMsg(m_sockDownload, head, &stReq)
                    && RecvMsg(m_sockDownload, buf)
                    && buf.ValidRsp(head);
        if (!bRet)
        {
            DOWNLOAD_STATUS(MSG_DOWNLOAD_FAIL);

            m_sockDownload.Close();
            if (m_bAutoReconn)
            {
                Msleep(1000);
                continue;
            }

            return;
        }

        if (!buf.IsSuccess())
        {
            DOWNLOAD_STATUS(MSG_DOWNLOAD_NOFILE);

            m_sockDownload.Close();
            if (m_bAutoReconn)
            {
                Msleep(1000);
                continue;
            }

            return;
        }

        MsgHeadWrap dataReq(MSG_PLAY_FEEDBACK, MSG_PLAY_INSUFFICIENT);
        dataReq.u2Version = m_pConn->m_u32MsgVersion;
        ::SendMsg(m_sockDownload, dataReq, NULL);

        RecvFiles();

    }
    while (m_bAutoReconn);

    m_sockDownload.Close();

    if (m_pSaveFile)
    {
        fclose(m_pSaveFile);
        m_pSaveFile = NULL;
    }

    DOWNLOAD_STATUS(MSG_DOWNLOAD_CLOSED);
}

VOID DownloadByTime::RecvFiles()
{
    FrameBuf frameBuf;
    PackBuf  packBuf;

    do
    {
        if (!packBuf.RecvPackTcp(m_sockDownload))
        {
			DOWNLOAD_STATUS(MSG_DOWNLOAD_OK);
			m_sockDownload.Close();
			m_bAutoReconn = FALSE;
            return;
        }

        if (packBuf.IsFileEnd() ||
                packBuf.IsNoData())
        {
            /* 回调一个结束帧 */
            if (m_stRecDldPrarm.fRecordDataCallback
                    && !m_stRecDldPrarm.ps8FileName)
            {
                ((UT_FRAME_HEAD_S *)frameBuf.GetFrame())->FrameType = FRAME_END;
                m_stRecDldPrarm.fRecordDataCallback(
                    frameBuf.GetFrame(),
                    0,
                    m_stRecDldPrarm.pUser);
            }

            m_bAutoReconn = FALSE;
            frameBuf.ClearUnCompleteFrame();

            /* 进度条调整到结束位置 */
            m_u32Pos = 100;

            DOWNLOAD_STATUS(MSG_DOWNLOAD_OK);

            break;
        }

        if (frameBuf.FrameComplete(packBuf))
        {
            UT_FRAME_HEAD_S* pHead = (UT_FRAME_HEAD_S*)frameBuf.GetFrame();
            if (pHead == NULL)
            {
                return ;
            }
            if (m_stRecDldPrarm.fRecordDataCallback
                    && !m_stRecDldPrarm.ps8FileName)
            {
                m_stRecDldPrarm.fRecordDataCallback(frameBuf.GetFrame(),
                                                    frameBuf.GetFrameTotalSize(),
                                                    m_stRecDldPrarm.pUser);
            }

            if (m_stRecDldPrarm.ps8FileName)
            {
                if (2 == m_u32DownloadMode)
                {
                    Record(frameBuf.GetFrame());
                }
                else
                {
                    if (m_pSaveFile)
                    {
						U32 frameSize = frameBuf.GetFrameTotalSize() - 40;
						if (m_u32WriteSize + frameSize > MAX_FWRITE_SIZE)
						{
							fwrite(m_pWriteData, m_u32WriteSize, 1, m_pSaveFile);
							m_u32WriteSize = 0;
							/* 如果下载速度大于写硬盘的速度，socket会超时，所以休眠控制下载过快 */
							Msleep(5);
						}
						else
						{
							memcpy(m_pWriteData + m_u32WriteSize, (S8 *)frameBuf.GetFrame() + 40, frameSize);
							m_u32WriteSize += frameSize;
						}						
                    }
                }
            }
        }

    }
    while (!m_bExit);
}

U32  DownloadByTime::GetDownloadPos()
{
    if (m_u32Pos > 100)
    {
        m_u32Pos = 100;
    }

    return m_u32Pos;
}

VOID DownloadByTime::Record(VOID *pFrame)
{
#ifdef _WIN32
#else
    AutoLock lock(m_mtxRecord);
    if (!m_bRecord)
    {
        return;
    }

    if (!m_pRecWriter)
    {
        return;
    }

    UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)pFrame;

    if (m_bRecordSwitch &&
            FRAME_I == pFrameHead->FrameType)
    {
        IWriter *pRecWriter = new RecordWriter;
        if (pRecWriter)
        {
            if (!pRecWriter->open(m_strFilePathSwitch.c_str()))
            {

                delete pRecWriter;
                pRecWriter = NULL;
            }
            else
            {
                delete m_pRecWriter;
                m_pRecWriter = pRecWriter;
            }

            m_bRecordSwitch = FALSE;
        }
    }

    m_pRecWriter->write(pFrame);
#endif
}
