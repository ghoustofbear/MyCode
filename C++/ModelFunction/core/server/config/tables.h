/**
 * @file      tables.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-30 08:43:10
 *
 * @note
 *  config database easySqlite tables define
 */
#ifndef __TABLES_H__
#define __TABLES_H__
#include "def.h"

#include "easySQLite/SqlCommon.h"
#include "easySQLite/SqlField.h"
#include "easySQLite/SqlDatabase.h"
#include "easySQLite/SqlTable.h"

sql::Field table_system[] =
{
	sql::Field("id", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("addrcode", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("name", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("swVersion", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("hwVersion", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("dbVersion", sql::type_text, sql::flag_not_null | sql::flag_unique),
	sql::Field("recycle", sql::type_int, sql::flag_not_null | sql::flag_unique),
	sql::Field("logHoldTime", sql::type_int, sql::flag_not_null),
	sql::Field("platePath", sql::type_text),
	sql::Field("facePath", sql::type_text),
	sql::Field("comment", sql::type_text),
	sql::Field("isShowPresetName", sql::type_int),
	sql::Field("X",sql::type_int),
	sql::Field("Y", sql::type_int),
	sql::Field("decoder_name", sql::type_text),
	sql::Field("decoder_password", sql::type_text),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_server[] =
{
    sql::Field("type",       sql::type_int,  sql::flag_not_null | sql::flag_primary_key),
    sql::Field("enable",     sql::type_bool, sql::flag_not_null),
    sql::Field("name",       sql::type_text, sql::flag_not_null | sql::flag_unique),
    sql::Field("bindIp",     sql::type_text),
    sql::Field("listenPort", sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("comment",    sql::type_text),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_user[] =
{
    sql::Field("id",       sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("addrcode", sql::type_text, sql::flag_not_null | sql::flag_unique),
    sql::Field("name",     sql::type_text, sql::flag_not_null | sql::flag_unique),
    sql::Field("pswd",     sql::type_text, sql::flag_not_null),
    sql::Field("realName", sql::type_text),
    sql::Field("identity", sql::type_text),
    sql::Field("phone",    sql::type_text),
    sql::Field("email",    sql::type_text),
    sql::Field("relogin",  sql::type_bool, sql::flag_not_null),
    sql::Field("aaa",      sql::type_bool, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_group[] =
{
    sql::Field("id",          sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("name",        sql::type_text, sql::flag_not_null | sql::flag_unique),
    sql::Field("type",        sql::type_int,  sql::flag_not_null),
    sql::Field("rightBit",    sql::type_int,  sql::flag_not_null),
    sql::Field("previewBit",  sql::type_int),
    sql::Field("playbackBit", sql::type_int),
    sql::Field("ctrlBit",     sql::type_int),
    sql::Field("level",       sql::type_int,  sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_user_group[] =
{
    sql::Field("userId",  sql::type_text, sql::flag_not_null),
    sql::Field("groupId", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field view_user_group[] =
{
    sql::Field("userId",    sql::type_text, sql::flag_not_null),
    sql::Field("groupId",   sql::type_text, sql::flag_not_null),
    sql::Field("userName",  sql::type_text, sql::flag_not_null),
    sql::Field("groupName", sql::type_text, sql::flag_not_null),
    sql::Field("groupType", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_platform[] =
{
    sql::Field("enable",    sql::type_bool, sql::flag_not_null | sql::flag_unique),
    sql::Field("protocol",  sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("sipPort",   sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("rtspPort",  sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("httpPort",  sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("rtpPort",   sql::type_int,  sql::flag_not_null | sql::flag_unique),
    sql::Field("aaaServIp", sql::type_text, sql::flag_not_null | sql::flag_unique),
    sql::Field("sendIp", sql::type_text),
	sql::Field("comment", sql::type_text),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_station[] =
{
    sql::Field("id", sql::type_int, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("station_name",  sql::type_text, sql::flag_not_null ),
    sql::Field("province",      sql::type_text, sql::flag_not_null),
    sql::Field("city",          sql::type_text, sql::flag_not_null),
    sql::Field("station_type",  sql::type_int,  sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_device[] =
{
    sql::Field("addrcode",   sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("name",       sql::type_text, sql::flag_not_null),
    sql::Field("parentCode", sql::type_text, sql::flag_not_null),
    sql::Field("type",       sql::type_int,  sql::flag_not_null),
    sql::Field("protocol",   sql::type_int,  sql::flag_not_null),
    sql::Field("level",      sql::type_int,  sql::flag_not_null),
    sql::Field("chnNum",     sql::type_int,  sql::flag_not_null),
    sql::Field("chnNo",      sql::type_int,  sql::flag_not_null),
    sql::Field("host",       sql::type_text),
    sql::Field("port",       sql::type_int),
    sql::Field("username",   sql::type_text),
    sql::Field("password",   sql::type_text),
    sql::Field("domain",     sql::type_int),
    sql::Field("comment",    sql::type_text),
    sql::Field("recvCall",   sql::type_int),
    sql::Field("reTrackCode",sql::type_text),
    sql::Field("linktype",   sql::type_int),
    sql::Field("startNo",    sql::type_int),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_sub_platform[] =
{
    sql::Field("addrcode",   sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("name",       sql::type_text, sql::flag_not_null),
    sql::Field("parentCode", sql::type_text, sql::flag_not_null),
    sql::Field("type",       sql::type_int,  sql::flag_not_null),
    sql::Field("protocol",   sql::type_int,  sql::flag_not_null),
    sql::Field("host",       sql::type_text, sql::flag_not_null),
    sql::Field("sipPort",    sql::type_int,  sql::flag_not_null),
    sql::Field("rtspPort",   sql::type_int,  sql::flag_not_null),
    sql::Field("httpPort",   sql::type_int,  sql::flag_not_null),
    sql::Field("rtpPort",    sql::type_int,  sql::flag_not_null),
    sql::Field("comment",    sql::type_text),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_sup_platform[] =
{
    sql::Field("addrcode",   sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("name",       sql::type_text, sql::flag_not_null),
    sql::Field("protocol",   sql::type_int,  sql::flag_not_null),
    sql::Field("username",   sql::type_text, sql::flag_not_null),
    sql::Field("password",   sql::type_text, sql::flag_not_null),
    sql::Field("host",       sql::type_text, sql::flag_not_null),
    sql::Field("sipPort",    sql::type_int,  sql::flag_not_null),
    sql::Field("rtspPort",   sql::type_int,  sql::flag_not_null),
    sql::Field("httpPort",   sql::type_int,  sql::flag_not_null),
    sql::Field("expires",    sql::type_int,  sql::flag_not_null),
    sql::Field("ifupIp",     sql::type_text, sql::flag_not_null),
    sql::Field("isregister", sql::type_int),
    sql::Field("ivauselocal", sql::type_int),
    sql::Field(sql::DEFINITION_END),
};


sql::Field table_video_dev_code[] =
{
    sql::Field("code", sql::type_int,  sql::flag_not_null),
    sql::Field("name", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_non_video_dev_code[] =
{
    sql::Field("code", sql::type_int,  sql::flag_not_null),
    sql::Field("name", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_camera_code[] =
{
    sql::Field("code", sql::type_int,  sql::flag_not_null),
    sql::Field("name", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_protocol_type[] =
{
    sql::Field("type",  sql::type_int,  sql::flag_not_null),
    sql::Field("value", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_system_log[] =
{
    sql::Field("time",   sql::type_text, sql::flag_not_null),
    sql::Field("type",   sql::type_int,  sql::flag_not_null),
    sql::Field("info",   sql::type_text, sql::flag_not_null),
    sql::Field("userId", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_user_log[] =
{
    sql::Field("time",   sql::type_text, sql::flag_not_null),
    sql::Field("info",   sql::type_text, sql::flag_not_null),
    sql::Field("userId", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_device_log[] =
{
    sql::Field("name",     sql::type_text, sql::flag_not_null),
    sql::Field("time",     sql::type_text, sql::flag_not_null),
    sql::Field("addrcode", sql::type_text, sql::flag_not_null),
    sql::Field("status",   sql::type_int,  sql::flag_not_null),
    sql::Field("info",     sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_alarm_log[] =
{
    sql::Field("name", sql::type_text, sql::flag_not_null),
    sql::Field("channelNo", sql::type_int),
    sql::Field("time",     sql::type_text, sql::flag_not_null),
    sql::Field("addrcode", sql::type_text, sql::flag_not_null),
    sql::Field("type",     sql::type_int,  sql::flag_not_null),
    sql::Field("level",    sql::type_int,  sql::flag_not_null),
    sql::Field("status",   sql::type_int,  sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_time[] =
{
    sql::Field("beTime",       sql::type_int, sql::flag_not_null),
    sql::Field("doTime",       sql::type_int, sql::flag_not_null),
    sql::Field("allowTimeIp",  sql::type_text, sql::flag_not_null),
    sql::Field("timeInterval", sql::type_int, sql::flag_not_null),
    sql::Field("ntpId",        sql::type_int, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_ntp[] =
{
    sql::Field("id",   sql::type_int, sql::flag_not_null),
    sql::Field("host", sql::type_text, sql::flag_not_null),
    sql::Field("port", sql::type_int, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_preset[] =
{
    sql::Field("addrcode", sql::type_text, sql::flag_not_null),
    sql::Field("name",     sql::type_text, sql::flag_not_null),
    sql::Field("idx",      sql::type_int, sql::flag_not_null),
    sql::Field("type",     sql::type_int, sql::flag_not_null),
    sql::Field("isIVS",    sql::type_int, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_track_dev[] =
{
    sql::Field("trackCode", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("devCode",   sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_camerareliva[] =
{
	sql::Field("camera_addrcode", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
	sql::Field("iva_addrcode", sql::type_text, sql::flag_not_null),
	sql::Field(sql::DEFINITION_END),
};

sql::Field table_camera_ivaability[] =
{
    sql::Field("addrcode", sql::type_text, sql::flag_not_null),
    sql::Field("ability", sql::type_int, sql::flag_not_null),
    sql::Field("subability", sql::type_int, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_taskresources[] =
{
    sql::Field("taskid", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("taskname", sql::type_text, sql::flag_not_null),
    sql::Field("addrcode", sql::type_text, sql::flag_not_null),
    sql::Field("preset", sql::type_int, sql::flag_not_null),
    sql::Field("taskprj", sql::type_int, sql::flag_not_null),
    sql::Field("analysetype", sql::type_int, sql::flag_not_null),
    sql::Field("analysesubtype", sql::type_int, sql::flag_not_null),
    sql::Field("analyseexe", sql::type_int, sql::flag_not_null),
    sql::Field("areanum", sql::type_int, sql::flag_not_null),
    sql::Field("param", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_cam_ivaability_term[] =
{
    sql::Field("id", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("name", sql::type_text, sql::flag_not_null),
    sql::Field("type", sql::type_text, sql::flag_not_null),
    sql::Field("parent_id", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_gate[] =
{
    sql::Field("camaddr", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("gateaddr", sql::type_int, sql::flag_not_null),
    sql::Field("lanename", sql::type_text, sql::flag_not_null),
    sql::Field("terminal_code", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_camera_plate[] =
{
    sql::Field("plate_camaddr", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
    sql::Field("nvr_camaddr", sql::type_text, sql::flag_not_null),
    sql::Field(sql::DEFINITION_END),
};

sql::Field table_iva_lmt[] =
{
	sql::Field("iva_addr", sql::type_text, sql::flag_not_null | sql::flag_primary_key),
	sql::Field("lmt_ip", sql::type_text, sql::flag_not_null),
	sql::Field("lmt_port", sql::type_int, sql::flag_not_null),
	sql::Field("lmt_user", sql::type_text, sql::flag_not_null),
	sql::Field("lmt_passwd", sql::type_text, sql::flag_not_null),
	sql::Field(sql::DEFINITION_END),
};

sql::Field tbl_camera_belong_platform[] =
{
	sql::Field("platform_addrcode", sql::type_text, sql::flag_not_null),
	sql::Field("camera_addrcode", sql::type_text, sql::flag_not_null ),
	sql::Field("remark", sql::type_text),
	sql::Field(sql::DEFINITION_END),
};

sql::Field tbl_catalog_info[] =
{
	sql::Field("id",       sql::type_int, sql::flag_not_null | sql::flag_primary_key),
	sql::Field("parentId", sql::type_int, sql::flag_not_null),
	sql::Field("addrcode", sql::type_text, sql::flag_not_null),
	sql::Field("name",     sql::type_text),
	sql::Field("comment",  sql::type_text),
	sql::Field(sql::DEFINITION_END),
};

sql::Field tbl_sub_platform_camera_info[] =
{
	sql::Field("sub_platform_addrcode", sql::type_text),
	sql::Field("camera_addrcode", sql::type_text, sql::flag_primary_key),
	sql::Field("camera_name", sql::type_text),
	sql::Field("remark", sql::type_text),
	sql::Field(sql::DEFINITION_END),
};

sql::Field tbl_camera_related[] =
{
	sql::Field("camera_addrcode", sql::type_text, sql::flag_primary_key),
	sql::Field("camera_related", sql::type_text),
	sql::Field(sql::DEFINITION_END),
};

map<string, sql::Field *> tables;

struct _tables
{
    _tables()
    {
        /* tables */
        table_user_group[0].setReferences("REFERENCES tbl_user   (id) On Delete CASCADE On Update CASCADE");
        table_user_group[1].setReferences("REFERENCES tbl_group  (id) On Delete CASCADE On Update CASCADE");
        table_track_dev[0].setReferences("REFERENCES tbl_device  (addrcode) On Delete CASCADE On Update CASCADE");

        tables.insert(pair<string, sql::Field *>("tbl_system",             table_system));
        tables.insert(pair<string, sql::Field *>("tbl_server",             table_server));
        tables.insert(pair<string, sql::Field *>("tbl_user",               table_user));
        tables.insert(pair<string, sql::Field *>("tbl_group",              table_group));
        tables.insert(pair<string, sql::Field *>("tbl_user_group",         table_user_group));
        tables.insert(pair<string, sql::Field *>("tbl_platform",           table_platform));
        tables.insert(pair<string, sql::Field *>("tbl_device",             table_device));
        tables.insert(pair<string, sql::Field *>("tbl_sub_platform",       table_sub_platform));
        tables.insert(pair<string, sql::Field *>("tbl_sup_platform",       table_sup_platform));
        tables.insert(pair<string, sql::Field *>("tbl_video_dev_code",     table_video_dev_code));
        tables.insert(pair<string, sql::Field *>("tbl_non_video_dev_code", table_non_video_dev_code));
        tables.insert(pair<string, sql::Field *>("tbl_camera_code",        table_camera_code));
        tables.insert(pair<string, sql::Field *>("tbl_protocol_type",      table_protocol_type));
        tables.insert(pair<string, sql::Field *>("tbl_system_log",         table_system_log));
        tables.insert(pair<string, sql::Field *>("tbl_user_log",           table_user_log));
        tables.insert(pair<string, sql::Field *>("tbl_device_log",         table_device_log));
        tables.insert(pair<string, sql::Field *>("tbl_alarm_log",          table_alarm_log));
        tables.insert(pair<string, sql::Field *>("tbl_time",               table_time));
        tables.insert(pair<string, sql::Field *>("tbl_ntp",                table_ntp));
        tables.insert(pair<string, sql::Field *>("tbl_preset",             table_preset));
		tables.insert(pair<string, sql::Field *>("tbl_track_dev",		   table_track_dev));
		tables.insert(pair<string, sql::Field *>("tbl_camerareliva",	   table_camerareliva));
        tables.insert(pair<string, sql::Field *>("tbl_camera_ivaability",  table_camera_ivaability));
        tables.insert(pair<string, sql::Field *>("tbl_taskresources",      table_taskresources));
        tables.insert(pair<string, sql::Field *>("tbl_cam_ivaability_term", table_cam_ivaability_term));
        tables.insert(pair<string, sql::Field *>("tbl_station",             table_station));
        tables.insert(pair<string, sql::Field *>("tbl_gate",                table_gate));
		tables.insert(pair<string, sql::Field *>("tbl_camera_plate",		table_camera_plate));
		tables.insert(pair<string, sql::Field *>("tbl_iva_lmt",				table_iva_lmt));
		tables.insert(pair<string, sql::Field *>("tbl_camera_belong_platform", tbl_camera_belong_platform));
		tables.insert(pair<string, sql::Field *>("tbl_catalog_info", tbl_catalog_info));
		tables.insert(pair<string, sql::Field *>("tbl_camera_related", tbl_camera_related));
		//tables.insert(pair<string, sql::Field *>("tbl_sub_platform_camera_info", tbl_sub_platform_camera_info));
        /* views */
        tables.insert(pair<string, sql::Field *>("view_user_group", view_user_group));
    };
    
} _tables;

#endif
