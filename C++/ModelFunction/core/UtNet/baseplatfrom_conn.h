#ifndef  __BASEPLATFORM_CONN_H__
#define  __BASEPLATFORM_CONN_H__

#include "ut_net.h"

class BasePlatformConn
{
public:
    BasePlatformConn(){}
    virtual ~BasePlatformConn(){}

	    /* ����������͵�¼�������� */
    virtual BOOL   Start(const NET_CONN_PARAM_S *pParam) = 0;

	    /* �رյ�¼���� */
    virtual VOID   Stop()= 0;

	    /* ����ʵʱԤ������ */
    virtual HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam)= 0;

	    /* ��ֹʵʱԤ�� */
    virtual VOID   StopPreview(HANDLE *hPView) = 0;
	    
	    /* ������̨�������� */
    virtual BOOL   PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)= 0;
    virtual BOOL   PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)= 0;

	    /* ¼���ļ��б��ѯ */
    virtual BOOL   QueryRecord(const NET_QRECORD_PARAM_S *pParam)= 0;
    virtual BOOL   QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam)= 0;

	    /* ����Ϣѭ�� */
    virtual VOID   MainMsgLoop()= 0;
	
	    /* ��ʼ¼��ط� */
    virtual HANDLE   StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam)= 0;
	
	    /* ��ʼ¼������ */
    virtual HANDLE   StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam)= 0;
	    
		/* ����ʱ�� ��ʼ¼������  */
    virtual HANDLE   StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)= 0;
	
    /*  ��ӱ�����ý�������*/
    virtual BOOL   AddStandby(const S8 *ip, U32 port){return TRUE;}

    /*  ɾ��������ý�������*/
	virtual BOOL   DelStandby(const S8 *ip, U32 port){return TRUE;}
	
		/* ��ȡ��¼������Ϣ */
	virtual	BOOL   GetLoginRsp(LOGIN_RSP_S *pRsp){return TRUE;}
	
		/**< ��ȡ�û��б���Ϣ */
	virtual	BOOL   GetUserList(const NET_USER_PARAM_S *pParam){return TRUE;}
	
		/**< ��ȡ�豸�б���Ϣ */
	virtual	BOOL   GetDeviceList(const NET_DEVICE_PARAM_S *pParam){return TRUE;}
	
		/* ������������Ϸ���һ���ؼ�֡ */
	virtual	BOOL   RequestIFrame(const NET_REQI_PARAM_S *pParam){return TRUE;}
	
		/* Ԥ�������л� */
	virtual	BOOL   PreviewSwitch(const NET_SWITCH_PARAM_S *pParam){return TRUE;}
	
		/* �����л� */
	virtual	BOOL   MatrixSwitch(const NET_MATRIX_PARAM_S *pParam){return TRUE;}

		/* �����������л� */
	virtual	BOOL   DecoderSwitch(const NET_DECODER_PARAM_S *pParam){return TRUE;}

		/* ��������ȡ��ʾͨ�� */
	virtual	BOOL   DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam){return TRUE;}

		/* ��������ʼ��̬���� */
	virtual	BOOL   DecoderStartDynamic(
				U8  * pu8DeviceIp,
				U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
				S32   s32DecChnNo,
				S32   s32DisOutPort,
				S32   s32DisWndNo,
				const NET_DEC_CHN_PARAM_S *pParam){return TRUE;}

		/* ������ֹͣ��̬���� */
	virtual	BOOL  DecoderStopDynamic(
			    U8 * pu8DeviceIp,
                U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
			    S32   s32DisOutPort){return TRUE;}
				
		/* ������������ص����� */
	virtual	BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser){return TRUE;}
		
		/* ���߿ͻ��б��ѯ liuwei 20140307 */
	virtual	BOOL   GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser){return TRUE;}
		
		/* ʱ��ͬ�� */
	virtual	BOOL   SynchrTime(const NET_SYNTIME_PARAM_S *pParam){return TRUE;}

		/* ��ѯʱ�� */
	virtual	BOOL   QueryTime(DATETIME_S *pDateTime){return TRUE;}
	
		/* ��ȡ�豸����״̬ */
	virtual	BOOL   GetDeviceState(U8 u8Ip[MAX_IPV4_LEN], U16 u16Port){return TRUE;}
	
		/* ��Դ��ȡ */
	virtual	BOOL   QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser){return TRUE;}

    
    /*��ʱ��ʵ�֣������Խ�����һ�׽ӿ� InitDevTalk*/
		/* ��ʼ���Խ��ŵ� */
	virtual	HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam){return NULL;}
	
		/* ȥ��ʼ���Խ��ŵ� */
    virtual	VOID   UnInitDualtalk(HANDLE hDTalk = NULL){}



		/* ����ת�� */
	virtual	BOOL   LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans){return TRUE;}
		
		/* ��ȡ�豸��Ϣ(���ݿ�) */
	virtual	CHN_INFO_S * GetDeviceData(U32 *u32DevNum){return NULL;}

		/* ���͹������������ */
	virtual	BOOL   RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam){return TRUE;}

		/* ������ֹͣĳ�������ͨ����¼�� */
	virtual	BOOL   ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart){return TRUE;}

		/* ��������(������ģʽ) */
    virtual BOOL   SetParameter(const NET_PARAMSET_PARAM_S *pParam){return TRUE;}
	
		/* ��������(jsonģʽ) */
	virtual	BOOL   SetParameterEx(const S8 * pParam){return TRUE;}
				
		/* ��������Ƶ�Խ�״̬ */
	virtual	BOOL   SetDTalkState(U32 u32DTalkState){return TRUE;}

		/* Ԥ��λ����(�ɹ�Լ) */
	virtual	BOOL   GetChnPreset(const NET_GETPRESET_PARAM_S *pParam){return TRUE;}
	virtual	BOOL   SetChnPreset(const NET_SETPRESET_PARAM_S *pParam){return TRUE;}

		/* Ԥ��λ����(�¹�Լ) */
	virtual	BOOL   GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam){return TRUE;}
	virtual	BOOL   SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam){return TRUE;}

#ifdef _WIN32
		/* ����Ԥ��λ��Ϣ����Ƶ���� */
	virtual	VOID   DrawPreset(const NET_PRESET_DRAW *pParam){}
		
		/* ע���ͼ�ص����� */
	virtual BOOL   SetRegDraw(const NET_REGDRAW_PARAM *pParam){return TRUE;}

		/* ����ص����� */
	virtual	BOOL   SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam) = 0;
#endif

		/* ���ܷ��� */
	virtual	BOOL   StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam){return TRUE;}
	virtual	BOOL   StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam){return TRUE;}

		/* ��ʼ���豸�Խ� */
	virtual	HANDLE InitDevTalk(int audioType){return NULL;}

		/* ������� */
	virtual	BOOL   SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam){return NULL;}
	virtual	BOOL   SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam){return NULL;}
	
		/*��ȡ��ý����Ԥ��λ��Ϣ*/
	virtual	BOOL   GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo){return TRUE;}
		
		/*�·���ʢ��(AXF|EST|DV|LINE)*/
	virtual	BOOL   SendHSResource(const S8 * ps8Resource, S32 s32Size){return TRUE;}

		/*���û�ɾ����ý����Ԥ��λ��Ϣ*/
	virtual	BOOL   SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size){return TRUE;}

		/*�·������ý��(json��ʽ)*/
    virtual BOOL   SendCommand(const S8 * ps8info, S32 s32Size){return TRUE;}

		/*��ȡ���ܷ������ٸ澯*/
	virtual	BOOL   GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo){return TRUE;}
		
		/*��ȡ���ܷ�����Ѳ�б�*/
	virtual	BOOL   GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList){return TRUE;}

		/*�������ܷ�����Ѳ��ͣ*/
	virtual	BOOL   ControlIvaCruise(const S8 * ps8Req, U32 u32Size){return TRUE;}
	
		/*��ѯ���ܷ��������б�*/
	virtual	BOOL   QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList){return TRUE;}

		/*��ѯӲ��¼���ͨ����¼��״̬*/
	virtual	BOOL QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState){return TRUE;}
	
		/*���������ȡ���ؽ��*/
	virtual	BOOL GetResponse(const S8 * ps8Req, U32 u32Size, S8 *p8Rsp,U32 &u32RspSize){return TRUE;}

protected: 
	
private:

};

#endif 
