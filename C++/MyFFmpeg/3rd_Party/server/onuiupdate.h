/**
 * @file      onuiupdate.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-28 15:17:56
 *   
 * @note
 *  onuiupdate.h defines 
 */

#ifndef __ONUIUPDATE_H__
#define __ONUIUPDATE_H__
#include <string>
#include <stdint.h>

class OnUiUpdate
{
public:
    virtual void       onUpdate(const std::string &json, void *data, uint32_t size) = 0;
};

#endif
