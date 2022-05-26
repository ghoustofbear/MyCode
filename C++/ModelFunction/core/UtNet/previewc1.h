/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * 预览 preview defines.
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

		/* 厂商类型定义 */
		enum
		{
			MANU_TYPE_HIK,
			MANU_TYPE_DAHUA,	
			MANU_TYPE_UT,
			MANU_TYPE_TIANDY,
			MANU_TYPE_HELMET,
		};

		/* 媒体帧类型定义 */
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

		/* 开始预览 */
		PreviewClient *Start(const NET_PVIEW_PARAM_S *pParam);

		/* 停止预览 */
		BOOL           Stop(PreviewClient* pPViewClient);
		VOID           Stop();

#ifdef _WIN32
		/* 注册绘图回调函数 */
		VOID           DrawPreset(S8 *ps8PresetName, U8 u8WndNum, U16 u16Left, U16 u16Top);
		VOID           DrawPreset(HDC &_hDc);
		VOID           SetRegDraw(HANDLE hDraw, HANDLE hUser);
#endif

		/* 解码回调函数 */
        VOID           SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser);

		/* 停止预览 */
		BOOL           StopAll();

		BOOL           IsSameStream(const NET_PVIEW_PARAM_S* pParam);

		/* 获取预览响应结构体 */
		BOOL           GetResponse(PVIEW_RSP_S *pRsp);

		/* 开始实时录像 */
		BOOL           SaveRealData(S8 *ps8FileName);

		/* 停下实时录像 */
		BOOL           StopSaveRealData();

		/* 实时抓拍 */
		BOOL           CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode);

		/* 打开声音 */
		BOOL                OpenSound();
		/* 关闭声音 */
		BOOL                CloseSound();

	protected:
		static VOID * PreviewFun(VOID *pArg);
		BOOL                IsTcp();

		/* 实时数据接收线程 */
		VOID                PreviewLoop();

		/* 实时数据请求操作 */
		BOOL                OpenStream();

		/* 实时数据接收操作 */
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
		U32                 m_u32Proto;                  /**< 连接类型,TCP或UDP */
		U32                 m_u32ChnNO;                  /**< 请求通道号 */
		U8                  m_u8ReqType;                 /**< 预览请求类型，0-IP+端口 1-前端地址编码 */
		U8                  m_u8Type;                    /**< 大码流或小码流 */
		U8                  m_u8Content;                 /**< 复合流或视频流 */
		U8                  m_u8DeviceIp[MAX_IPV4_LEN];
		U8                  m_u8SubServIp[MAX_IPV4_LEN];
		U16                 m_u16DevicePort;
		U16                 m_u16SubServPort;

		S8                  m_s8FrontAddrCode[MAX_ADDR_LEN];/**< 前面设备地址编码 */
		
		Connection *        m_pConn;                     /**< 设备主连接类指针 */
		BOOL                m_bIsGetUdpPViewKey;

		Mutex               m_mtxPViewParams;
		SyncSocket          m_PViewSock;                 /**< 预览连接链路 */
		Mutex               m_mtxPViewSock;
		THREAD              m_trdPView;                  /**< 当前预览线程ID */
		BOOL volatile       m_bAutoReconn;               /**< 是否重连 */
		BOOL volatile       m_bExit;                     /**< 是否退出 */

		list<PreviewClient> m_PViewClients;              /**< 当前预览实例链表 */
		PVIEW_RSP_S 	    m_PViewRsp;                  /**< 预览请求响应 */

		S8                  m_s8HikSysHeader[40];
		BOOL                m_bWriteHikSysHeader;
		FILE              * m_pSaveFile;                 /**< 实时录像保存文件指针 */

		LONG                m_lPort;
		S32                 m_iPlayerID;
		
		BOOL                m_bSoundState;               /**< 音频状态 */
		U32                 m_u32ManuType;               /**< 厂商类型 */
		U32                 m_u32FrameType;              /**< 媒体帧类型 */

		S8                  m_s8TiandySysHeader[88];
		BOOL                m_bWriteTiandySysHeader;

		BOOL                m_bWriteDahuaSysHeader;

	public:
		S8                  m_s8PresetName[MAX_NAME_LEN];  /**< 预置位名称 */
		U8                  m_u8WndNum;                    /**< 当前窗口数目 */
		U16                 m_u16Px;                       /**< 字符叠加左 */
		U16                 m_u16Py;                       /**< 字符叠加上 */
		HWND                m_hWnd;                        /**< 窗口句柄 */

		typedef struct
		{
             HANDLE         hDraw;                       /**< 窗口绘图回调函数 */
		     HANDLE         hUser;                       /**< 用户数据 */
		}stDrawParam;

		stDrawParam         m_stDrawParam;

        typedef struct
		{
             HANDLE         hDecCb;                      /**< 解码回调函数 */
			 HANDLE         hUser;                       /**< 用户数据 */
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
        S32                 m_s32EncodeType;       /* 编码类型 0x15:h264 0x17:h265 0x29:mjpeg*/

		time_t              m_tmPrevTime;

		Mutex               m_mtxHelmet;
		list< void * >      m_listHelmetData;
	    HANDLE              m_hHelmetPlayCtrl;
		BOOL                m_bTrdPlayExit;
		VARENDER_HANDLE     m_hPlayerHandle;             /**< 音视频句柄 */
        
        HANDLE              m_mixAudioVideo;
};
#endif
