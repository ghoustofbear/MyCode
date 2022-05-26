/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * Connection defines.
 */
#ifndef _CONNECTION_H__
#define _CONNECTION_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "tlistc1.h"
#include "baseplatfrom_conn.h"
#include "dualtalkEx.h"

#define CONN_TIMEOUT	  5000
#define SEND_TIMEOUT	  5000
#define RECV_TIMEOUT	  5000
#define DOWNLOAD_TIMEOUT  10000


class Preview;
class Dualtalk;
class Connection:public BasePlatformConn
{
	public:
#ifdef _WIN32
#define LOGIN_STATUS(code) \
		do \
		{ \
			if (m_hWnd) \
			{ \
				PostMessage(m_hWnd, m_u32Msg, (WPARAM)code, (LPARAM)m_pUser); \
			} \
			if (m_fConnCallback) \
			{ \
				m_fConnCallback(code, m_pUser); \
			} \
		} while (0);
#else
#define LOGIN_STATUS(code) \
		do \
		{ \
			if (m_fConnCallback) \
			{ \
				m_fConnCallback(code, m_pUser); \
			} \
		} while (0);
#endif
		Connection();
		virtual ~Connection();

		/* 向服务器发送登录连接请求 */
		BOOL   Start(const NET_CONN_PARAM_S *pParam);

		/* 关闭登录连接 */
		VOID   Stop();

		/* 获取登录返回信息 */
		BOOL   GetLoginRsp(LOGIN_RSP_S *pRsp);

		/**< 获取用户列表信息 */
		BOOL   GetUserList(const NET_USER_PARAM_S *pParam);

		/**< 获取设备列表信息 */
		BOOL   GetDeviceList(const NET_DEVICE_PARAM_S *pParam);

		/* 发起实时预览请求 */
		HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam);

		/* 终止实时预览 */
		VOID   StopPreview(HANDLE *hPView);


		/* 请求服务器马上发送一个关键帧 */
		BOOL   RequestIFrame(const NET_REQI_PARAM_S *pParam);

		/* 预览画面切换 */
		BOOL   PreviewSwitch(const NET_SWITCH_PARAM_S *pParam);

		/* 矩阵切换 */
		BOOL   MatrixSwitch(const NET_MATRIX_PARAM_S *pParam);

		/* 解码器画面切换 */
		BOOL   DecoderSwitch(const NET_DECODER_PARAM_S *pParam);

		/* 解码器获取显示通道 */
		BOOL   DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam);

		/* 解码器开始动态解码 */
		BOOL   DecoderStartDynamic(
				U8  * pu8DeviceIp,
				U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
				S32   s32DecChnNo,
				S32   s32DisOutPort,
				S32   s32DisWndNo,
				const NET_DEC_CHN_PARAM_S *pParam);

		BOOL  DecoderStopDynamic(
			    U8 * pu8DeviceIp,
                U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
			    S32   s32DisOutPort);

		/* 设置五防联动回调函数 */
		BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);
		
		/* 时间同步 */
		BOOL   SynchrTime(const NET_SYNTIME_PARAM_S *pParam);

		/* 查询时间 */
		BOOL   QueryTime(DATETIME_S *pDateTime);

		/* 是否已经登录 */
		BOOL   IsLogined()  { return m_bLogined; };

		/* 获取登录关键码 */
		U32    GetUserKey() { return m_stLoginRsp.stRsp.u32UserKey; };

		/* 获取IP */
		PCSTR  GetIP()      { return m_szIP.c_str(); };

		/* 获取登录端口 */
		U16    GetPort()    { return m_u16Port; };

		/* 发送云台控制命令 */
		BOOL   PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
		BOOL   PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

		/* 录像文件列表查询 */
		BOOL   QueryRecord(const NET_QRECORD_PARAM_S *pParam);
		BOOL   QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam);

		/* 在线客户列表查询 liuwei 20140307 */
		BOOL   GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser);

		/* 获取设备在线状态 liuwei 20140522 */
		BOOL   GetDeviceState(U8 u8Ip[MAX_IPV4_LEN], U16 u16Port);
		
		/* 获取设备信息(数据库) liuwei 20140718 */
		CHN_INFO_S * GetDeviceData(U32 *u32DevNum);

		/* 资源获取 */
		BOOL   QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser);

		/* 初始化对讲信道 */
		HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam);

		/* 初始化对讲信道 */
		VOID   UnInitDualtalk(HANDLE hDTalk = NULL);
		
		/* 联动转发 liuwei 20140624 */
		BOOL   LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans);

		/* 发送轨道车控制命令 */
		BOOL   RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

		/* 参数设置(二进制模式) */
        BOOL   SetParameter(const NET_PARAMSET_PARAM_S *pParam);

		/* 参数设置(json模式) */
		/* 启动或停止某个摄像机通道的录像 */
		BOOL   ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);

		/* 参数设置(json模式) */
		BOOL   SetParameterEx(const S8 * pParam);
		
		/* 设置音视频对讲状态 */
		BOOL   SetDTalkState(U32 u32DTalkState);

		/* 预置位操作(旧规约) */
		BOOL   GetChnPreset(const NET_GETPRESET_PARAM_S *pParam);
		BOOL   SetChnPreset(const NET_SETPRESET_PARAM_S *pParam);

		/* 预置位操作(新规约) */
		BOOL   GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam);
		BOOL   SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam);

		/* 智能分析 */
		BOOL   StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
		BOOL   StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);

		/* 红外测温 */
		BOOL   SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam);
		BOOL   SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam);

		BOOL   GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo);
		BOOL   SendHSResource(const S8 * ps8Resource, S32 s32Size);
		BOOL   SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size);
        BOOL   SendCommand(const S8 * ps8info, S32 s32Size);
		BOOL   GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo);
		BOOL   GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList);
		BOOL   ControlIvaCruise(const S8 * ps8Req, U32 u32Size);
		BOOL   QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList);

		BOOL   AddStandby(const S8 *ip, U32 port);
		BOOL   DelStandby(const S8 *ip, U32 port);
		void   SwitchHost();
        
	   /* 开始录像回放 */
       HANDLE   StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam);

       /* 开始录像下载 */
       HANDLE   StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam);

       /* 根据时间 开始录像下载  */
       HANDLE   StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);
       
	   /* 初始化设备对讲 */
	   HANDLE InitDevTalk(int audioType);

#ifdef _WIN32
		/* 叠加预置位信息到视频画面 */
		VOID   DrawPreset(const NET_PRESET_DRAW *pParam);
		
		/* 注册绘图回调函数 */
	    BOOL   SetRegDraw(const NET_REGDRAW_PARAM *pParam);

		/* 解码回调函数 */
		BOOL   SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam);
#endif
		/* 消息传输操作 */
		BOOL Transaction(const MsgHeadWrap &reqHead, const VOID *reqData, MsgBuf &rspMsg, DWORD milliseconds = 5000)
		{
			MsgTransaction trans;
			trans.Init(reqHead, rspMsg);
			return Transaction(trans, reqData, milliseconds);
		}
		
		/* 消息传输操作 */
		BOOL Transaction(MsgTransaction &msgTrans, const VOID *reqData = NULL, DWORD milliseconds = 5000)
  		{
			m_MsgTransQueue.Req(msgTrans);

			BOOL hasRsp = FALSE;
			if (SendReq(*msgTrans.m_pReq, (VOID *)reqData))
				hasRsp = msgTrans.WaitRsp(milliseconds);

			m_MsgTransQueue.Clear(msgTrans);
			return hasRsp;
		}

        /* 写入远程音视频数据*/
        void WritePDualTalkEx(void *data,int len){m_pDualTalkEx->WriteMuxData(data,len);}

        /* 获取对讲通道*/
        int GetTalkChannel();

		/*内部调用 */
	protected:
		/* 连接操作 */
		BOOL Connect();

		/* 登录操作 */
		BOOL Login();

		S32  StartMainMsgLoop();

		/* 主消息循环 */
		VOID MainMsgLoop();

		VOID BeginTransaction();

		VOID EndTransaction();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		BOOL QueryParam(MsgBuf &buf, U32 u32SubType, VOID *pReq, U32 u32Size);

		VOID ProcessAlarmReport(MsgBuf &buf);

		static VOID *ThreadEntry(VOID *pArg);

	public:
			U32               m_u32MsgVersion;  /**< 消息版本号 */
	private:
			string            m_szIP;           /**< 登录IP */
			U16               m_u16Port;        /**< 登录端口 */
			string            m_szUsername;     /**< 登录用户名 */
			string            m_szPassword;     /**< 登录密码 */
			string            m_szName;         /**< 客户端名称 */
			string            m_szClientIp;     /**< 客户端IP */
			U16               m_u16CTalkPort;   /**< 客户端对讲服务端口 */
			HWND              m_hWnd;
			U32               m_u32Msg;
			VOID           *  m_pUser;          /**< 用户数据 */
			VOID           *  m_pWFUser;        /**< 五防联动用户数据 */
			BOOL    volatile  m_bAutoReconn;    /**< 是否自动重连 */
			BOOL    volatile  m_bAlarmDesired;  /**< 是否订阅报警信息 */
			BOOL    volatile  m_bExit;          /**< 是否退出 */

			pfnAlarmCallback  m_fAlarmCallback; /**< 报警数据回调函数入口 */
			pfnConnCallback   m_fConnCallback;  /**< 登录状态信息回调函数入口 */
            pfnWFLinkageCallback m_fWFLinkageCallback; /**< 五防联动回调函数入口 */

			S32               m_s32PviewCliNum;
			S32               m_s32LoginRspSize;
			
			struct { 
				LOGIN_RSP_S stRsp; 
				S8 Reserved[1024*40]; 
			} m_stLoginRsp;                     /**< 登录返回数据 */

			SyncSocket        m_MainSock;       /**< 主消息链路 */
			BOOL              m_bLogined;
			Mutex             m_mtxLogin;
			Mutex             m_mtxTrans;
			THREAD            m_trdMain;
			MsgTransQueue     m_MsgTransQueue;  /**< 消息队列 */

			Mutex             m_mtxPView;
			vector<Preview *> m_vcPViews;       /**< 当前预览实例数组 */

			BOOL              m_bInitDTalk;
			Dualtalk        * m_pDualtalk;      /**< 对讲通道对象实体 */

			typedef struct Host
			{
                S8   ip[MAX_IP_STR_LEN];
				U32  port;

				bool operator==(const Host &rv)
				{
					return (strcmp(ip, rv.ip)==0) && (port==rv.port);
				}
			}Host;

			Host             m_host;
			TList<Host>      m_lsHost;

            //语音对讲句柄
            DualtalkEx * m_pDualTalkEx;
};


#endif

