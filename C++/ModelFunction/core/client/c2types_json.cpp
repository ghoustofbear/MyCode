#include "c2types_json.h"
#include "c2sdk.h"
#include "sys.h"
#include "tls_error.h"
#include "md5.h"
#include "stream.h"
#include "base64_ex.h"

/**
 * C2Login
 */
C2Login::C2Login(const c2_login_s &login)
{
    memcpy(this, &login, sizeof(c2_login_s));
}

C2Login::~C2Login()
{

}

string
C2Login::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Login::validate()
{
    if (!this->servIp)
    {
        ERRE_SDK("invalidate servIp parameter.");
        return false;
    }

    if (!this->username || string(this->username) == "")
    {
        ERRE_SDK("invalidate username parameter.");
        return false;
    }

    return true;
}

long
C2Login::fromStr(const char *str)
{
    SHL_Json::Reader reader;

    if (!reader.parse(str, m_value, false))
    {
        return -1;
    }

    return 0;
}

long
C2Login::fromJson()
{
    return -1;
}

void
C2Login::fill()
{
    m_value["version"]  = JSON_DATA_VERSION;
    m_value["username"] = this->username;
    

    bool disableMd5 = false;

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;
        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            if (strcmp(extParam[i].key, "disableMD5") == 0 && strcmp(extParam[i].value, "yes") == 0)
            {
                disableMd5 = true;
                continue;
            }

            m_value[extParam[i].key] = extParam[i].value;
        }
    }

    if (!disableMd5)
    {
        char md5[33] = { 0 };
        MD5((uint8_t *)this->password, (uint8_t *)md5, sizeof(md5));
        m_value["password"] = md5;
    }
    else
    {
        m_value["password"] = this->password;
    }
}

/**
 * C2Preview
 */
C2Preview::C2Preview(const c2_preview_s &view)
{
    memcpy(this, &view, sizeof(c2_preview_s));
    m_json.clear();
}

C2Preview::~C2Preview()
{

}

string
C2Preview::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Preview::validate()
{
    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if ((this->obtainType < 0) || (this->obtainType > 3))
    {
        ERRE_SDK("invalidate obtainType parameter.");
        return false;
    }

    if ((0 == this->obtainType || 1 == this->obtainType) && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }
    else if ((2 == this->obtainType || 3 == this->obtainType) && !this->addrcode)
    {
        ERRE_SDK("invalidate addrcode parameter.");
        return false;
    }

    return true;
}

void
C2Preview::fill()
{
    m_value["version"]     = JSON_DATA_VERSION;
    m_value["channelNo"]   = this->channelNo;
    m_value["obtainType"]  = this->obtainType;

    if (0 == this->obtainType || 1 == this->obtainType)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        m_value["addrcode"] = this->addrcode;
    }

    m_value["streamType"]  = this->streamType;
    m_value["transport"]   = this->transport;

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2ExParam
 */
C2ExParam::C2ExParam()
{
}

C2ExParam::~C2ExParam()
{
}

string
C2ExParam::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool C2ExParam::validate()
{

    for (list<c2_ex_param_s>::const_iterator i = begin(); i != end(); ++i)
    {
        if (!i->key || !i->value)
        {
            return false;
        }

        m_value[i->key] = i->value;
    }

    return true;
}

void
C2ExParam::fill()
{
    m_value["version"]  = JSON_DATA_VERSION;

    for (list<c2_ex_param_s>::iterator i = begin(); i != end(); ++i)
    {
        m_value[i->key] = i->value;
    }
}

/**
 * C2QueryDevInfo
 */
C2QueryDevInfo::C2QueryDevInfo(const c2_query_devinfo_s &query)
{
    memcpy(this, &query, sizeof(c2_query_devinfo_s));
    m_json.clear();
}

C2QueryDevInfo::~C2QueryDevInfo()
{

}

string
C2QueryDevInfo::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2QueryDevInfo::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate parameter.");
        return false;
    }

    return true;
}

void
C2QueryDevInfo::fill()
{
    m_value["version"]    = JSON_DATA_VERSION;
    m_value["type"]       = this->type;
    m_value["addrcode"]   = this->addrcode;
    m_value["deviceHost"] = this->deviceHost;
    m_value["devicePort"] = this->devicePort;

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2QueryChaInfo
 */
C2QueryChaInfo::C2QueryChaInfo(const c2_query_channel_s &query)
{
    memcpy(this, &query, sizeof(c2_query_channel_s));
    m_json.clear();
}

C2QueryChaInfo::~C2QueryChaInfo()
{

}

bool C2QueryChaInfo::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate parameter.");
        return false;
    }

    return true;
}

string C2QueryChaInfo::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

void C2QueryChaInfo::fill()
{
    m_value["version"]    = JSON_DATA_VERSION;

    if (this->deviceHost)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;
        m_value["channelNo"] = this->channelNo;
    }
    else
    {
        m_value["addrcode"]   = this->addrcode;
    }

    if (this->subServHost)
    {
        m_value["subServHost"] = this->subServHost;
        m_value["subServPort"] = this->subServPort;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2Ptz
 */
C2Ptz::C2Ptz(int cmd, const c2_ptz_s &ptz)
{
    memcpy(this, &ptz, sizeof(c2_ptz_s));
    m_cmd = cmd;
    m_json.clear();
}

C2Ptz::~C2Ptz()
{

}

string
C2Ptz::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Ptz::validate()
{
    if (m_cmd <= PTZ_CMD_NONE || m_cmd >= PTZ_CMD_END)
    {
        ERRE_SDK("invalidate commond parameter.");
        return false;
    }

    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if ((this->speed < 0) || (this->speed > 10))
    {
        ERRE_SDK("invalidate speed parameter, must 0~10");
        return false;
    }

    if ((this->preset < 0) || (this->preset > 128))
    {
        ERRE_SDK("invalidate preset parameter, must 0~128");
        return false;
    }

    if (!this->deviceHost && !this->addrcode)
    {
        ERRE_SDK("invalidate host or addrcode parameter.");
        return false;
    }

    return true;
}

void
C2Ptz::fill()
{
    m_value["version"]   = JSON_DATA_VERSION;
    m_value["cmd"]       = m_cmd;
    m_value["channelNo"] = this->channelNo;
    m_value["speed"]     = this->speed;
    m_value["preset"]    = this->preset;
    m_value["stop"]      = this->stop;

    if (this->addrcode && string(this->addrcode) != "")
    {
        m_value["addrcode"] = this->addrcode;
    }
    else
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2QueryPreset
 */
C2QueryPreset::C2QueryPreset(const c2_query_preset_s &query)
{
    memcpy(this, &query, sizeof(c2_query_preset_s));
    m_json.clear();
}

C2QueryPreset::~C2QueryPreset()
{
}

string
C2QueryPreset::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2QueryPreset::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost or addrcode parameter.");
        return false;
    }

    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    return true;
}

void
C2QueryPreset::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["channelNo"] = this->channelNo;
    
    if (this->addrcode && string(this->addrcode) != "")
    {
        m_value["addrcode"] = this->addrcode;
    }
    else
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
* C2SetPreset
*/
C2SetPreset::C2SetPreset(const c2_set_preset_s &query)
{
    memcpy(this, &query, sizeof(c2_set_preset_s));
    m_json.clear();
}

C2SetPreset::~C2SetPreset()
{
}

string
C2SetPreset::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}


bool
C2SetPreset::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost or addrcode parameter.");
        return false;
    }

    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if (this->presetNum != 0 && this->presetList == NULL)
    {
        ERRE_SDK("invalidate preset list.");
        return false;
    }

    return true;
}

void
C2SetPreset::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["channelNo"] = this->channelNo;

    if (this->addrcode && string(this->addrcode) != "")
    {
        m_value["addrcode"] = this->addrcode;
    }
    else
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }

    if (this->presetNum > 0)
    {
        SHL_Json::Value presetInfo;
        SHL_Json::Value presetList;
        for (int presetcnt = 0; presetcnt < this->presetNum; presetcnt++)
        {
            presetInfo["idx"] = this->presetList[presetcnt].idx;
            presetInfo["isIVS"] = this->presetList[presetcnt].isIVS;
            presetInfo["name"] = this->presetList[presetcnt].name;
            presetInfo["type"] = this->presetList[presetcnt].type;
            presetList.append(presetInfo);
        }
        m_value["presetList"] = presetList;
    }
    m_value["presetNum"] = this->presetNum;

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}


/**
 * C2QueryRecord
 */
C2QueryRecord::C2QueryRecord(const c2_query_record_s &query)
{
    memcpy(this, &query, sizeof(c2_query_record_s));
    m_json.clear();
}

C2QueryRecord::~C2QueryRecord()
{

}

string
C2QueryRecord::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2QueryRecord::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost or addrcode parameter.");
        return false;
    }

    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if ((this->fromIndex < 1) || (this->fromIndex > 10000))
    {
        ERRE_SDK("invalidate fromIndex parameter.");
        return false;
    }

    if ((this->toIndex < 1) || (this->toIndex > 10000))
    {
        ERRE_SDK("invalidate toIndex parameter.");
        return false;
    }

    if (this->start.year < 1970
            || this->start.month < 1
            || this->start.month > 12
            || this->start.day < 1
            || this->start.day > 31)
    {

        ERRE_SDK("invalidate start time parameter.");
        return false;
    }

    if (this->end.year < 1970
            || this->end.month < 1
            || this->end.month > 12
            || this->end.day < 1
            || this->end.day > 31)
    {

        ERRE_SDK("invalidate end time parameter.");
        return false;
    }

    return true;
}

void
C2QueryRecord::fill()
{
    m_value["version"]   = JSON_DATA_VERSION;
    m_value["channelNo"] = this->channelNo;
    m_value["type"]      = this->type;
    m_value["fromIndex"] = this->fromIndex;
    m_value["toIndex"]   = this->toIndex;

    m_value["start"]     = sys->strftime(this->start);
    m_value["end"]       = sys->strftime(this->end);

    if (this->addrcode && string(this->addrcode) != "")
    {
        m_value["addrcode"] = this->addrcode;
    }
    else
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2PByName
 */
C2PByName::C2PByName(const c2_pb_name_s &pb)
{
    memcpy(this, &pb, sizeof(c2_pb_name_s));
    m_json.clear();
}

C2PByName::~C2PByName()
{

}

string
C2PByName::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

void C2PByName::setReverse()
{
    m_value["reverse"] = 1;
}

bool
C2PByName::validate()
{
    if (!this->filename)
    {
        ERRE_SDK("invalidate filename parameter.");
        return false;
    }

    if ((this->obtainType < 0) || (this->obtainType > 3))
    {
        ERRE_SDK("invalidate obtainType parameter.");
        return false;
    }

    if ((0 == this->obtainType || 1 == this->obtainType) && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }
    else if ((2 == this->obtainType || 3 == this->obtainType) && !this->addrcode)
    {
        ERRE_SDK("invalidate addrcode parameter.");
        return false;
    }

    return true;
}

void
C2PByName::fill()
{
    m_value["version"]    = JSON_DATA_VERSION;
    m_value["filename"]   = this->filename;
    m_value["obtainType"] = this->obtainType;

    if (0 == this->obtainType || 1 == this->obtainType)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"]   = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        m_value["addrcode"] = this->addrcode;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2PByTime
 */
C2PByTime::C2PByTime(const c2_pb_time_s &pb)
{
    memcpy(this, &pb, sizeof(c2_pb_time_s));
    m_json.clear();
}

C2PByTime::~C2PByTime()
{

}

string
C2PByTime::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2PByTime::validate()
{
    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if (this->start.year < 1970
            || this->start.month < 1
            || this->start.month > 12
            || this->start.day < 1
            || this->start.day > 31)
    {

        ERRE_SDK("invalidate start time parameter.");
        return false;
    }

    if (this->end.year < 1970
            || this->end.month < 1
            || this->end.month > 12
            || this->end.day < 1
            || this->end.day > 31)
    {

        ERRE_SDK("invalidate end time parameter.");
        return false;
    }

    if ((this->obtainType < 0) || (this->obtainType > 3))
    {
        ERRE_SDK("invalidate obtainType parameter.");
        return false;
    }

    if ((0 == this->obtainType || 1 == this->obtainType) && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }
    else if ((2 == this->obtainType || 3 == this->obtainType) && !this->addrcode)
    {
        ERRE_SDK("invalidate addrcode parameter.");
        return false;
    }

    return true;
}

void
C2PByTime::fill()
{
    m_value["version"]    = JSON_DATA_VERSION;
    m_value["channelNo"]  = this->channelNo;
    m_value["obtainType"] = this->obtainType;

    m_value["start"]      = sys->strftime(this->start);
    m_value["end"]        = sys->strftime(this->end);

    if (0 == this->obtainType || 1 == this->obtainType)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"]   = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        m_value["addrcode"] = this->addrcode;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2Download
 */
C2Download::C2Download(const c2_download_s &pb)
{
    memcpy(this, &pb, sizeof(c2_download_s));
    m_json.clear();
}

C2Download::~C2Download()
{

}

string
C2Download::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Download::validate()
{
    if (!this->filename)
    {
        ERRE_SDK("invalidate filename parameter.");
        return false;
    }
    if (!this->savepath)
    {
        ERRE_SDK("invalidate savepath parameter.");
        return false;
    }

    if ((this->obtainType < 0) || (this->obtainType > 3))
    {
        ERRE_SDK("invalidate obtainType parameter.");
        return false;
    }

    if ((0 == this->obtainType || 1 == this->obtainType) && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }
    else if ((2 == this->obtainType || 3 == this->obtainType) && !this->addrcode)
    {
        ERRE_SDK("invalidate addrcode parameter.");
        return false;
    }

    return true;
}

void
C2Download::fill()
{
    m_value["version"]    = JSON_DATA_VERSION;
    m_value["filename"]   = this->filename;
    m_value["obtainType"] = this->obtainType;

    if (0 == this->obtainType || 1 == this->obtainType)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"]   = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        m_value["addrcode"] = this->addrcode;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}


/**
* C2DownloadByTime
*/
C2DownloadByTime::C2DownloadByTime(const c2_download_time_s &pb)
{
    memcpy(this, &pb, sizeof(c2_download_time_s));
    m_json.clear();
}

C2DownloadByTime::~C2DownloadByTime()
{

}

string
C2DownloadByTime::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2DownloadByTime::validate()
{
    if ((this->channelNo < 0) || (this->channelNo > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if (this->start.year < 1970
        || this->start.month < 1
        || this->start.month > 12
        || this->start.day < 1
        || this->start.day > 31)
    {

        ERRE_SDK("invalidate start time parameter.");
        return false;
    }

    if (this->end.year < 1970
        || this->end.month < 1
        || this->end.month > 12
        || this->end.day < 1
        || this->end.day > 31)
    {

        ERRE_SDK("invalidate end time parameter.");
        return false;
    }

    if ((this->obtainType < 0) || (this->obtainType > 3))
    {
        ERRE_SDK("invalidate obtainType parameter.");
        return false;
    }

    if ((0 == this->obtainType || 1 == this->obtainType) && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }
    else if ((2 == this->obtainType || 3 == this->obtainType) && !this->addrcode)
    {
        ERRE_SDK("invalidate addrcode parameter.");
        return false;
    }

    return true;
}

void
C2DownloadByTime::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["channelNo"] = this->channelNo;
    m_value["obtainType"] = this->obtainType;

    m_value["start"] = sys->strftime(this->start);
    m_value["end"] = sys->strftime(this->end);

    if (0 == this->obtainType || 1 == this->obtainType)
    {
        m_value["deviceHost"] = this->deviceHost;
        m_value["devicePort"] = this->devicePort;

        if (this->subServHost)
        {
            m_value["subServHost"] = this->subServHost;
            m_value["subServPort"] = this->subServPort;
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        m_value["addrcode"] = this->addrcode;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

/**
 * C2Synct
 */
C2Synct::C2Synct(const c2_synct_s &synct)
{
    memcpy(this, &synct, sizeof(c2_synct_s));
    m_json.clear();
}

C2Synct::~C2Synct()
{

}

string
C2Synct::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Synct::validate()
{
    if (this->time.year < 1970
            || this->time.month < 1
            || this->time.month > 12
            || this->time.day < 1
            || this->time.day > 31)
    {

        ERRE_SDK("invalidate sync time parameter.");
        return false;
    }

    return true;
}

void
C2Synct::fill()
{
    m_value["version"]  = JSON_DATA_VERSION;
    m_value["time"]     = sys->strftime(this->time);
    m_value["setLocal"] = this->setLocal;

    if (this->zone && string(this->zone) != "")
    {
        m_value["zone"] = this->zone;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}


/**
* C2DecoderParam
*/
C2DecoderParam::C2DecoderParam(const c2_decoder_param_s param)
{
    memcpy(this, &param, sizeof(c2_decoder_param_s));
    this->m_json.clear();
}


C2DecoderParam::~C2DecoderParam()
{

}


void
C2DecoderParam::fill()
{
    m_value["version"] = JSON_DATA_VERSION;

    

    m_value["deviceHost"] = this->deviceHost;
    m_value["devicePort"] = this->devicePort;
    m_value["mode"] = this->mode;
    m_value["outPort"] = this->outPort;
    m_value["channelNo"] = this->channelNo;



    if (this->subServHost)
    {
        m_value["subServHost"] = this->subServHost;
        m_value["subServPort"] = this->subServPort;
    }

}


bool
C2DecoderParam::validate()
{

    //if (!deviceHost)
    //{
    //    ERRE_SDK("invalidate deviceHost parameter.");
    //    return false;
    //}

    //if (channelNo < 0 || channelNo > MAX_CHN_NUM)
    //{
    //    ERRE_SDK("invalidate channelNo");
    //    return false;
    //}

    //if (outPort < 1)
    //{
    //    ERRE_SDK("invalidate outport");
    //    return false;
    //}

    //if (devicePort < 0)
    //{
    //    ERRE_SDK("invalidate deviceport");
    //    return false;
    //}

    return true;
}


string 
C2DecoderParam::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           jsonstr;

    jsonstr = writer.write(m_value);
    return jsonstr;
}



/**
* C2Dualtalk
*/
C2Dualtalk::C2Dualtalk(const c2_dualtalk_dev_s &view)
{
    memcpy(this, &view, sizeof(c2_dualtalk_dev_s));
    m_json.clear();
}

C2Dualtalk::~C2Dualtalk()
{

}

string
C2Dualtalk::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Dualtalk::validate()
{
    if ((this->chnNum < 0) || (this->chnNum > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate channelNo parameter.");
        return false;
    }

    if (this->chnList == NULL)
    {
        ERRE_SDK("invalidate chnList parameter.");
        return false;
    }

    return true;
}

void
C2Dualtalk::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["channelCount"] = this->chnNum;
    m_value["obtainType"] = this->obtainType;
    SHL_Json::Value chaList;
    SHL_Json::Value chList;
    if (0 == this->obtainType || 1 == this->obtainType)
    {
        for (int i = 0; i < this->chnNum; ++i)
        {
            chaList["deviceHost"] = this->chnList[i].deviceHost;
            chaList["devicePort"] = this->chnList[i].devicePort;
            chaList["channelNo"] = this->chnList[i].channelNo;
            chList.append(chaList);
        }
    }
    else if (2 == this->obtainType || 3 == this->obtainType)
    {
        for (int i = 0; i < this->chnNum; ++i)
        {
            chaList["addrcode"] = this->chnList[i].addrcode;
            chList.append(chaList);
        }
            
    }
    m_value["channels"] = chList;

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}



/**
* C2Matrix
*/
C2Matrix::C2Matrix(const c2_matrix_s &view)
{
    memcpy(this, &view, sizeof(c2_matrix_s));
    m_json.clear();
}

C2Matrix::~C2Matrix()
{

}

string
C2Matrix::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2Matrix::validate()
{
    if ((this->inPort < 0) || (this->inPort > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate inPort parameter.");
        return false;
    }

    if ((this->outPort < 0) || (this->outPort > MAX_CHN_NUM))
    {
        ERRE_SDK("invalidate outPort parameter.");
        return false;
    }

    if (!this->deviceHost || !this->devicePort)
    {
        ERRE_SDK("invalidate deviceHost parameter.");
        return false;
    }


    return true;
}

void
C2Matrix::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["deviceHost"] = this->deviceHost;
    m_value["devicePort"] = this->devicePort;
    m_value["inPort"]     = this->inPort;
    m_value["outPort"]    = this->outPort;

    if (this->subServHost)
    {
        m_value["subServHost"] = this->subServHost;
        m_value["subServPort"] = this->subServPort;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;

        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}



/**
* C2SetFace
*/
C2SetFace::C2SetFace(const c2_set_face_s &view)
{
    memcpy(this, &view, sizeof(c2_set_face_s));
    m_json.clear();
    m_picData = NULL;
}

C2SetFace::~C2SetFace()
{
    if (m_picData)
    {
        delete[] m_picData;
        m_picData = NULL;
    }
}

string
C2SetFace::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2SetFace::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost or addrcode parameter.");
        return false;
    }

    if (this->Id == NULL)
    {
        ERRE_SDK("Id NULL");
        return false;
    }

    if (this->opera == 0)
    {
        if (this->picPath == NULL)
        {
            ERRE_SDK("invalidate picPath parameter.");
            return false;
        }

        FileStream fs;
        fs.openReadOnly(this->picPath);
        if (!fs.isopen())
        {
            ERRE_SDK("invalidate picture path parameter.");
            return false;
        }

        if (fs.length() > 1024 * 512 * 3)
        {
            ERRE_SDK("picture size too big");
            fs.close();
            return false;
        }
        
        if (this->exLen > 0)
        {
            c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;
            for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
            {
                m_value[extParam[i].key] = extParam[i].value;
            }
        }

        fs.close();
    }

    return true;
}

void
C2SetFace::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["Method"] = "setFace";
    m_value["DevIp"] = this->deviceHost;
    m_value["DevPort"] = this->devicePort;
    m_value["DevChn"] = this->channelNo;
    m_value["ID"] = this->Id;
    m_value["opera"] = this->opera;
    m_value["name"] = this->name;
     
    if (this->subServHost)
    {
        m_value["subServHost"] = this->subServHost;
        m_value["subServPort"] = this->subServPort;
    }

    if (this->opera == 0)
    {
        
        FileStream fs;
        fs.openReadOnly(this->picPath);
        int len = fs.length();
        if (m_picData != NULL)
        {
            delete[]m_picData;
        }

        m_picData = new char[len];
        fs.read(m_picData, len);
        m_value["picData"] = base64_encode((unsigned char const*)m_picData, len);
        fs.close();
    }
    
    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;
        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}


/**
* C2PtzPattern
*/
C2PtzPattern::C2PtzPattern(const c2_ptz_pattern_s& param)
{
    memcpy(this, &param, sizeof(c2_ptz_pattern_s));
    m_json.clear();
}

C2PtzPattern::~C2PtzPattern()
{
    ;
}

string
C2PtzPattern::toStr()
{
    if (!validate())
    {
        return "";
    }

    fill();

    SHL_Json::FastWriter writer;
    string           json;

    json = writer.write(m_value);
    /* json = sys->ascii2utf8(json); */

    return json;
}

bool
C2PtzPattern::validate()
{
    if (!this->addrcode && !this->deviceHost)
    {
        ERRE_SDK("invalidate deviceHost or addrcode parameter.");
        return false;
    }

    return true;
}

void
C2PtzPattern::fill()
{
    m_value["version"] = JSON_DATA_VERSION;
    m_value["Method"] = "ptzPatternCtrl";
    m_value["DevIp"] = this->deviceHost;
    m_value["DevPort"] = this->devicePort;
    m_value["DevChn"] = this->channelNo;
    m_value["PatternCmd"] = this->patternCmd;
    m_value["PatternId"] = this->patternID;

    if (this->subServHost)
    {
        m_value["subServHost"] = this->subServHost;
        m_value["subServPort"] = this->subServPort;
    }

    if (this->exLen > 0)
    {
        c2_ex_param_s * extParam = (c2_ex_param_s *)this->exParam;
        for (int i = 0; i < (int)(this->exLen / sizeof(c2_ex_param_s)); ++i)
        {
            m_value[extParam[i].key] = extParam[i].value;
        }
    }
}

