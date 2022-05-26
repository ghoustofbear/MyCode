/**
 * @file      group.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-30 17:30:50
 *
 * @note
 *  group.h defines
 */
#ifndef __GROUP_H__
#define __GROUP_H__
#include "def.h"

class Group
{
public:
    Group();
    Group(string id, int type, string name, uint64_t rightBit, uint64_t previewBit, uint64_t playbackBit, uint64_t ctrlBit, int level);
    virtual ~Group();
    
    string             id();
    int                type();
    uint64_t           right();
    int                optlevel() { return m_level; }

    void               modify(string name, uint64_t rightBit, uint64_t previewBit, uint64_t playbackBit, uint64_t ctrlBit, int level);
private:
    string             m_id;
    int                m_type;
    string             m_name;
    uint64_t           m_rightBit;
    uint64_t           m_previewBit;
    uint64_t           m_playbackBit;
    uint64_t           m_ctrlBit;
    int                m_level;
};

#endif
