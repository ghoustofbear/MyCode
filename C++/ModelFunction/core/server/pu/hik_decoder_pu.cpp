#include "hik_decoder_pu.h"
#include "json/json.h"
#include "global_str_def.h"
#include "math.h"
#include "system.h"

#include "log.h"
#include "putree.h"
#include "debug.h"

void CALLBACK decExcept_cb(
        DWORD    dwType,
        LONG     lUserID,
        LONG     lHandle,
        void     *pUser
        )
{
    HikDecoderPu *pu = (HikDecoderPu *)pUser;

    
    LOGI_MSV("exception occur type: 0x%x, ip: %s, userId: %d", dwType, pu->ip().c_str(), lUserID);

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pu->name();
    value["deviceHost"] = pu->host();
    value["devicePort"] = pu->port();

    if (EXCEPTION_EXCHANGE == dwType)
    {
        /* Make sure write log once time */
        if (pu->m_login)
        {
            pu->m_login = false;
            pu->writeDevLog(STR_MSV_OFF_LINE);
        }
    }
    else if (RESUME_EXCHANGE == dwType || ALARM_RECONNECTSUCCESS == dwType)
    {
        pu->m_login = true;
        pu->writeDevLog(STR_MSV_RE_ON_LINE);
    }
    else
    {
        return;
    }
}

void CALLBACK decLogin_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser)
{
    HikDecoderPu *pu = (HikDecoderPu *)pUser;

    if (1 == result)
    {
        LOGI_MSV("Device: %s login success! user ID: %d", pu->ip().c_str(), userId);
        pu->m_login = true;
        pu->writeDevLog(STR_MSV_ON_LINE);
    }

    LOGI_MSV("login reslut: %d, %s", result, pu->ip().c_str());
}


HikDecoderPu::HikDecoderPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HIK;
    m_port = port;
    m_domainType = domainType;
    //m_userId = -1;
    //m_startTalkChn = 0;
}



HikDecoderPu::~HikDecoderPu()
{
    
}

int HikDecoderPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    /* 初始化软件包 */
    NET_DVR_Init();                  

    /* 设置连接的超时设置 */
    NET_DVR_SetConnectTime(3000, 10);

    uint32_t u32Version = NET_DVR_GetSDKBuildVersion();

    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n",
            (0xff000000 & u32Version)>>24,
            (0x00ff0000 & u32Version)>>16,
            (0x0000ff00 & u32Version)>>8,
            (0x000000ff & u32Version));
    printf(strTemp);

    m_init = TRUE;

    return 0;
}

void HikDecoderPu::stop()
{
    if (!m_init)
        return;

    m_init = FALSE;

    logout();
    
    Pu::stop();
}

int HikDecoderPu::login()
{
    if (m_login)
        return 0;

    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    memset(&struDeviceInfo, 0, sizeof(struDeviceInfo));

    /* 设置异常消息回调函数 */
    NET_DVR_SetExceptionCallBack_V30(0, NULL, decExcept_cb, (void *)this);
    NET_DVR_SetReconnect(5000);

    NET_DVR_USER_LOGIN_INFO loginInfo;
    NET_DVR_DEVICEINFO_V40  deviceInfo;
    memset(&loginInfo, 0, sizeof(loginInfo));
    memset(&deviceInfo, 0, sizeof(deviceInfo));

    loginInfo.wPort = m_port;
    loginInfo.bUseAsynLogin = true;
    loginInfo.cbLoginResult = decLogin_cb;
    loginInfo.pUser = this;

    strncpy(loginInfo.sDeviceAddress, m_ip.c_str(), sizeof(loginInfo.sDeviceAddress));
    strncpy(loginInfo.sUserName, m_user.c_str(), sizeof(loginInfo.sUserName));
    strncpy(loginInfo.sPassword, m_pswd.c_str(), sizeof(loginInfo.sPassword));

    m_lUserID = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
    if (m_lUserID < 0)
    {
        LOGE_MSV("NET_DVR_Login_V30 error, %d", NET_DVR_GetLastError());
        return -1;
    }

    if (m_type == Pu::DEV_TYPE_DECODE)
    {
        NET_DVR_DEVICECFG_V40 dvrCfg;
        DWORD dwReturn;
        NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_DEVICECFG_V40, -1, &dvrCfg, sizeof(dvrCfg), &dwReturn);
        LOGI_MSV("hik matrix version:%d\n", dvrCfg.byDVRType);
        //GetDecoderAbility();
    }

    return 0;
}

void HikDecoderPu::logout()
{

    if (!m_login)
        return;

    if (-1 != m_lUserID)
    {
        NET_DVR_Logout_V30(m_lUserID);
        m_lUserID = -1;
    }

    m_login = false;
    //g_pLog->DvivceLog(GetAddrCode(), "下线");    
}

int
HikDecoderPu::onCustomCommand(string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string jMethod = rvalue["Method"].asString();
    SHL_Json::Value itemlist = rvalue["Item"];
    int realNum = itemlist.size();
    int outPort = rvalue["DisPort"].asInt();


    if (jMethod == "WindowDecoder")
    {
        if (type() == Pu::DEV_TYPE_DECODE_6901)
        {
            if (0 != openOutWindow(outPort, realNum))
            {
                return -1;
            }

            int  i = 0;
            for (i = 0; i < realNum; ++i)
            {
                SHL_Json::Value element;
                element = itemlist[i];

                string strIp = element["Ip"].asCString();
                int    u16Port = element["Port"].asInt();
                int    u16Chn = element["Chn"].asInt();

                Pu *pDecPu = putree->findPu(strIp, u16Port);
                if (pDecPu)
                {
                    decoderVideo(outPort, i, strIp, u16Port, u16Chn, pDecPu->userName(), pDecPu->passWord());
                }
            }
        }
        else if (type() == Pu::DEV_TYPE_DECODE)
        {
            int abc = DecoderStopDynamic(outPort, "", 0);
            int iSubNum = ceil(sqrt(realNum));
            abc = DecoderSetDisplayMode(outPort, iSubNum*iSubNum, "", 0);

            int  i = 0;
            int idecNo = 0;
            for (i = 0; i < realNum; ++i)
            {
                SHL_Json::Value element;
                element = itemlist[i];

                string strIp = element["Ip"].asCString();
                int    u16Port = element["Port"].asInt();
                int    u16Chn = element["Chn"].asInt();

                Pu *pDecPu = putree->findPu(strIp, u16Port);
                if (pDecPu)
                {
					while (idecNo < MAX_UT_CHANNEL_NUM)
                    {
                        idecNo++;
                        if (0 != DecoderBandDecChnToDisWnd(idecNo, outPort, i, "", 0))
                        {

                            continue;

                        }
                        if (0 != DecoderStartDynamic(idecNo, strIp, u16Port, u16Chn, pDecPu->userName(), pDecPu->passWord(), "", 0))
                        {

                            continue;
                        }
                        break;
                    }
                }
            }
        }
        else if (type() == Pu::DEV_TYPE_DECODE_B20)
        {
            if (0 != B2xColseAllWin(outPort))
            {
                return -1;
            }

            int w = 0;
            int h = 0;
            if (0 != B2xGetResolution(outPort, w, h))
            {
                return -1;
            }

            if (0 != B2xNewWnd(outPort, w, h, realNum))
            {
                return -1;
            }


            for (int i = 0; i < realNum; ++i)
            {
                SHL_Json::Value element;
                element = itemlist[i];

                const char* strIp = element["Ip"].asCString();
                int    u16Port = element["Port"].asInt();
                int    u16Chn = element["Chn"].asInt();
				std::string decoder_name;
				std::string decoder_password;

                Pu *pDecPu = putree->findPu(strIp, u16Port);


                if (pDecPu)
                {
					if (pDecPu->userName().empty() || pDecPu->passWord().empty())
					{
						decoder_name = systm->decoder_name();
						decoder_password = systm->decoder_password();
					}
					else
					{
						decoder_name = pDecPu->userName();
						decoder_password = pDecPu->passWord();
					}
					if (0 != B2xStartDecode(outPort, i + 1, strIp, u16Port, u16Chn, decoder_name.c_str(), decoder_password.c_str()))
                    {
                        return -1;
                    }
                    else
                    {
                        LOGD_MSV("B2xStartDecode SUCCESS");
                    }
                }

            }
        }

        return 0;

    }
    else
    {
        ERRE_MSV("not support command type:%s", jMethod.c_str());
        return -1;
    }

    
}

int
HikDecoderPu::DecoderSwitch(uint32_t u32Mode, uint32_t u32Channel, uint32_t u32OutPort, string subHost, int subPort)
{
    return 0;
}

int
HikDecoderPu::DecoderSetDisplayMode(uint32_t u32DisPort, uint32_t u32Mode, string subHost, int subPort)
{
    if (u32DisPort < 0 || u32DisPort >= 32)
    {
        return -1;
    }
    
    uint32_t s32VGAPort = u32DisPort + m_Ability.struBncInfo.byChanNums;
    m_DispChanCfg[s32VGAPort].dwWindowMode = u32Mode;
    m_DispChanCfg[s32VGAPort].dwResolution = _1080P_60HZ;
    m_DispChanCfg[s32VGAPort].byScale      = 0;
    m_DispChanCfg[s32VGAPort].byVedioFormat= 2; // PAL-2,NTSL-1
    m_DispChanCfg[s32VGAPort].byAudio      = 0;
    m_DispChanCfg[s32VGAPort].byAudioWindowIdx = 0;
    
    return 0;
}

/* u32DisPort从VGA开始，0代表VGA1，1代表VGA2 */
int
HikDecoderPu::DecoderBandDecChnToDisWnd(int s32DecChnNo, uint32_t u32DisPort, int s32WndNo, string  subHost, int subPort)
{

    if (u32DisPort < 0 || u32DisPort >= 32)
    {
        return -1;
    }
   

    uint32_t s32VGAPort = u32DisPort + m_Ability.struBncInfo.byChanNums;	
    
    if (m_Ability.byStartChan + s32DecChnNo > m_Ability.byDecChanNums)
    {
        return -1;
    }
    
    m_DispChanCfg[s32VGAPort].byJoinDecChan[s32WndNo] = m_Ability.byStartChan + s32DecChnNo;
    
    int iChanIndex = u32DisPort + m_Ability.struVgaInfo.byStartChan;
    
    if (-1 != m_lUserID && m_login)
    {
    if (!NET_DVR_MatrixSetDisplayCfg_V41(m_lUserID, u32DisPort, &m_DispChanCfg[s32VGAPort]))
        {
            LOGE_MSV("Error code : %d\n", NET_DVR_GetLastError());
            return -1;
        }
        else
        {
            return 0;
        }
    }
    
    return -1;
}
int
HikDecoderPu::DecoderStartDynamic(int s32DecChnNo, string host, int port, int chnNo, string name, string password, string  subHost, int subPort)
{
    memset(&m_StreamCfg, 0, sizeof(m_StreamCfg));
    m_StreamCfg.dwSize = sizeof(NET_DVR_PU_STREAM_CFG);
    m_StreamCfg.struDevChanInfo.byChannel = chnNo;
    m_StreamCfg.struDevChanInfo.wDVRPort = port;
    m_StreamCfg.struDevChanInfo.byTransMode = 0;
    m_StreamCfg.struDevChanInfo.byTransProtocol = 0;
    m_StreamCfg.struDevChanInfo.byDeviceType = 1;
    m_StreamCfg.struDevChanInfo.byFactoryType = 0;
    memcpy(m_StreamCfg.struDevChanInfo.struIP.sIpV4, host.c_str(), MAX_NAME_LEN);
    memcpy(m_StreamCfg.struDevChanInfo.sUserName, name.c_str(), MAX_NAME_LEN);
    memcpy(m_StreamCfg.struDevChanInfo.sPassword, password.c_str(), MAX_NAME_LEN);

    if (!NET_DVR_MatrixStartDynamic_V30(m_lUserID, m_Ability.byStartChan + s32DecChnNo, &m_StreamCfg))
    {
        LOGE_MSV("NET_DVR_MatrixStartDynamic_V30 Error code : %d  decNo:%d\n", NET_DVR_GetLastError(), m_Ability.byStartChan + s32DecChnNo);
        return -1;
    }
    else
    {   
        LOGD_MSV("NET_DVR_MatrixStartDynamic_V30 success  decNo:%d\n", m_Ability.byStartChan + s32DecChnNo);
        return 0;
    }
}

int
HikDecoderPu::DecoderStopDynamic(int s32DisOutPort, string subHost, int subPort)
{
    if ( -1 == m_lUserID ||
        false == m_login ||
        GetDecoderAbility() < 0)
    {
        return -1;
    }
     
    uint32_t s32VGAPort = s32DisOutPort + m_Ability.struBncInfo.byChanNums;	
    
    for(int i = 0;i < MAX_WINDOWS_V41; ++i)
    {
        if (m_DispChanCfg[s32VGAPort].byJoinDecChan[i] != 0)
        {
            NET_DVR_MatrixStopDynamic(m_lUserID, m_DispChanCfg[s32VGAPort].byJoinDecChan[i]);
            m_DispChanCfg[s32VGAPort].byJoinDecChan[i] = 0;
        }
    }
    
    return 0;
}
    
int 
HikDecoderPu::GetDecoderAbility()
{
    if (!NET_DVR_GetDeviceAbility(
        m_lUserID,
        MATRIXDECODER_ABILITY_V41,
        NULL,
        0,
        (char*)&m_Ability,
        sizeof(NET_DVR_MATRIX_ABILITY_V41)))
    {

        LOGE_MSV("NET_DVR_GetDeviceAbility error: %d\n", NET_DVR_GetLastError());
        return -1;
    }

    int i = 0;
    int iDispNums = 0;
    for (i = 0; i < m_Ability.struBncInfo.byChanNums; i++)
    {
        if (!NET_DVR_MatrixGetDisplayCfg_V41(m_lUserID, m_Ability.struBncInfo.byStartChan + i, &m_DispChanCfg[iDispNums + i]))
        {
            LOGE_MSV("\nError: NET_DVR_MatrixGetLoopDecChanInfo[%d] = %d\n", m_Ability.struBncInfo.byStartChan + i, NET_DVR_GetLastError());
        }
    }

    iDispNums += m_Ability.struBncInfo.byChanNums;

    for (i = 0; i < m_Ability.struVgaInfo.byChanNums; i++)
    {
        if (!NET_DVR_MatrixGetDisplayCfg_V41(m_lUserID, i + m_Ability.struVgaInfo.byStartChan, &m_DispChanCfg[iDispNums + i]))
        {
            LOGE_MSV("\nError: NET_DVR_MatrixGetLoopDecChanInfo[%d] = %d\n", i, NET_DVR_GetLastError());
        }
    }

    iDispNums += m_Ability.struVgaInfo.byChanNums;
    for (i = 0; i < m_Ability.struHdmiInfo.byChanNums; i++)
    {
        if (!NET_DVR_MatrixGetDisplayCfg_V41(m_lUserID, m_Ability.struHdmiInfo.byStartChan + i, &m_DispChanCfg[iDispNums + i]))
        {
            LOGE_MSV("\nError: NET_DVR_MatrixGetLoopDecChanInfo[%d] = %d\n", m_Ability.struHdmiInfo.byStartChan + i, NET_DVR_GetLastError());
        }
    }

    iDispNums += m_Ability.struHdmiInfo.byChanNums;
    for (i = 0; i < m_Ability.struDviInfo.byChanNums; i++)
    {
        if (!NET_DVR_MatrixGetDisplayCfg_V41(m_lUserID, m_Ability.struDviInfo.byStartChan + i, &m_DispChanCfg[iDispNums + i]))
        {
            LOGE_MSV("\nError: NET_DVR_MatrixGetLoopDecChanInfo[%d] = %d\n", m_Ability.struDviInfo.byStartChan + i, NET_DVR_GetLastError());
        }
    }

    return 0;
}



int        
HikDecoderPu::openOutWindow(int disPortId, int wndCount)
{
    if ((0 > m_lUserID) || (0 == wndCount))
    {
        ERRE_MSV("not login or window count is zero");
        return -1;
    }

    if ((disPortId > MAX_DISPLAY_NUM) || (disPortId <= 0)) 
    {
        ERRE_MSV("display id or display port invalid");
        return -1;
    }

    if (m_type == Pu::DEV_TYPE_DECODE_6901)
    {
        resetOutWindow(disPortId);


        int iSubNum = ceil(sqrt(wndCount));

        NET_DVR_IN_PARAM struInputPapam = { 0 };
        NET_DVR_OUT_PARAM struOutputPapam = { 0 };

        DWORD dwStatus[512] = { 0 };
        DWORD dwRetWinNum[256] = { 0 };
        NET_DVR_VIDEOWALLWINDOWPOSITION *wallPos = new NET_DVR_VIDEOWALLWINDOWPOSITION[wndCount];
        DWORD ModWinNo[256] = { 0 };

        int s32Width = 1920 / iSubNum;
        int s32Height = 1920 / iSubNum;
        int s32XCoor = 0;
        int s32YCoor = 0;

        /* 计算第N块屏初始位置 */
        DWORD dwRet = 0;
        NET_DVR_DISPLAYCFG struDisplayCfg = { 0 };
        struDisplayCfg.dwSize = sizeof(struDisplayCfg);
        if (!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_VIDEOWALLDISPLAYNO, 0, &struDisplayCfg, sizeof(struDisplayCfg),
            &dwRet))
        {
            delete[] wallPos;
            LOGE_MSV("NET_DVR_GET_VIDEOWALLDISPLAYNO failed, err: %d\n", NET_DVR_GetLastError());
            return -1;
        }

        DWORD dwWindowNo = struDisplayCfg.struDisplayParam[disPortId - 1].dwDisplayNo;

        DWORD dwDispNum = 1;
        DWORD lDispChan[1]; //显示输出号，通过NET_DVR_GET_VIDEOWALLDISPLAYNO 获取
        lDispChan[0] = dwWindowNo;

        DWORD dwStatusWnd[1];
        NET_DVR_VIDEOWALLDISPLAYPOSITION struWallDispPos[1];
        memset(dwStatusWnd, 0, sizeof(dwStatusWnd));
        memset(struWallDispPos, 0, sizeof(struWallDispPos));
        if (!NET_DVR_GetDeviceConfig(m_lUserID, NET_DVR_GET_VIDEOWALLDISPLAYPOSITION, dwDispNum, lDispChan, \
            sizeof(DWORD) * dwDispNum, dwStatusWnd, struWallDispPos, dwDispNum*
            sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)))
        {
            delete[] wallPos;
            LOGE_MSV("NET_DVR_GET_VIDEOWALLDISPLAYPOSITION failed, err: %d\n", NET_DVR_GetLastError());
            return -1;
        }

        if ((0 == struWallDispPos[0].struRectCfg.dwHeight) ||
            (0 == struWallDispPos[0].struRectCfg.dwWidth))
        {
            delete[] wallPos;
            return -1;
        }

        for (int s32I = 0; s32I < wndCount; ++s32I)
        {
            memset(&wallPos[s32I], 0, sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION));
            wallPos[s32I].dwSize = sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
            wallPos[s32I].byEnable = 1;
            wallPos[s32I].dwWindowNo = 1 << 24;
            wallPos[s32I].struRect.dwXCoordinate = struWallDispPos[0].struRectCfg.dwXCoordinate + (s32I % iSubNum)*s32Width;
            wallPos[s32I].struRect.dwYCoordinate = struWallDispPos[0].struRectCfg.dwYCoordinate + (s32I / iSubNum)*s32Height;
            wallPos[s32I].struRect.dwWidth = s32Width;
            wallPos[s32I].struRect.dwHeight = s32Height;
            ModWinNo[s32I] = 1 << 24;
        }

        struInputPapam.struCondBuf.pBuf =  ModWinNo;
        struInputPapam.struCondBuf.nLen = wndCount * sizeof(DWORD);
        struInputPapam.struInParamBuf.pBuf = wallPos;
        struInputPapam.struInParamBuf.nLen = wndCount * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
        struOutputPapam.lpStatusList = dwStatus;
        struOutputPapam.struOutBuf.pBuf = dwRetWinNum;
        struOutputPapam.struOutBuf.nLen = wndCount*sizeof(DWORD);

        if (!NET_DVR_SetDeviceConfigEx(m_lUserID, NET_DVR_SET_VIDEOWALLWINDOWPOSITION, wndCount, &struInputPapam, &struOutputPapam))
        {
            int iErr = NET_DVR_GetLastError();
            delete[] wallPos;
            return -1;
        }

        char *pTemp = new char[4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION)];
        memset(pTemp, 0, 4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION));
        DWORD dwWallNo = 1;
        dwWallNo <<= 24;
        BOOL bRet = TRUE;
        if (!NET_DVR_GetDeviceConfig(m_lUserID, NET_DVR_GET_VIDEOWALLWINDOWPOSITION, 0xffffffff, &dwWallNo, 4, NULL, pTemp, 4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION)))
        {
            int iErr = NET_DVR_GetLastError();
            delete[] pTemp;
            delete[] wallPos;
            return -1;
        }

        DWORD dwWinCount = *((DWORD*)pTemp);
        LPNET_DVR_VIDEOWALLWINDOWPOSITION videoWallPos = (LPNET_DVR_VIDEOWALLWINDOWPOSITION)(pTemp + 4);

        vector<DWORD> vecWndNo;
        for (int s32I = 0; s32I < wndCount; ++s32I)
        {
            for (int s32J = 0; s32J < dwWinCount; ++s32J)
            {
                if ((videoWallPos[s32J].struRect.dwXCoordinate == wallPos[s32I].struRect.dwXCoordinate) &&
                    (videoWallPos[s32J].struRect.dwYCoordinate == wallPos[s32I].struRect.dwYCoordinate) &&
                    (videoWallPos[s32J].struRect.dwWidth == wallPos[s32I].struRect.dwWidth) &&
                    (videoWallPos[s32J].struRect.dwHeight == wallPos[s32I].struRect.dwHeight))
                {
                    dwRetWinNum[s32I] = videoWallPos[s32J].dwWindowNo;
                    break;
                }
            }

            vecWndNo.push_back(dwRetWinNum[s32I]);
        }

        m_decChnMap.clear();
        m_decChnMap[disPortId] = vecWndNo;

        delete[] wallPos;
        delete[] pTemp;

        return 0;

    }
    else if (m_type == Pu::DEV_TYPE_DECODE)
    {
        GetDecoderAbility();

        DecoderStopDynamic(disPortId, "", 0);

        int iSubNum = ceil(sqrt(wndCount));

        DecoderSetDisplayMode(disPortId, iSubNum*iSubNum, "", 0);
        
    }
    else
    {
        return -1;
    }
    return -1;
}

int
HikDecoderPu::decoderVideo(int displayPort, int index, string host, int port, int chnNo, string username, string password)
{
    if (m_type == Pu::DEV_TYPE_DECODE_6901)
    {

        if (0 > m_lUserID)
        {
            return -1;
        }

        vector<DWORD> vecWndNo = m_decChnMap[displayPort];
        if (0 == vecWndNo.size())
        {
            return 0;
        }

        DWORD dwChannel = vecWndNo[index] + (((1) & 0xff) << 16);


        NET_DVR_MATRIX_DECCHAN_CONTROL param;
        memset(&param, 0, sizeof(param));
        param.dwSize = sizeof(param);
        param.byDecChanScaleStatus = 0;
        param.byDecodeDelay = 0;
        param.byEnableSpartan = 1;
        param.byLowLight = 0;
        param.byNoiseReduction = 2;
        param.byDefog = 0;
        param.byEnableVcaDec = 1;
        param.byEnableAudio = 0;
        param.dwAllCtrlType = 0xffff;
        if (NET_DVR_MatrixSetDecChanCfg(m_lUserID, dwChannel, &param))
        {

        }

        NET_DVR_PU_STREAM_CFG_V41 struParam;
        memset(&struParam, 0, sizeof(struParam));
        struParam.dwSize = sizeof(NET_DVR_PU_STREAM_CFG_V41);
        struParam.byStreamMode = 1;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransProtocol = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransMode = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byDeviceType = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byDispChan = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.bySubDispChan = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChanType = 255;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byResolution = 4;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransMode = 0;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransMode = 0;
        strcpy((char*)struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byAddress, host.c_str());
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.wDVRPort = port;
        struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChannel = chnNo;
        strcpy((char*)struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.sUserName, username.c_str());
        strcpy((char*)struParam.uDecStreamMode.struDecStreamDev.struDevChanInfo.sPassword, password.c_str());

        if (!NET_DVR_MatrixStartDynamic_V41(m_lUserID, dwChannel, &struParam))
        {
            int iErr = NET_DVR_GetLastError();
            return -1;
        }


    }
    else if (m_type == Pu::DEV_TYPE_DECODE)
    {



        NET_DVR_PU_STREAM_CFG StreamCfg;
        memset(&StreamCfg, 0, sizeof(NET_DVR_PU_STREAM_CFG));

        StreamCfg.dwSize = sizeof(NET_DVR_PU_STREAM_CFG);
        m_StreamCfg.struDevChanInfo.byChannel = chnNo;
        m_StreamCfg.struDevChanInfo.wDVRPort = port;
        m_StreamCfg.struDevChanInfo.byTransMode = 0;
        m_StreamCfg.struDevChanInfo.byTransProtocol = 0;
        strcpy(m_StreamCfg.struDevChanInfo.struIP.sIpV4, host.c_str());
        strcpy((char*)m_StreamCfg.struDevChanInfo.sUserName, username.c_str());
        strcpy((char*)m_StreamCfg.struDevChanInfo.sPassword, password.c_str());

        if (!NET_DVR_MatrixStartDynamic_V30(m_lUserID, m_Ability.byStartChan + chnNo, &m_StreamCfg))
        {
            LOGE_MSV("NET_DVR_MatrixStartDynamic_V30 Error code : %d\n", NET_DVR_GetLastError());
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return -1;
    }
    return -1;
}

int
HikDecoderPu::resetOutWindow(int portId)
{
    if (0 > m_lUserID)
    {
        return -1;
    }

    if ((portId > MAX_DISPLAY_NUM) || (portId <= 0))
    {
        return -1;
    }

    DWORD dwRet = 0;
    NET_DVR_DISPLAYCFG struDisplayCfg = { 0 };
    struDisplayCfg.dwSize = sizeof(struDisplayCfg);
    if (!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_VIDEOWALLDISPLAYNO, 0, &struDisplayCfg, sizeof(struDisplayCfg),
        &dwRet))
    {
        LOGE_MSV("NET_DVR_GET_VIDEOWALLDISPLAYNO failed, err: %d\n", NET_DVR_GetLastError());
        return -1;
    }

    DWORD dwWindowNo = struDisplayCfg.struDisplayParam[portId - 1].dwDisplayNo;

    DWORD dwDispNum = 1;
    DWORD lDispChan[1];
    lDispChan[0] = dwWindowNo;

    DWORD dwStatusWnd[1];
    NET_DVR_VIDEOWALLDISPLAYPOSITION struWallDispPos[1];
    memset(dwStatusWnd, 0, sizeof(dwStatusWnd));
    memset(struWallDispPos, 0, sizeof(struWallDispPos));
    if (!NET_DVR_GetDeviceConfig(m_lUserID, NET_DVR_GET_VIDEOWALLDISPLAYPOSITION, dwDispNum, lDispChan, \
        sizeof(DWORD) * dwDispNum, dwStatusWnd, struWallDispPos, dwDispNum*
        sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)))
    {
        LOGE_MSV("NET_DVR_GET_VIDEOWALLDISPLAYPOSITION failed, err: %d\n", NET_DVR_GetLastError());
        return -1;
    }

    if ((0 == struWallDispPos[0].struRectCfg.dwHeight) ||
        (0 == struWallDispPos[0].struRectCfg.dwWidth))
    {
        return -1;
    }

    char *pTemp = new char[4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION)];
    memset(pTemp, 0, 4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION));
    DWORD dwWallNo = 1;
    dwWallNo <<= 24;
    BOOL bRet = TRUE;
    if (!NET_DVR_GetDeviceConfig(m_lUserID, NET_DVR_GET_VIDEOWALLWINDOWPOSITION, 0xffffffff, &dwWallNo, 4, NULL, pTemp, 4 + 256 * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION)))
    {
        int iErr = NET_DVR_GetLastError();
        delete[] pTemp;
        return -1;
    }

    NET_DVR_IN_PARAM struInputPapam = { 0 };
    NET_DVR_OUT_PARAM struOutputPapam = { 0 };
    DWORD dwStatus[512] = { 0 };
    DWORD dwRetWinNum[256] = { 0 };
    DWORD ModWinNo[256] = { 0 };

    DWORD dwWinCount = *((DWORD*)pTemp);
    LPNET_DVR_VIDEOWALLWINDOWPOSITION videoWallPos = (LPNET_DVR_VIDEOWALLWINDOWPOSITION)(pTemp + 4);

    int wallWndModCount = 0;
    int s32I = 0;
    for (s32I = 0; s32I < dwWinCount; ++s32I)
    {
        if ((videoWallPos[s32I].struRect.dwXCoordinate >= struWallDispPos[0].struRectCfg.dwXCoordinate) &&
            (videoWallPos[s32I].struRect.dwYCoordinate >= struWallDispPos[0].struRectCfg.dwYCoordinate) &&
            ((videoWallPos[s32I].struRect.dwXCoordinate + videoWallPos[s32I].struRect.dwWidth) <= (struWallDispPos[0].struRectCfg.dwXCoordinate + struWallDispPos[0].struRectCfg.dwWidth)) &&
            ((videoWallPos[s32I].struRect.dwYCoordinate + videoWallPos[s32I].struRect.dwHeight) <= (struWallDispPos[0].struRectCfg.dwYCoordinate + struWallDispPos[0].struRectCfg.dwHeight)))
        {
            wallWndModCount++;
        }
    }

    if (0 == wallWndModCount)
    {
        delete[]pTemp;
        return -1;
    }

    int indexWallPostion = 0;
    LPNET_DVR_VIDEOWALLWINDOWPOSITION videoWallPosition = new NET_DVR_VIDEOWALLWINDOWPOSITION[wallWndModCount];
    for (s32I = 0; s32I < dwWinCount; ++s32I)
    {
        if ((videoWallPos[s32I].struRect.dwXCoordinate >= struWallDispPos[0].struRectCfg.dwXCoordinate) &&
            (videoWallPos[s32I].struRect.dwYCoordinate >= struWallDispPos[0].struRectCfg.dwYCoordinate) &&
            ((videoWallPos[s32I].struRect.dwXCoordinate + videoWallPos[s32I].struRect.dwWidth) <= (struWallDispPos[0].struRectCfg.dwXCoordinate + struWallDispPos[0].struRectCfg.dwWidth)) &&
            ((videoWallPos[s32I].struRect.dwYCoordinate + videoWallPos[s32I].struRect.dwHeight) <= (struWallDispPos[0].struRectCfg.dwYCoordinate + struWallDispPos[0].struRectCfg.dwHeight)))
        {
            videoWallPosition[indexWallPostion] = videoWallPos[s32I];
            videoWallPosition[indexWallPostion].byEnable = 0;
            ModWinNo[indexWallPostion] = videoWallPos[s32I].dwWindowNo;
            indexWallPostion++;
        }
    }

    struInputPapam.struCondBuf.pBuf = ModWinNo;
    struInputPapam.struCondBuf.nLen = wallWndModCount * sizeof(DWORD);
    struInputPapam.struInParamBuf.pBuf = videoWallPosition;
    struInputPapam.struInParamBuf.nLen = wallWndModCount * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
    struOutputPapam.lpStatusList = dwStatus;
    struOutputPapam.struOutBuf.pBuf = dwRetWinNum;
    struOutputPapam.struOutBuf.nLen = dwWinCount*sizeof(DWORD);

    if (!NET_DVR_SetDeviceConfigEx(m_lUserID, NET_DVR_SET_VIDEOWALLWINDOWPOSITION, wallWndModCount, &struInputPapam, &struOutputPapam))
    {
        delete[] videoWallPosition;
        delete[] pTemp;
        return -1;
    }

    delete[] videoWallPosition;
    delete[] pTemp;
    return 0;
}

int
HikDecoderPu::B2xColseAllWin(DWORD wallNo)
{
    wallNo <<= 24;
    char szLan[128] = { 0 };
    if (!NET_DVR_RemoteControl(m_lUserID, NET_DVR_VIDEOWALLWINDOW_CLOSEALL, &wallNo, sizeof(wallNo)))
    {
        LOGE_MSV("NET_DVR_VIDEOWALLWINDOW_CLOSEALL, Error Code %d", szLan, NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        LOGD_MSV("NET_DVR_VIDEOWALLWINDOW_CLOSEALL succ, wallNo:%d", wallNo);
        return 0;
    }
}

int
HikDecoderPu::B2xGetResolution(DWORD wallNo, int &w, int &h)
{
    char *pTemp = new char[4 + 256 * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)];
    memset(pTemp, 0, 4 + 256 * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION));

    wallNo <<= 24;

    if (!NET_DVR_GetDeviceConfig(
        m_lUserID, 
        NET_DVR_GET_VIDEOWALLDISPLAYPOSITION, 
        0xffffffff, 
        &wallNo, 
        sizeof(DWORD), 
        NULL, 
        pTemp, 
        4 + 256 * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)))
    {
        LOGE_MSV("NET_DVR_GET_VIDEOWALLDISPLAYPOSITION faild, code:%d", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        w = 0;
        h = 0;

        int dwDispNum = *((DWORD*)pTemp);

        NET_DVR_VIDEOWALLDISPLAYPOSITION* vdlist = (NET_DVR_VIDEOWALLDISPLAYPOSITION*)(pTemp + 4);

        for (int i = 0; i< dwDispNum; i++)
        {
            if (vdlist[i].byEnable == 0)
            {
                continue;
            }

            if (vdlist[i].struRectCfg.dwXCoordinate > w)
            {
                w = vdlist[i].struRectCfg.dwXCoordinate;
            }

            if (vdlist[i].struRectCfg.dwYCoordinate > h)
            {
                h = vdlist[i].struRectCfg.dwYCoordinate;
            }

        }
        
        w += 1920;
        h += 1920;
        LOGD_MSV("wall w=%d, h=%d",w , h);
        return 0;
    }

}

int
HikDecoderPu::B2xNewWnd(DWORD wallNo, int w, int h, int wndcount)
{
    DWORD RetWinNum[256] = { 0 };   //返回的窗口号
    DWORD WinNumEx[256];            //1字节墙号 + 1字节保留 + 2字节窗口
    DWORD dwStatus[256] = { 0 };

    for (int i = 0; i < wndcount; ++i)
    {
        //窗口号从0开始
        WinNumEx[i] = (0 & 0xffff) + (wallNo << 24);
    }


    int iSubNum = ceil(sqrt(wndcount));
    int x = 0, y = 0;
    int sw = w / iSubNum;
    int sh = h / iSubNum;
    NET_DVR_VIDEOWALLWINDOWPOSITION struModifyList[256] = {0};
    for (int i = 0; i < wndcount; ++i)
    {

        x = (i % iSubNum)*sw;
        y = (i / iSubNum)*sh;       

        struModifyList[i].byEnable = 1;
        struModifyList[i].byWndOperateMode = 0;
        struModifyList[i].dwLayerIndex = 0xffff;
        struModifyList[i].dwSize = sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
        struModifyList[i].dwWindowNo = WinNumEx[i];
        struModifyList[i].struRect.dwHeight = sh;
        struModifyList[i].struRect.dwWidth = sw;
        struModifyList[i].struRect.dwXCoordinate = x;
        struModifyList[i].struRect.dwYCoordinate = y;

        printf("winNo:%d x:%d y:%d w:%d h:%d\n", WinNumEx[i], x, y, sw, sh);
    }

    NET_DVR_IN_PARAM struInputPapam = { 0 };
    NET_DVR_OUT_PARAM struOutputPapam = { 0 };
    struInputPapam.struCondBuf.pBuf = WinNumEx;
    struInputPapam.struCondBuf.nLen = wndcount * sizeof(DWORD);
    struInputPapam.struInParamBuf.pBuf = struModifyList;
    struInputPapam.struInParamBuf.nLen = wndcount * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
    struOutputPapam.lpStatusList = dwStatus;
    struOutputPapam.struOutBuf.pBuf = RetWinNum;
    struOutputPapam.struOutBuf.nLen = wndcount * sizeof(DWORD);
    

    if (!NET_DVR_SetDeviceConfigEx(
        m_lUserID, 
        NET_DVR_SET_VIDEOWALLWINDOWPOSITION, 
        wndcount, 
        &struInputPapam, 
        &struOutputPapam))
    {

        LOGE_MSV("NET_DVR_SET_VIDEOWALLWINDOWPOSITION faild,Code:%d", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        LOGD_MSV("open window success");
        return 0;
    }
}

int
HikDecoderPu::B2xStartDecode(DWORD wallNo, DWORD wndNo, const char* ip, int port, int chnNo, const char* username,const char* password)
{
    LOGD_MSV("wallNo:%d wndNo:%d ip:%s chnNo:%d", wallNo, wndNo, ip, chnNo);

    NET_DVR_PU_STREAM_CFG_V41 dt = { 0 };

    dt.byStreamMode = 1;
    dt.dwSize = sizeof(NET_DVR_PU_STREAM_CFG_V41);    
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChanType = 0;
    sprintf((char *)dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byStreamId, "%s", "");
    sprintf((char *)dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byAddress, "%s", ip);
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.wDVRPort = port;
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChannel = 0;
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransProtocol = 0;
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransMode = 0;
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.dwChannel = chnNo;
    dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.byFactoryType = 0;
    sprintf((char *)dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.sPassword, "%s", password);
    sprintf((char *)dt.uDecStreamMode.struDecStreamDev.struDevChanInfo.sUserName, "%s", username);

    //16777219

    DWORD dwDecChanNum = wndNo + ((0 & 0xff) << 16) + ((wallNo & 0xff) << 24);   

    if (!NET_DVR_MatrixStartDynamic_V41(
        m_lUserID, 
        dwDecChanNum, 
        &dt))
    {
        LOGE_MSV("chnNo:%d NET_DVR_MatrixStartDynamic = %d\n", dwDecChanNum, NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        LOGD_MSV("NET_DVR_MatrixStartDynamic_V41 success, decChnID:%d", dwDecChanNum);
    }
    return 0;
}
