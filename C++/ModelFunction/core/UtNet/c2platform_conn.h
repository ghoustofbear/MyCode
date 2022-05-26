// c2platform_conn.h: interface for the c2platform_conn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_CONN_H__74FFA44C_AA92_47DA_B361_EDD0CD209215__INCLUDED_)
#define AFX_C2PLATFORM_CONN_H__74FFA44C_AA92_47DA_B361_EDD0CD209215__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h"
#include "c2sdkc1.h"
#include "concourrencyc1.h"
#include "baseplatfrom_conn.h"
#include "c2Platform_DualtalkEx.h"

#define MAX_STRING_LEN (1024 * 400)

class C2PlatformPreview;
class C2PlatformConn : public BasePlatformConn 
{
public:
	C2PlatformConn();
	~C2PlatformConn();

	//������¼
	BOOL Start(const NET_CONN_PARAM_S *pParam);	

	//ֹͣ
	VOID Stop();	

	//����ĳһͨ����ʼԤ��
	HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam); 

	//ֹͣԤ��
	VOID StopPreview(HANDLE *hPViewCilent);

	//��̨����
	BOOL PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

	//��̨����
	BOOL PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
	
	//¼�����
	BOOL QueryRecord(const NET_QRECORD_PARAM_S *pParam); 
	
	//¼�����(���Ӹ澯����)
	BOOL QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam);  

	VOID MainMsgLoop();

	// ��ʼ¼��ط� 
	HANDLE StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam);	    
	
	// ��ʼ¼������ 
	HANDLE StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam);	
	
	// ����ʱ�� ��ʼ¼������ 
	HANDLE StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);

	//���һ����¼����������ַ
	BOOL AddStandby(const S8 *ip, U32 port); 

	//ɾ��һ����¼����������ַ
	BOOL DelStandby(const S8 *ip, U32 port);	

	//��ȡ��¼��ý���������Ϣ
	BOOL GetLoginRsp(LOGIN_RSP_S *pRsp);  
	
	//��ȡ�û��б�
	BOOL GetUserList(const NET_USER_PARAM_S *pParam); 
	
	//��ȡ�豸�б�
	BOOL GetDeviceList(const NET_DEVICE_PARAM_S *pParam); 

	//����ʵʱ֡����һ��I֡
	BOOL RequestIFrame(const NET_REQI_PARAM_S *pParam);	 

	//Ԥ���л�
	BOOL PreviewSwitch(const NET_SWITCH_PARAM_S *pParam);
	
	//�����л�
	BOOL MatrixSwitch(const NET_MATRIX_PARAM_S *pParam);
   
	//�����������л�
	BOOL DecoderSwitch(const NET_DECODER_PARAM_S *pParam);

	//��ѯ���ܷ��������б�
	BOOL QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList);

	//�������ܷ�����Ѳ��ͣ
	BOOL ControlIvaCruise(const S8 * ps8Req, U32 u32Size);

	//��ȡ���ܷ�����Ѳ�б�
	BOOL GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList);
	BOOL GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo);
	BOOL SendCommand(const S8 * ps8info, S32 s32Size);
	BOOL SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size);

	//�·���ʢ��(AXF|EST|DV|LINE)
	BOOL SendHSResource(const S8 * ps8Resource, S32 s32Size);
	BOOL GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo);
	BOOL SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam);
	BOOL SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam);
	BOOL StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
	BOOL StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
	BOOL SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam);
	BOOL SetRegDraw(const NET_REGDRAW_PARAM * pParam);
	VOID DrawPreset(const NET_PRESET_DRAW * pParam);

	//����Ԥ��λ����(�¹�Լ)
	BOOL SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam);

	//��ȡԤ��λ����(�¹�Լ)
	BOOL GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam);

	//����Ԥ��λ����(�ɹ�Լ)
	BOOL SetChnPreset(const NET_SETPRESET_PARAM_S *pParam);

	//��ȡԤ��λ����(�ɹ�Լ)
	BOOL GetChnPreset(const NET_GETPRESET_PARAM_S *pParam);

	//��������Ƶ�Խ�״̬
	BOOL SetDTalkState(U32 u32DTalkState);

	//��������(jsonģʽ)
	BOOL SetParameterEx(const S8 * pParam);

	//��������(������ģʽ)
	BOOL SetParameter(const NET_PARAMSET_PARAM_S *pParam);

	//������ֹͣĳ�������ͨ����¼��
	BOOL ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);
	BOOL RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
	CHN_INFO_S * GetDeviceData(U32 *u32DevNum);
	
	//����ת��
	BOOL LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans);

	//
	VOID UnInitDualtalk(HANDLE hDTalk = NULL);
	HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam);

	//��Դ��ȡ
	BOOL QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser);

	//��ȡ�豸����״̬
	BOOL GetDeviceState(U8 u8Ip[], U16 u16Port);

	//��ѯʱ��
	BOOL QueryTime(DATETIME_S *pDateTime);

	//ʱ��ͬ��
	BOOL SynchrTime(const NET_SYNTIME_PARAM_S *pParam);

	//���߿ͻ��б��ѯ
	BOOL GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser);
	BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);

	//��ѯӲ��¼���ͨ����¼��״̬
	BOOL QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState);

	//���������ȡ���ؽ��
	BOOL GetResponse(const S8 * ps8Req, U32 u32Size, S8 *p8Rsp,U32 &u32RspSize);

	//��������ʼ��̬����
	BOOL DecoderStartDynamic(
		U8  * pu8DeviceIp,
		U16   u16DevicePort,
		U8  * pu8SubServIp,
		U16   u16SubServPort,
		S32   s32DecChnNo,
		S32   s32DisOutPort,
		S32   s32DisWndNo,
    const NET_DEC_CHN_PARAM_S *pParam);

	//������ֹͣ��̬����
	BOOL DecoderStopDynamic(
    U8 * pu8DeviceIp,
    U16 u16DevicePort,
    U8 * pu8SubServIp,
    U16 u16SubServPort,
    S32 s32DisOutPort);

	//��������ȡ��ʾͨ��
	BOOL DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam);
 
	friend int C2AlarmCallback(const char* json, int size, void* userdata);
	friend VOID C2DeviceListCallback(c2_devinfo_s *info, void *userdata);	
	friend VOID C2GetDeviceDataCallback(c2_devinfo_s *info, void* userdata);
	friend void C2LoginCallback(int code, void* userdata);
	friend void C2RecordCallback(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata);
	friend void C2PresetCallback(c2_preset_s *preset, void *pUser);
	friend void C2PresetCallbackEx(c2_preset_s *preset, void *pUser);
	friend void C2UserInfoCallback(c2_user_info_s *info, void* userdata);		
	
   /* ��ʼ���豸�Խ� */
   HANDLE InitDevTalk(int audioType);

	HANDLE m_hLogin;

	pfnAlarmCallback m_pfnAlarmCallback;  //�澯�ص�����
	pfnConnCallback m_pfnConnCallback;	//���ӻص�����
	pfnDeviceListCallback m_pfnDeviceListCallback;//�豸�б���Ϣ�ص�����
	pfnQueryDetailCallback  m_pfnQueryDetailCallback; //¼���б�ص�
	pfnQueryCalendarCallback m_pfnQueryCalendarCallback;  //��Щ����¼��ص�
	pfnPresetListCallback m_pfnPresetListCallback; //�豸�б����ݻص�
	pfnPresetListCallbackEx m_pfnPresetListCallbackEx;	 //�豸�б����ݻص�(�¹�Լ)
	pfnUserListCallback m_pfnUserListCallback;//�û��б����ݻص�
	pfnWFLinkageCallback m_pfnWFLinkageCallback;	//��������ص�

	void * m_pUserPreset;
	void * m_pUserDevice;
	void * m_pUserLogin; 
	void * m_pUserRecord;
	void * m_pUserData;	
	void * m_pUserWF;	
	void * m_pAlarmUserData;

	vector<C2PlatformPreview *> GetPreview();
	vector<c2_devinfo_s> m_vcDeviceInfo;	//�豸��Ϣvector
private: 

	char m_ip[MAX_ADDR_LEN];
	char m_subName[MAX_OBJECTNAME_LEN];
	char m_subIp[MAX_ADDR_LEN];
	char m_addr[MAX_ADDR_LEN];
	char m_password[MAX_OBJECTNAME_LEN];
	int m_port;
	int m_subPort;
 
	Mutex m_mtxPView;  
	vector<C2PlatformPreview *> m_vcPViews;       /**< ��ǰԤ��ʵ������ */

	CHN_INFO_S  *m_devInfo;  //�豸��Ϣ
	char out[MAX_STRING_LEN]; 

	string m_presetList;
	string m_chanRecordStatus;//ͨ��¼��״̬

	//�����Խ����
    C2PlatformDualtalkEx * m_pDualTalkEx;
};

#endif // !defined(AFX_C2PLATFORM_CONN_H__74FFA44C_AA92_47DA_B361_EDD0CD209215__INCLUDED_)
