/**
 * @file      sumgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-28 15:10:50
 *
 * @note
 *  sumgr.h defines
 */
#ifndef __SUMGR_H__
#define __SUMGR_H__
#include "tlist.h"

class Su;
class OnUiUpdate;
class SuMgr : public tlist<Su *>
{
public:
    SuMgr();
    virtual ~SuMgr();

    void               addSu(Su *su);
    void               delSu(Su *su);

    void               setUi(OnUiUpdate *ui);
    void               setUi(const string &addrcode, OnUiUpdate *ui);

    void               updateUi();
private:
    OnUiUpdate       * m_ui;
};

extern SuMgr *dualtalkSuMgr;
#endif
