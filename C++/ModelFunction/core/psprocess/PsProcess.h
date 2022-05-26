#ifndef  _PSPROCESS_H_
#define  _PSPROCESS_H_

#include "PsProcessApi.h"
#include "thread.h"
#include "tqueue.h"

class PsProcess
{
public:

	typedef struct dataInfo
	{
		uint8_t*data;
		int size;
		int type;
		uint64_t pts;
		uint64_t frameTime;
	};

	PsProcess();

	virtual ~PsProcess();

	virtual void setstreamcb(pfnstreamdatacallback fnstreamcb, void * puser);

	virtual void inputstreamdata(uint8_t*data, int size, int type, uint64_t pts, uint64_t frameTime);
	virtual void inputstreamdata_ex(uint8_t*data, int size, int type, uint64_t pts, uint64_t frameTime);

	virtual void process(dataInfo *info) = 0;

	virtual int start();

	virtual void stop();

	virtual int getNextNalu(uint8_t* data, int data_Len, uint8_t*buff, int &size, int &type);

	int               flag = 0;          //0ΪPS��װ��1ΪPS���װ
	int               m_doubleField   = 0; //0Ϊ������1Ϊ˫��
	bool              m_beFirstPacket = true;
	//int               m_NaluNum       = 0;;
	bool              m_ptsflag       = false;//Ĭ��ʹ���ڲ�ʱ���
protected:
	pfnstreamdatacallback m_fnstreamcb;
	void * m_puser;

	static void     * threadEntry(ThreadInfo *arg);

    tqueue<dataInfo*> m_dataQue;

	Thread            m_thread;

	int8_t            m_spsbuf[512];
	int8_t            m_ppsbuf[512];
	int               m_spslen;
	int               m_ppslen;
	uint8_t* workspace = NULL;

    bool              m_block;  //shl ���������
};

#endif