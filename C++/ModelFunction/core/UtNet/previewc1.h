/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * Ԥ�� preview defines.
 */

#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "basepview.h"

#ifdef _WIN32
#include <richedit.h>
#endif

#include "helmet_interface_player.h"

class IWriter;
class Preview:public BasePView
{
#ifdef _WIN32
		friend VOID RealDataRenderCallback(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);
		friend VOID *HelmetPreviewFun(VOID *pArg);
#endif

		/* �������Ͷ��� */
		enum
		{
			MANU_TYPE_HIK,
			MANU_TYPE_DAHUA,	
			MANU_TYPE_UT,
			MANU_TYPE_TIANDY,
			MANU_TYPE_HELMET,
		};

		/* ý��֡���Ͷ��� */
		enum
		{
			FRAME_TYPE_HIK,
			FRAME_TYPE_DAHUA,	
			FRAME_TYPE_UT,
			FRAME_TYPE_TIANDY,
			FRAME_TYPE_HI_H264,
			FRAME_TYPE_STD_H264,
			FRAME_TYPE_STD_H264_NALU,
			FRAME_TYPE_STD_H264_FU,
			FRAME_TYPE_STD_H264_FU_PAYLOAD,
			FRAME_TYPE_HELMET_GX,
		};
	public:
		Preview(Connection *pConn);

		/* ��ʼԤ�� */
		PreviewClient *Start(const NET_PVIEW_PARAM_S *pParam);

		/* ֹͣԤ�� */
		BOOL           Stop(PreviewClient* pPViewClient);
		VOID           Stop();

#ifdef _WIN32
		/* ע���ͼ�ص����� */
		VOID           DrawPreset(S8 *ps8PresetName, U8 u8WndNum, U16 u16Left, U16 u16Top);
		VOID           DrawPreset(HDC &_hDc);
		VOID           SetRegDraw(HANDLE hDraw, HANDLE hUser);
#endif

		/* ����ص����� */
        VOID           SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser);

		/* ֹͣԤ�� */
		BOOL           StopAll();

		BOOL           IsSameStream(const NET_PVIEW_PARAM_S* pParam);

		/* ��ȡԤ����Ӧ�ṹ�� */
		BOOL           GetResponse(PVIEW_RSP_S *pRsp);

		/* ��ʼʵʱ¼�� */
		BOOL           SaveRealData(S8 *ps8FileName);

		/* ͣ��ʵʱ¼�� */
		BOOL           StopSaveRealData();

		/* ʵʱץ�� */
		BOOL           CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode);

		/* ������ */
		BOOL                OpenSound();
		/* �ر����� */
		BOOL                CloseSound();

	protected:
		static VOID * PreviewFun(VOID *pArg);
		BOOL                IsTcp();

		/* ʵʱ���ݽ����߳� */
		VOID                PreviewLoop();

		/* ʵʱ����������� */
		BOOL                OpenStream();

		/* ʵʱ���ݽ��ղ��� */
		VOID                RecvStream();

		VOID                Record(VOID *pFrame);
		VOID                SaveSwitch(S8 *ps8FilePath, U32 u32FileType);
	public:
		LONG                GetPort() { return m_lPort;}
        S32                 GetPlayerID() { return m_iPlayerID;}

	public:
#ifdef _WIN32
		CHARFORMAT          m_cfPreset;
#endif
		
	private:
		U32                 m_u32Proto;                  /**< ��������,TCP��UDP */
		U32                 m_u32ChnNO;                  /**< ����ͨ���� */
		U8                  m_u8ReqType;                 /**< Ԥ���������ͣ�0-IP+�˿� 1-ǰ�˵�ַ���� */
		U8                  m_u8Type;                    /**< ��������С���� */
		U8                  m_u8Content;                 /**< ����������Ƶ�� */
		U8                  m_u8DeviceIp[MAX_IPV4_LEN];
		U8                  m_u8SubServIp[MAX_IPV4_LEN];
		U16                 m_u16DevicePort;
		U16                 m_u16SubServPort;

		S8                  m_s8FrontAddrCode[MAX_ADDR_LEN];/**< ǰ���豸��ַ���� */
		
		Connection *        m_pConn;                     /**< �豸��������ָ�� */
		BOOL                m_bIsGetUdpPViewKey;

		Mutex               m_mtxPViewParams;
		SyncSocket          m_PViewSock;                 /**< Ԥ��������· */
		Mutex               m_mtxPViewSock;
		THREAD              m_trdPView;                  /**< ��ǰԤ���߳�ID */
		BOOL volatile       m_bAutoReconn;               /**< �Ƿ����� */
		BOOL volatile       m_bExit;                     /**< �Ƿ��˳� */

		list<PreviewClient> m_PViewClients;              /**< ��ǰԤ��ʵ������ */
		PVIEW_RSP_S 	    m_PViewRsp;                  /**< Ԥ��������Ӧ */

		S8                  m_s8HikSysHeader[40];
		BOOL                m_bWriteHikSysHeader;
		FILE              * m_pSaveFile;                 /**< ʵʱ¼�񱣴��ļ�ָ�� */

		LONG                m_lPort;
		S32                 m_iPlayerID;
		
		BOOL                m_bSoundState;               /**< ��Ƶ״̬ */
		U32                 m_u32ManuType;               /**< �������� */
		U32                 m_u32FrameType;              /**< ý��֡���� */

		S8                  m_s8TiandySysHeader[88];
		BOOL                m_bWriteTiandySysHeader;

		BOOL                m_bWriteDahuaSysHeader;

	public:
		S8                  m_s8PresetName[MAX_NAME_LEN];  /**< Ԥ��λ���� */
		U8                  m_u8WndNum;                    /**< ��ǰ������Ŀ */
		U16                 m_u16Px;                       /**< �ַ������� */
		U16                 m_u16Py;                       /**< �ַ������� */
		HWND                m_hWnd;                        /**< ���ھ�� */

		typedef struct
		{
             HANDLE         hDraw;                       /**< ���ڻ�ͼ�ص����� */
		     HANDLE         hUser;                       /**< �û����� */
		}stDrawParam;

		stDrawParam         m_stDrawParam;

        typedef struct
		{
             HANDLE         hDecCb;                      /**< ����ص����� */
			 HANDLE         hUser;                       /**< �û����� */
		}stDecParam;

		stDecParam          m_stDecParam;

		typedef struct
		{
			long             lWidth;
			long             lHeight;
		}stPicResolution;
		stPicResolution      m_stPicParam;

		volatile BOOL       m_bRecord;
		volatile BOOL       m_bRecordSwitch;
		IWriter           * m_pRecWriter;
		Mutex               m_mtxRecord;
		string              m_strFilePathSwitch;
        S32                 m_s32EncodeType;       /* �������� 0x15:h264 0x17:h265 0x29:mjpeg*/

		time_t              m_tmPrevTime;

		Mutex               m_mtxHelmet;
		list< void * >      m_listHelmetData;
	    HANDLE              m_hHelmetPlayCtrl;
		BOOL                m_bTrdPlayExit;
		VARENDER_HANDLE     m_hPlayerHandle;             /**< ����Ƶ��� */
        
        HANDLE              m_mixAudioVideo;
};
#endif
