/**
 * @file      idecoder.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-04 10:38:22
 *
 * @note
 *  idecoder.h defines
 */
#ifndef __IDECODER_H__
#define __IDECODER_H__
#include "def.h"

class iDecoder
{
protected:
    virtual int    bind() { return 0; };
    virtual int    dynamic() { return 0; };
};
#endif
