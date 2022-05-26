/**
 * @file      gwb_pu.h
 * @copyright Copyright (c) 2018, UT Technology Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-13 15:58:04
 *
 * @note
 *  gwb_pu.h defines
 */
#ifndef __GWB_PU_H__
#define __GWB_PU_H__
#include "gw_pu.h"

typedef struct
{
    string     strAddrCode;          /**< 设备地址编码  */
    string     strDevName;           /**< 设备名称  */
    int32_t    s32Index;             /**< 序号, 从1开始 */
    int32_t    s32Status;            /**< 设备状态  */
    int32_t    s32DecoderTag;        /**< 解码插件标签 */
    string     strLongitude;         /**< 经度 */
    string     strLatitude;          /**< 纬度 */
}DEV_CHN_INFO_S;


typedef struct
{
    string    strFileUrl;            /**< 文件地址，如"rtsp:// www.videoServer.com/"*/
    string    strFilename;           /**< 视频文件名*/
    int32_t   s32FileSize;           /**< 录象文件大小*/
    int32_t   s32Index;              /**< 序号 */

    string     strBegin;             /**< 开始时间*/
    string     strEnd;	    	     /**< 结束时间*/

    int32_t    s32DecoderTag;        /**< 解码标签，参见RTP Payload	*/
    uint32_t   u32RecType;           /**< 录象类型, 参考国网录像类型定义 */
}RECORD_INFO_S;

typedef struct
{
    string strNodeName;              /**< 节点名称 */
    string strAddrCode;              /**< 告警设备地址编码 */
    string strBegin;                 /**< 告警开始时间*/

    int32_t    s32Index;             /**< 序号 */
    int32_t    s32AlarmStatus;       /**< 告警状态 */
    uint32_t   u32AlarmType;         /**< 告警类型 */
}ALARM_LOG_S;


class GwBPu : public GwPu
{
public:

	friend class GwRecordGu;

    GwBPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
    virtual ~GwBPu();

public:
    virtual int        start();

    virtual void       onTimer();

protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();

public:
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

private:
    /* data */
    tlist<DEV_CHN_INFO_S>     m_devList;
};
#endif
