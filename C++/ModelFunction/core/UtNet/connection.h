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

		/* ����������͵�¼�������� */
		BOOL   Start(const NET_CONN_PARAM_S *pParam);

		/* �رյ�¼���� */
		VOID   Stop();

		/* ��ȡ��¼������Ϣ */
		BOOL   GetLoginRsp(LOGIN_RSP_S *pRsp);

		/**< ��ȡ�û��б���Ϣ */
		BOOL   GetUserList(const NET_USER_PARAM_S *pParam);

		/**< ��ȡ�豸�б���Ϣ */
		BOOL   GetDeviceList(const NET_DEVICE_PARAM_S *pParam);

		/* ����ʵʱԤ������ */
		HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam);

		/* ��ֹʵʱԤ�� */
		VOID   StopPreview(HANDLE *hPView);


		/* ������������Ϸ���һ���ؼ�֡ */
		BOOL   RequestIFrame(const NET_REQI_PARAM_S *pParam);

		/* Ԥ�������л� */
		BOOL   PreviewSwitch(const NET_SWITCH_PARAM_S *pParam);

		/* �����л� */
		BOOL   MatrixSwitch(const NET_MATRIX_PARAM_S *pParam);

		/* �����������л� */
		BOOL   DecoderSwitch(const NET_DECODER_PARAM_S *pParam);

		/* ��������ȡ��ʾͨ�� */
		BOOL   DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam);

		/* ��������ʼ��̬���� */
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

		/* ������������ص����� */
		BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);
		
		/* ʱ��ͬ�� */
		BOOL   SynchrTime(const NET_SYNTIME_PARAM_S *pParam);

		/* ��ѯʱ�� */
		BOOL   QueryTime(DATETIME_S *pDateTime);

		/* �Ƿ��Ѿ���¼ */
		BOOL   IsLogined()  { return m_bLogined; };

		/* ��ȡ��¼�ؼ��� */
		U32    GetUserKey() { return m_stLoginRsp.stRsp.u32UserKey; };

		/* ��ȡIP */
		PCSTR  GetIP()      { return m_szIP.c_str(); };

		/* ��ȡ��¼�˿� */
		U16    GetPort()    { return m_u16Port; };

		/* ������̨�������� */
		BOOL   PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
		BOOL   PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

		/* ¼���ļ��б��ѯ */
		BOOL   QueryRecord(const NET_QRECORD_PARAM_S *pParam);
		BOOL   QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam);

		/* ���߿ͻ��б��ѯ liuwei 20140307 */
		BOOL   GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser);

		/* ��ȡ�豸����״̬ liuwei 20140522 */
		BOOL   GetDeviceState(U8 u8Ip[MAX_IPV4_LEN], U16 u16Port);
		
		/* ��ȡ�豸��Ϣ(���ݿ�) liuwei 20140718 */
		CHN_INFO_S * GetDeviceData(U32 *u32DevNum);

		/* ��Դ��ȡ */
		BOOL   QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser);

		/* ��ʼ���Խ��ŵ� */
		HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam);

		/* ��ʼ���Խ��ŵ� */
		VOID   UnInitDualtalk(HANDLE hDTalk = NULL);
		
		/* ����ת�� liuwei 20140624 */
		BOOL   LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans);

		/* ���͹������������ */
		BOOL   RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

		/* ��������(������ģʽ) */
        BOOL   SetParameter(const NET_PARAMSET_PARAM_S *pParam);

		/* ��������(jsonģʽ) */
		/* ������ֹͣĳ�������ͨ����¼�� */
		BOOL   ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);

		/* ��������(jsonģʽ) */
		BOOL   SetParameterEx(const S8 * pParam);
		
		/* ��������Ƶ�Խ�״̬ */
		BOOL   SetDTalkState(U32 u32DTalkState);

		/* Ԥ��λ����(�ɹ�Լ) */
		BOOL   GetChnPreset(const NET_GETPRESET_PARAM_S *pParam);
		BOOL   SetChnPreset(const NET_SETPRESET_PARAM_S *pParam);

		/* Ԥ��λ����(�¹�Լ) */
		BOOL   GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam);
		BOOL   SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam);

		/* ���ܷ��� */
		BOOL   StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
		BOOL   StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);

		/* ������� */
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
        
	   /* ��ʼ¼��ط� */
       HANDLE   StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam);

       /* ��ʼ¼������ */
       HANDLE   StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam);

       /* ����ʱ�� ��ʼ¼������  */
       HANDLE   StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);
       
	   /* ��ʼ���豸�Խ� */
	   HANDLE InitDevTalk(int audioType);

#ifdef _WIN32
		/* ����Ԥ��λ��Ϣ����Ƶ���� */
		VOID   DrawPreset(const NET_PRESET_DRAW *pParam);
		
		/* ע���ͼ�ص����� */
	    BOOL   SetRegDraw(const NET_REGDRAW_PARAM *pParam);

		/* ����ص����� */
		BOOL   SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam);
#endif
		/* ��Ϣ������� */
		BOOL Transaction(const MsgHeadWrap &reqHead, const VOID *reqData, MsgBuf &rspMsg, DWORD milliseconds = 5000)
		{
			MsgTransaction trans;
			trans.Init(reqHead, rspMsg);
			return Transaction(trans, reqData, milliseconds);
		}
		
		/* ��Ϣ������� */
		BOOL Transaction(MsgTransaction &msgTrans, const VOID *reqData = NULL, DWORD milliseconds = 5000)
  		{
			m_MsgTransQueue.Req(msgTrans);

			BOOL hasRsp = FALSE;
			if (SendReq(*msgTrans.m_pReq, (VOID *)reqData))
				hasRsp = msgTrans.WaitRsp(milliseconds);

			m_MsgTransQueue.Clear(msgTrans);
			return hasRsp;
		}

        /* д��Զ������Ƶ����*/
        void WritePDualTalkEx(void *data,int len){m_pDualTalkEx->WriteMuxData(data,len);}

        /* ��ȡ�Խ�ͨ��*/
        int GetTalkChannel();

		/*�ڲ����� */
	protected:
		/* ���Ӳ��� */
		BOOL Connect();

		/* ��¼���� */
		BOOL Login();

		S32  StartMainMsgLoop();

		/* ����Ϣѭ�� */
		VOID MainMsgLoop();

		VOID BeginTransaction();

		VOID EndTransaction();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		BOOL QueryParam(MsgBuf &buf, U32 u32SubType, VOID *pReq, U32 u32Size);

		VOID ProcessAlarmReport(MsgBuf &buf);

		static VOID *ThreadEntry(VOID *pArg);

	public:
			U32               m_u32MsgVersion;  /**< ��Ϣ�汾�� */
	private:
			string            m_szIP;           /**< ��¼IP */
			U16               m_u16Port;        /**< ��¼�˿� */
			string            m_szUsername;     /**< ��¼�û��� */
			string            m_szPassword;     /**< ��¼���� */
			string            m_szName;         /**< �ͻ������� */
			string            m_szClientIp;     /**< �ͻ���IP */
			U16               m_u16CTalkPort;   /**< �ͻ��˶Խ�����˿� */
			HWND              m_hWnd;
			U32               m_u32Msg;
			VOID           *  m_pUser;          /**< �û����� */
			VOID           *  m_pWFUser;        /**< ��������û����� */
			BOOL    volatile  m_bAutoReconn;    /**< �Ƿ��Զ����� */
			BOOL    volatile  m_bAlarmDesired;  /**< �Ƿ��ı�����Ϣ */
			BOOL    volatile  m_bExit;          /**< �Ƿ��˳� */

			pfnAlarmCallback  m_fAlarmCallback; /**< �������ݻص�������� */
			pfnConnCallback   m_fConnCallback;  /**< ��¼״̬��Ϣ�ص�������� */
            pfnWFLinkageCallback m_fWFLinkageCallback; /**< ��������ص�������� */

			S32               m_s32PviewCliNum;
			S32               m_s32LoginRspSize;
			
			struct { 
				LOGIN_RSP_S stRsp; 
				S8 Reserved[1024*40]; 
			} m_stLoginRsp;                     /**< ��¼�������� */

			SyncSocket        m_MainSock;       /**< ����Ϣ��· */
			BOOL              m_bLogined;
			Mutex             m_mtxLogin;
			Mutex             m_mtxTrans;
			THREAD            m_trdMain;
			MsgTransQueue     m_MsgTransQueue;  /**< ��Ϣ���� */

			Mutex             m_mtxPView;
			vector<Preview *> m_vcPViews;       /**< ��ǰԤ��ʵ������ */

			BOOL              m_bInitDTalk;
			Dualtalk        * m_pDualtalk;      /**< �Խ�ͨ������ʵ�� */

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

            //�����Խ����
            DualtalkEx * m_pDualTalkEx;
};


#endif

