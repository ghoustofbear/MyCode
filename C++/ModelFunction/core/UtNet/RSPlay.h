#ifndef __RSPlay_h__
#define __RSPlay_h__

#ifdef RSPLAY_EXPORTS
#define RSPLAY_API __declspec(dllexport)
#else
#define RSPLAY_API __declspec(dllimport)
#endif

#define RSPLAY_CALL __stdcall

//���ſ���
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
	long nWidth; // �������λΪ���أ��������Ƶ������Ϊ0
	long nHeight; // ����ߣ���λΪ���أ��������Ƶ������Ϊ0
	long nStamp; // ʱ����Ϣ����λ����
	long nType; //��������
	long nFrameRate;// ����ʱ������ͼ��֡��
}RS_FRAME_INFO;

typedef int (CALLBACK *DECODE_CALLBACK) (long hHandle, char *pData, long lSize, RS_FRAME_INFO *pInfo, DWORD dwUser); 

//����
#define VOLUM_MIN	0
#define VOLUM_MAX	100

//��������
#define PLAY_UNKNOW_TYPE     -1    //no file
#define PLAY_LOCALFILE_TYPE  0     //local file
#define PLAY_NETFILE_TYPE    1     //net file
#define PLAY_NETSTREAM_TYPE  2     //net stream
#define PLAY_DUALTALK_TYPE	 3	   //dual talk
#define PLAY_SYNC_TYPE		 4     //sync play

//������������
#define DST_SPEAKERS		0	//������  
#define SRC_WAVEOUT			1   //����
#define SRC_SYNTHESIZER		2	//����ϳ���
#define SRC_COMPACTDISC     3	//CD����
#define SRC_MICROPHONE		4   //��˷�

#define MAX_SET_OSD_NUM		5	//��������OSD����Ŀ

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
//���һ�����Ŵ��ڵ�ָ���Ĳ���ʵ���У�����һ�ν���ര����Ⱦ��ģʽ��
RSPLAY_API int RSPLAY_CALL RSPlaySetPlayWnd(unsigned long player_id, HWND hwnd);
//��ָ���Ĳ���ʵ����ɾ��һ�����Ŵ��ڣ�����һ�ν���ര����Ⱦ��ģʽ��
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
//ע��ָ�����Ŵ��ڵ���Ļ��ͼ�ص�����������һ�ν���ര����Ⱦ��ģʽ��
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDrawFun_s(unsigned long player_id, DRAW_FUN_CALLBACK DrawFun, void* pUser,HWND hwnd);
//ע��ָ������ʵ������Ļ��ͼ�ص����������ڵ��ν��뵥������Ⱦ��ģʽ��
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDrawFun(unsigned long player_id, DRAW_FUN_CALLBACK DrawFun, void* pUser);
//bEnable = 0, ʵʱ
//bEnable = 1, iMax = 0���⽫��֡���ݵ�ʱ��ţ������֡Ҫ���������ƣ���Ҳ��net_fileģʽ��
//bEnable = 1, iMax��150��5000��,iMaxΪ�����õ�֡����
RSPLAY_API int RSPLAY_CALL  RSPlaySetCushionRange(unsigned long player_id, BOOL bEnable, int iMax);
RSPLAY_API int RSPLAY_CALL RSPlayRegisterDecCallback(unsigned long player_id, DECODE_CALLBACK DecCallback, DWORD dwUser);

#define RE_NORMAL			0
#define RE_MIRROR_LEFTRIGHT	1
#define RE_MIRROR_UPDOWN	2
RSPLAY_API int RSPLAY_CALL RSPlaySetRenderEffect(unsigned long player_id, int type);

//enable 0:ƽ�̵��������� ��������ʵ��ͼ�������ʾ������ʵ��ʱĬ��Ϊ0
RSPLAY_API void RSPLAY_CALL RSPlayRenderActualProportion(unsigned long player_id, int enable); 
//�����Ƿ�ʹ��ˮӡ����������
RSPLAY_API void RSPLAY_CALL RSUseWaterMarkDecorder(BOOL bUse);

//�Ƿ������ʾˮӡ��FALSE��ʾ��Ƶ��������֤ʧ�ܣ���Ƶ���ܱ��۸Ĺ�
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
