#ifndef __TEVENT_QUEUE_H__
#define __TEVENT_QUEUE_H__

#include "def.h"
#include "concourrency.h"

template <class T1, class T2>
class CM_API TEvent
{
public:
	TEvent()
	{
		m_bHasRsp = FALSE;
	};
	/* ��ʼ�� */
	VOID  Init(const T1 req, T2 &rsp);
	
	/* �ȴ���Ϣ��Ӧ */
	BOOL  WaitRsp(U32 milliseconds);
	
	/* ֪ͨ��Ӧ��Ϣ */
	BOOL  TryRsp(T1 req, T2 &rsp);
	
	/* ȡ���ȴ���Ϣ��Ӧ */
	VOID  CancelWaitRsp();
	
public:
	Event            m_Event;     /**< ͬ���¼� */
	BOOL volatile    m_bHasRsp;   /**< �Ƿ��Ѿ���Ӧ */
	T1               m_stReq;
	T2             * m_pstRsp;	
};

template <class T1, class T2>
VOID TEvent<T1, T2>::Init(const T1 req, T2 &rsp)
{   
    m_stReq  = req;
    m_pstRsp = &rsp;
}

template <class T1, class T2>
BOOL TEvent<T1, T2>::WaitRsp(U32 milliseconds)
{
    m_Event.Wait(milliseconds);
    
    return m_bHasRsp;
}

template <class T1, class T2>
BOOL TEvent<T1, T2>::TryRsp(T1 req, T2 &rsp)
{
    if (m_stReq == req)
    {
	   *m_pstRsp  = rsp;
        m_bHasRsp = TRUE;
        m_Event.Notify();
		
        return TRUE;
    }
	
    return FALSE;
}

template <class T1, class T2>
VOID TEvent<T1, T2>::CancelWaitRsp()
{	
}

template <class T1, class T2>
class CM_API TEventQueue
{
public:
    /* ������������봫����� */
    VOID  Add(TEvent<T1, T2> &call);
	
    /* ��������ƥ���Ӧ��Ϣ */
    VOID  Rsp(T1 req, T2 &rsp);
	
    /* ��պ���������� */
    VOID  Clear(TEvent<T1, T2> &call);
    VOID  Clear();
	
protected:
    Mutex m_Mutex;                                  /**< ����������б����� */
    std::vector<TEvent<T1, T2> *> m_vcCallQueue;    /**< ����������� */
};

template <class T1, class T2>
VOID TEventQueue<T1, T2>::Add(TEvent<T1, T2> &call)
{
    AutoLock lock(m_Mutex);
    m_vcCallQueue.push_back(&call);
}

template <class T1, class T2>
VOID TEventQueue<T1, T2>::Rsp(T1 req, T2 &rsp)
{
    AutoLock lock(m_Mutex);
    for (size_t i = 0; i < m_vcCallQueue.size(); ++i)
    {
        if (m_vcCallQueue[i]->TryRsp(req, rsp))
            break;
    }
}

template <class T1, class T2>
VOID TEventQueue<T1, T2>::Clear(TEvent<T1, T2> &call)
{
    AutoLock lock(m_Mutex);
#ifdef _WIN32
    vector<TEvent<T1, T2> *>::iterator i = find(m_vcCallQueue.begin(), m_vcCallQueue.end(), &call);
#else
    typename vector<TEvent<T1, T2> *>::iterator i = find(m_vcCallQueue.begin(), m_vcCallQueue.end(), &call);
#endif
    if (i != m_vcCallQueue.end())
    {
        m_vcCallQueue.erase(i);
        call.CancelWaitRsp();
    }
}

template <class T1, class T2>
VOID TEventQueue<T1, T2>::Clear()
{
    AutoLock lock(m_Mutex);
    for (size_t i = 0; i < m_vcCallQueue.size(); ++i)
        m_vcCallQueue[i]->CancelWaitRsp();
	
    m_vcCallQueue.clear();
}

#endif
