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
	/* 初始化 */
	VOID  Init(const T1 req, T2 &rsp);
	
	/* 等待消息回应 */
	BOOL  WaitRsp(U32 milliseconds);
	
	/* 通知回应消息 */
	BOOL  TryRsp(T1 req, T2 &rsp);
	
	/* 取消等待消息回应 */
	VOID  CancelWaitRsp();
	
public:
	Event            m_Event;     /**< 同步事件 */
	BOOL volatile    m_bHasRsp;   /**< 是否已经回应 */
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
    /* 将呼叫请求加入传输队列 */
    VOID  Add(TEvent<T1, T2> &call);
	
    /* 遍历队列匹配回应消息 */
    VOID  Rsp(T1 req, T2 &rsp);
	
    /* 清空呼叫请求队列 */
    VOID  Clear(TEvent<T1, T2> &call);
    VOID  Clear();
	
protected:
    Mutex m_Mutex;                                  /**< 呼叫请求队列保护锁 */
    std::vector<TEvent<T1, T2> *> m_vcCallQueue;    /**< 呼叫请求队列 */
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
