/**
* @file      matrix.h
* @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2018-01-04 10:38:22
*
* @note
*  matrix.h defines
*/
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "pu.h"

class Matrix : public Pu 
{
public:
    /* 流媒体服务器基本配置 */
    typedef struct
    {
        char         s8Url[128];                     /**< 流媒体服务器IP */
        uint16_t     u16Port;                        /**< 流媒体服务器端口 */
        char       * ps8Username;                    /**< 登录用户名 */
        char       * ps8Password;                    /**< 登录用密码 */
        uint32_t     u32TransProtocol;               /**< 传输协议类型 0-TCP，1-UDP*/
    }DEC_STREAM_CFG_S;

    /* DVR设备通道信息 */
    typedef struct
    {
        char       * ps8Ip;                          /**< DVR IP地址 */
        uint16_t     u16Port;                        /**< DVR 端口号 */
        char       * ps8Username;                    /**< DVR 登录用户名 */
        char       * ps8Password;                    /**< DVR 登录用密码 */
        uint32_t     u32ChnNo;                       /**< DVR 通道号 */
        uint32_t     u32TransProtocol;               /**< 传输协议类型0-TCP，1-UDP */
        uint32_t     u32TransMode;                   /**< 传输码流模式 0－主码流 1－子码流 */
        uint32_t     u32ProtoType;                   /**< 前端设协议类型 */
    }DEC_DVR_CFG_S;

    typedef struct
    {
        bool         bValidStream;                   /**< 流媒体服务器是否可用 */
        union
        {
            DEC_DVR_CFG_S    stDvrCfg;
            DEC_STREAM_CFG_S stStreamCfg;
        };
    } DEC_CHN_PARAM_S;

public:
    Matrix(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~Matrix();


    virtual int        PortSwitch(uint32_t u32InPort, uint32_t u32OutPort, string subHost, int subPort) = 0;
    virtual int        DecoderSwitch(uint32_t u32Mode, uint32_t u32Channel, uint32_t u32OutPort, string subHost, int subPort) = 0;
    virtual int        DecoderSetDisplayMode(uint32_t u32DisPort, uint32_t u32Mode, string subHost, int subPort) = 0;
    virtual int        DecoderBandDecChnToDisWnd(int s32DecChnNo, uint32_t u32DisPort, int s32WndNo, string  subHost, int subPort) = 0;
    virtual int        DecoderStartDynamic(int s32DecChnNo, string host, int port, int chnNo, string name, string pass, string  subHost, int subPort) = 0;
    virtual int        DecoderStopDynamic(int s32DisOutPort, string subHost, int subPort) = 0;


    virtual int        openOutWindow(int disPortId, int wndCount) = 0;
    virtual int        decoderVideo(int displayPort, int index, string host, int port, int chnNo, string username, string password) = 0;
};

#endif
