/**
 * @file      hik_pu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 11:55:51
 *
 * @note
 *  hik_pu.h defines
 */

#ifndef __HIK_PU_H__
#define __HIK_PU_H__
#include "3rd/hik/sdk-plat.h"
#include "pu.h"
#include "tmap.h"
#include "tevqueue.h"

#define CBK            CALLBACK 

typedef NET_DVR_DEVICECFG_V40 HIK_DVR_CFG;
typedef tmap<long, Pu *>      UserMap;


class HikPu : public Pu
{
public:
    HikPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikPu();

    long               getUserID();
    HIK_DVR_CFG        getDvrCfg();

    virtual int        start();

protected:
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    virtual int        ptzCtrl(int cmd, int channelNo ,   int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode , int speed, int preset, bool stop);

    virtual int        queryRecord(int channelNo ,  int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

    virtual int        syncTime(datetime_s &dt, const string &tzone);
    
    virtual int        onCustomCommand(string& json);
    virtual int        onRequestConfig(string & jin, string& jout);

	virtual void       onTimer();

    virtual int				getPicSize(int chanNo);
	int GetSize(unsigned char  * buff, long length, int &height, int &weidth);

private:
    int                getAbilityPlate();
    int                startListenCall();

    int                doCallAnswer(uint32_t u32Cmd);
    int                getPrivacyMask(int chnNo, NET_DVR_PICCFG_V40* shelter);
    int                setPrivacyMask(int chnNo, int enable, NET_VCA_POINT* shelter, int cnt);
    int                getShowString(int chnNo, NET_DVR_SHOWSTRING_V30 *showString);
    int                setShowString(int chnNo, NET_DVR_SHOWSTRING_V30 *showString);
    int                ptzPostion3D(int chnNo, NET_DVR_POINT_FRAME* netDvrPtFrame);
	int				   setOSDInfo(int chnNo, NET_DVR_PICCFG_V30 *showString);

    int                getPtzCruise(int chnNo, int cruiseRoute, NET_DVR_CRUISEPOINT_V50* struCruisePointV50);
    int                addPtzCruise(int chnNo, int cruiseRoute, int seq, int presetNum, int dwell, int speed);
    int                delPtzCruise(int chnNo, int cruiseRoute, int seq, int presetNum);
    int                enablePtzCruise(int chnNo, int cruiseRoute, bool bStart);
    int                delPtzCruiseRoute(int chnNo, int curiseID);

    /*liuwei 20190422 */
    int                getVideoEffect(int channelNo, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue);
    int                setVideoEffect(int channelNo, DWORD brightValue, DWORD contrastValue, DWORD saturationValue, DWORD hueValue);
    int                getVideoEffect(string addrcode, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue);
    int                setVideoEffect(string addrcode, DWORD brightValue, DWORD contrastValue, DWORD saturationValue, DWORD hueValue);
    int                lockRecordFile(string fileName, bool lock);
    int                getRecordLabel(int channelNo, datetime_s &start, datetime_s &end, string &json);
    int                ptzPatternCtrl(int channelNo, int patternCmd, int patternId);
    int                setPtzParkaction(int chn, int enable, int parkTime, int actionType, int id);
    int                getPtzParkaction(int chn, int &enable, int &parkTime, int &actionType, int &id);
    int                lockRecordFileByTime(int channelNo, string &startTime, string &stopTime, int duration);

    /* gate ctrl zhangshenyu 20200509*/
    int                getEntance(NET_DVR_ENTRANCE_CFG *pStruEntrance);
    int                setEntance(NET_DVR_ENTRANCE_CFG *pStruEntrance, BYTE ctrlMode);
    int                gateCtrl(int cmd);
    void               getGateStatus();
	int                controlDoorGate(int ctrlValue);

    int                vehicleCtrl(string &json);
    int                getWhiteBlackList(string & jin, string& jout);

    /* 人数统计清零 */
    int                resetCounter(string json);
    
    int                onGetChannelStatus(const string jin, string &jout);
	
	int				   onGetChannelStatus_all(const string jin, string &jout);
	/*??è?OSD×?±ê2?êy*/
	int getOSDIfoCor(int chnNo, NET_DVR_PICCFG_V40 *showString);

    /* capture a picture 20200908 zhangshenyu add*/
    //int                capturePic(string json);
	int capturePic(string json, string& jout);

	void	getResolution(int index, int& height, int& weight);
	int                AnalysisHotPicData(char* P2PDataBuff, char*buff, int Height, int Width, char* filePath, bool b_save_file, float* p_maxTemp, float* p_minTemp);

	int                captureThermalFile(const string jin, string &jout);


    /* ????????(????????????) */
    int onSetPTZPos(const string jin);

    /* ????????(????????????) */
    int onGetPTZPos(const string jin, string &jout);

    /* ??????????????? */
    int onSetThermometryPresetInfo(const string jin);

    /* ???????? */
    int onGetThermometryResult(const string jin, string &jout);

	/*开始测温*/
	int	StartMeaTemperature(const string jin, string &jout);

	/*停止测温*/
	int	StopMeaTemperature(const string jin);

	/*获取测温结果*/
	int	getMeaTemperature(const string jin,string &jout);
	
	int getPtzStatus(int channelNo, string& jout);

	int remoteCapture(const string json, string &jout);

	/*测温参数配置*/
	int	temperatureParaset(const string jout);

	int getRealpicture(const string json, string &jout);


    friend void CBK    login_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser);
    friend void CBK    callData_cb(DWORD dateType, void* data, DWORD size, void* userData);
    friend void CBK    alarm_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData);
    friend void CBK    excep_cb(DWORD type, LONG userId, LONG handle, void *userData);
    friend void CBK    vehicle_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData);
    friend void CBK    thermInfo_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData);

    int                updateChnStatus();

private:
    string             m_privDomain;
    long               m_userId;
    long               m_remoteCtrlId;
    int                m_manuPlateType;



    int                          m_callStatus; // 0: normal     1:ring      2:fail
    static UserMap               m_userMap;
    HIK_DVR_CFG                  m_dvrCfg;
	LPNET_DVR_THERMOMETRY_UPLOAD m_lpThermometry;
	unsigned long                m_meatemptype;
	unsigned long                m_meatempstatus;


    tevent<string, int> m_evtSetVihicle;
    tevent<string, string> m_evtGetTemp;

    //shl
    Mutex              m_setStringMtx;
};

#endif
