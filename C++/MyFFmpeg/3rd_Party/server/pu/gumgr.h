/**
 * @file      gumgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-28 15:10:50
 *
 * @note
 *  gumgr.h defines
 */
#ifndef __GUMGR_H__
#define __GUMGR_H__
#include "tlist.h"

class Gu;
class OnUiUpdate;
class GuMgr : public tlist<Gu *>
{
public:
    GuMgr();
    virtual ~GuMgr();

    void               addGu(Gu *gu);
    void               delGu(Gu *gu);

    void               setUi(OnUiUpdate *ui);
    void               setUi(const string &addrcode, OnUiUpdate *ui);

    void               updateUi();
    void               bandWidth(uint32_t &u32Bandwidth, uint32_t &u32StreamNum);
private:
    OnUiUpdate       * m_ui;
};

extern GuMgr *realGuMgr;
extern GuMgr *recordGuMgr;
#endif
