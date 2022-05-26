#include "view_user_group.h"
#include "db.h"

static
UserGroupView  view(View::DB_EASYSQLITE);
UserGroupView* usrgrView = &view;

UserGroupView::UserGroupView(int type)
    : View(type)
{

}

UserGroupView::~UserGroupView()
{

}

int
UserGroupView::query(string& json, string condition)
{
    return m_db->query("view_user_group", json, condition);
}



