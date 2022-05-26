/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * 下载 defines.
 */

#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "basedownload.h"

class Connection;
class IWriter;

class Download : public BaseDownload
{
#define DOWNLOAD_STATUS(code) \
		do \
		{ \
			if (m_fStatusCallback) \
			{ \
				m_fStatusCallback(code, m_pUser); \
			} \
		} while (0);

public:
	/* 构造函数 */
	Download(Connection *pConn);

	/* 开始下载 */
	BOOL Start(const NET_RECDOWNLOAD_PARAM_S * pParam);

	/* 结束下载 */
	VOID Stop();

	/* 获取下载进度 */
	U32  GetDownloadPos();

protected:
	/* 录像文件接收线程 */
	static VOID *RecDownloadLoop(VOID *pArg);

	/* 接收操作 */
	VOID Recv();
	VOID RecvFiles();

	VOID Record(VOID *pFrame);

private:
	pfnRecordDownloadStatusCallback  m_fStatusCallback;  /**< 下载状态回调函数指针 */
	VOID                  * m_pUser;
	NET_RECDOWNLOAD_PARAM_S m_stRecDldPrarm;
	Connection            * m_pConn;                     /**< 主链接指针 */
	BOOL volatile           m_bExit;                     /**< 是否退出 */
	BOOL volatile           m_bAutoReconn;               /**< 是否重连 */

	U64                     m_u64FileSzie;               /**< 文件总长度 */
	U64                     m_u64FileLastlen;            /**< 已下载长度 */
	U32                     m_u32Num;
	SyncSocket              m_sockDownload;              /**< 下载通信链路 */
	REC_DETAIL_RSP_S      * m_pRecordFiles;
	THREAD                  m_trdDownload;               /**< 下载线程ID */

	string                  m_szDeviceIp;
	string                  m_szSubServIp;
	U16                     m_u16DevicePort;
	U16                     m_u16SubServPort;

	FILE                  * m_pSaveFile;                 /**< 下载保存文件指针 */
	U32                     m_u32Pos;                    /**< 下载进度 */

	U32                     m_u32DownloadMode;           /**< 下载模式 1:私有 2:标准 */
	string                  m_strFrontAddrCode;          /**< 设备地址码 */

	
	volatile BOOL           m_bRecord;
	volatile BOOL           m_bRecordSwitch;
	IWriter               * m_pRecWriter;
	Mutex                   m_mtxRecord;
	string                  m_strFilePathSwitch;
	bool                    m_bDownloadStardard;
};

#endif
