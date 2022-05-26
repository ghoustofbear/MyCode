/**
 * @file      groupmgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 09:56:46
 *
 * @note
 *  GroupMgr observer GroupCfg
 */
#ifndef __GROUPMGR_H__
#define __GROUPMGR_H__
#include "observer.h"
#include "tmap.h"

class Group;
class GroupMgr : public tmap<string, Group *>, public Observer
{
public:
    GroupMgr();
    virtual ~GroupMgr();

    int                init();

    Group            * group(string id);

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);
};

extern GroupMgr *groupMgr;
#endif
