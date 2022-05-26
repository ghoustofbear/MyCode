#ifndef  _CDT_WFLINK_PU_H_
#define  _CDT_WFLINK_PU_H_

#include "pu.h"
#include "thread.h"
#include "socket.h"
#include <string>

#define  LINK_TYPE_NORECORD (0)
#define  LINK_TYPE_RECORD   (1)

typedef struct {
    uint32_t    u32Guid;
    std::string strName;
    datetime_s  taskTime;
}TASK_INFO;

class CdtWfLinkPu : public Pu
{
public:

    enum 
    {
        T_RCVEVENT,
    };

    enum
    {
        WFLINK_TASK_START = 1,                                   /* 5������Ʊ����ʼ */
        WFLINK_TASK_OPERING,                                     /* 5������Ʊ������� */
        WFLINK_TASK_END, 	                                     /* 5������Ʊ������� */
        WFLINK_SIMU_OPERING,                                     /* 5��ģ����� */
        WFLINK_SIMU_SCOUT,  		                             /* 5��ģ����Ѳ�� */
        CDT_CLOSEVIDEO,                                          /* �ر���Ƶ   */
        CDT_ONLINKVIDEO,                                         /* ��������Ƶ */
        CDT_OFFLINKVIDEO,                                        /* ��������Ƶ */
    };

    CdtWfLinkPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);

    ~CdtWfLinkPu();

protected:

    virtual int        start();
    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo){ return true; }
    virtual bool       validate(string gucode) { return true; }

    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

    static void *      threadProc(ThreadInfo *info);

    void               recvEvent();
private:
    Socket           * m_sock;
    Thread           * m_trdRcvEvent;
    uint8_t            m_s8RecvBuff[1000];
    char               m_s8SendBuff[64];

    list<TASK_INFO>    m_TaskList;//����������������ID��
    uint16_t           m_u16NowTaskId;
    std::string        m_strNowTaskName;
    datetime_s         m_NowTaskTime;

    void			   ProcessOperBefore(unsigned char* pFrame, uint32_t DataLength, uint32_t iType = LINK_TYPE_NORECORD);//�������ǰ�������ʾ����Ʊ���豸���汨��
    void			   ProcessOpering(unsigned char* pFrame, uint32_t DataLength, uint32_t iType = LINK_TYPE_NORECORD);//��������У������豸������һͨ����������л���
    void			   ProcessOperEnd(unsigned char* pFrame, uint32_t DataLength, uint32_t iType = LINK_TYPE_NORECORD);//���������󣬶�������н���
    void			   ProcessTaskName(unsigned char* pFrame, uint32_t DataLength);//������������
    void			   TimeSynchronize(unsigned char* pFrame, uint32_t DataLength);//���Уʱ
    uint16_t           SaveNewTask(string strTaskName);
    uint64_t           m_lastLiveTime;
};

#endif
