/**
 * @file      imatrix.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-04 10:38:22
 *
 * @note
 *  imatrix.h defines
 */
#ifndef __IMAXTRIX_H__
#define __IMAXTRIX_H__
#include "def.h"

class iMatrix
{
protected:
    virtual int        switchPort(string addrcode, int inport, int outport) { return 0; };
};
#endif
