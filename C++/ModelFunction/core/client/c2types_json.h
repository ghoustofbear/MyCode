/**
 * @file      c2types_json.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-02 14:40:10
 *
 * @note
 *  c2types_json.h defines
 */

#ifndef __C2TYPES_JSON_H__
#define __C2TYPES_JSON_H__
#include "c2types.h"
#include "tlist.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

/**
 * C2Login
 */
class C2Login : public c2_login_s
{
public:
    C2Login(const c2_login_s &login);
    ~C2Login();

    bool               validate();
    string             toStr();

    long               fromStr(const char *str);
    long               fromJson();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
};

/**
 * C2Preview
 */
class C2Preview: public c2_preview_s
{
public:
    C2Preview(const c2_preview_s &view);
    ~C2Preview();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2QueryDevInfo
 */
class C2QueryDevInfo: public c2_query_devinfo_s
{
public:
    C2QueryDevInfo(const c2_query_devinfo_s &query);
    ~C2QueryDevInfo();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2QueryChaInfo
 */
class C2QueryChaInfo: public c2_query_channel_s
{
public:
    C2QueryChaInfo(const c2_query_channel_s &query);
    ~C2QueryChaInfo();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};


/**
 * C2Ptz
 */
class C2Ptz: public c2_ptz_s
{
public:
    C2Ptz(int cmd, const c2_ptz_s &ptz);
    ~C2Ptz();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
    int                m_cmd;
};

/**
* C2QueryPreset
*/
class C2QueryPreset : public c2_query_preset_s
{
public:
    C2QueryPreset(const c2_query_preset_s &query);
    ~C2QueryPreset();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
* C2SetPreset
*/
class C2SetPreset : public c2_set_preset_s
{
public:
    C2SetPreset(const c2_set_preset_s &query);
    ~C2SetPreset();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};


/**
 * C2QueryRecord
 */
class C2QueryRecord: public c2_query_record_s
{
public:
    C2QueryRecord(const c2_query_record_s &query);
    ~C2QueryRecord();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2PByName 
 */
class C2PByName: public c2_pb_name_s
{
public:
    C2PByName(const c2_pb_name_s &pb);
    ~C2PByName();

    bool               validate();
    string             toStr();
    void               setReverse();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2PByTime
 */
class C2PByTime: public c2_pb_time_s
{
public:
    C2PByTime(const c2_pb_time_s &pb);
    ~C2PByTime();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2Download 
 */
class C2Download: public c2_download_s
{
public:
    C2Download(const c2_download_s &pb);
    ~C2Download();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
* C2DownloadByTime
*/
class C2DownloadByTime : public c2_download_time_s
{
public:
    C2DownloadByTime(const c2_download_time_s &pb);
    ~C2DownloadByTime();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2Synct
 */
class C2Synct: public c2_synct_s
{
public:
    C2Synct(const c2_synct_s &synct);
    ~C2Synct();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
 * C2ExParam
 */
class C2ExParam : public tlist<c2_ex_param_s>
{
public:
    C2ExParam();
    ~C2ExParam();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
};

/**
* C2Dualtalk
*/
class C2Dualtalk : public c2_dualtalk_dev_s
{
public:
    C2Dualtalk(const c2_dualtalk_dev_s &view);
    ~C2Dualtalk();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
* C2DecoderParam
*/

class C2DecoderParam : public c2_decoder_param_s
{
public:
    C2DecoderParam(const c2_decoder_param_s param);
    ~C2DecoderParam();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

/**
* C2Matrix
*/

class C2Matrix : public c2_matrix_s
{
public:
    C2Matrix(const c2_matrix_s& param);
    ~C2Matrix();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};


/**
* C2SetFace
*/
class C2SetFace : public c2_set_face_s
{
public:
    C2SetFace(const c2_set_face_s& param);
    ~C2SetFace();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
    char*              m_picData;
};

/**
* C2PtzPattern
*/
class C2PtzPattern : public c2_ptz_pattern_s
{
public:
    C2PtzPattern(const c2_ptz_pattern_s& param);
    ~C2PtzPattern();

    bool               validate();
    string             toStr();

protected:
    void               fill();

private:
    SHL_Json::Value    m_value;
    std::string        m_json;
};

#endif
