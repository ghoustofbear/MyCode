/**
 * @file      view_user_group.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-13 10:23:36
 *   
 * @note
 *  view_user_group.h defines 
 */
#ifndef __VIEW_USER_GROUP_H__
#define __VIEW_USER_GROUP_H__

#include "view.h"

class UserGroupView: public View
{
public:
    UserGroupView(int type);
    virtual ~UserGroupView();

    virtual int        query(string& json, string condition = "");
};

MS_API extern UserGroupView* usrgrView;
#endif
