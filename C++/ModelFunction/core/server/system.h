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
#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "observer.h"
#include "tlist.h"

#define SV_VERSION "v1.00.00.05"

class TriggerListener;
class Mutex;
class OnUiUpdate;
class System : public tlist<TriggerListener *>, public Observer
{
public:
    System();
    virtual ~System();

public:
    int                init();
    std::string        addrcode();
    std::string        name();
    std::string        dbver();
    std::string        platePath();
    std::string        facePath();
    int                pport();
    int                vport();
    int                iport();
    int                wport();
    int                recycle();
    int                logHoldTime();
	int                bShowPresetName();
	int                X();
	int                Y();
	std::string		   decoder_name();
	std::string        decoder_password();


    void               setUi(OnUiUpdate *ui);
    void               updateUi();

    void               addTriggerListener(TriggerListener *listener);
    void               delTriggerListener(TriggerListener *listener);
    void               notifyAlarm(string &json, void *data, uint32_t size);

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    std::string        m_addrcode;
    std::string        m_name;
    std::string        m_swver;
    std::string        m_hwver;
    std::string        m_dbver;
    std::string        m_comment;
    std::string        m_platePath;
    std::string        m_facePath;
    int                m_pport;
    int                m_vport;
    int                m_iport;
    int                m_wport;
    int                m_recycle;
    int                m_logHoldTime;
	int                m_bShowPresetName;
	int                m_x;
	int                m_y;
	std::string        m_decoder_name;
	std::string        m_decoder_password;


    OnUiUpdate       * m_ui;
    Mutex            * m_uimtx;
    uint64_t           m_lastTime;
#ifdef UNIX
    int                m_pid;
#else
    int                m_pid;
#endif
};
extern System * systm;
#endif
