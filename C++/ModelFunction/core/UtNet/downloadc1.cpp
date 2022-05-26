#include "downloadc1.h"
#include "connection.h"
#include "excep.h"
#include "fs_rwc1.h"

Download::Download(Connection *pConn)
    :BaseDownload()
{
	m_pConn        = pConn;
	m_bExit        = FALSE;
	m_bAutoReconn  = TRUE;
	m_u64FileSzie  = 0;
	m_u32Pos       = 0;
	m_pSaveFile    = NULL;
	m_pRecordFiles = NULL;
	m_u32DownloadMode = 1;

	m_bRecord            = FALSE;
	m_pRecWriter         = NULL;
	m_bRecordSwitch      = FALSE;
	m_bDownloadStardard  = FALSE;
}

BOOL Download::Start(const NET_RECDOWNLOAD_PARAM_S * pParam)
{
//	ASSERT(pParam->ps8DeviceIp);
//	ASSERT(pParam->ps8SubServIp);

	m_stRecDldPrarm   = *pParam; 
	m_fStatusCallback = pParam->fStatusCallback;
	m_pUser           = pParam->pUser;

	U32 u32Num = 1;
	m_bAutoReconn = TRUE;
	m_u64FileLastlen = 0;
	m_pRecordFiles   = NULL;

	if (!m_pRecordFiles)
	{
		m_pRecordFiles = new REC_DETAIL_RSP_S[u32Num];
		if (!m_pRecordFiles)
			return  FALSE;
	}

	memset(m_pRecordFiles, 0, sizeof(REC_DETAIL_RSP_S));
	m_pRecordFiles[0].u32StSize = sizeof(REC_DETAIL_RSP_S);
	m_pRecordFiles[0].u32Type   = RECORD_ALL;
	m_pRecordFiles[0].u32Size   = pParam->u32FileSize;
	strncpy(m_pRecordFiles[0].s8FileName, pParam->ps8RecFileName, MAX_FILENAME_LEN);

	m_u64FileSzie    = pParam->u32FileSize;
	if (0 == m_u64FileSzie)
	{
		return FALSE;
	}

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
			std::string fileName = pParam->ps8FileName;
			if (fileName.find("cctv.utv") != string::npos)
			{
				m_bDownloadStardard = true;
				
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
			else
			{			
				m_pSaveFile = fopen(pParam->ps8FileName, "wb");
				if (!m_pSaveFile)
				{
					DOWNLOAD_STATUS(MSG_DOWNLOAD_STOREFAIL);
					return  FALSE;
				}
			}
		}
	}

	CreateJoinableThread(RecDownloadLoop, this, &m_trdDownload);

	return  TRUE;	
}

VOID Download::Stop()
{
	m_bExit = TRUE;
	m_bAutoReconn = FALSE;
	m_u64FileLastlen = 0;
	m_sockDownload.KillSocket();

	if (2 == m_u32DownloadMode ||
		m_bDownloadStardard)
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

	if (m_pRecordFiles)
	{
		delete []m_pRecordFiles;
	}
}

VOID *Download::RecDownloadLoop(VOID *pArg)
{
	Download *pDown = (Download *)pArg;
	pDown->Recv();

	return 0;
}

VOID Download::Recv()
{
	do 
	{
		if (!m_pConn->IsLogined())
		{
			Msleep(100);
			continue;
		}

		U32 u32Num  = 1;
		U32 msgSize = sizeof(REC_DOWNLOAD_REQ_S) + u32Num * sizeof(REC_DETAIL_RSP_S);
		S8 *pData   = new S8[msgSize];

		REC_DOWNLOAD_REQ_S &stReq = *(REC_DOWNLOAD_REQ_S *)pData;
		stReq.u32UserKey = m_pConn->GetUserKey();

		if (1 == m_u32DownloadMode)
		{
			stReq.u8DownloadMode = 1;
			stReq.u32ChnNO       = 0;
			
			stReq.u16DevicePort  = m_u16DevicePort;
			stReq.u16SubServPort = m_u16SubServPort;
			INET_PTON(m_szDeviceIp.c_str(),  stReq.u8DeviceIp);
			INET_PTON(m_szSubServIp.c_str(), stReq.u8SubServIp);
		}
		else if (2 == m_u32DownloadMode)
		{
			stReq.u8DownloadMode  = 2;
			strncpy(stReq.s8FrontAddrCode, m_strFrontAddrCode.c_str(), MAX_ADDR_LEN);
		}
		
		memcpy(pData + sizeof(REC_DOWNLOAD_REQ_S), m_pRecordFiles, u32Num * sizeof(REC_DETAIL_RSP_S));

		MsgHeadWrap head(MSG_DOWNLOAD, 0, msgSize);
		head.u2Version = m_pConn->m_u32MsgVersion;
		MsgBuf buf;

		m_sockDownload.Close();
		BOOL bRet = m_sockDownload.Socket(TRUE) 
			&& m_sockDownload.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE) 
			&& SendMsg(m_sockDownload, head, pData) 
			&& RecvMsg(m_sockDownload, buf) 
			&& buf.ValidRsp(head);

		delete [] pData;

		if (!bRet)
		{
			if(m_bAutoReconn)
			{
				m_sockDownload.Close();
				Msleep(1000);
				continue;
			}

			return;
		}

		if (buf.IsSuccess())
		{
			m_u32Num = u32Num;
			if (!m_sockDownload.SetSendTimeOut(SEND_TIMEOUT) 
					|| !m_sockDownload.SetRecvTimeOut(RECV_TIMEOUT)
					|| !m_sockDownload.SetRecvBuf(16*1024*1024))
			{
				return;
			}			
		}
		else
		{
			if(m_bAutoReconn)
			{
				m_sockDownload.Close();
				Msleep(1000);
				continue;
			}
		}

		RecvFiles();

	} while (m_bAutoReconn);


	m_sockDownload.Close();

	if (m_pSaveFile)
	{
		fclose(m_pSaveFile);
		m_pSaveFile = NULL;
	}

	DOWNLOAD_STATUS(MSG_DOWNLOAD_CLOSED);
}

VOID Download::RecvFiles()
{
	FrameBuf frameBuf;
	PackBuf  packBuf;
	U32      u32Num = 0;

	for (u32Num = 0; u32Num < m_u32Num && !m_bExit; ++u32Num)
	{
		U64 size = m_u64FileLastlen;
		do 
		{
			if (!packBuf.RecvPackTcp(m_sockDownload))
			{
				return;
			}

			if (packBuf.IsFileEnd())
			{
				if (size == 0)
				{
					DOWNLOAD_STATUS(MSG_DOWNLOAD_NOFILE);
				}
				else
				{
					DOWNLOAD_STATUS(MSG_DOWNLOAD_OK);
				}

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
					if (2 == m_u32DownloadMode ||
						m_bDownloadStardard)
					{
						Record(frameBuf.GetFrame());
					}
					else
					{
						if (m_pSaveFile)
						{
							fwrite((S8 *)frameBuf.GetFrame() + 40, frameBuf.GetFrameTotalSize() - 40, 1, m_pSaveFile);
						}
					}	
				}

				size += frameBuf.GetFrameTotalSize() - 40;
				m_u64FileLastlen = size;

				/* 计算进度条 */
				m_u32Pos = (100 * m_u64FileLastlen) / m_u64FileSzie;

				//cctv成都华创按接收字节来做结束判断
                S64 temppp = m_u64FileLastlen - m_u64FileSzie;
                if (abs(temppp) <= 44)
				{
					m_u32Pos = 100;
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
					break;
				}

				/* 如果下载速度大于写硬盘的速度，socket会超时，所以休眠控制下载过快 */
				Msleep(1);
			}

		} while (!m_bExit);
	}

	if (u32Num != m_u32Num)
	{
		DOWNLOAD_STATUS(MSG_DOWNLOAD_FAIL);
	}
}

U32  Download::GetDownloadPos()
{
	if (m_u32Pos > 100)
		m_u32Pos = 100;

	return m_u32Pos;	
}

VOID Download::Record(VOID *pFrame)
{
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
}
