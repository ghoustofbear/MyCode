#ifndef __RSPlay_h__
#define __RSPlay_h__

#ifdef RSPLAY_EXPORTS
#define RSPLAY_API __declspec(dllexport)
#else
#define RSPLAY_API __declspec(dllimport)
#endif

#define RSPLAY_CALL __stdcall

//播放控制
#define PLAY_MODE_NORMAL			0x0001
#define PLAY_MODE_STOP				0x0002
#define PLAY_MODE_PAUSE				0x0003
#define PLAY_MODE_SLOW				0x0004
#define PLAY_MODE_SLOW4				0x0005	
#define PLAY_MODE_SLOW8				0x0006
#define PLAY_MODE_SLOW16			0x0007	
#define PLAY_MODE_FAST_FORWARD2		0x0010
#define PLAY_MODE_FAST_FORWARD4		0x0011
#define PLAY_MODE_FAST_FORWARD8		0x0012
#define PLAY_MODE_FAST_FORWARD16	0x0013
#define PLAY_MODE_FAST_FORWARD32	0x0014
#define PLAY_MODE_SINGLE_FRAME		0x0030

//audio setting
#define BIT_PER_SAMPLE				16
#define PLAY_AUDIO_SAMPLE_POOR		8000
#define HI3510_TALK_DARABLOCK_SIZE	80
	
typedef struct{ 
	long nWidth; // 画面宽，单位为像素，如果是音频数据则为0
	long nHeight; // 画面高，单位为像素，如果是音频数据则为0
	long nStamp; // 时标信息，单位毫秒
	long nType; //数据类型
	long nFrameRate;// 编码时产生的图像帧率
}RS_FRAME_INFO;

typedef int (CALLBACK *DECODE_CALLBACK) (long hHandle, char *pData, long lSize, RS_FRAME_INFO *pInfo, DWORD dwUser); 

//音量
#define VOLUM_MIN	0
#define VOLUM_MAX	100

//播放类型
#define PLAY_UNKNOW_TYPE     -1    //no file
#define PLAY_LOCALFILE_TYPE  0     //local file
#define PLAY_NETFILE_TYPE    1     //net file
#define PLAY_NETSTREAM_TYPE  2     //net stream
#define PLAY_DUALTALK_TYPE	 3	   //dual talk
#define PLAY_SYNC_TYPE		 4     //sync play

//声音控制类型
#define DST_SPEAKERS		0	//主音量  
#define SRC_WAVEOUT			1   //波形
#define SRC_SYNTHESIZER		2	//软件合成器
#define SRC_COMPACTDISC     3	//CD音量
#define SRC_MICROPHONE		4   //麦克风

#define MAX_SET_OSD_NUM		5	//最多可设置OSD的数目

typedef struct __osd_info_t
{
	char font[24];
	int font_width;
	int font_height;
	unsigned char osd_color_red;		//0-255
	unsigned char osd_color_green;		//0-255
	unsigned char osd_color_blue;		//0-255
	RECT osd_rect;

	char osd_string[128];
}osd_info_t;

typedef struct{
	int iRate;
	int iWidth;
	int iFormat;
	int iAmr_mode;
	int iAmr_format;
}AudioTalkConfig_t;

//RSPlaySetCallbackMessage message wparam
#define NET_FILE_DATA_NOT_ENOUGH	1
#define FILE_PLAY_END				11
//lparm = userParam

//record
#ifndef RSFileType
#define RSFileType		1
#define RSAVIFileType	2
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int (CALLBACK* DUAL_SPEAK_CALLBACK)(int size, const char* data, DWORD dwUser);
typedef void (CALLBACK* DRAW_FUN_CALLBACK) (HDC hDC, DWORD with, DWORD height, void* pUser);
typedef void (CALLBACK* PLAY_FUN_CALLBACK)(UINT message, void* userParam);
RSPLAY_API int RSPLAY_CALL RSPlayInitPlayLib(void);
RSPLAY_API int RSPLAY_CALL RSPlayDeinitPlayLib(void);

RSPLAY_API unsigned long RSPLAY_CALL RSPlayCreatePlayInstance(int play_type); 
RSPLAY_API int RSPLAY_CALL RSPlayDestroyPlayInstance(unsigned long player_id);

RSPLAY_API int RSPLAY_CALL RSPlaySetCallbackMessage(unsigned long player_id, HWND hwnd, UINT message, void* userParam); //NET_FILE_DATA_NOT_ENOUGH...

RSPLAY_API int RSPLAY_CALL RSPlaySetCallbackMessageEx(unsigned long player_id, PLAY_FUN_CALLBACK callback, void* userParam);

RSPLAY_API int RSPLAY_CALL RSPlayAddSyncPlayInstance(unsigned long sync_play, unsigned long player_id);

RSPLAY_API int RSPLAY_CALL RSPlaySetNetPlayBufNum(unsigned long player_id, int num);
//添加一个播放窗口到指定的播放实例中，用于一次解码多窗口渲染的模式下
RSPLAY_API int RSPLAY_CALL RSPlaySetPlayWnd(unsigned long player_id, HWND hwnd);
//从指定的播放实例中删除一个播放窗口，用于一次解码多窗口渲染的模式下
RSPLAY_API int RSPLAY_CALL RSPlayDelPlayWnd(unsigned long player_id, HWND hwnd);

RSPLAY_API int RSPLAY_CALL RSPlaySetPlayPicSize(unsigned long player_id, int width, int height);
RSPLAY_API int RSPLAY_CALL RSPlayGetNetPlayBufNum(unsigned long player_id);
RSPLAY_API HWND RSPLAY_CALL RSPlayGetPlayWnd(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlayGetPlayPicSize(unsigned long player_id, int *width, int *height);

RSPLAY_API int RSPLAY_CALL RSPlayOpenPlayFile(unsigned long player_id, const char *file_name);
RSPLAY_API int RSPLAY_CALL RSPlayClosePlayFile(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlayGetPlayFileTimeRange(unsigned long player_id, ULONGLONG& beginTime, ULONGLONG& endTime);
RSPLAY_API ULONGLONG RSPLAY_CALL RSPlayGetCurPlayedTime(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlaySetCurPlayedTime(unsigned long player_id, ULONGLONG time);

RSPLAY_API int RSPLAY_CALL RSPlayGetFrameRate(unsigned long player_id, BOOL bFrame_rate);
RSPLAY_API int RSPLAY_CALL RSPlayGetBitRate(unsigned long player_id, BOOL bBit_rate);

RSPLAY_API int RSPLAY_CALL RSPlayStartPlay(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlayStopPlay(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlaySetPlayMode(unsigned long player_id, int play_mode);
RSPLAY_API int RSPLAY_CALL RSPlayGetPlayMode(unsigned long player_id);


RSPLAY_API int RSPLAY_CALL RSPlayStartlocalrecord(unsigned long player_id, const char *recordfile_name, UINT fileType);
RSPLAY_API int RSPLAY_CALL RSPlaySwitchRecordFile(unsigned long player_id, const char *recordfile_name, UINT fileType);
RSPLAY_API int RSPLAY_CALL RSPlayStoplocalrecord(unsigned long player_id);

RSPLAY_API int RSPLAY_CALL RSPlayInputNetFrame(unsigned long player_id, void *buf, int size);
RSPLAY_API int RSPLAY_CALL RSPlayGetListFrameNum(unsigned long player_id);

RSPLAY_API int RSPLAY_CALL RSPlayOpenSound(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL  RSPlayCloseSound(unsigned long player_id);
//Control the system volume,cannot use in win7 OS
RSPLAY_API int RSPLAY_CALL RSPlaySetPlayVolume(int volume);
RSPLAY_API int RSPLAY_CALL RSPlayGetPlayVolume();
RSPLAY_API int RSPLAY_CALL RSPlaySetMute(BOOL bMute);
RSPLAY_API BOOL RSPLAY_CALL RSPlayGetMute();
//
//Only control volume on one play instance
RSPLAY_API int RSPLAY_CALL RSPlaySetPlayVolume_s(unsigned long player_id,int volume);
RSPLAY_API int RSPLAY_CALL RSPlayGetPlayVolume_s(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlaySetMute_s(unsigned long player_id,BOOL bMute);
RSPLAY_API BOOL RSPLAY_CALL RSPlayGetMute_s(unsigned long player_id);
//

RSPLAY_API int RSPLAY_CALL RSPlaySetMicPlayVolume(int volume);
RSPLAY_API int RSPLAY_CALL RSPlayGetMicPlayVolume();
RSPLAY_API int RSPLAY_CALL RSPlaySetMicMute(BOOL bMute);
RSPLAY_API BOOL RSPLAY_CALL RSPlayGetMicMute();

RSPLAY_API int RSPLAY_CALL RSPlaySetOsdString(unsigned long player_id, int index, osd_info_t *p_osd_info);
RSPLAY_API int RSPLAY_CALL RSPlayRemoveOsdstring(unsigned long player_id, int index);

RSPLAY_API int RSPLAY_CALL RSPlayCapturePicture(unsigned long player_id, char *file_name);
RSPLAY_API int RSPLAY_CALL RSPlayCapturePictureBuf(unsigned long player_id, char *buf);
RSPLAY_API int RSPLAY_CALL RSPlayCaptureYUVBuf(unsigned long player_id, char *buf,UINT &width,UINT &height);

RSPLAY_API int RSPLAY_CALL RSPlayGetSDKVesion(char *ver_string);
RSPLAY_API int RSPLAY_CALL RSPlayGetLastError(unsigned long player_id, char *err_string);

RSPLAY_API int RSPLAY_CALL RSPlayStartDualTalk(unsigned long player_id, AudioTalkConfig_t *pAudioTalk = NULL);
RSPLAY_API int RSPLAY_CALL RSPlayStopDualTalk(unsigned long player_id);
RSPLAY_API int RSPLAY_CALL RSPlayRegisterSetDualCallback(unsigned long player_id, DUAL_SPEAK_CALLBACK callback, DWORD dwUser);

RSPLAY_API int RSPLAY_CALL RSPlayGetCurrentFrameNo(unsigned long player_id); 
RSPLAY_API int RSPLAY_CALL RSPlayAdjustViewArea(unsigned long player_id, int left, int right, int top, int bottom);
//注册指定播放窗口的屏幕绘图回调函数，用于一次解码多窗口渲染的模式下
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDrawFun_s(unsigned long player_id, DRAW_FUN_CALLBACK DrawFun, void* pUser,HWND hwnd);
//注册指定播放实例的屏幕绘图回调函数，用于单次解码单窗口渲染的模式下
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDrawFun(unsigned long player_id, DRAW_FUN_CALLBACK DrawFun, void* pUser);
//bEnable = 0, 实时
//bEnable = 1, iMax = 0，库将按帧数据的时间放，缓冲的帧要由你来控制，这也是net_file模式。
//bEnable = 1, iMax（150～5000）,iMax为缓冲多久的帧数据
RSPLAY_API int RSPLAY_CALL  RSPlaySetCushionRange(unsigned long player_id, BOOL bEnable, int iMax);
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDecCallback(unsigned long player_id, DECODE_CALLBACK DecCallback, DWORD dwUser);

#define RE_NORMAL			0
#define RE_MIRROR_LEFTRIGHT	1
#define RE_MIRROR_UPDOWN	2
RSPLAY_API int RSPLAY_CALL RSPlaySetRenderEffect(unsigned long player_id, int type);

//enable 0:平铺到整个窗口 其他：按实际图像比例显示，创建实例时默认为0
RSPLAY_API void RSPLAY_CALL RSPlayRenderActualProportion(unsigned long player_id, int enable); 
//设置是否使用水印解码器解码
RSPLAY_API void RSPLAY_CALL RSUseWaterMarkDecorder(BOOL bUse);

//是否可以显示水印，FALSE表示视频流解密验证失败，视频可能被篡改过
RSPLAY_API BOOL RSPLAY_CALL RSShowWaterMark(unsigned long player_id);

RSPLAY_API void RSPLAY_CALL RSPlayRePaint(unsigned long play_id);

/*add by huijun 20110801*/
typedef int (CALLBACK *CONVERT_STATUS_CALLBACK) (int progress,void* param); 
RSPLAY_API int RSPLAY_CALL RSPlay264ToAviStart(unsigned long play_id,PCSTR _264FileName,PCSTR aviFileName,void* param,CONVERT_STATUS_CALLBACK status_callback);
RSPLAY_API int RSPLAY_CALL RSPlay264ToAviStop(unsigned long play_id);
#ifdef __cplusplus
}
#endif

#endif // __Play_h__
