#ifndef DZM_ACTIONCTROL_H
#define DZM_ACTIONCTROL_H

#include "GlobalTypes.h"
//0x00-0x63 general  function
//0x00-0x2f decoder control
#define     REGISTER						0         //register  function
#define     MOVE_UP							1         //ptz up
#define     MOVE_UP_STOP					2         //stop ptz up
#define     MOVE_DOWN						3         //ptz down
#define     MOVE_DOWN_STOP					4         //stop ptz down
#define     MOVE_LEFT						5         //ptz left
#define     MOVE_LEFT_STOP					6         //stop ptz left
#define     MOVE_RIGHT						7         //ptz right
#define     MOVE_RIGHT_STOP					8         //stop ptz right
#define     MOVE_UP_LEFT					9         //ptz top left
#define     MOVE_UP_LEFT_STOP				10        //stop ptz top left
#define     MOVE_UP_RIGHT					11        //ptz top right
#define     MOVE_UP_RIGHT_STOP				12        //stop ptz top right
#define     MOVE_DOWN_LEFT					13        //ptz down left
#define     MOVE_DOWN_LEFT_STOP				14        //stop ptz down left
#define     MOVE_DOWN_RIGHT					15        //ptz down right
#define     MOVE_DOWN_RIGHT_STOP			16        //stop ptz down right
/************************************************************************/
/* PTZ control  treaty                                                                   */
/************************************************************************/
#define     PROTOCOL_MOVE_UP                1        //move up        
#define     PROTOCOL_MOVE_DOWN              2        //move down      
#define     PROTOCOL_MOVE_LEFT              3        //move left      
#define     PROTOCOL_MOVE_RIGHT             4        //move right     
#define     PROTOCOL_MOVE_UP_RIGHT          5        //move up_right  
#define     PROTOCOL_MOVE_UP_LEFT           6        //move up_left   
#define     PROTOCOL_MOVE_DOWN_RIGHT        7        //move down_right
#define     PROTOCOL_MOVE_DOWN_LEFT         8        //move down_left 
#define     PROTOCOL_MOVE_STOP              9        //stop move    
#define     PROTOCOL_ZOOMIN                 10       //magnify
#define     PROTOCOL_ZOOMOUT                11       //lessen
#define		PROTOCOL_IST					12		//stop change
#define		PROTOCOL_FOCUS_ON				13		//focus near
#define		PROTOCOL_FOCUS_OFF				14		//focus far
#define		PROTOCOL_PTZ_FST		 		15		//stop focus change
#define		PROTOCOL_IA_ON			 		16		//aperture automatic adjust(aperture change stop)
#define		PROTOCOL_IRIS_OPEN				17 	    //aperture bigger
#define		PROTOCOL_IRIS_CLOSE			    18		//aperture samller
#define		PROTOCOL_RAIN_ON				19		//windshield wiper start
#define		PROTOCOL_RAIN_OFF				20		//windshield wiper stop
#define		PROTOCOL_LIGHT_ON				21		//light on 
#define		PROTOCOL_LIGHT_OFF				22		//light off
#define		PROTOCOL_AUTO_SCAN_START		23		//start scanning
#define		PROTOCOL_AUTO_SCAN_STOP			24		//stop scaning
#define		PROTOCOL_PRESET					25		//run to the preset spot
#define		PROTOCOL_START_TRACK_CRUISE		26		//start cruise
#define		PROTOCOL_STOP_TRACK_CRUISE 		27		//stop cruise
#define		PROTOCOL_SET_PRESET		 		28       //set preset spot
#define		PROTOCOL_POWER_ON		 		29		//take on power
#define		PROTOCOL_POWER_OFF		 		30		//take off power
#define		PROTOCOL_3D_POSITION	 		31		//PTZ 3D location
#define		PROTOCOL_3D_ROCKER		 		32		//PTZ 3D rocker	 
#define		PROTOCOL_ASSISTANT_ON	 		33		//assistant on
#define		PROTOCOL_ASSISTANT_OFF	 		34		//assistant off
#define		PTZ_START_ROUTINE				35		//ptz start pattern routine
#define		PTZ_STOP_ROUTINE				36		//ptz stop pattern routine
#define		PROTOCOL_DELETE_PRESET			37		//delete preset spot
#define		PTZ_ABFN						38		//back focus near
#define		PTZ_ABFR						39		//back focus far
#define		PTZ_ABFST						40		//back focus stop change


#define     SET_LEFT_BORDER					17      //set  left border
#define     SET_RIGHT_BORDER				18      //set  right border 
#define     SET_UP_BORDER					19      //set  up border
#define     SET_DOWN_BORDER					20      //set  down border
#define     HOR_AUTO						21      //horizontal auto        
#define     HOR_AUTO_STOP					22		//stop horizontal auto
#define     SET_HOR_AUTO_BEGIN				23		//set  horizontal auto start
#define     SET_HOR_AUTO_END				24      //set  horizontal auto end
#define     SET_HOR_AUTO_SPEED				25      //set  horizontal auto speed
#define     VER_AUTO						26      //vertical auto
#define     SET_VER_AUTO_BEGIN				27      //set  vertical auto start
#define     SET_VER_AUTO_END				28      //set  verticall auto end
#define     SET_VER_AUTO_SPEED				29      //set  verticall auto speed
#define     VER_AUTO_STOP					30      //stop vertical auto

#define     ZOOM_BIG						31      //zoom big
#define     ZOOM_BIG_STOP					32      //stop zoom big
#define     ZOOM_SMALL						33      //zoom small
#define     ZOOM_SMALL_STOP					34      //stop zoom small
#define     FOCUS_FAR_TD					35      //focus far
#define     FOCUS_FAR_STOP					36      //stop focus far
#define     FOCUS_NEAR_TD					37      //focus near
#define     FOCUS_NEAR_STOP					38      //stop focus near
#define     IRIS_OPEN_TD					39      //open iris 
#define     IRIS_OPEN_STOP					40      //stop open iris
#define     IRIS_CLOSE_TD					41      //close iris
#define     IRIS_CLOSE_STOP					42      //stop close iris

#define     LIGHT_ON						43      //light on
#define     LIGHT_OFF						44      //light off
#define     POWER_ON						45      //power on
#define     POWER_OFF						46      //power off
#define     RAIN_ON							47      //rain on
#define     RAIN_OFF						48      //rain off
#define     TALK_ON							49      //talk on 
#define     TALK_OFF						50      //talk off
#define     DEF_ON							51      //arm 
#define     DEF_OFF							52      //disarm

#define     BROWSE							53      //inquiry
#define     ALARM_ANSWER					54      //alarm response
#define     STATE_ASK						55      //ask for upload control state


//0x30-0x4f dome control
#define     MOVE							60      //dome move
#define     MOVE_STOP						61      //stop dome move
#define     CALL_VIEW						62      //call camera
#define     SET_VIEW						63      //set preset
#define     DELETE_VIEW						64      //delete preset
#define     SEQUENCE_BEGIN					65      //start cruise
#define     SEQUENCE_END					66      //stop cruise
#define     ADD_PRESET						67      //add  cruise
#define     DELETE_PRESET					68      //delete cruise
#define     CLEAR_SEQUENCE					69      //clear cruise setup
#define     TIME_SEQUENCE					70      //set cruise time
#define     SET_GUARD						71      //set guard
#define     DELETE_GUARD					72      //delete guard
#define     GET_HOR_AUTO_SPEED				73      //get horizontal auto speed
#define     GET_TRACK_TIME					74      //get  cruise time
#define     GET_TRACK_SEQUENCE				75      //get  cruise sequence
#define     GUARD_TIME						76      //set  guard time
#define     GET_VIEW_LIST					77      //get  cameras' sequence
#define     MEMU_OPEN						78      //open Menu  Call 95
#define     MEMU_CLOSE              		79      //close menu Call 96

//0x64-0xc7 special function
#define     SWITCH							101     //manually switch matrix
#define     SWITCH_MONITOR					102		//switch monitor     
#define     SWITCH_VIDICON					103     //switch camera
#define     SWITCH_ORDER					104     //sequential switch  
#define     SWITCH_ORDER_STOP				105     //stop sequential switch                                       
#define     SWITCH_GROUP					106     //group switch
#define     SWITCH_GROUP_STOP				107     //stop group switch


//Heavy-duty PTZ  Coaxial mode control function
#define     COMMON_MOVE_LEFT               	1        //common left
#define     COMMON_MOVE_UP_LEFT            	2        //common top left
#define     COMMON_MOVE_UP                 	3        //common up
#define     COMMON_MOVE_UP_RIGHT           	4        //common top right
#define     COMMON_MOVE_RIGHT              	5        //common right
#define     COMMON_MOVE_DOWN_RIGHT         	6        //common down right
#define     COMMON_MOVE_DOWN               	7        //common down
#define     COMMON_MOVE_DOWN_LEFT          	8        //common down left
#define     COMMON_MOVE_STOP               	0        //common stop


typedef struct tagData_t
{
	int				iAddress;			//device address
	int				iPreset;			//preset spot
	POINT			point;				//move coordinate
	unsigned char	DecBuf[256];		//out put information num
	int				iCount;				//in or out num
	int				iGuardTime;			//guard time
	int				param[7];			//when 3D location use this ,and the params above are useless
} Data_t, *pData_t;

#endif

