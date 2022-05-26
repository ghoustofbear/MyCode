#ifndef __GLOBAL_TYPES_H__
#define __GLOBAL_TYPES_H__

#if (defined(_WIN32) || defined(_WIN64)) && !defined(__WIN__)
#define __WIN__
#endif

#ifndef __WIN__
#include <stdlib.h>
#include <sys/resource.h>
#endif

#ifndef __WIN__
typedef struct _RECT 
{ 
    int left; 
    int top; 
    int right; 
    int bottom; 
}RECT, *LPRECT;

typedef struct tagPOINT
{
    int  x;
    int  y;
}POINT, *LPPOINT;

#define __stdcall
#define __cdecl

#ifndef OUT
#define OUT
#endif

#ifndef PVOID
#define PVOID void*
#endif

#else
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#endif

/**********************************************************************************************
* Character length                                                                 
**********************************************************************************************/
#define LEN_0									0
#define LEN_2									2
#define LEN_4									4
#define LEN_5									5
#define LEN_8   								8
#define LEN_10									10
#define LEN_16  								16
#define LEN_17  								17
#define LEN_18  								18
#define LEN_20  								20
#define LEN_24  								24
#define LEN_32  								32
#define LEN_33  								33
#define LEN_36  								36
#define LEN_64  								64
#define LEN_65  								65
#define LEN_63  								63
#define LEN_128 								128
#define LEN_250 								250
#define LEN_256 								256
#define LEN_255 								255
#define LEN_512 								512
#define LEN_300 								300
#define LEN_1024 								1024
#define LEN_1300 								1300 
#define LEN_2048 								2048 
/**********************************************************************************************/


/**********************************************************************************************
* Intelligent analysis of drawing area limits
**********************************************************************************************/
#define VCA_MAX_POLYGON_POINT_NUM_TD				16	//The maximum number of points allowed per polygon
#define VCA_MAX_POLYGON_POINT_NUMEX				32	//Max Tripwire Points Number
#define VCA_MAX_POLYGON_NUM						25	//The maximum number of polygons allowed per VCA rule
#define MAX_AREA_NUM							4	//Max Area Number for 300w Leave Detect
/**********************************************************************************************/


/**********************************************************************************************
* The intelligent analysis information stored in the video file
**********************************************************************************************/
#define MAX_SAVE_TARGET_NUM						32
#define MAX_SAVE_TARGET_NUM_NEW					128
#define VCAINFO_SEARCHRULE_MIN					0
#define VCAINFO_SEARCHRULE_FRAME				(VCAINFO_SEARCHRULE_MIN+0)
#define VCAINFO_SEARCHRULE_POLYGON				(VCAINFO_SEARCHRULE_MIN+1)
#define VCAINFO_SEARCHRULE_TRIPWIRE				(VCAINFO_SEARCHRULE_MIN+2)
#define VCAINFO_SEARCHRULE_MAX					(VCAINFO_SEARCHRULE_MIN+3)
/**********************************************************************************************/


/**********************************************************************************************
* The intelligent analysis information stored in the video file
**********************************************************************************************/
#define SIP_CMD_SET_MIN							(2)
#define SIP_CMD_SET_ALARMCHANNEL				(SIP_CMD_SET_MIN+0)
#define SIP_CMD_SET_VIDEOCHANNEL				(SIP_CMD_SET_MIN+1)
#define SIP_CMD_SET_MAX							(SIP_CMD_SET_MIN+2)
/**********************************************************************************************/


/**********************************************************************************************
* Video Size                                                                      
**********************************************************************************************/
#define QCIF									0		//QCif 	176*144
#define HCIF									1		//HCif	352*144	
#define FCIF									2		//Cif	352*288	
#define HD1										3		//HD1	720*288
#define FD1										4		//D1	720*576
#define MD1										5		//MD1	720*288
#define QVGA									6		//QVGA	320x240
#define VGA										7		//VGA	640*480
#define HVGA									8		//HVGA	640*240
#define HD_720P									9		//720p	1280*720
#define HD_960P									10		//960P	1280*960
#define HD_200W									11		//200W	1600*1200
#define HD_1080P								12		//1080P	1920*1080
#define HD_QXGA									13		//QXGA	2048*1536

#define QHD										0x100	//QHD	960*540
#define VZ_960H									0x200	//960H	960*576
#define VZ_720_720								0x201	//		720*720
#define VZ_2MP									0x202	//2MP	1280*1280
#define VZ_3MP									0x203	//3MP	3072*1152
#define VZ_1440P								0x210	//1440P	2560*1440
#define VZ_4MP									0x220	//4MP	2592*1520
#define WUXGA									0x230	//WUXGA 1920*1200
#define HK_4MP									0x240	//4MP	2688*1520
#define VZ_FEC_4MP								0x250	//4MP	2048*2048
#define VZ_5MA									0x300	//5.0MP	2448*2048
#define VZ_5M									0x400	//5M(2)	2560*1920
#define VZ_5MB									0x410	//5M(3)	2592*1944
#define VZ_5MC									0x420	//5M(4)	2592*2048
#define VZ_5MD									0x430	//5M(5)	2528*2128
#define VZ_5ME									0x500	//5M	2560*2048
#define VZ_6M									0x600	//6M	2752*2208
#define VZ_6MA									0x610	//6M	3008*2008
#define VZ_6MB									0x620	//6M	3408*2008
#define VZ_FEC_6M								0x630	//FEC6M	2560*2560
#define VZ_7M									0x700	//7M	3392*2008
#define VZ_FEC_7M								0x700	//FEC7M	3072*2304
#define VZ_8MA									0x800	//8M	3840*2160
#define VZ_8MB									0x810	//8M	3264*2448
#define VZ_8MC									0x820	//8M	329682472
#define VZ_9M									0x900	//9M	3072*3072
#define VZ_12M									0x1200	//12M	4000*3072
/*********************************************************************************************/



/**********************************************************************************************
** User data information                                                                    
**********************************************************************************************/
#define GET_USERDATA_INFO_MIN                   (0)                           
#define GET_USERDATA_INFO_TIME					(GET_USERDATA_INFO_MIN )	//Get time information in user data
#define GET_USERDATA_INFO_OSD					(GET_USERDATA_INFO_MIN + 1)	//Get OSD information in user data
#define GET_USERDATA_INFO_GPS                   (GET_USERDATA_INFO_MIN + 2)	//Get GPS information in user data
#define GET_USERDATA_INFO_VCA                   (GET_USERDATA_INFO_MIN + 3)	//Get VCA information in user data
#define GET_USERDATA_INFO_USER_DEFINE           (GET_USERDATA_INFO_MIN + 4)	//Get the custom information in the user data
#define GET_USERDATA_INFO_VITAL_SIGN_DATA		(GET_USERDATA_INFO_MIN + 5)	//Get vital signs information in user data
#define GET_USERDATA_INFO_DEMOGRAPHICS			(GET_USERDATA_INFO_MIN + 6)	//Get the count in the user data
#define GET_USERDATA_INFO_LPR					(GET_USERDATA_INFO_MIN + 7)	//Get license plate identification information in user data
#define GET_USERDATA_INFO_OSD_EX				(GET_USERDATA_INFO_MIN + 8)	//Get OSD information in user data
#define GET_USERDATA_INFO_LDC					(GET_USERDATA_INFO_MIN + 9)	//Get Obtain distortion correction information in user data
#define GET_USERDATA_INFO_ZF                    (GET_USERDATA_INFO_MIN + 10)//Get ZF focus mark
#define GET_USERDATA_INFO_VCA_SRC_SIZE          (GET_USERDATA_INFO_MIN + 11)//Get src size VCA information in user data 
#define GET_USERDATA_INFO_TIMESTAMP				(GET_USERDATA_INFO_MIN + 12)//Get timestamp in user data 
#define GET_USERDATA_INFO_TARGET_ATTR           (GET_USERDATA_INFO_MIN + 13)//Get target attribute in user data
#define GET_USERDATA_INFO_VCA_RULE				(GET_USERDATA_INFO_MIN + 14)//Get target attribute in user data
#define GET_USERDATA_PTZ_DATA					(GET_USERDATA_INFO_MIN + 15)//Get Ptz Data
#define GET_USERDATA_VIRGAUGE_DATA              (GET_USERDATA_INFO_MIN + 16)//Get VirtualGauge Data
#define GET_USERDATA_INFO_ILLEGAL_AREA			(GET_USERDATA_INFO_MIN + 17)//Get ILLEGAL AREA Data
#define GET_USERDATA_INFO_WATER_SPEED_BASE		(GET_USERDATA_INFO_MIN + 18)//Get Water Speed Base Data
#define GET_USERDATA_INFO_WATER_SPEED_ALGINFO	(GET_USERDATA_INFO_MIN + 19)//Get Water Speed AlgInfo Data
#define GET_USERDATA_INFO_ILLEGAL_PARK_AREA		(GET_USERDATA_INFO_MIN + 20)//Get ILLEG ALPARK AREA
#define GET_USERDATA_INFO_ILLEGAL_PARK_CAR		(GET_USERDATA_INFO_MIN + 21)//Get CPC INCREASE
#define GET_USERDATA_INFO_CPC_INCREASE			(GET_USERDATA_INFO_MIN + 22)//Get CPC Count
#define GET_USERDATA_INFO_CPC_COUNT				(GET_USERDATA_INFO_MIN + 23)//Get ILLEG ALPARK CAR
#define GET_USERDATA_INFO_TRAFFIC_TARGET		(GET_USERDATA_INFO_MIN + 24)//Get ILLEG ALPARK CAR
#define GET_USERDATA_INFO_TEMP_INFO      		(GET_USERDATA_INFO_MIN + 25)//Get ILLEG ALPARK CAR
#define GET_USERDATA_INFO_MAX                   (GET_USERDATA_INFO_MIN + 26)

#define GET_ALL_USERDATA_INFO					(0xFF)
#define GET_USERDATA_END						(0xFFFFFF) 
/**********************************************************************************************/

//High precision PTZ information
typedef struct
{
	int m_PanPosition;   //Horizontal coordinates p*10000，p：[0,360]
	int	m_TiltPosition;  //Vertical coordinates t*10000, t：[-90,90]
	int	m_ZoomPosition;  //Multiplying factor z*10000, z：[1,Actual ratio]
} TPtzInfoEx;

//Vi Wide height
typedef struct
{
	unsigned short		Width;		//device vi width
	unsigned short		Height;		//device vi height
}TWSViSize;

//The velocity meter elevation and other information
typedef struct
{
	int	altitude;		//Camera height，unit:mm
	int	waterlevel;		//Water level，unit:mm
	int	width;			//The actual scene width of the center of the camera，unit:mm
}TWSScene;

typedef struct
{
	TPtzInfoEx		ptz;
	TWSViSize		video;
	TWSScene		scene;
}TWaterSpeedBase;


typedef struct
{
	unsigned short		x;		//The matching point is the coordinate，millionth
	unsigned short		y;		//Match point ordinate，millionth
	unsigned int	time;	//Match time stamp，unit:ms
}TWSPointInfo;

#define WATER_SPEED_POINT_NUM  2		  
typedef struct
{
	TWSPointInfo		point[WATER_SPEED_POINT_NUM];			//A group Match point
} TWaterSpeedAlgInfo;


/* License plate string length */                                                        
#define LPR_PLATE_STR_LEN						11
#define MAX_OUTPUT_LPR_NUM						10

/* * License Plate Color * */  
#define LPR_PLATE_COLOR_UNKNOWN					0	// Unknown
#define LPR_PLATE_COLOR_BLUE					1	// Blue card
#define LPR_PLATE_COLOR_YELLOW					2	// Yellow card
#define LPR_PLATE_COLOR_WHITE					3	// White card
#define LPR_PLATE_COLOR_BLACK					4	// Black card
#define LPR_PLATE_COLOR_GREEN					5	// Green card
#define LPR_PLATE_COLOR_YELLOWTOGREEN			51	// Yellow to green card(New energy vehicle) 
#define LPR_PLATE_COLOR_GRADIENTGREEN			52	// Gradient green card(New energy vehicle)
#define LPR_PLATE_COLOR_RED						53	// Red card

/* * The Color of Car * */  
#define LPR_CAR_COLOR_WHITE					0	// White
#define LPR_CAR_COLOR_RED					1	// Red
#define LPR_CAR_COLOR_YELLOW				2	// Yellow
#define LPR_CAR_COLOR_YELLOW1				3	// Yellow
#define LPR_CAR_COLOR_BLUE					4	// Blue
#define LPR_CAR_COLOR_GREEN					5	// Green
#define LPR_CAR_COLOR_GREEN1				6	// Green
#define LPR_CAR_COLOR_PURPLE				7	// Purple
#define LPR_CAR_COLOR_PINK					8	// Pink
#define LPR_CAR_COLOR_BLACK					9	// Black
#define LPR_CAR_COLOR_RED1					10	// Red
#define LPR_CAR_COLOR_YELLOW2				11	// Yellow
#define LPR_CAR_COLOR_YELLOW3				12	// Yellow
#define LPR_CAR_COLOR_GRAY					13	// Gray
#define LPR_CAR_COLOR_YELLOW4				14	// Yellow
#define LPR_CAR_COLOR_BLUE1					15	// Blue
#define LPR_CAR_COLOR_BLUE2					16	// Blue
#define LPR_CAR_COLOR_GREEN2				17	// Green
#define LPR_CAR_COLOR_GREEN3				18	// Green
#define LPR_CAR_COLOR_WHITE1				19	// White
#define LPR_CAR_COLOR_GREEN4				20	// Green
#define LPR_CAR_COLOR_CYAN_BLUE				21	// cyan-blue
#define LPR_CAR_COLOR_YELLOW5				22	// Yellow
#define LPR_CAR_COLOR_RED2					23	// Red
#define LPR_CAR_COLOR_BLUE3					24	// Blue
#define LPR_CAR_COLOR_BLUE4					25	// Blue
#define LPR_CAR_COLOR_GRAY1					26	// Gray
#define LPR_CAR_COLOR_PURPLE1				27	// Purple
#define LPR_CAR_COLOR_PURPLE2				28	// Purple
#define LPR_CAR_COLOR_BROWN					29	// Brown
#define LPR_CAR_COLOR_BROWN1				30	// Brown
#define LPR_CAR_COLOR_BROWN2				31	// Brown
#define LPR_CAR_COLOR_UNKNOWN				99	// Unknown

/* Currently Supported Models */
#define LPR_VT_UNKOWN							0	//Unknown model
#define LPR_VT_BUS								1	//Buses and coaches
#define LPR_VT_CAR								2	//Cars
#define LPR_VT_TRUCK							3	//Trucks, including large and small goods
#define LPR_VT_VAN								4	//The van
#define LPR_VT_BIG_TRUCK						5	//large trucks
#define LPR_VT_SMALL_TRUCK						6	//A minivan
#define LPR_VT_TWO_THREE_WHEELER				7	//Two or three wheels
#define LPR_VT_PERSON							8	//Pedestrians	
#define LPR_VT_SUV								9	//SUV
#define LPR_VT_MID_BUS							10	//Medium Buses
#define LPR_VT_PLATE_ASKEW						11	//License plate hanging
#define LPR_VT_BGS								14	//Background detection detected
#define LPR_VT_OTHER							15	// Other models
/**********************************************************************************************/
/* Car PLATE Type  */                                                                  
#define LPR_PLATE_TYPE_UNKOWN			   		0			//Unknown
#define LPR_PLATE_TYPE_CAR_NORMAL		        1			//Ordinary car, private car.(Blue card,Black card)
#define LPR_PLATE_TYPE_BIG_NORMAL	            (1<< 1)		//Ordinary cart, truck.(Yellow card)
#define LPR_PLATE_TYPE_POLICE					(1<< 2)		//Police car.(White card)(separator between bit1 and bit2)
#define LPR_PLATE_YTPE_WJ          				(1<< 3)		//Armed police car.
#define LPR_PLATE_YTPE_HK						(1<< 4)		//Hong Kong and Macau car.
#define LPR_PLATE_YTPE_DOUBLELINE				(1<< 5)		//Double decker car.(Yellow card)
#define LPR_PLATE_YTPE_MILITARY					(1<< 6)		//Military
#define LPR_PLATE_TYPE_AMBASSADOR				(1<< 7)		//Embassy car.
#define LPR_PLATE_TYPE_DGREEN_TYPE2				(1<< 8)		//The second type of green car,electric bicycle.
#define LPR_PLATE_TYPE_POLICE_TYPE2				(1<< 9)		//Second class police car.(separator between bit2 and bit3)	
#define LPR_PLATE_TYPE_XUE						(1<< 10)	//Coach car.("xue")
#define LPR_PLATE_TYPE_MOTOR					(1<< 11)	//Motorcycle
#define LPR_PLATE_TYPE_LIAOTEMP					(1<< 12)	//Temporary license plate.("lin")
#define LPR_PLATE_TYPE_DGREEN					(1<< 13)	//Double decker car.Agricultural car.(Green card)
#define LPR_PLATE_TYPE_GUA						(1<< 14)	//Trailer.("gua")
#define LPR_PLATE_TYPE_NEWENERGY 				(1<< 15)	//New energy car.
#define LPR_PLATE_TYPE_TJELECTRICBICYCLE  		(1<< 16)	//Tianjin electric bicycle.
#define LPR_PLATE_TYPE_GOVERNMENTPURPOSE  		(1<< 17)	//Government Purpose.
/**********************************************************************************************/
/* Car Logo  */                                                                  
#define LPR_LOGO_UNKNOWN						0	// Unknown
#define LPR_LOGO_VOLKSWAGEN						1	//Volkswagen(Volkswagen / Volkswagen)
#define LPR_LOGO_HONDA							2	//Honda
#define LPR_LOGO_TOYOTA							3	//Toyota
#define LPR_LOGO_DONGFENG						4	//DongFeng
#define LPR_LOGO_FAWCAR							5	//First Automobile Works
#define LPR_LOGO_BUICKT							6	//Buick
#define LPR_LOGO_SUZUKI							7	//Suzuki
#define LPR_LOGO_CITROEN						8	//Citroen
#define LPR_LOGO_KIA							9	//KIA
#define LPR_LOGO_XIALI							10	//Xiali
#define LPR_LOGO_AUDI							11	//Audi
#define LPR_LOGO_HYUNDAI						12	//Hyundai
#define LPR_LOGO_SGMW							13	//SGMW
#define LPR_LOGO_CHEERY							14	//Chery Automobile Corporation
#define LPR_LOGO_MAZDA							15	//Mazda 
#define LPR_LOGO_HAFEI							16	//Hafei 
#define LPR_LOGO_CCAG							17	//Chongqing Changan
#define LPR_LOGO_NISSAN							18	//Nissan
#define LPR_LOGO_GREATWALL						19	//GreatWall
#define LPR_LOGO_BUILDYD						20	//build your dreams	
#define LPR_LOGO_FORD							21	//Ford
#define LPR_LOGO_CHEVROLET          			22	//Chevrolet
#define LPR_LOGO_BAOJUN							23	//BaoJun
#define LPR_LOGO_BENZ							24	//Benz
#define LPR_LOGO_EMGRAND						25	//Emgrand
#define LPR_LOGO_LEXUS							26	//Lexus
#define LPR_LOGO_MG								27	//MingJue
#define LPR_LOGO_BMW							28	//BMW
#define LPR_LOGO_PEUGEOT						29	//Peugeot
#define LPR_LOGO_PEUGEOT2						30	//Peugeot
#define LPR_LOGO_HAIMA							31	//Haima
#define LPR_LOGO_ZHONGHUA						32	//Zhonghua
#define LPR_LOGO_SKODA							33	//Skoda
#define LPR_LOGO_SOUTHEAST						34	//Southeast
#define LPR_LOGO_CROWN							35	//Crown
#define LPR_LOGO_JINBEI							36	//he Gold Cup
#define LPR_LOGO_MITSUBISHI						37	//Mitsubishi 	
#define LPR_LOGO_ROEWE							38	// Roewe
#define LPR_LOGO_GEELY							39	//Geely
#define LPR_LOGO_ENGLONCAR						40	//England
#define LPR_LOGO_GLEAGLE						41	//Global Hawk
#define LPR_LOGO_ANKAI							42	//Ankai
#define LPR_LOGO_PORSCHE						43	//Poriche
#define LPR_LOGO_BAICBAW						44	//Beiqi BAW
#define LPR_LOGO_BAICMOTOR 						45	//Beijing car
#define LPR_LOGO_BAICWEIWANG					46	//Beiwei Wei Wang
#define LPR_LOGO_CCAG2							47	//ChongQingChangAn
#define LPR_LOGO_CHANGHE						48	//ChaneHe
#define LPR_LOGO_DODGE							49	//Dodge
#define LPR_LOGO_FIAT							50	//Fiat
#define LPR_LOGO_FOTON							51	//Fukuda
#define LPR_LOGO_QOROS							52	//Concept
#define LPR_LOGO_HIGER							53	//Hagrid
#define LPR_LOGO_HAVAL							54	//Harvard
#define LPR_LOGO_CNHTCHOWO						55	//China Heavy truck ho Wo
#define LPR_LOGO_HONGQI							56	//red flag
#define LPR_LOGO_HUANGHAI						57	//Yellow Sea
#define LPR_LOGO_HUIZHONG						58	//Shanghai Huizhong
#define LPR_LOGO_JAGUAR							59	//Jaguar
#define LPR_LOGO_JAGUAR2						60	//Jaguar2
#define LPR_LOGO_JEEP							61	//Jeep
#define LPR_LOGO_KINGLONG						62	//Golden Dragon
#define LPR_LOGO_GOLDENDRAGON					63	//Xiamen Golden Brigade
#define LPR_LOGO_JAC							64	//JAC
#define LPR_LOGO_JMC							65	//Jiangling
#define LPR_LOGO_JMC2							66	//Jiangling2
#define LPR_LOGO_CADILLAC						67	//Cadillac
#define LPR_LOGO_CHRYSLER						68	//Chrysler
#define LPR_LOGO_LINCOLN						69	//Lincoln
#define LPR_LOGO_RENAULT						70	//Renault
#define LPR_LOGO_LIFAN							71	//Lifan
#define LPR_LOGO_EVERUS							72	//idea
#define LPR_LOGO_LEOPAARD						73	//Cheetah
#define LPR_LOGO_MINI							74	//Mini
#define LPR_LOGO_MASERATI						75	//Maserati
#define LPR_LOGO_ACURA							76	//Acura
#define LPR_LOGO_SHANXIAUTOMOBILE				77	//Shaanxi Auto
#define LPR_LOGO_SHAOLINBUS						78	//Shaolin
#define LPR_LOGO_SUBARU							79	//Subaru
#define LPR_LOGO_SMART							80	//Mercedes-Benz
#define LPR_LOGO_VOLVO							81	//Volvo
#define LPR_LOGO_ISUZU							82	//Isuzu
#define LPR_LOGO_INFINITI						83	//Infiniti
#define LPR_LOGO_YUEJIN							84	//Leap forward
#define LPR_LOGO_NAVECO							85	//Iveco
#define LPR_LOGO_YUTONGBUS						86	//Yutong
#define LPR_LOGO_ZHONGTONGBUS					87	//Zhongtong bus
#define LPR_LOGO_JINLING						88	//Jinling
#define LPR_LOGO_SUNWIN							89	//Shenwo bus
#define LPR_LOGO_YANGTSE						90	//Yangtze River
#define LPR_LOGO_LUXGEN							91	//Satisfied Chi Jie
#define LPR_LOGO_TKING							92	//Tang Jun Ou Ling
#define LPR_LOGO_FOTON2							93	//Fukuda2
#define LPR_LOGO_KAMA							94	//Kema
#define LPR_LOGO_FORLAND						95	//Times Fukuda

/**********************************************************************************************/
/* Currently supported illegal types */
#define LPR_IT_UNKOWN			0,     //未知类型
#define LPR_IT_NOSAFETYBELT		1,    //Do not wear seat belts
#define LPR_IT_PHONE			(1<< 1)         // Call
#define LPR_IT_NOPRESSLINE		(1<< 2)
#define LPR_IT_NOILL			(1<< 3)
#define LPR_IT_NOOVERLINE		(1<< 4)
#define LPR_IT_NORED			(1<< 5)
#define LPR_IT_NOCONTRAYDIRET	(1<< 6)
#define LPR_IT_PLATEASKEW		(1<< 7)
#define LPR_IT_SUNVISORMAIN		(1<< 8)
#define LPR_IT_SUNVISORSEC		(1<< 9)
#define LPR_IT_BIGTRUCKLIMIT	(1<< 10)
/**********************************************************************************************/

/**********************************************************************************************/
/* AVPlay video and audio render types */
#define CMD_AV_MODE_MIN							0
#define CMD_VIDEO_SHOW_MAIN_MODE		CMD_AV_MODE_MIN + 0
#define CMD_VIDEO_SHOW_SUB_MODE			CMD_AV_MODE_MIN + 1
#define CMD_AV_DEC_SHOW_MODE			CMD_AV_MODE_MIN + 2
#define CMD_AV_DEC_HWDECODE_PARAM		CMD_AV_MODE_MIN + 3  // HwDecodeParam
#define CMD_AVMODE_VIDEO_DECLIB			CMD_AV_MODE_MIN + 4  // Video decoding library type 
#define CMD_AVMODE_AUDIOCOMMON_DECLIB	CMD_AV_MODE_MIN + 5  // Audio common decoding library type
#define CMD_AVMODE_AUDIOAAC_DECLIB		CMD_AV_MODE_MIN + 6  // Audio aac decoding library type
#define CMD_AVMODE_AUDIO_PLAY_MODE		CMD_AV_MODE_MIN + 7  // Audio aac decoding library type
#define CMD_AV_MODE_MAX					CMD_AV_MODE_MIN + 8

#define SHOW_MAIN_MODE_DRAW		0	//draw Display, support by windows
#define SHOW_MAIN_MODE_D3D		1	//d3d Display, support by windows
#define SHOW_MAIN_MODE_SDL		4	//sdl display, support by linux
#define SHOW_MAIN_MODE_OPENGL	5	//opengl display, support by linux
#define SHOW_MAIN_MODE_TEXTURE	6	//d3d texture display support by windows

#define SHOW_SUB_MODE_YUV420	0	//420 Display
#define SHOW_SUB_MODE_YUV422	1	//422 Display

#define VIDEO_DLIB_HISI						0
#define VIDEO_DLIB_FFMPEG					1
#define VIDEO_DLIB_CUDA						2
#define COMMON_AUDIO_DLIB_HISI				0
#define COMMON_AUDIO_DLIB_OPENSOURCE		1
#define AAC_AUDIO_DLIB_FAAD					0
#define AAC_AUDIO_DLIB_FFMPEG				1

#define HWDEC_ENABLE_STREAM	0x1
#define HWDEC_ENABLE_VOD	0x2
#define HWDEC_ENABLE_FILE	0x4

#define HWDEC_LIMIT_LOCAL	0
#define HWDEC_LIMIT_GLOBAL	1

#define VIDEO_DLIB_HISI						0
#define VIDEO_DLIB_FFMPEG					1
#define VIDEO_DLIB_CUDA						2
#define COMMON_AUDIO_DLIB_HISI				0
#define COMMON_AUDIO_DLIB_OPENSOURCE		1
#define AAC_AUDIO_DLIB_FAAD					0
#define AAC_AUDIO_DLIB_FFMPEG				1

typedef struct tagAVDecodeMode
{
	int			iSize;
	int			iVideoDecLibType;			//Video Decoder Library Types：Hisi，Ffmpeg
	int			iCommonAudioDecLibType;		//Universal Audio Decoder Library Type: Hisi, Open Source Library
	int			iAACAudioDecLibType;		//Types of AAC Audio Decoder Library：faad，Ffmpeg
	int			iVideoShowMainMode;			//Main mode of video display：Draw，D3D
	int			iVideoShowSubMode;			//Video Display Submode：Yuv420，Yuv422
} AVDecodeMode, * pAVDecodeMode;

//CMD_AV_DEC_HWDECODE_PARAM
typedef struct tagHwDecodeParam
{
	int iSize;
	int iLimitType; //HWDEC_LIMIT_LOCAL or HWDEC_LIMIT_GLOBAL 
	int iMaxCount;  //if iLimitType == HWDEC_LIMIT_GLOBAL, iMaxCount should less than 2
	int iEnableFlag;//HWDEC_ENABLE_STREAM | HWDEC_ENABLE_VOD | HWDEC_ENABLE_FILE
}HwDecodeParam;
/**********************************************************************************************/

/* struct of vca */                                                                 
typedef struct tagvca_TRect
{
	unsigned short	usLeft;
	unsigned short	usTop;
	unsigned short	usRight;
	unsigned short	usBottom;
} vca_TRect, *pvca_TRect;

#define ALG_TRIPWIRE						1
#define ALG_DOUBLE_TRIP						2
#define ALG_PERIMETER_DERECTION				3 
#define ALG_LINGER							4 
#define ALG_PARK							5 
#define ALG_RUN								6
#define ALG_PERSONNEL_DENSITY				7 
#define ALG_STOLEN							8
#define ALG_DEREKUCT						9 
#define ALG_FACE_DETECTION					10
#define ALG_VIDEO_DIAGNOSIS					11
#define ALG_INTELLIGENT_TRACKING			12
#define ALG_FLOW_STATISTICS					13
#define ALG_CROWD							14
#define ALG_POST_DETECTION					15
#define ALG_WATER_LEVEL_MONITORING			16
#define ALG_AUDIO_DIAGNOSIS					17
#define ALG_MOSAIC							18
#define ALG_REVER_FLOATER					19
#define ALG_STEALING_UNLOADING				20
#define ALG_ILLEGAL_PARKING					21 
#define ALG_FIGHT							22
#define ALG_ALERT							23 
#define ALG_LPR	

#define RENDER_WND_QTWIDGET			0		//Qt widget flag
#define RENDER_WND_QTLAYOUT			1		//Qt layout flag

typedef struct tagvca_TTargetInfo
{
	vca_TRect		rect;
	unsigned short	usType;  //ALG_SINGLE_TRIP ~ ALG_LPR
	unsigned short	usVelocity;
	unsigned short	usDirection;
	unsigned short	id;	
} vca_TTargetInfo, *pvca_TTargetInfo;

typedef struct tagvca_TTargetSet
{
	unsigned short	usCount;
	vca_TTargetInfo	targets[MAX_SAVE_TARGET_NUM];
} vca_TTargetSet, *pvca_TTargetSet;

//Used to store the number of statistical results
typedef struct tagTvca_cpcInfo
{
	short 			sCountIn;
	short 			sCountOut;
} Tvca_cpcInfo, *pTvca_cpcInfo;

typedef struct tagTvca_cpcInfoV2
{
	unsigned short	usValue;			
	unsigned short	usCountIn;
	unsigned short	usCountOut;
	unsigned short	usCountPass;
	unsigned short	usCountZone;
	unsigned short	usReserved[4];
}Tvca_cpcInfoV2, *pTvca_cpcInfoV2;

typedef struct tagTvca_CpcData
{
	Tvca_cpcInfo 	tCpcInfo;			//Statistical result
	vca_TTargetSet	tVCATarget;			//Destination location
	Tvca_cpcInfoV2  tCpcInfoV2;			//Statistical Result Expand
} Tvca_CpcData, *pTvca_CpcData;

typedef struct tagTvca_RGB
{
	unsigned char 	ucR;
	unsigned char 	ucG;
	unsigned char 	ucB;
} Tvca_RGB, *pTvca_RGB;

typedef struct tagITS_vca_TRect
{
	int				iTop;
	int				iBottom;
	int				iLeft;
	int				iRight;
} ITS_vca_TRect, *pITS_vca_TRect;

typedef struct tagTvca_LPRInfo
{
	Tvca_RGB		stCarColor;				//Body color
	int				iPlateColor;			//license plate color
	int      		iCarType;				//vehicle type
	int	      		iPlateType;				//License plate type
	int				iBrandType;				//The type of the cursor
	int				iIllgalType;			//Illegal type
	ITS_vca_TRect	stRectPlate;			//License plate position rectangle
	ITS_vca_TRect	stRectFace;				//face rectangle
	ITS_vca_TRect	stRectLogo;				//vehicle coordinates rectangular
	char			cPlateRec[LPR_PLATE_STR_LEN];// license plate number
	int				iHeadTail;				//front or rear: 0 unknown, 1 front, 2 rear
	ITS_vca_TRect	stRectCopilotFace;		//co-pilot face coordinate rectangle
	ITS_vca_TRect	stCarRegion;			//vehicle profile
} Tvca_LPRInfo, *pTvca_LPRInfo;

typedef struct tagTvca_LPRData
{
	int				iNumOutputs;			//output the number of results
	int				iSize;					//Input Parameters: Enter a single Tvca_LPRInfo structure size
	Tvca_LPRInfo	stOutputs[MAX_OUTPUT_LPR_NUM];//Can output multiple results
} Tvca_LPRData, *pTvca_LPRData;

typedef struct tagZFUserMarkPara
{
	int m_iUserMarkType;	//type, 0-no mark, 1-focus mark, 2-audio mark(the last two parameters are valid)
	int m_iVideoChan;		//video channel
	int m_iAudioTag;		//0-no audio(audio end), 1-has audio(audio start)
} ZFUserMarkPara, *pZFUserMarkPara;

typedef struct tagUDTimestamp
{
	unsigned int	iTimeStamp;	
} UDTimestamp, *pUDTimestamp;

typedef struct tagVcaTarget
{
	unsigned short	iTagetSize;				//size of TargetAttr
	unsigned short	iTargetCount;
}VcaTarget, *pVcaTarget;

typedef struct tagTargetAttrInfo
{
	unsigned short	iAttrInfoSize;			//size of TargetAttrInfo
	unsigned short	iTargetId;
	unsigned short	iAttrParamNum;
}TargetAttrInfo, *pTargetAttrInfo;

typedef struct tagTargetAttrParam
{
	int				iEnable;
	int				iType;
	int				iValue;
}TargetAttrParam, *pTargetAttrParam;

typedef struct
{
	int 	iPanPosition;		//Current horizontal coordinates 0-36,000 correspond to 0-360 degrees
	int		iTiltPosition;		//Current vertical coordinates 1000-19000 correspond to - 90-90 degrees
	int		iZoomPosition;		//Current multiplier 0-100000 corresponds to 0-1000 times
}TPtzInfo, *pTPtzInfo;

/**********************************************************************************************/


typedef struct _tagCurrentFrameInfo
{
	unsigned int	uiSize;					//size of struct
	unsigned int	uiFrameNO;				//frame index
	unsigned int	uiFrameType;			//frame type:I=0, P=1,B=2
	unsigned int	uiTimeStamp;			//time stamp 
} CurrentFrameInfo, *pCurrentFrameInfo;




/* Auto Test  */                                                                 
#define AUTOTEST_PARAM_SIZE			5
#define AUTOTEST_STRING_SIZE		65
typedef struct _strAutoTest_Para
{
	int			iTestParam[AUTOTEST_PARAM_SIZE];
	char			cTestParam[AUTOTEST_STRING_SIZE];
	char			cTestParam1[AUTOTEST_STRING_SIZE];
	int            iChannelNo;
}strAutoTest_Para;
/**********************************************************************************************/

typedef struct	//custom common information
{
	char			m_cParam1[LEN_64];
	char			m_cParam2[LEN_64];
	char			m_cParam3[LEN_64];
	char			m_cParam4[LEN_64];
	char    		m_cParam5[LEN_64];
	char    		m_cParam6[LEN_64];
	char    		m_cParam7[LEN_64];
	char    		m_cParam8[LEN_64];
	char    		m_cParam9[LEN_64];
	char    		m_cParam10[LEN_64];
	char    		m_cParam11[LEN_64];
	char    		m_cParam12[LEN_64];
	char    		m_cParam13[LEN_64];
	char    		m_cParam14[LEN_64];
	char    		m_cParam15[LEN_64];
	char    		m_cParam16[LEN_64];
	char    		m_cParam17[LEN_64];
	char    		m_cParam18[LEN_64];
	char    		m_cParam19[LEN_64];
	char    		m_cParam20[LEN_64];
}DZ_INFO_PARAM, *PDZ_INFO_PARAM;

/* NVSSDK, PlaySDKM4 Common structure  */                                                                  
typedef struct
{
    unsigned short  m_ulMajorVersion;
    unsigned short  m_ulMinorVersion;
    unsigned short  m_ulBuilder;
    char*           m_cVerInfo;
}SDK_VERSION;

#define MAX_MONITOR_DESCLEN			512
typedef struct MONITOR_INFO					
{
	char			cDriverDesc[MAX_MONITOR_DESCLEN];
	char			cDriverName[MAX_MONITOR_DESCLEN];	
	RECT			rect;
}MONITOR_INFO, *PMONITOR_INFO;

//Callback YUV data
#define AV_CBK_TYPE_VIDEO			0
#define AV_CBK_TYPE_AUDIO			1
#define AV_CBK_TYPE_VIDEO_CUDA		2
typedef struct tagDecAVInfo
{
	int				iType;				//0: video 1: audio
	int				iFormat;
	int				iWidth;
	int				iHeight;
	const unsigned char *pucData;
	int iDataLen;
}DecAVInfo, *PDecAVInfo;

typedef struct tagDecVideoOtherInfo
{
	unsigned int uiTimeStamp;
	int	iFrameRate;
	int iReserved;
	void* pvUserPutData;
	int iPitch;
	void* pvCudaContext;
}DecVideoOtherInfo;

typedef struct tagAudioSampleValue
{ 
	int			iChannel;				//audio channel num                
	int			iMaxSampleValue1;		//audio channel 1 max sample value range[-300~300]
	int			iMinSampleValue1;		//audio channel 1 min sample value
	int			iIsMaxBeforeMin1;		//whether audio channel 1 max sample value appear before  min sample value
	int			iMaxSampleValue2;		//audio channel 2 max sample value 
	int			iMinSampleValue2;		//audio channel 2 min sample value
	int			iIsMaxBeforeMin2;		//whether audio channel 2 max sample value appear before  min sample value
}AudioSampleValue, *PAudioSampleValue;

typedef struct tagDecAudioOtherInfo
{
	unsigned int uiTimeStamp;
	int	iFrameRate;
	AudioSampleValue* ptAudioSampleValue;//audio sample value
	int iAudioSampleRate;//audio sample rate
	int iAudioChannelNum; //audio channel num
}DecAudioOtherInfo;

typedef struct tagDecAVInfoEx
{
	DecAVInfo 		m_pInfo;
	//judge below value type with m_iOtherInfoSize 1:TimeStamp(unsigned int), 2:FrameRate(int)
	int				m_iOtherInfoSize;		
	void*	m_pvOtherInfo;  //DecVideoOtherInfo* or DecAudioOtherInfo*

}DecAVInfoEx, *PDecAVInfoEx;

typedef struct tagPSPACK_INFO
{
	unsigned long	nWidth;
	unsigned long	nHeight;
	unsigned long	nStamp;
	unsigned long	nType;
	unsigned long	nFrameRate;
	unsigned long	nReserved;
} PSPACK_INFO, *PPSPACK_INFO;

typedef struct
{
	unsigned long	nWidth;				//Video width, audio data is 0；
	unsigned long	nHeight;			//Video height, audio data is 0；
	unsigned long	nStamp;				//Time stamp(ms)。
	unsigned long	nType;				//Audio type，T_AUDIO8,T_YUV420，。
	unsigned long	nFrameRate;			//Frame rate。
	void*	nReserved;					//reserve
}FRAME_INFO_TD;
/**********************************************************************************************/

/*vital signs instrument information collect   */                                                               
#define VITAL_SIGN_HR		1			//heart rate
#define VITAL_SIGN_BO		2			//blood oxygen concentration
#define VITAL_SIGN_BP		3			//blood pressure
#define MAX_POINTS_NUM		512
typedef struct __tagVitalSignData 
{
	char			cVersion;			//treaty version
	char			cType;				//vital signs instrument information type,1：heart rate 2：blood oxygen concentration 3：blood pressure
	char			cMonFlg;			//1:normal 2:Upper limit 3：Ultra-low limit 4:probe off 5:device offline    
	char			iOscilloGramSeqID;	//从1开始，心电最多三波依次传递1，2，3，血氧一波，使用1
	int				iMonVal[4];			//体征实时值, 血压3个,SYS DIA PR依次存放，精度：原值乘以100;
	int				iGramRate;			//1s有多少个点
	int				iSizeType;			//视频叠加层大小  1:大 2：中 3：小
	int				iXPosition;			//相对于视频区左上角(0,0)的相对横坐标(万分比)
	int				iYPosition;			//相对于视频区左上角(0,0)的相对纵坐标(万分比)
	int				iPointLen;
	unsigned char	cPointValue[MAX_POINTS_NUM];
} VitalSignData, *pVitalSignData;

//frame absoluteTime time parameter
#if defined(IPHONE)
typedef struct __SAbsoluteTime
{
    unsigned short     iYear;                //Year
    unsigned short     iMonth;                //Month
    unsigned short     iDay;                //Day
    unsigned short     iHour;                //Hour
    unsigned short     iMinute;            //Minute
    unsigned short     iSecond;            //Second
    unsigned int uiTime;
} AbsoluteTimeIOS, *pAbsoluteTimeIOS;
#define AbsoluteTime AbsoluteTimeIOS
#else
typedef struct tagAbsoluteTime
{
    unsigned short    iYear;                //Year
    unsigned short    iMonth;                //Month
    unsigned short    iDay;                //Day
    unsigned short    iHour;                //Hour
    unsigned short    iMinute;            //Minute
    unsigned short    iSecond;            //Second
    unsigned int    uiTime;
} AbsoluteTime, AbsoluteTimeIOS, *pAbsoluteTime, *pAbsoluteTimeIOS;
#endif

typedef struct __tagTStreamData
{
	int				iSize;
	char			cStreamData[64];
	int				iStreamLen;
} TStreamData, *PTStreamData;

#define OSD_TYPE_CHANNELNAME		1
#define OSD_TYPE_TIMEDATE			2
#define OSD_TYPE_LOGOCOLOR			3
#define OSD_TYPE_ADDITIONALCHAR		4
#define OSD_TYPE_ITS				5
#define OSD_TYPE_ITS_COMBINEPIC		6
#define OSD_TYPE_DEBUGINFO      7

typedef struct
{
	unsigned int		uiOSDType;			//1：Channel name（Text） 2：Time and date 3:logo color 4：append osd  characters 5:traffic ipc exclusive use  6：traffic ipc combining picture
	unsigned int		uiOSDBGColor;		// back color, total 32 bit low 24 bit means color rgb,bgr in figure：0x00BBGGRR。High 8bit useless
	unsigned int		uiOSDColor;			//text color
	unsigned int		uiOSDSize;			// 1～5,total 32 bit low 16 bit means Height ，High 16 bit means width,0-Auto
	unsigned int		uiOSDDiaphaneity;	//diaphaneity  range【0～100】
	unsigned int		uiOSDPostionX;		//abscissa or location number
	//1：if uiOSDPostionY equal to 0 ；uiOSDPostionX means location number ，range【0～15】，
	//2：if uiOSDPostionY greater t 0；uiOSDPostionX and uiOSDPostionY combine a point means osd postion，uiOSDPostionY must plus 1 ，eg: want set(16,0)，network treaty send（16，1），ipc actual set（16，0）
	unsigned int		uiOSDPostionY;		
	int					ilength;
	char				cText[LEN_512];		//osd text content
} osd_TOSDSet;

typedef struct
{
	double f64Fx;
	double f64Fy;
	double f64Cx;
	double f64Cy;
	double af64K[12];
} UdtCameraPara;

typedef struct
{
	int					iType;		//0-reserveed 1-warehouse camera 2-fish camera
	int					ilength;
	UdtCameraPara		tUdtCameraPara;
} osd_TLDCSet;
/**********************************************************************************************/

typedef struct __tagExtraInfo
{
	int				m_iInfoLen;
	void*			m_pvExtInfo;	
}FRAME_EXT_INFO, *LPFRAME_EXT_INFO;

typedef struct _RECT_S 
{ 
	short 			sLeft; 
	short 			sTop; 
	short 			sRight; 
	short 			sBottom; 
}RECT_S, *LPRECT_S;

//Its Face Coordinate Structure
typedef struct _RECT_ITS 
{ 
	short 			sLeft; 
	short 			sRight; 
	short 			sTop; 
	short 			sBottom; 
}RECT_ITS, *LPRECT_ITS;

//Schedle time
typedef struct
{
	unsigned short  iStartHour;
	unsigned short  iStartMin;
	unsigned short  iStopHour;
	unsigned short  iStopMin;
	unsigned short  iRecordMode;			//iEnable;
}NVS_SCHEDTIME,*PNVS_SCHEDTIME;

typedef struct
{
	unsigned short  m_u16Brightness;
	unsigned short  m_u16Hue;
	unsigned short  m_u16Contrast;
	unsigned short  m_u16Saturation;
	NVS_SCHEDTIME strctTempletTime;
}STR_VideoParam;


#define TYPE_NVS_T          0     //T NVS
#define TYPE_KEYBOARD       1     //Net keyboard
#define TYPE_CTL_ALLOTER    2     //Control code distributor
#define TYPE_ALARMHOST      3     //Network alarm host
#define TYPE_NVS_S          4     //S NVS
#define TYPE_MATRIX         5     //Matrix
#define TYPE_NET_PU         6     //Network Front-end device
#define TYPE_NET_GURAD      7     //Net Entrance Gurad
#define TYPE_PC_DVR         8     //PC DVR
#define TYPE_S_DECODER      9     //S Decoder
#define TYPE_NVS_TPLUS      10    //TPLUS NVS
#define TYPE_WIH_S          11    //Unattended host
#define TYPE_EMBED_DVR      12    //embedded DVR
#define TYPE_SMS_200        13    //Linux stream media host
#define TYPE_ALARM_HOST42   14    //network alarm host
#define TYPE_TC_T820GE      15    //external trigger intersection camera
#define TYPE_TC_T860GE      16    //pure video intersection camera
#define TYPE_TC_T890GE      17    //external trigger ptz
#define TYPE_TC_HDC         18    //HD camera   
#define TYPE_ONVIF          19    //Onvif device 
#define TYPE_NVS_LG         20    //LG device 

/**********************************************************************************************
* public notify                                                            
**********************************************************************************************/
#ifndef __WIN__
typedef void (*pfCBGetDecAV)(int _iID, const DecAVInfo* _pDecAVInfo, void* _iUser);
#endif

typedef void (__stdcall *DEVUSERDATA_NOTIFY)(unsigned int _uiID, int _iType, void* _pvData
			 , int _iDataLen, CurrentFrameInfo* _ptInfo, void* _pvUdata);

//video and audio date after decoding
typedef void (*DECYUV_NOTIFY_V4)(unsigned int _ulID,unsigned char *_cData, int _iLen, 
								 const FRAME_INFO_TD *_pFrameInfo, void* _iUser);

typedef int  (*PSPACK_NOTIFY)(int _iPlayerID, unsigned char* _ucData, int _iSize, 
							  PSPACK_INFO* _pPsPackInfo, void* _pContext);
/**********************************************************************************************/

#define	ENABLE_LAST_FRAME   0x13001		//keep last frame when disconnect
#define ENABLE_PREFERENCE   0x13002		//preview preference set
#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN		64
#endif
#define FILE_COUNT			16	//max support num
#define FILE_COUNT_EX		8	

//存储参数导出文件名，支持所有设备 
//Storage parameters export file name, support all devices
#define DEVPARAMFILE_STORAGE				"config_rec.dat"

//报警参数导出文件名，支持所有设备
//Alarm parameter export file name, support all devices
#define DEVPARAMFILE_ALARM					"config_alm.dat"
#define DEVPARAMFILE_ALARM_CONFIG			"config_alarm.ini"
#define DEVPARAMFILE_ALARM_SERIAL_ALARM		"config_serialalarmercnf.dat"
#define DEVPARAMFILE_ALARM_NET_ALARM		"config_netalarmercnf.dat"

//预览参数导出文件名，支持所有设备
//Preview parameter export file name, support all devices
#define DEVPARAMFILE_PREVIEW_DISPLAY		"./gui_config/display.cnf"
#define DEVPARAMFILE_PREVIEW_SWITCH			"config_switch.dat"

//智能分析参数导出文件名，支持所有设备 
//Intelligent analysis parameter export file name, support all devices
#define DEVPARAMFILE_VCA					"config_vca.dat"
#define DEVPARAMFILE_SCENE					"config_scene.dat"

//智能分析参数导出文件名，仅支持存储nvr系列设备
//Intelligent analysis parameter export file name, only support to store NVR series device
#define DEVPARAMFILE_VCAEX					"config_vca_ex.dat"
#define	DEVPARAMFILE_ITS					"config_its.ini"
#define DEVPARAMFILE_ITS_DAT				"config_its.dat"

//系统参数导出文件名，支持所有设备
//System parameters export file name, support all devices
#define DEVPARAMFILE_SYSTEM_SERVER			"config_server.ini"
#define DEVPARAMFILE_SYSTEM_EXTEND			"extendword.txt"
#define DEVPARAMFILE_SYSTEM_PTZ				"config_ptz.dat"
#define DEVPARAMFILE_SYSTEM_RIGHT			"config_right.dat"
#define DEVPARAMFILE_SYSTEM_DG				"config_dg.dat"
#define DEVPARAMFILE_SYSTEM_BURN			"config_burn.dat"
//系统参数导出文件名，仅支持SmartS3E系列设备
//System parameters export file name, only support smarts3e series devices
#define DEVPARAMFILE_SYSTEM_COVER			"config_pdcover.dat"
//系统参数导出文件名，仅支持多目相机系列
//System parameters export file name, only support multi camera series
#define DEVPARAMFILE_CHN0					"config_chn0.ini"
#define DEVPARAMFILE_NET					"config_net.ini"
//系统参数导出文件名，仅支持政法球系列
//System parameters export file name, only support political and legal ball series
#define DEVPARAMFILE_SYSTEM_DOMESETTING		"dome_cfg/dome_setting.dat"
#define DEVPARAMFILE_SCENE_PATTERN			"dome_cfg/pattern.dat"

typedef struct tagExportConfig
{
	int		iSize;		    //Size of the structure,must be initialized before used
	int		iCount;		    //需要导出的文件总个数	Total number of files to be exported      
	char   	cFileList[FILE_COUNT][MAX_NAME_LEN];	//需要导出的文件名称列表 List of file names to be exported
	char    cFileOut[LEN_128];  //导出到本地生成的box文件名 Export to locally generated box file name
	char   	cFileListEx[FILE_COUNT_EX][MAX_NAME_LEN];	//附加文件名称列表，超过16个文件的赋值给该变量 Attached file name list, more than 16 files assigned to this variable 
} ExportConfig  ,*PExportConfig;

typedef int RAWFRAMETYPE;
#define VI_FRAME 0
#define VP_FRAME 1
#define AUDIO_FRAME 5

#define RAW_VIDEO_H264		1
#define RAW_VIDEO_MPEG4		2
#define RAW_VIDEO_MJPEG		41
#define RAW_VIDEO_H265		23

#define RAW_AUDIO_G711_A	0x01
#define RAW_AUDIO_G711_U	0x02
#define RAW_AUDIO_ADPCM_A	0x03
#define RAW_AUDIO_AAC		0x16

typedef struct
{
	unsigned int nWidth;    //Video width, audio data is 0
	unsigned int nHeight;   //Video height, audio data is 0
	unsigned int nStamp;    //Time stamp(ms)
	unsigned int nType;     //RAWFRAMETYPE, I Frame:0,P Frame:1,B Frame:2,Audio:5
	unsigned int nEnCoder;  //Audio or Video encoder(Video,0:H263,1:H264, 2:MP4. Audio:0,G711_A:0x01,G711_U:0x02,ADPCM_A:0x03,G726:0x04)
	unsigned int nFrameRate;//Frame rate
	unsigned int nAbsStamp; //Absolute Time(s)
	unsigned char ucBitsPerSample;// bit per sample [8/16/24] default 16
	unsigned int uiSamplesPerSec;// Samples Per Sec，default 8000
}RAWFRAME_INFO;

typedef struct
{
	unsigned int nWidth;    //Video width, audio data is 0
	unsigned int nHeight;   //Video height, audio data is 0
	unsigned int nStamp;    //Time stamp(ms)
	unsigned int nType;     //RAWFRAMETYPE, I Frame:0,P Frame:1,B Frame:2,Audio:5
	unsigned int nEnCoder;  //Audio or Video encoder(Video,0:H263,1:H264, 2:MP4. Audio:0,G711_A:0x01,G711_U:0x02,ADPCM_A:0x03,G726:0x04)
	unsigned int nFrameRate;//Frame rate
	unsigned int nAbsStamp; //Absolute Time(s)
	unsigned char ucBitsPerSample;// bit per sample [8/16/24] default 16
	unsigned int uiSamplesPerSec;// Samples Per Sec，default 8000
	unsigned int ui32FrameNO;
	unsigned int uiResPara1;
	unsigned int uiResPara2;
	unsigned int uiResPara3;
	unsigned int uiResPara4;
	char		 cResPara5[LEN_64];
	char		 cResPara6[LEN_256];
}RAWFRAME_INFOEX;

//Not decode the standard data before the pure h264 data
typedef void (__stdcall *RAWFRAME_NOTIFY)(unsigned int _ulID,unsigned char* _cData,int _iLen, RAWFRAME_INFO *_pRawFrameInfo, void* _iUser);
typedef void (__stdcall *RAWFRAME_NOTIFY_EX)(unsigned int _ulID,unsigned char* _cData,int _iLen, RAWFRAME_INFOEX *_pRawFrameInfo, void* _iUser);

#define RAW_NOTIFY_ALLOW_DECODE		0
#define RAW_NOTIFY_FORBID_DECODE	1
typedef struct _tagRawFrameNotifyInfo
{
	RAWFRAME_NOTIFY pcbkRawFrameNotify;		//raw data notify with play control
	void*			pUserData;
	int				iForbidDecodeEnable;	//whether need to play     0-play 1-no need decode and play
}RawFrameNotifyInfo, *pRawFrameNotifyInfo;

#define PLAY_VIDEO_HEADER_LEN			88
typedef struct __structRawCbkPlayerHeader
{
	int iSize;			//buf size
	void* pWnd;
	unsigned char cHeader[PLAY_VIDEO_HEADER_LEN];
	RawFrameNotifyInfo tRawNotify;
}S_RawCbkPlayerHeader, *PS_RawCbkPlayerHeader;
#endif
