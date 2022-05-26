/**
* @file      hik_decoder_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  hik_decoder_pu.h defines
*/
#ifndef __HIK_DECODER_PU_H__
#define __HIK_DECODER_PU_H__

#include "pu.h"
#include "3rd/hik/sdk-plat.h"
#include "tmap.h"


class HikDecoderPu : public Pu
{
public:
    friend void CALLBACK decExcept_cb(DWORD    dwType,LONG     lUserID,LONG     lHandle,void     *pUser);
    friend void CALLBACK decLogin_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser);

    HikDecoderPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikDecoderPu();

    virtual int        start();

protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();


    virtual bool       validate(int channelNo){ return true; }
    virtual bool       validate(string gucode) { return true; }

    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

    virtual int        onCustomCommand(string& json);

    //matrix
    int        DecoderSwitch(uint32_t u32Mode, uint32_t u32Channel, uint32_t u32OutPort, string subHost, int subPort);
    int        DecoderSetDisplayMode(uint32_t u32DisPort, uint32_t u32Mode, string subHost, int subPort);
    int        DecoderBandDecChnToDisWnd(int s32DecChnNo, uint32_t u32DisPort, int s32WndNo, string  subHost, int subPort);
    int        DecoderStartDynamic(int s32DecChnNo, string host, int port, int chnNo, string name, string pass, string  subHost, int subPort);
    int        DecoderStopDynamic(int s32DisOutPort, string subHost, int subPort);

    int        openOutWindow(int disPortId, int wndCount);
    int        decoderVideo(int displayPort, int index, string host, int port, int chnNo, string username, string password);


    //B20 --songhongliang
    //关闭所有窗口
    int        B2xColseAllWin(DWORD wallNo);

    //获取电视墙分辨率
    int        B2xGetResolution(DWORD wallNo, int &w, int &h);

    //创建窗口
    int        B2xNewWnd(DWORD wallNo, int w, int h, int wndcount);

    //动态解码
    int        B2xStartDecode(DWORD wallNo, DWORD wndNo, const char* ip, int port, int chnNo,const char* username, const char* password);


private:
    int                GetDecoderAbility();
    int                resetOutWindow(int id);

protected:

public:

private:
    NET_DVR_MATRIX_ABILITY_V41 m_Ability;
    NET_DVR_PU_STREAM_CFG      m_StreamCfg;
    NET_DVR_MATRIX_VOUTCFG     m_DispChanCfg[MAX_ANALOG_CHANNUM];  
    LONG                       m_lUserID;


    tmap<int, vector<DWORD> > m_decChnMap;
};
#endif
