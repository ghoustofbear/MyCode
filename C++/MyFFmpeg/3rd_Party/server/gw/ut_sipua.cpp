#include "ut_sipua.h"
#include "j2x.h"
#include "putree.h"
#include "superior.h"


UtSipUA::UtSipUA(int transport, string bindip, int port)
    : GWSipUA(transport, bindip, port)
{

}


UtSipUA::~UtSipUA()
{
}

int
UtSipUA::query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
string &content, int jx, int fromidx, int toidx)
{
    if (fromidx <1 || toidx < fromidx)
    {
        return -1;
    }

    SIP_REQ_S stReq;
    stReq.strType = "Request_Resource";
    stReq.stResouceReq.ps8AddrCode = (int8_t*)code.c_str();
    stReq.stResouceReq.s32FromIndex = fromidx;
    stReq.stResouceReq.s32ToIndex = toidx;

    bool bFinished = false;
    int s32Ret = -1;

    SHL_Json::Value  respValue;
	SHL_Json::StreamWriterBuilder writer;
    int relTotal = 0;

    while (!bFinished)
    {
        string strXml;
        makeXmlReq(stReq, strXml);

        string stReponse;
        s32Ret = message(fromn, fromh, fromp, ton, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }

        string jsonResp;
        xml2json(stReponse.c_str(), jsonResp);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
		if (!reader.parse(jsonResp, rvalue, false))
        {
            return -1;
        }

        int realNum = atoi(rvalue["SIP_XML"]["SubList"]["@RealNum"].asString().c_str());
        int toIndex = atoi(rvalue["SIP_XML"]["SubList"]["@ToIndex"].asString().c_str());
        int subNum = atoi(rvalue["SIP_XML"]["SubList"]["@SubNum"].asString().c_str());

        if (subNum>0)
        {
			SHL_Json::Value  arrayV = rvalue["SIP_XML"]["SubList"]["Item"];
            if (arrayV.isArray())
            {
                for (int i = 0; i < arrayV.size(); i++)
                {
                    respValue["SIP_XML"]["SubList"]["Item"].append(arrayV[i]);
                }
            }
            else
            {
                respValue["SIP_XML"]["SubList"]["Item"].append(arrayV);
            }

            respValue["SIP_XML"]["SubList"]["@EventType"] = rvalue["SIP_XML"]["SubList"]["@EventType"];
            respValue["SIP_XML"]["SubList"]["@Code"] = rvalue["SIP_XML"]["SubList"]["@Code"];
            relTotal += subNum;
        }
        if (toidx > realNum)
        {
            toidx = realNum;
        }

        if (toIndex < toidx)
        {
            stReq.stResouceReq.s32FromIndex = toIndex + 1;
            stReq.stResouceReq.s32ToIndex   = toidx;
        }
        else
        {
            respValue["SIP_XML"]["SubList"]["@RealNum"] = std::to_string(relTotal);
            respValue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(respValue.size());
            respValue["SIP_XML"]["SubList"]["@FromIndex"] = std::to_string(1);
            respValue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(relTotal);

            stReponse = SHL_Json::writeString(writer, respValue);
            if (jx == 0)
			{
                content = stReponse;
            }
            else
            {
                json2xml(stReponse.c_str(), content);
            }

            bFinished = true;
            s32Ret = 1;
        }
    }

    return s32Ret;
}
