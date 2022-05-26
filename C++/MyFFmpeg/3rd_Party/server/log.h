/**
 * @file      System.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:04:09
 *
 * @note
 *  System.h defines
 */
#ifndef __LOGMAN_H__
#define __LOGMAN_H__
#include "observer.h"
#include "thread.h"
#include "tqueue.h"
#include "triggerlistener.h"

class User;
class OnUiUpdate;
class AuthListener;

class LogMan : public TriggerListener
{
public:
    struct LogInfo
    {
        std::string tbl;
        std::string json;
    };

    LogMan();
    virtual ~LogMan();

    int                start();
    int                stop();
    void               setUi(OnUiUpdate *ui);
    int                writeLog(const std::string& table, const std::string json);
    void               onTimer();

protected: 
    virtual void       onAlarm(string &json, void *data, uint32_t size);
    virtual void       onAIYX(string &json, void *data, uint32_t size);
    virtual void       onAIYC(string &json, void *data, uint32_t size);

private:
    static void *      threadProc(ThreadInfo *);
    void*              writePorc();
    int                onWrite(LogInfo*);
    

private:
    Thread              m_thread;
    tqueue<LogInfo*>    m_wque;
    OnUiUpdate         *m_ui;

    int                 m_logHoldDay;
};

extern LogMan* logOb;
#endif
