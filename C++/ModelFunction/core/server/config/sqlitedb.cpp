#include "sqlitedb.h"
#include "cfg.h"
#include "tables.h"
#include "tls_error.h"
#include "stream.h"
#include "sys.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif


#define S_RETRY_COUNT_ 10
#define S_RETRY_SLESSP 10

static
SqliteDB  cfg;
SqliteDB* sqliteDb = &cfg;


SqliteDB::SqliteDB()
{
    m_db = new sql::Database();
    open(CONFIG_DB);
}

SqliteDB::~SqliteDB()
{
    if (m_db)
    {
        delete m_db;
        m_db = NULL;
    }
}

int
SqliteDB::open(const string &db)
{
    return m_db->open(db) ? 0 : -1;
}

void
SqliteDB::close()
{
}

int
SqliteDB::insert(const string &table, const string &json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        ERRE_MSV("Invalidate json args.");

        return -1;
    }

    int i;
    for (i = 0; i < S_RETRY_COUNT_; ++i)
    {
        try
        {
			const sql::Field* definition;
			if (0 == table.find("tbl_subplat_cam_"))
			{
				definition = tbl_sub_platform_camera_info;
			}
			else
			{
				definition = tables[table];
			}
			sql::Table tbl(m_db->getHandle(), table, definition);

            if (!tbl.exists())
            {
                tbl.create();
            }

            sql::Record record(tbl.fields());

            SHL_Json::Value::Members mem = rvalue.getMemberNames();
            /* for (SHL_Json::Value::Members::iterator  iter = mem.begin(); iter != mem.end(); iter++) */
            for (auto iter = mem.begin(); iter != mem.end(); iter++)
            {
                if (rvalue[*iter].type() == SHL_Json::stringValue)
                {
                    record.setString(*iter, rvalue[*iter].asString());
                }
                else if (rvalue[*iter].type() == SHL_Json::uintValue)
                {
                    if (rvalue[*iter].isUInt64())
                        record.setInteger(*iter, rvalue[*iter].asUInt64());
                    else
                        record.setInteger(*iter, rvalue[*iter].asUInt());
                }
                else if (rvalue[*iter].type() == SHL_Json::intValue)
                {
                    if (rvalue[*iter].isInt64())
                        record.setInteger(*iter, rvalue[*iter].asInt64());
                    else
                        record.setInteger(*iter, rvalue[*iter].asInt());
                }
                else if (rvalue[*iter].type() == SHL_Json::booleanValue)
                {
                    record.setBool(*iter, rvalue[*iter].asBool());
                }
                else if (rvalue[*iter].type() == SHL_Json::realValue)
                {
                    record.setDouble(*iter, rvalue[*iter].asDouble());
                }
                else
                {
                    record.setString(*iter, rvalue[*iter].asString());
                }
            }

            tbl.replaceRecord(&record);
        }
        catch (sql::Exception e)
        {
            ERRE_MSV("table:%s, json: %s, %s", table.c_str(),
                     json.c_str(), e.msg().c_str());
            sys->sleep(S_RETRY_SLESSP);
            continue;
        }
        break;

    }
    if (i >= S_RETRY_COUNT_)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int
SqliteDB::update(const string &table, const string &rec)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(rec, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        ERRE_MSV("Invalidate json args.");

        return -1;
    }

    int i;

    for (i = 0; i < S_RETRY_COUNT_; ++i)
    {
        try
        {
            const sql::Field* definition;
            if (0 == table.find("tbl_subplat_cam_"))
            {
                definition = tbl_sub_platform_camera_info;
            }
            else
            {
                definition = tables[table];
            }
            sql::Table tbl(m_db->getHandle(), table, definition);

            if (!tbl.exists())
            {
                tbl.create();
            }

            sql::Record record(tbl.fields());

            SHL_Json::Value::Members mem = rvalue.getMemberNames();
            /* for (SHL_Json::Value::Members::iterator  iter = mem.begin(); iter != mem.end(); iter++) */
            for (auto iter = mem.begin(); iter != mem.end(); iter++)
            {
                if (rvalue[*iter].type() == SHL_Json::stringValue)
                {
                    record.setString(*iter, rvalue[*iter].asString());
                }
                else if (rvalue[*iter].type() == SHL_Json::uintValue)
                {
                    if (rvalue[*iter].isUInt64())
                        record.setInteger(*iter, rvalue[*iter].asUInt64());
                    else
                        record.setInteger(*iter, rvalue[*iter].asUInt());
                }
                else if (rvalue[*iter].type() == SHL_Json::intValue)
                {
                    if (rvalue[*iter].isInt64())
                        record.setInteger(*iter, rvalue[*iter].asInt64());
                    else
                        record.setInteger(*iter, rvalue[*iter].asInt());
                }
                else if (rvalue[*iter].type() == SHL_Json::booleanValue)
                {
                    record.setBool(*iter, rvalue[*iter].asBool());
                }
                else if (rvalue[*iter].type() == SHL_Json::realValue)
                {
                    record.setDouble(*iter, rvalue[*iter].asDouble());
                }
                else
                {
                    record.setString(*iter, rvalue[*iter].asString());
                }
            }

            /* tbl.replaceRecord(&record); */
            tbl.updateRecord(&record);
        }
        catch (sql::Exception e)
        {
            ERRE_MSV("%s", e.msg().c_str());
            sys->sleep(S_RETRY_SLESSP);
            continue;
        }

        break;
    }

    if (i >= S_RETRY_COUNT_)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int
SqliteDB::deletex(const string &table, const string &condition)
{
    //if ("" == condition)
    //{
    //    ERRE_MSV("Table %s is not exist", table.c_str());
    //    return -1;
    //}
    bool ret = false;
    int i;
    for (i = 0; i < S_RETRY_COUNT_; ++i)
    {
        try
        {
			const sql::Field* definition;
			if (0 == table.find("tbl_subplat_cam_"))
			{
				definition = tbl_sub_platform_camera_info;
			}
			else
			{
				definition = tables[table];
			}
			sql::Table tbl(m_db->getHandle(), table, definition);
            if (!tbl.exists())
            {
                ERRE_MSV("Table %s is not exist", table.c_str());
                return -1;
            }

            ret = tbl.deleteRecords(condition);
        }
        catch (sql::Exception e)
        {
            ERRE_MSV("%s", e.msg().c_str());
            sys->sleep(S_RETRY_SLESSP);
            continue;
        }
        break;
    }

    if (i >= S_RETRY_COUNT_)
    {
        return -1;
    }
    else
    {
        return ret ? 0 : -1;
    }
}

int
SqliteDB::query(const string &table, string &json, const string &condition)
{
    try
    {
		const sql::Field* definition;
		if (0 == table.find("tbl_subplat_cam_"))
		{
			definition = tbl_sub_platform_camera_info;
		}
		else
		{
			definition = tables[table];
		}
		sql::Table tbl(m_db->getHandle(), table, definition);

        if (!tbl.exists())
        {
            ERRE_MSV("Table %s is not exist",   table.c_str());

            return -1;
        }

        if ("" != condition)
        {
            tbl.open(condition);
        }
        else
        {
            tbl.open();
        }

        if (tbl.recordCount() <= 0)
        {
            return 0;
        }

        SHL_Json::FastWriter writer;
        SHL_Json::Value      wvalue;

        for (int i = 0; i < tbl.recordCount(); i++)
        {
            if (sql::Record* record = tbl.getRecord(i))
            {
                SHL_Json::Value item;
                for (int j = 0; j < record->columnCount(); ++j)
                {
                    string key = record->fields()->getByIndex(j)->getName();
                    if (sql::type_text == record->fields()->getByIndex(j)->getType())
                    {
                        item[key] = record->getValue(key)->asString();
                    }
                    else if (sql::type_int == record->fields()->getByIndex(j)->getType())
                    {
                        item[key] = (int64_t)record->getValue(key)->asInteger();
                    }
                    else if (sql::type_float == record->fields()->getByIndex(j)->getType())
                    {
                        item[key] = record->getValue(key)->asDouble();
                    }
                    else if (sql::type_bool == record->fields()->getByIndex(j)->getType())
                    {
                        item[key] = record->getValue(key)->asBool();
                    }
                    else
                    {
                        item[key] = record->getValue(key)->asString();
                    }
                }

                wvalue.append(item);

                sql::log(record->toString());
            }
        }

        json = writer.write(wvalue);
    }
    catch (sql::Exception e)
    {
        ERRE_MSV("%s",   e.msg().c_str());

        return -1;
    }

    return 0;
}

int        
SqliteDB::count(const string &table, const string &condition)
{
    try
    {
        sql::Table tbl(m_db->getHandle(), table, tables[table]);

        if (!tbl.exists())
        {
            ERRE_MSV("Table %s is not exist",   table.c_str());

            return -1;
        }

        if ("" != condition)
        {
            tbl.open(condition);
        }
        else
        {
            tbl.open();
        }

        return tbl.recordCount();
    }
    catch (sql::Exception e)
    {
        ERRE_MSV("%s",   e.msg().c_str());

        return -1;
    }
    return 0;
}


int        
SqliteDB::compress()
{
    sqlite3 *db = m_db->getHandle();
    char      s8Sql[128] = { 0 };
    snprintf(s8Sql, 128, "vacuum;");
    char * pErrMsg = NULL;

    int s32Ret = 0;
    int s32Count = 0;
    do
    {
        s32Ret = sqlite3_exec(db, s8Sql, NULL, NULL, &pErrMsg);
        if (s32Ret == SQLITE_BUSY)
        {
            ERRE_MSV("%s\n", pErrMsg);
            sys->sleep(1000);
            if (++s32Count > 100)
            {
                return -1;
            }
            continue;
        }
        break;
    } while (1);
    
    return 0;
}

int
SqliteDB::resume(const string &dbname)
{
    bool ret = false;
    sql::Database *s_db = new sql::Database();
    if (!s_db->open(dbname))
    {
        ERRE_MSV("source_db->open(%s) failed\n", dbname.c_str());
        delete s_db;
        return -1;
    }

    map<string, sql::Field *>::const_iterator i;

    for (i = tables.cbegin(); i != tables.cend(); ++i)
    {
        if (i->first == "view_user_group" || 
            i->first == "tbl_device_log" ||
            i->first == "tbl_alarm_log" ||
            i->first == "tbl_user_log" ||
            i->first == "tbl_system_log")
            continue;

        sql::Table stbl(s_db->getHandle(), i->first, i->second);
        sql::Table dtbl(m_db->getHandle(), i->first, i->second);

        if (!stbl.exists())
        {
            continue;
        }

        try
        {
            ret = dtbl.backup(stbl);
            if (!ret)
            {
                s_db->close();
                delete s_db;
                return -1;
            }
                
        }
        catch (sql::Exception e)
        {
            ERRE_MSV("backup %s", e.msg().c_str());
            s_db->close();
            delete s_db;
            return -1;
        }
    }

    s_db->close();
    delete s_db;

    return 0;
}

int
SqliteDB::dbUpdate()
{
    //backup old database
    string backupName = "tmp_c2msv.db";
    FileStream fsrc;
    fsrc.openReadOnly("c2msv.db");
    if (!fsrc.isopen())
    {
        return -1;
    }
    if (0 != fsrc.copyTo(backupName.c_str()))
    {
        fsrc.close();
        return -1;
    }
    fsrc.close();


    //upgrade current database
    bool ret = false;
    sql::Database *s_db = new sql::Database();
    if (!s_db->open(backupName))
    {
        ERRE_MSV("source_db->open(%s) failed\n", backupName.c_str());
        delete s_db;
        return -1;
    }
    
    //sub platform update
    do 
    {
        sql::Table tbl(s_db->getHandle(), "tbl_sub_platform", table_sub_platform);
        if (!tbl.exists())
        {
            break;
        }

        tbl.open();
        if (tbl.recordCount() <= 0)
        {
            break;
        }

        for (int i = 0; i < tbl.recordCount(); i++)
        {
            if (sql::Record* record = tbl.getRecord(i))
            {
                for (int j = 0; j < record->columnCount(); ++j)
                {
                    string key = record->fields()->getByIndex(j)->getName();
                    if ("addrcode" == key)
                    {
                        string sub_addrcode = record->getValue(key)->asString();

                        tables.insert(pair<string, sql::Field *>(sub_addrcode, table_sub_platform));
                        break;
                    }
                }
            }
        }
    } while (0);

    map<string, sql::Field *>::const_iterator i;
    for (i = tables.cbegin(); i != tables.cend(); ++i)
    {
        if (i->first == "view_user_group")
            continue;

        sql::Table stbl(s_db->getHandle(), i->first, i->second);
        sql::Table dtbl(m_db->getHandle(), i->first, i->second);

        if (dtbl.exists())
        {
            dtbl.remove();
        }
        else
        {
            dtbl.create();
        }

        if (!stbl.exists())
        {
            continue;
        }

        if (i->first == "tbl_system_log" || 
            i->first == "tbl_user_log" || 
            i->first == "tbl_device_log" ||
            i->first == "tbl_alarm_log")
        {
            //日志数据过多，升级会消耗大量时间
            continue;
        }

        if (i->first == "tbl_sup_platform")
        {
            //该表删除了部分字段，无法拷贝表数据，只能拷贝表结构
            dtbl.create();
            continue;
        }
            

        try
        {
            ret = dtbl.backup(stbl);
            if (!ret)
            {
                s_db->close();
                delete s_db;
                return -1;
            }
                
        }
        catch (sql::Exception e)
        {
            ERRE_MSV("backup %s", e.msg().c_str());
            s_db->close();
            delete s_db;
            return -1;
        }
    }

    s_db->close();
    delete s_db;
    return 0;
}

int   
SqliteDB::createTable(const string &table,const string  &definition)//create table
{
	try
	{
		//sql::Table tbl(m_db->getHandle(), table,tables[definition]);
		if (definition == "tbl_sub_platform_camera_info")
		{
			sql::Table tbl(m_db->getHandle(), table, tbl_sub_platform_camera_info);
			if (!tbl.exists())
			{
				tbl.create();
			}
		}
	}
	catch (sql::Exception e)
	{
		ERRE_MSV("%s", e.msg().c_str());
		return -1;
	}
	return 0;
}

int
SqliteDB::deleteTable(const string &table, const string  &definition)
{
	try
	{
		if (definition == "tbl_sub_platform_camera_info")
		{
			sql::Table tbl(m_db->getHandle(), table, tbl_sub_platform_camera_info);
			if (tbl.exists())
			{
				tbl.remove();
			}
		}
	}
	catch (sql::Exception e)
	{
		ERRE_MSV("%s", e.msg().c_str());
		return -1;
	}
	return 0;
}