/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * ���� defines.
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
	/* ���캯�� */
	Download(Connection *pConn);

	/* ��ʼ���� */
	BOOL Start(const NET_RECDOWNLOAD_PARAM_S * pParam);

	/* �������� */
	VOID Stop();

	/* ��ȡ���ؽ��� */
	U32  GetDownloadPos();

protected:
	/* ¼���ļ������߳� */
	static VOID *RecDownloadLoop(VOID *pArg);

	/* ���ղ��� */
	VOID Recv();
	VOID RecvFiles();

	VOID Record(VOID *pFrame);

private:
	pfnRecordDownloadStatusCallback  m_fStatusCallback;  /**< ����״̬�ص�����ָ�� */
	VOID                  * m_pUser;
	NET_RECDOWNLOAD_PARAM_S m_stRecDldPrarm;
	Connection            * m_pConn;                     /**< ������ָ�� */
	BOOL volatile           m_bExit;                     /**< �Ƿ��˳� */
	BOOL volatile           m_bAutoReconn;               /**< �Ƿ����� */

	U64                     m_u64FileSzie;               /**< �ļ��ܳ��� */
	U64                     m_u64FileLastlen;            /**< �����س��� */
	U32                     m_u32Num;
	SyncSocket              m_sockDownload;              /**< ����ͨ����· */
	REC_DETAIL_RSP_S      * m_pRecordFiles;
	THREAD                  m_trdDownload;               /**< �����߳�ID */

	string                  m_szDeviceIp;
	string                  m_szSubServIp;
	U16                     m_u16DevicePort;
	U16                     m_u16SubServPort;

	FILE                  * m_pSaveFile;                 /**< ���ر����ļ�ָ�� */
	U32                     m_u32Pos;                    /**< ���ؽ��� */

	U32                     m_u32DownloadMode;           /**< ����ģʽ 1:˽�� 2:��׼ */
	string                  m_strFrontAddrCode;          /**< �豸��ַ�� */

	
	volatile BOOL           m_bRecord;
	volatile BOOL           m_bRecordSwitch;
	IWriter               * m_pRecWriter;
	Mutex                   m_mtxRecord;
	string                  m_strFilePathSwitch;
	bool                    m_bDownloadStardard;
};

#endif
