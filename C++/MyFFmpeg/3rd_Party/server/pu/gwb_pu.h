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
    string     strAddrCode;          /**< �豸��ַ����  */
    string     strDevName;           /**< �豸����  */
    int32_t    s32Index;             /**< ���, ��1��ʼ */
    int32_t    s32Status;            /**< �豸״̬  */
    int32_t    s32DecoderTag;        /**< ��������ǩ */
    string     strLongitude;         /**< ���� */
    string     strLatitude;          /**< γ�� */
}DEV_CHN_INFO_S;


typedef struct
{
    string    strFileUrl;            /**< �ļ���ַ����"rtsp:// www.videoServer.com/"*/
    string    strFilename;           /**< ��Ƶ�ļ���*/
    int32_t   s32FileSize;           /**< ¼���ļ���С*/
    int32_t   s32Index;              /**< ��� */

    string     strBegin;             /**< ��ʼʱ��*/
    string     strEnd;	    	     /**< ����ʱ��*/

    int32_t    s32DecoderTag;        /**< �����ǩ���μ�RTP Payload	*/
    uint32_t   u32RecType;           /**< ¼������, �ο�����¼�����Ͷ��� */
}RECORD_INFO_S;

typedef struct
{
    string strNodeName;              /**< �ڵ����� */
    string strAddrCode;              /**< �澯�豸��ַ���� */
    string strBegin;                 /**< �澯��ʼʱ��*/

    int32_t    s32Index;             /**< ��� */
    int32_t    s32AlarmStatus;       /**< �澯״̬ */
    uint32_t   u32AlarmType;         /**< �澯���� */
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
