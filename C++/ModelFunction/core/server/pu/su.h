/**
 * @file      su.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-19 16:28:25
 *
 * @note
 *  su.h defines
 */

#ifndef __SU_H__
#define __SU_H__
#include "tlist.h"
#include "streamlistener.h"
#include "gsession.h"

class Pu;
class OnUiUpdate;

/**
 * Su
 */
class Su : public StreamSpeaker
{
public:
    Su(Pu *pu, int channelNo);
    Su(Pu *pu, string addrcode);
    virtual ~Su();

    string             addrcode();
    string             name();
    string             puname();
    string             puip();
    double             bps();
    int                status();


    void               setUi(OnUiUpdate *ui);
    void               updateUi();

    virtual int        start() = 0;
    virtual void       stop()  = 0;
    virtual int        reset() = 0;

    virtual void       onTimer();

    bool               isSame(string addrcode);
    bool               isSame(int chnNo);

    void               traffic(uint32_t size);
    void               setgessionlistener(GSession *listener) { m_listener = listener; }

protected:
    Pu               * m_pu;
    int                m_status;

    string             m_addrcode;                         /**< as id */
    string             m_name;

    int                m_channelNo;


    uint64_t           m_tmcnt;                            /**< timer driver count */
    uint64_t           m_lastpts;

    uint64_t           m_bpspts;
    //uint64_t           m_sesspts;
    GSession         * m_listener;

private:
    uint64_t           m_transSize;
    OnUiUpdate       * m_ui;
};

#endif
