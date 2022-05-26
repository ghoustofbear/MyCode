#ifndef  __BASEPLATFORM_CONN_H__
#define  __BASEPLATFORM_CONN_H__

#include "ut_net.h"

class BasePlatformConn
{
public:
    BasePlatformConn(){}
    virtual ~BasePlatformConn(){}

	    /* 向服务器发送登录连接请求 */
    virtual BOOL   Start(const NET_CONN_PARAM_S *pParam) = 0;

	    /* 关闭登录连接 */
    virtual VOID   Stop()= 0;

	    /* 发起实时预览请求 */
    virtual HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam)= 0;

	    /* 终止实时预览 */
    virtual VOID   StopPreview(HANDLE *hPView) = 0;
	    
	    /* 发送云台控制命令 */
    virtual BOOL   PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)= 0;
    virtual BOOL   PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)= 0;

	    /* 录像文件列表查询 */
    virtual BOOL   QueryRecord(const NET_QRECORD_PARAM_S *pParam)= 0;
    virtual BOOL   QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam)= 0;

	    /* 主消息循环 */
    virtual VOID   MainMsgLoop()= 0;
	
	    /* 开始录像回放 */
    virtual HANDLE   StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam)= 0;
	
	    /* 开始录像下载 */
    virtual HANDLE   StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam)= 0;
	    
		/* 根据时间 开始录像下载  */
    virtual HANDLE   StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)= 0;
	
    /*  添加备用流媒体服务器*/
    virtual BOOL   AddStandby(const S8 *ip, U32 port){return TRUE;}

    /*  删除备用流媒体服务器*/
	virtual BOOL   DelStandby(const S8 *ip, U32 port){return TRUE;}
	
		/* 获取登录返回信息 */
	virtual	BOOL   GetLoginRsp(LOGIN_RSP_S *pRsp){return TRUE;}
	
		/**< 获取用户列表信息 */
	virtual	BOOL   GetUserList(const NET_USER_PARAM_S *pParam){return TRUE;}
	
		/**< 获取设备列表信息 */
	virtual	BOOL   GetDeviceList(const NET_DEVICE_PARAM_S *pParam){return TRUE;}
	
		/* 请求服务器马上发送一个关键帧 */
	virtual	BOOL   RequestIFrame(const NET_REQI_PARAM_S *pParam){return TRUE;}
	
		/* 预览画面切换 */
	virtual	BOOL   PreviewSwitch(const NET_SWITCH_PARAM_S *pParam){return TRUE;}
	
		/* 矩阵切换 */
	virtual	BOOL   MatrixSwitch(const NET_MATRIX_PARAM_S *pParam){return TRUE;}

		/* 解码器画面切换 */
	virtual	BOOL   DecoderSwitch(const NET_DECODER_PARAM_S *pParam){return TRUE;}

		/* 解码器获取显示通道 */
	virtual	BOOL   DecoderGetDisplay(const NET_DEC_DIS_PARAM_S *pParam){return TRUE;}

		/* 解码器开始动态解码 */
	virtual	BOOL   DecoderStartDynamic(
				U8  * pu8DeviceIp,
				U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
				S32   s32DecChnNo,
				S32   s32DisOutPort,
				S32   s32DisWndNo,
				const NET_DEC_CHN_PARAM_S *pParam){return TRUE;}

		/* 解码器停止动态解码 */
	virtual	BOOL  DecoderStopDynamic(
			    U8 * pu8DeviceIp,
                U16   u16DevicePort,
				U8  * pu8SubServIp,
				U16   u16SubServPort,
			    S32   s32DisOutPort){return TRUE;}
				
		/* 设置五防联动回调函数 */
	virtual	BOOL SetWFLingageCallback(pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser){return TRUE;}
		
		/* 在线客户列表查询 liuwei 20140307 */
	virtual	BOOL   GetClientList(pfnGetClientCallback fGetClientCallback, VOID *pUser){return TRUE;}
		
		/* 时间同步 */
	virtual	BOOL   SynchrTime(const NET_SYNTIME_PARAM_S *pParam){return TRUE;}

		/* 查询时间 */
	virtual	BOOL   QueryTime(DATETIME_S *pDateTime){return TRUE;}
	
		/* 获取设备在线状态 */
	virtual	BOOL   GetDeviceState(U8 u8Ip[MAX_IPV4_LEN], U16 u16Port){return TRUE;}
	
		/* 资源获取 */
	virtual	BOOL   QueryResource(pfnResourceCallback fResourceCallback, VOID *pUser){return TRUE;}

    
    /*暂时不实现，语音对讲用另一套接口 InitDevTalk*/
		/* 初始化对讲信道 */
	virtual	HANDLE InitDualtalk(const NET_DTALK_PARAM_S *pParam){return NULL;}
	
		/* 去初始化对讲信道 */
    virtual	VOID   UnInitDualtalk(HANDLE hDTalk = NULL){}



		/* 联动转发 */
	virtual	BOOL   LinkTransmit(LINKAGE_TRANS_RPT_S *pLinkTrans){return TRUE;}
		
		/* 获取设备信息(数据库) */
	virtual	CHN_INFO_S * GetDeviceData(U32 *u32DevNum){return NULL;}

		/* 发送轨道车控制命令 */
	virtual	BOOL   RailCarControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam){return TRUE;}

		/* 启动或停止某个摄像机通道的录像 */
	virtual	BOOL   ControlRecord(const NET_CHANNEL_PARAM_S *pParam, BOOL bStart){return TRUE;}

		/* 参数设置(二进制模式) */
    virtual BOOL   SetParameter(const NET_PARAMSET_PARAM_S *pParam){return TRUE;}
	
		/* 参数设置(json模式) */
	virtual	BOOL   SetParameterEx(const S8 * pParam){return TRUE;}
				
		/* 设置音视频对讲状态 */
	virtual	BOOL   SetDTalkState(U32 u32DTalkState){return TRUE;}

		/* 预置位操作(旧规约) */
	virtual	BOOL   GetChnPreset(const NET_GETPRESET_PARAM_S *pParam){return TRUE;}
	virtual	BOOL   SetChnPreset(const NET_SETPRESET_PARAM_S *pParam){return TRUE;}

		/* 预置位操作(新规约) */
	virtual	BOOL   GetChnPresetEx(const NET_GETPRESET_PARAM_S_EX *pParam){return TRUE;}
	virtual	BOOL   SetChnPresetEx(const NET_SETPRESET_PARAM_S_EX *pParam){return TRUE;}

#ifdef _WIN32
		/* 叠加预置位信息到视频画面 */
	virtual	VOID   DrawPreset(const NET_PRESET_DRAW *pParam){}
		
		/* 注册绘图回调函数 */
	virtual BOOL   SetRegDraw(const NET_REGDRAW_PARAM *pParam){return TRUE;}

		/* 解码回调函数 */
	virtual	BOOL   SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam) = 0;
#endif

		/* 智能分析 */
	virtual	BOOL   StartIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam){return TRUE;}
	virtual	BOOL   StopIvaAnalyse(const NET_IVA_ANALYSE_PARAM *pParam){return TRUE;}

		/* 初始化设备对讲 */
	virtual	HANDLE InitDevTalk(int audioType){return NULL;}

		/* 红外测温 */
	virtual	BOOL   SetThermometryRuleCfg(const NET_THERM_RULE_LIST *pParam){return NULL;}
	virtual	BOOL   SetThermDisplayMode(const NET_TERM_SHOWPARAM_S *pParam){return NULL;}
	
		/*获取流媒体下预置位信息*/
	virtual	BOOL   GetPresetInfo(const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo){return TRUE;}
		
		/*下发和盛资(AXF|EST|DV|LINE)*/
	virtual	BOOL   SendHSResource(const S8 * ps8Resource, S32 s32Size){return TRUE;}

		/*设置或删除流媒体下预置位信息*/
	virtual	BOOL   SendPresetInfo(const S8 * ps8PresetInfo, S32 s32Size){return TRUE;}

		/*下发命令到流媒体(json格式)*/
    virtual BOOL   SendCommand(const S8 * ps8info, S32 s32Size){return TRUE;}

		/*获取智能分析总召告警*/
	virtual	BOOL   GetRemoteIvaAlarmData(const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo){return TRUE;}
		
		/*获取智能分析轮巡列表*/
	virtual	BOOL   GetIvaCruiseList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList){return TRUE;}

		/*控制智能分析轮巡启停*/
	virtual	BOOL   ControlIvaCruise(const S8 * ps8Req, U32 u32Size){return TRUE;}
	
		/*查询智能分析分析列表*/
	virtual	BOOL   QueryIvaRunList(const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaList){return TRUE;}

		/*查询硬盘录像机通道的录像状态*/
	virtual	BOOL QueryChanRecordState(const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState){return TRUE;}
	
		/*发送命令并获取返回结果*/
	virtual	BOOL GetResponse(const S8 * ps8Req, U32 u32Size, S8 *p8Rsp,U32 &u32RspSize){return TRUE;}

protected: 
	
private:

};

#endif 
