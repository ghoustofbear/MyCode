#ifndef _AVPLAYSDK_TYPES_H_
#define _AVPLAYSDK_TYPES_H_

#include "GlobalTypes.h"

#ifdef __WIN__
#define PLAYHWND		HWND
#define PLAYHDC			HDC
#define SDKint64		__int64
#else
#define PLAYHWND		void*
#define PLAYHDC			void*
#define SDKint64		long long
#define UINT			unsigned int
#define DWORD			unsigned long
#define WM_USER			0xFFFF
#endif

#ifndef __cplusplus
#define bool			int
#endif

/*********************************************************************************************/
#define CALLING_CONVENTION			__cdecl
#define MAX_PLAYER_NUM				128
#define MAX_SYC_GROUP_NUM			4				//Maximum Number Of Groups Played Back Synchronously
#define MAX_SYC_PLAYER_NUM			16				//Maximum Player Number for Synchronized Playback
#define MSG_PLAYSDKM4				(WM_USER+0x01FF)//Default Values For Playback Library Messages
/*********************************************************************************************/


/**********************************************************************************************
* Functional Macro Definition                                                                 
**********************************************************************************************/
#define	SAFE_CLOSE_THREAD(x) if (NULL != (x))	{WaitForSingleObject((x),INFINITE); CloseHandle((x)); (x) = NULL;}
#define	SAFE_CLOSE_HANDLE(x) if (NULL != (x))	{CloseHandle((x)); (x) = NULL;}
#define SAFE_DELETE(x)	if ((x) != NULL)	{delete (x); (x) = NULL;}
#define SAFE_FREE(x)	if ((x) != NULL)	{free((x)); (x) = NULL;}
#define SAFE_DELETE_ARRAY(x)	if ((x) != NULL)	{delete [] (x); (x) = NULL;}
#define SAFE_CLOSE_FILE(x) if ((x) != NULL)	{fclose((x)); (x) = NULL;}
/*********************************************************************************************/



/**********************************************************************************************
* Number of Show Video                                                                   
**********************************************************************************************/
#define	MAX_DRAW_NUM				8
/*********************************************************************************************/


/**********************************************************************************************
* List of LPARAM Parameter Values For Playback Library Messages                                                                     
**********************************************************************************************/
#define LPARAM_FILE_PLAY_END		0		//Play Finished
#define LPARAM_FILE_SEARCH_END		1		//Find The End Of The File
#define LPARAM_NEED_DECRYPT_KEY		2		//The Password Needs To Be Decrypted
#define LPARAM_DECRYPT_KEY_FAILED	3		//Decryption Password Error
#define LPARAM_DECRYPT_SUCCESS		4		//Decrypt Success
#define LPARAM_STREAM_SEARCH_END	5		//There Is No Data In The Stream Buffer
#define LPARAM_VOD_STREAM_END		6		//VOD File Stream Played Out
/*********************************************************************************************/

/**********************************************************************************************
* return value for encrypt frame                                                                      
**********************************************************************************************/
#define ENCRYPT_SUCCESS				0		// encrypt sucess
#define ENCRYPT_INVALID_PARAM		(-1)	// error param
#define ENCRYPT_KEY_NOT_READY		(-2)	// invalid key
#define ENCRYPT_FAILED				(-3)	// encrypt failed
/*********************************************************************************************/

/**********************************************************************************************
* Decrypt frame return value                                                                     
**********************************************************************************************/
#define DECRYPT_SUCCESS				0		//Decrypt Success
#define DECRYPT_INVALID_PARAM		(-1)	//Parameter Err
#define DECRYPT_KEY_NOT_READY		(-2)	//Not Set Decrypt Key
#define DECRYPT_FRAME_NOT_ENCRYPT	(-3)	//Frame Is Not Encrypt Frame
#define DECRYPT_FAILED				(-4)	//Frame Decrypt Failed
/*********************************************************************************************/


/**********************************************************************************************
* Record File                                                                       
**********************************************************************************************/
#define REC_FILE_TYPE_NORMAL		0
#define REC_FILE_TYPE_AVI			1
#define REC_FILE_TYPE_ASF			2
#define REC_FILE_TYPE_AUDIO			3
#define REC_FILE_TYPE_WAV			4
#define REC_FILE_TYPE_MP3			5
#define REC_FILE_TYPE_ZFMP4         10
/*********************************************************************************************/


/**********************************************************************************************
* Player Control Code Used By TC_PlayerControl                                                             
**********************************************************************************************/
#define CTRL_VOD_SETOVERFLAG			0		//set vod play end flag
#define CTRL_VOD_CLEANBUFF				1		//clean vod buffer
#define CTRL_RESETDECODE				2		//resetdecode clean yuv
#define CTRL_RESVERSE_PLAY				3		//reverse play
#define CTRL_REGIST_USERDATA_NOTIFY		4		//regist usrdata notify
#define CTRL_REGIST_DRAW_NOTIFY			5		//regist draw notify
#define CTRL_REGIST_RAW_FRAME_NOTIFY	6		//regist rwa frame notify
#define CTRL_SEEK_BY_ABSTIME			7		//seek play by abs time
#define CTRL_SEEK_BY_RELATIVETIME		8		//seek play by Relative time
#define CTRL_REGIST_DRAW_YUV_NOTIFY		9		//regist draw yuv data notify
#define CTRL_GET_CUDA_INFO				10		//get cuda context
#define CTRL_DRAW_FACE_PARAM			11		//draw face ident rect
#define CTRL_DRAW_ON_HDC				12		//inner draw on HDC
#define CTRL_DRAW_TARGET_NOTIFY			13		//regist usrdata target notify
#define CTRL_REGIST_DRAW_V5				14		//regist drawFunc notify
/*********************************************************************************************/

//CTRL_REGIST_USERDATA_NOTIFY
typedef struct tagUserDataNotify
{
	int iSize;
	DEVUSERDATA_NOTIFY pNotifyFun;
	void* pvUdata;
}UserDataNotify;

typedef struct tagRECTEx
{
	double		left;					//Proportion Of Original Video
	double		top;					//Proportion Of Original Video
	double		right;					//Proportion Of Original Video
	double		bottom;					//Proportion Of Original Video
}RECTEX, *PRECTEX;

typedef struct tagDrawParam
{
	PLAYHDC hDrawDc;
	unsigned int iTimeStamp;
	int iDcWidth;
	int iDcHeight;
	RECT rcShow;
	RECTEX rcSrc;
}DrawParam;
typedef void (*DRAW_NOTIFY_V4)(unsigned int _ulID, DrawParam* _ptDrawParam,  void* _iUser);
typedef int  (*DRAW_NOTIFY_V5)(unsigned int _ulID, void* _ptDraw, int _iSize, void* _pUser);
//CTRL_REGIST_DRAW_NOTIFY
typedef struct tagDrawNotify
{
	int iSize;
	DRAW_NOTIFY_V4 pNotifyFun;
	void* pvUdata;
}DrawNotify;

typedef struct tagDrawNotifyV5
{
	int iSize;
	DRAW_NOTIFY_V5  pDrawFun;
	void* pvUdata;
}DrawNotifyV5;

typedef struct tagYuvDataInfo
{
	unsigned char* pucYuvBuf;
	int iYuvBufLen;
	int iWidth;
	int iHeight;
	unsigned int uiTime;
	void* pvUsrData;
} YuvDataInfo;

typedef void (*DRAW_YUV_CallBack)(long _lPlayerHandle, YuvDataInfo* _ptYuv, int _iSize, void* _pvUdata);
typedef struct tagDrawYuvNotify
{
	int iSize;
	DRAW_YUV_CallBack pYuvCallBack;
	void* pvUdata;
} DrawYuvNotify;

typedef struct tagCudaInfo
{
	int iSize;
	int iGpuId;
	void* pvCudaCtx;
} CudaInfo;

typedef struct tagDrawEnable
{
	int iSize;
	int	iType;
	int iEnable;
}DrawEnable;

typedef DrawEnable DrawFaceInfo;

/**********************************************************************************************
* Draw Type                                                                      
**********************************************************************************************/
#define DRAW_TYPE_FACE_TARGET		0
#define DRAW_TYPE_FACE_ATTR			1
#define DRAW_TYPE_VCA_RULE			2
#define DRAW_TYPE_VCA_TARGET		3
#define DRAW_TYPE_VCA_ATTR			4
#define DRAW_TYPE_ILLEGAL_PARK		5
#define DRAW_TYPE_TEMP_DETEC        6
#define DRAW_TYPE_LIGHTNING         7
#define DRAW_TYPE_MAX				8

#define DRAW_FACE_TYPE_RECT			DRAW_TYPE_FACE_TARGET
#define DRAW_FACE_TYPE_TEXT			DRAW_TYPE_FACE_ATTR
/*********************************************************************************************/


/**********************************************************************************************
* Audio type                                                                      
**********************************************************************************************/
#define T_AUDIO8_TD					0
#define T_YUV420_TD					1
#define T_YUV422_TD					2
/*********************************************************************************************/


/**********************************************************************************************
* Play Mode                                                                      
**********************************************************************************************/
#define	PLAYMODE_FILE				0		//File Mode Play
#define	PLAYMODE_VOD_FILE			1		//VOD Mode Play
#define	PLAYMODE_STREAM				2		//Stream Mode Play
#define PLAYMODE_UNIVERSAL_FILE     3       // universal file mode play
/*********************************************************************************************/


/**********************************************************************************************
* Sleep Time                                                                     
**********************************************************************************************/
#define	SLEEP_1						1		//1 Millisecond
#define	SLEEP_100					100		//100 Millisecond
/*********************************************************************************************/


/**********************************************************************************************
* 返回值错误码含义                                                                        
**********************************************************************************************/
#define RET_INVALID_PLAYERID		-1		//Illegal PLAYER ID
#define RET_ERR_PLAYMODE			-2		//Play Mode Err
#define RET_INVALID_PARAM			-3		//Illegal Parameter
/*********************************************************************************************/


/**********************************************************************************************
* 刷新功能控制码                                                                        
**********************************************************************************************/
#define CTRL_REFRESHPLAYER			0		//Initialize The PLAYER Window As A Black Background

#define PLAY_SPEED_VALUE_1			1		//2-Fold Velocity Value
#define PLAY_SPEED_VALUE_2			2		//4-Fold Velocity Value
#define PLAY_SPEED_VALUE_3			3		//8-Fold Velocity Value
#define PLAY_SPEED_VALUE_4			4		//16-Fold Velocity Value
#define PLAY_SPEED_VALUE_5			5		//32-Fold Velocity Value

#define PLAY_SPEED_2X_RATE			2		//2-fold rate
#define PLAY_SPEED_4X_RATE			4		//4-fold rate
#define PLAY_SPEED_8X_RATE			8		//8-fold rate
#define PLAY_SPEED_16X_RATE			16		//16-fold rate
#define PLAY_SPEED_32X_RATE			32		//32-fold rate
/*********************************************************************************************/


/**********************************************************************************************
* Mode Rule                                                                    
**********************************************************************************************/
#define MODERULE_AUTO_ADJUST_DRAW_NO				(0x00)	//Adaptive Data with Different Resolution--No
#define MODERULE_AUTO_ADJUST_DRAW_YES				(0x01)	//Adaptive Data with Different Resolution--Yes
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_YES	(0x11)	//Adaptively adjust the playback rate of streaming mode -- yes
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_NO	(0x10)	//Adaptively adjust the playback rate of streaming mode -- No
#define MODERULE_STREAM_ADAPTIVE					(0x24)	//Set to adaptive mode
#define MODERULE_STREAM_PLAY_BALANCE				(0x23)	//Set to Equilibrium Mode
#define MODERULE_STREAM_PLAY_NO_DELAY				(0x22)	//Set to no-delay mode
#define MODERULE_STREAM_PLAY_HIGH_SMOOTH			(0x21)	//Set to High Fluency Mode
#define MODERULE_STREAM_PLAY_LOW_DELAY				(0x20)	//Set to Low Delay Mode
#define MODERULE_AVIDEO								(0x30)	//Set to Audio and Video
#define MODERULE_PURE_AUDIO							(0x31)	//Set to pure audio mode
#define MODERULE_FAST_FORWARD_SKIP_I				(0x40)	//Set to Fast Forward Jump I Frame
#define MODERULE_FAST_FORWARD_NO_SKIP_I				(0x41)	//Set to Fast Forward No Jump I Frame
#define MODERULE_SINGLE_FILE						(0x50)	//Set to single file mode
#define MODERULE_MUTI_FILE							(0x51)	//Set to multi-file mode
#define MODERULE_FIX_RATIO							(0x60)	//Proportional video display
#define MODERULE_FIX_WINDOW							(0x61)	//Fill window display
#define MODERULE_VIDEO_OPTIMIZE_CLOSE				(0x70)	//Optimized closure of video effects
#define MODERULE_VIDEO_OPTIMIZE_OPEN				(0x71)	//Optimizing Opening of Video Effect
/*********************************************************************************************/


/*********************************************************************************************/
typedef void (CALLING_CONVENTION* pfCBPlayEnd)(int _iID);
#ifdef __WIN__
typedef void (CALLING_CONVENTION* pfCBGetDecAV)(int _iID, const DecAVInfo* _pDecAVInfo, void* _iUser);
#endif
typedef void (CALLING_CONVENTION* pfCBVideoEdit)(int _iNotifyCode);
typedef int  (CALLING_CONVENTION* pfCBDrawFun)(long _lHandle, PLAYHDC _hDc, void* _lUserData);
typedef int  (CALLING_CONVENTION* pfCBDrawFunEx)(long _lHandle, PLAYHDC _hDc, long _lWidth, long _lHeight, void* _lUserData);
#ifdef __WIN__
typedef int  (CALLING_CONVENTION* pfCBCommonEventNotify)(int _iPlayerID, int _iEventMessage);
#else
typedef int (*pfCBCommonEventNotify)(int _iPlayerID, int _iEventMessage, void* _pUserData);
#endif
typedef int  (CALLING_CONVENTION* pfCBCommonEventNotifyEx)(int _iPlayerID, int _iEventMessage, void* _lpUserData);
typedef void (__stdcall* pfAudioCaptureCbk)(unsigned char* _pucData, int _iLen, void* _pvUserData);
//Callback function of original stream
typedef void (__stdcall* pfRawFrameCbk)(int _iPlayerID, unsigned char* _pucData, int _iLen, void* _pvUserData);
typedef void (*pfCBGetRaw)(int _iID, char* _pData, int _iSize, void* _iUser);
//Audio acquisition for intercom
typedef int (*RECORD_VOICE_NOTIFY)(unsigned char *_pucAuidoBuffer, int _iAudioLen);

//Create player type flags on demand
typedef enum __tagPlayModeCmd
{
	E_NORMAL_PLAYER				= -1,
	E_FILE_ONLYDECODE			= 0,			//File mode decoding only
	E_STREAM_ONLYDECODE			= 1,			//Stream mode decoding only
	E_YUV_PLAYER				= 2,			//YUV Player
	E_RGB_PLAYER				= 3,			//RGB Player
	E_SYC_PLAYER				= 4,			//Sync Player
	E_VOD_RAWNOTIFY				= 5,			//Front-end playback original stream callback
	E_HWDECODE_PLAYER			= 6,			//Hard Dec
	E_CUDA_STREAMPLAYER			= 7,            //CUDA
	E_NORMAL_FILEPLAYER			= 8,            //File Mode Player
	E_NORMAL_STREAMPLAYER		= 9,            //Stream Mode Player
	E_NORMAL_VODPLAYER			= 10,           //VOD Mode Player
	E_CUDA_FILE_ONLYDECODE		= 11,           //CUDA File mode decoding only
	E_CUDA_FILEPLAYER			= 12,			//CUDA File Mode Player
	E_UNIVERSAL_STREAMPLAYER	= 13,			//universal flow Player
	E_CUDA_STREAM_ONLYDECODE	= 14,            //CUDA Stream mode decoding only
    E_UNIVERSAL_FILEPLAYER      = 15
}PlayModeCmd, *LPPlayModeCmd, linux_PlayModeCmd;

#define SINGLE_THREAD_WORK		0	//Single Thread Decoding Mode
#define MULTI_THREAD_WORK		1	//Multithread Decoding Mode
#define DEFAULT_THREAD_COUNT	4	//The default number of worker threads in multithreaded decoding mode is 4

#define STREAM_DECODE_MODE		0
#define FRAME_DECODE_MODE		1

typedef struct tagDecoderPara		//SDK defaults to multithreaded decoding mode
{
	int iSize;
	int iWorkMode;		//Decoder working mode：0-->Single Thread: 1-->Multithread  
	int iThreadCount;	//Number of worker threads in multithreading mode, default is 4
	int iDecodeMode;	//解码模式：0--流式解码，1--帧解码
} DecoderPara, linux_DecoderPara;

#define MAX_FILENAME_LEN	256
typedef struct tagFilePlayer
{
	int				iSize;
	PLAYHWND		hWnd;
	char			cFileName[MAX_FILENAME_LEN];
	DecoderPara		tDecoderPara;
	int				iWndFlag;			//Property identifying wnd object: 0 -- QtWidget object (default), 1 -- QtLayout object
} FilePlayer, *pFilePlayer, linux_FilePlayer, *plinux_FilePlayer, UniversalFilePlayer, *PUniversalFilePlayer;

typedef struct tagStreamPlayer
{
	int				iSize;
	PLAYHWND		hWnd;
	unsigned char	ucVideoHeadBuf[PLAY_VIDEO_HEADER_LEN];
	int				iMaxStreamBufSize;
	DecoderPara		tDecoderPara;
	int				iWndFlag;			//Property identifying wnd object: 0 -- QtWidget object (default), 1 -- QtLayout object
} StreamPlayer, *pStreamPlayer, VodPlayer, *pVodPlayer, linux_StreamPlayer, *plinux_StreamPlayer, linux_VodPlayer, *plinux_VodPlayer;

typedef struct tagCudaPlayer
{
	int				iSize;
	int				iGpuID;
	PLAYHWND		hWnd;
	unsigned char	ucVideoHeadBuf[PLAY_VIDEO_HEADER_LEN];
}CudaPlayer, pCudaPlayer;

typedef struct tagCudaFilePlayer
{
	int				iSize;
	int				iGpuID;
	PLAYHWND		hWnd;	//Window handle passes NULL, decoding only and not displaying
	char			cFileName[MAX_FILENAME_LEN];
} CudaFilePlayer, *pCudaFilePlayer;

typedef struct tagUniversalStreamPlayer
{
	int				iSize;
	PLAYHWND		hWnd;
	DecoderPara		tDecoderPara;
} UniversalStreamPlayer, *pUniversalStreamPlayer;

typedef struct __tagPlayerHeader
{
	int			iSize;
	PLAYHWND	hWnd;
	int			iWidth;
	int			iHeight;
	int			iWndFlag;			//Property identifying wnd object: 0 -- QtWidget object (default), 1 -- QtLayout object
} TPlayerHeader, *PTPlayerHeader;

typedef void (CALLING_CONVENTION* FISHEYE_CallBack_TD)(int _iID, int _iSubID, PLAYHDC _pvDC, unsigned int _uiWidth, unsigned int _uiHeight, void* _pvUsrData);

//Installation Method of Fisheye Camera
#define FEC_MOUNTING_WALL			1	//Wall mounting method (normal level)
#define FEC_MOUNTING_FLOOR			2	//Ground Installation (Normal Up)
#define FEC_MOUNTING_CEILING		3	//Top mounting mode (normal downward)

//Correction Method of Fisheye Camera
#define FEC_CORRECT_PTZ		0x100		//PTZ 
#define FEC_CORRECT_180		0x200		//180 degree correction
#define FEC_CORRECT_360		0x300		//360 Panoramic Correction
#define FEC_CORRECT_LAT		0x400		//Latitude development

//Setting Fisheye Correction Parameter CMD
#define CMD_CORRECT_PTZPARA				1		//PTZ Correction parameter
#define CMD_CORRECT_CYCLERADIUS			2		//Fisheye image center parameter for the center, this command only supports acquisition
#define CMD_CORRECT_WIDESCANOFFSET		3		//180 or 360 degree correction parameters
#define CMD_RESERVE_LAST_FRAME			0XFF	//Whether to keep the last frame or not,0-No reservation,1-reservation. Default does not reserve, video screen display black screen.

//PTZ Return to the contour point of the corresponding image type
#define PTZ_RET_TYPE_NONE	 0x0000		//Instead of returning contour points, only images are returned
#define PTZ_RET_TYPE_FISH	 0x1000		//PTZ Return to the outline of the fish eye image
#define PTZ_RET_TYPE_180	 0x2000		//PTZ Return to 180 degree correction image contour points, temporarily not supported
#define PTZ_RET_TYPE_360	 0x3000		//PTZ Return to the outline point of 360 degree corrected image
#define PTZ_RET_TYPE_LAT	 0x4000		//PTZ Return to the contour point of the latitude unwrapped corrected image
typedef struct tagCorrectPtzPara
{
	int				iSize;
	int				iPtzID;				//Ptz 窗口编号,范围0-7
	int				iPTZPositionX;		//PTZ Displayed central position X coordinates
	int				iPTZPositionY;		//PTZ Display center position Y coordinates
	int				iZoom;				//PTZ Displayed range parameters
	int				iPTZRstWidth;		//PTZ The width of the returned image
	int				iPTZRstHeight;		//PTZ Return the height of the image
	int				iPTZRetType;		//PTZ Return to the corresponding image type
} CorrectPtzPara, *pCorrectPtzPara;

typedef struct tagCorrectCyclePara
{
	int		iSize;					
	int		iRadiusLeft;		//The leftmost X-coordinate of a circle, expressed in tens of thousands of decimal ratios
	int		iRadiusRight;		//The rightmost X-coordinate of a circle, expressed in tens of thousands of decimal ratios
	int		iRadiusTop;			//Y coordinates on the top edge of a circle, expressed in tens of thousands of decimal ratios
	int		iRadiusBottom;		//Y coordinates at the bottom of a circle, expressed in decimal ratio
} CorrectCyclePara, *pCorrectCyclePara;

typedef struct tagCorrectWideScanPara
{
	int		iSize;					
	float   fWideScanOffset;        // Migration angles corrected at 180 or 360 degrees
} CorrectWideScanPara, *pCorrectWideScanPara;

typedef struct tagCreateCorrectPara
{
	int					iSize;					
	int					iPlaceType;		//Installation Method of Fisheye Camera
	int					iCorrectType;	//Correction Method of Fisheye Camera
	CorrectCyclePara	tCycleRadius;	//Fisheye image center parameters
	CorrectWideScanPara	tWideScanPara;	//80 or 360 Degree Corrected Migration Angle
	CorrectPtzPara		tPtzPara;		//PTZ Correction parameter
	PLAYHWND			hWnd;
} CreateCorrectPara, *pCreateCorrectPara;

//Snapshot type
#define CAPTURE_PICTURE_TYPE_BMP		1
#define CAPTURE_PICTURE_TYPE_JPG		2 
#define CAPTURE_PICTURE_TYPE_FEC_BMP	3
#define CAPTURE_PICTURE_TYPE_FEC_JPG	4 

#define DEFAULT_CYCLE_RADIUS_LEFT		0
#define DEFAULT_CYCLE_RADIUS_RIGHT		10000
#define DEFAULT_CYCLE_RADIUS_TOP		0
#define DEFAULT_CYCLE_RADIUS_BOTTOM		10000

#define MAX_FEC_TRACK_COUNT		8
#define START_ALL_FEC_TRACK		-1
#define STOP_ALL_FEC_TRACK		-1
typedef struct tagFECTrackInfo
{
	int			iSize;
	int			iWindowCount;						//Number of windows to be tracked, currently up to 8PTZ tracing is supported
	int			iPtzSubID[MAX_FEC_TRACK_COUNT];		//Each PTZ tracks the corresponding correction ID, and the SubID returned by interface TC_CreateFishEyeCorrect during PTZ correction
} FECTrackInfo, *pFECTrackInfo;

typedef struct _SwscaleInfo
{
	char* yuv;
	int width;
	int height;
} SwscaleInfo;

#endif

