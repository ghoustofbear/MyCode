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

	//启动登录
	BOOL Start(const NET_CONN_PARAM_S *pParam);	

	//停止
	VOID Stop();	

	//启动某一通道开始预览
	HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam); 

	//停止预览
	VOID StopPreview(HANDLE *hPViewCilent);

	//云台控制
	BOOL PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

	//云台控制
	BOOL PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
	
	//录像查找
	BOOL QueryRecord(const NET_QRECORD_PARAM_S *pParam); 
	
	//录像查找(增加告警类型)
	BOOL QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam);  

	VOID MainMsgLoop();

	// 开始录像回放 
	HANDLE StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam);	    
	
	// 开始录像下载 
	HANDLE StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam);	
	
	// 根据时间 开始录像下载 
	HANDLE StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);

	//添加一个登录备服务器地址
	BOOL AddStandby(const S8 *ip, U32 port); 

	//删除一个登录备服务器地址
	BOOL DelStandby(const S8 *ip, U32 port);	

	//获取登录流媒体服务器信息
	BOOL GetLoginRsp(LOGIN_RSP_S *pRsp);  
	
	//获取用户列表
	BOOL GetUserList(const NET_USER_PARAM_S *pParam); 
	
	//获取设备列表
	BOOL GetDeviceList(const NET_DEVICE_PARAM_S *pParam); 

	//请求实时帧产生一个I帧
	BOOL RequestIFrame(const NET_REQI_PARAM_S *pParam);	 

	//预览切换
	BOOL PreviewSwitch(const NET_SWITCH_PARAM_S *pParam);
	
	//矩阵切换
	BOOL MatrixSwitch(const NET_MATRIX_PARAM_S *pParam);
   
	//解码器画面切换
	BOOL DecoderSwitch(const NET_DECODER_PARAM_S *pParam);

	//查询智能分析分析列表
	BOOL QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList);

	//控制智能分析轮巡启停
	BOOL ControlIvaCruise(const S8 * ps8Req, U32 u32Size);

	//获取智能分析轮巡列表
	BOOL GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList);
	BOOL GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo);
	BOOL SendCommand(const S8 * ps8info, S32 s32Size);
	BOOL SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size);

	//下发和盛资(AXF|EST|DV|LINE)
	BOOL SendHSResource(const S8 * ps8Resource, S32 s32Size);
	BOOL GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo);
	BOOL SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam);
	BOOL SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam);
	BOOL StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
	BOOL StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam);
	BOOL SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam);
	BOOL SetRegDraw(const NET_REGDRAW_PARAM * pParam);
	VOID DrawPreset(const NET_PRESET_DRAW * pParam);

	//设置预置位操作(新规约)
	BOOL SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam);

	//获取预置位操作(新规约)
	BOOL GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam);

	//设置预置位操作(旧规约)
	BOOL SetChnPreset(const NET_SETPRESET_PARAM_S *pParam);

	//获取预置位操作(旧规约)
	BOOL GetChnPreset(const NET_GETPRESET_PARAM_S *pParam);

	//设置音视频对讲状态
	BOOL SetDTalkState(U32 u32DTalkState);

	//参数设置(json模式)
	BOOL SetParameterEx(const S8 * pParam);

	//参数设置(二进制模式)
	BOOL SetParameter(const NET_PARAMSET_PARAM_S *pParam);

	//启动或停止某个摄像机通道的录像
	BOOL ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);
	BOOL RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);
	CHN_INFO_S * GetDeviceData(U32 *u32DevNum);
	
	//联动转发
	BOOL LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans);

	//
	VOID UnInitDualtalk(HANDLE hDTalk = NULL);
	HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam);

	//资源获取
	BOOL QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser);

	//获取设备在线状态
	BOOL GetDeviceState(U8 u8Ip[], U16 u16Port);

	//查询时间
	BOOL QueryTime(DATETIME_S *pDateTime);

	//时间同步
	BOOL SynchrTime(const NET_SYNTIME_PARAM_S *pParam);

	//在线客户列表查询
	BOOL GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser);
	BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);

	//查询硬盘录像机通道的录像状态
	BOOL QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState);

	//发送命令并获取返回结果
	BOOL GetResponse(const S8 * ps8Req, U32 u32Size, S8 *p8Rsp,U32 &u32RspSize);

	//解码器开始动态解码
	BOOL DecoderStartDynamic(
		U8  * pu8DeviceIp,
		U16   u16DevicePort,
		U8  * pu8SubServIp,
		U16   u16SubServPort,
		S32   s32DecChnNo,
		S32   s32DisOutPort,
		S32   s32DisWndNo,
    const NET_DEC_CHN_PARAM_S *pParam);

	//解码器停止动态解码
	BOOL DecoderStopDynamic(
    U8 * pu8DeviceIp,
    U16 u16DevicePort,
    U8 * pu8SubServIp,
    U16 u16SubServPort,
    S32 s32DisOutPort);

	//解码器获取显示通道
	BOOL DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam);
 
	friend int C2AlarmCallback(const char* json, int size, void* userdata);
	friend VOID C2DeviceListCallback(c2_devinfo_s *info, void *userdata);	
	friend VOID C2GetDeviceDataCallback(c2_devinfo_s *info, void* userdata);
	friend void C2LoginCallback(int code, void* userdata);
	friend void C2RecordCallback(c2_record_s *record, int realNum, int fromIndex, int toIndex, void* userdata);
	friend void C2PresetCallback(c2_preset_s *preset, void *pUser);
	friend void C2PresetCallbackEx(c2_preset_s *preset, void *pUser);
	friend void C2UserInfoCallback(c2_user_info_s *info, void* userdata);		
	
   /* 初始化设备对讲 */
   HANDLE InitDevTalk(int audioType);

	HANDLE m_hLogin;

	pfnAlarmCallback m_pfnAlarmCallback;  //告警回调函数
	pfnConnCallback m_pfnConnCallback;	//连接回调函数
	pfnDeviceListCallback m_pfnDeviceListCallback;//设备列表信息回调函数
	pfnQueryDetailCallback  m_pfnQueryDetailCallback; //录像列表回调
	pfnQueryCalendarCallback m_pfnQueryCalendarCallback;  //哪些天有录像回调
	pfnPresetListCallback m_pfnPresetListCallback; //设备列表数据回调
	pfnPresetListCallbackEx m_pfnPresetListCallbackEx;	 //设备列表数据回调(新规约)
	pfnUserListCallback m_pfnUserListCallback;//用户列表数据回调
	pfnWFLinkageCallback m_pfnWFLinkageCallback;	//五防联动回调

	void * m_pUserPreset;
	void * m_pUserDevice;
	void * m_pUserLogin; 
	void * m_pUserRecord;
	void * m_pUserData;	
	void * m_pUserWF;	
	void * m_pAlarmUserData;

	vector<C2PlatformPreview *> GetPreview();
	vector<c2_devinfo_s> m_vcDeviceInfo;	//设备信息vector
private: 

	char m_ip[MAX_ADDR_LEN];
	char m_subName[MAX_OBJECTNAME_LEN];
	char m_subIp[MAX_ADDR_LEN];
	char m_addr[MAX_ADDR_LEN];
	char m_password[MAX_OBJECTNAME_LEN];
	int m_port;
	int m_subPort;
 
	Mutex m_mtxPView;  
	vector<C2PlatformPreview *> m_vcPViews;       /**< 当前预览实例数组 */

	CHN_INFO_S  *m_devInfo;  //设备信息
	char out[MAX_STRING_LEN]; 

	string m_presetList;
	string m_chanRecordStatus;//通道录像状态

	//语音对讲句柄
    C2PlatformDualtalkEx * m_pDualTalkEx;
};

#endif // !defined(AFX_C2PLATFORM_CONN_H__74FFA44C_AA92_47DA_B361_EDD0CD209215__INCLUDED_)
