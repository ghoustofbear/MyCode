#include "putree.h"
#include "system.h"
#include "ntp.h"
#include "hik_pu.h"
#include "hik_decoder_pu.h"
#include "hik_face_auth_pu.h"
#include "hik_gateway_pu.h"
#include "hik_ehome_pu.h"
#include "tindy_pu.h"
#include "hs_pu.h"
#include "msv_pu.h"
#include "gwb_pu.h"
#include "utsip_pu.h"
#include "car_pu.h"
#include "dahua_pu.h"
#include "helmet_gx_pu.h"
#include "hik_face_rgn_pu.h"
#include "tls_error.h"
#include "onuiupdate.h"
#include "gumgr.h"
#include "sumgr.h"
#include "hik_terminal_pu.h"
#include "huadian_pu.h"
#include "launch_pu.h"
#include "cfg_sub_platform.h"
#include "cfg_device.h"
#include "gwa_pu.h"
#include "ut_iva_pu.h"
#include "xunwei_matrix_pu.h"
#include "cdt_wflink_pu.h"
#include "jiyuan_iva_pu.h"
#include "json/json.h"
#include "gb28181_pu.h"
#include "gqy_decoder_pu.h"

static void
free_nodedata(string key, void *nodedata, void *userdata)
{
    if (!nodedata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data)
    {
        return;
    }

    if (1 == ndata->type)
    {
        Pu *pu = (Pu *)ndata->data;
        delete pu;
    }
    else if (0 == ndata->type)
    {}

    delete ndata;
}

static void
stop_nodedata(string key, void *nodedata, void *userdata)
{
    if (!nodedata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data)
    {
        return;
    }

    if (1 == ndata->type)
    {
        Pu *pu = (Pu *)ndata->data;
        pu->stop();
    }
    else if (0 == ndata->type)
    {}
}

static void
add_trigger(string key, void *nodedata, void *userdata)
{
    if (!nodedata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data || !(1 == ndata->type))
    {
        return;
    }

    Pu *pu = (Pu *)ndata->data;
    pu->addTriggerListener((TriggerListener *)userdata);
}

static
void del_trigger(string key, void *nodedata, void *userdata)
{
    if (!nodedata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data || !(1 == ndata->type))
    {
        return;
    }

    Pu *pu = (Pu *)ndata->data;
    pu->delTriggerListener((TriggerListener *)userdata);
}

static void
timer_nodedata(string key, void *nodedata, void *userdata)
{
    if (!nodedata || !userdata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data || !(1 == ndata->type))
    {
        return;
    }

    Pu *pu = (Pu *)ndata->data;
    pu->onTimer();
}

static void
update_status(string key, void *nodedata, void *userdata)
{
    if (!nodedata || !userdata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (3 == ndata->type)
    {
        PuTree *putre = (PuTree *)userdata;
        Pu *pu = putre->findPu(key);
        if (pu)
        {
            ndata->status = pu->validate(ndata->channel);
        }
    }
    else if (1 == ndata->type)
    {
        Pu *pu = (Pu *)ndata->data;
        if (pu)
        {
            ndata->status = pu->online() ? 1 : 0;
        }
    }

}

static void
timer_synctime(string key, void *nodedata, void *userdata)
{
    if (!nodedata)
    {
        return;
    }

    nodedata_t *ndata = (nodedata_t *)nodedata;
    if (!ndata->data || !(1 == ndata->type))
    {
        return;
    }

    Pu *pu = (Pu *)ndata->data;
    if (userdata)
    {
        datetime_s *dt = (datetime_s *)userdata;

        pu->syncTime(*dt, "");
    }
    else
    {
        /* pu->syncTime(*dt, ""); */
    }
}

void
get_nodedata(string key, void *nodedata, void *userdata)
{
    if (!nodedata || !userdata)
    {
        return;
    }

    nodedata_t  * ndata = (nodedata_t *)nodedata;
    SHL_Json::Value * value = (SHL_Json::Value *)userdata;
    SHL_Json::Value   item;

    item["type"]    = ndata->type;
    item["code"]    = key;
    item["name"]    = ndata->name;
    item["host"]    = ndata->host;
    item["port"]    = ndata->port;
    item["ablity"]  = ndata->ablity;
    item["channel"] = ndata->channel;
    item["subnum"]  = ndata->subnum;
	item["devtype"] = ndata->devtype;
	item["manutype"] = ndata->manutype;

//    if (ndata->data)
//    {
//        item["status"]  = ((Pu *)(ndata->data))->online()?1:0;
//    }
//    else
    {
        item["status"]  = ndata->status;
    }

    value->append(item);
}

PuTree::PuTree() : ttree<string>(systm->addrcode(), new nodedata_t(0, systm, systm->name(), "127.0.0.1", systm->pport()))
{
    m_root  = systm->addrcode();
    m_ui    = NULL;
	m_puui = NULL;
	m_puuiAddrcode = "";
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    deviceCfg->addObserver(this);
    suberCfg->addObserver(this); 
    suberCamInfoCfg->addObserver(this);


    m_lastSync = 0;
}

PuTree::~PuTree()
{
    traversal(m_root, free_nodedata, this);
}

int
PuTree::start()
{
    int result = 0;

    string json;
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    /* add video device and sub media stream */
    deviceCfg->query(json, "level=1 or level=2");

    if (reader.parse(json, rvalue, false))
    {
        for (uint32_t i = 0; i < rvalue.size(); i++)
        {
            result = addPu(rvalue[i]["protocol"].asInt(),
                           rvalue[i]["type"].asInt(),
                           rvalue[i]["addrcode"].asString(),
                           rvalue[i]["name"].asString(),
                           rvalue[i]["chnNum"].asInt(),
                           rvalue[i]["username"].asString(),
                           rvalue[i]["password"].asString(),
                           rvalue[i]["host"].asString(),
                           rvalue[i]["port"].asInt(),
                           rvalue[i]["domain"].asInt(),
                           rvalue[i]["recvCall"].asInt(),
                           rvalue[i]["linktype"].asInt()
                          );

        }
    }

    traversal(systm->addrcode());

    json.clear();
    rvalue.clear();

    /* add sub platform */
    suberCfg->query(json);
    if (reader.parse(json, rvalue, false))
    {
        for (uint32_t i = 0; i < rvalue.size(); i++)
        {
            result += addPu(rvalue[i]["protocol"].asInt(),
                            rvalue[i]["type"].asInt(),
                            rvalue[i]["addrcode"].asString(),
                            rvalue[i]["name"].asString(),
                            rvalue[i]["host"].asString(),
                            rvalue[i]["sipPort"].asInt(),
                            rvalue[i]["rtspPort"].asInt(),
                            rvalue[i]["httpPort"].asInt(),
                            rvalue[i]["rtpPort"].asInt()
                           );
        }
    }

#ifdef TIMERITSELF
    m_timerThread.setEntry(threadEntry, this, T_TIMER);
    result += m_timerThread.start();
#endif

    return result;
}

void
PuTree::stop()
{
#ifdef TIMERITSELF
    m_timerThread.stop();
#endif

    {
        lock_guard<recursive_mutex> locker(_mutex);
        for (auto  i = begin(); i != end(); ++i)
        {
            if (!i->second.data)
            {
                continue;
            }

            nodedata_t *nodedata = (nodedata_t *)i->second.data;
            if (!nodedata->data)
            {
                continue;
            }

            if (1 == nodedata->type)
            {
                Pu *pu = (Pu *)nodedata->data;
                pu->stop();
            }
        }
    }
}

int
PuTree::addPu(int    protocol,
              int    type,
              string addrcode,
              string name,
              int    chnNum,
              string user,
              string pswd,
              string host,
              int    port,
              int    domainType,
              int    callListen,
              int    linktype)
{
    Pu * pu = NULL;

    if (protocol == Pu::PROTO_TYPE_HIK)
    {
		if (type == Pu::DEV_TYPE_DVR || type == Pu::DEV_TYPE_NVR)
            pu = new HikPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        else if (type == Pu::DEV_TYPE_DECODE || type == Pu::DEV_TYPE_DECODE_6901 || type == Pu::DEV_TYPE_DECODE_B20)
            pu = new HikDecoderPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        else if (type == Pu::DEV_TYPE_FACEAUTH_RZ)
            pu = new HikFaceAuthPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
		else if (type == Pu::DEV_TYPE_GATEWAY)
			pu = new HikGateWayPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        else if (type == Pu::DEV_TYPE_FACEAUTH_RL)
            pu = new HikFaceRngPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        else if (type == Pu::DEV_TYPE_HIK_TERMINAL)
            pu = new HikTerminalPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        else
            pu = new HikPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        
        if (type == Pu::DEV_TYPE_MENKOU)
        {
            pu->setCallListen(callListen);
        }
    }
    else if (protocol == Pu::PROTO_TYPE_DAHUA)
    {
        pu = new DahuaPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType, linktype);
    }
    else if (protocol == Pu::PROTO_TYPE_TIANDY)
    {
        pu = new TindyPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
    }
    else if (protocol == Pu::PROTO_TYPE_UT)
    {
        if (type == Pu::DEV_TYPE_CAR)
        {
            pu = new CarPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        }
        else if (type == Pu::DEV_TYPE_STREAMSERV)
        {
            pu = new MsvPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        }
        else if (type == Pu::DEV_TYPE_AI)
        {
            pu = new UtIvaPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
        }
		else if (type == Pu::DEV_TYPE_LAUNCH)
		{
			pu = new LaunchPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
		}
    }
    else if (protocol == Pu::PROTO_TYPE_HELMET)
    {
        pu = new HelmetGXPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
    }
    else if (protocol == Pu::PROTO_TYPE_CDT)
    {
        pu = new CdtWfLinkPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
    }
    else if (protocol == Pu::PROTO_TYPE_XUNWEI)
    {
        pu = new XunWeiMatrixPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
    }
	else if (protocol == Pu::PROTO_TYPE_JIYUAN)
	{
		pu = new JiYuanIvaPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
	}
	else if (protocol == Pu::PROTO_TYPE_EHOME)
	{
		pu = new HikEhomePu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
	}
    else if (protocol == Pu::PROTO_TYPE_HUADIAN)
    {
        pu = new HuaDianPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
    }
	else if (protocol == Pu::PROTO_TYPE_GQY)
	{
		pu = new GqyDecoderPu(type, addrcode, name, chnNum, user, pswd, host, port, domainType);
	}
    if (!pu)
    {
        return -1;
    }

    nodedata_t *data = new nodedata_t(1, pu, name, pu->ip(), pu->port(), 0, chnNum, 0, 1, type, protocol, linktype);
    if (insert_child(m_root, pu->addrcode(), data) < 0)
    {
        ERRE_MSV("insert_child failed");
        delete data;
        delete pu;

        return -1;
    }

    if (pu->start() < 0)
    {
        ERRE_MSV("pu: %s, %s:%d start fail.", pu->addrcode().c_str(), pu->host().c_str(), pu->port());
        delete pu;
        delete data;
        delete_sub(addrcode);

        return -1;
    }

    /* update root nodedata subnum field */
    nodedata_t *rootdata = (nodedata_t *)get_data(m_root);
    rootdata->subnum = child_num(m_root);

    return 0;
}

int
PuTree::addPu(int    protocol,
              int    type,
              string addrcode,
              string name,
              string host,
              int    sport,
              int    rport,
              int    hport,
              int    rtport
             )
{
    Pu * pu = NULL;
    switch (protocol)
    {
    case Pu::PROTO_TYPE_GW_B_HS:
        /* pu = new HsPu(type, addrcode, name, host, sport, rport, hport, rtport); */
        break;

    case Pu::PROTO_TYPE_GW_A:
         pu = new GwAPu(type, addrcode, name, host, sport, rport, hport, rtport);
        break;

    case Pu::PROTO_TYPE_GW_B:
         pu = new GwBPu(type, addrcode, name, host, sport, rport, hport, rtport);
         break;
    case Pu::PROTO_TYPE_GW_B_UT:
         pu = new UtSipPu(type, addrcode, name, host, sport, rport, hport, rtport);
         break;
    case Pu::PROTO_TYPE_GB28181:
        pu = new GB28181Pu(type, addrcode, name, host, sport, rport, hport, rtport);
        break;

    default:
        break;
    }

    if (!pu)
    {
        return -1;
    }

    nodedata_t *data = new nodedata_t(1, pu, name, pu->ip(), pu->port());
    if (insert_child(m_root, pu->addrcode(), data) < 0)
    {
        ERRE_MSV("insert_child failed");
        delete data;
        delete pu;
        return -1;
    }

    if (pu->start() < 0)
    {
        ERRE_MSV("pu: %s, %s:%d start fail", pu->addrcode().c_str(), pu->host().c_str(), pu->port());
        delete pu;
        delete data;
        delete_sub(addrcode);

        return -1;
    }

    nodedata_t *rootdata = (nodedata_t *)get_data(m_root);
    rootdata->subnum = child_num(m_root);

    return 0;
}

int
PuTree::delPu(string addrcode)
{
    lock_guard<recursive_mutex> locker(_mutex);
    traversal(addrcode, stop_nodedata, this);
    traversal(addrcode, free_nodedata, this);
    delete_sub(addrcode);

    nodedata_t *rootdata = (nodedata_t *)get_data(m_root);
    rootdata->subnum = child_num(m_root);

    return 0;
}

Pu *
PuTree::findPu(string host, int port)
{
    lock_guard<recursive_mutex> locker(_mutex);
    for (auto i = begin(); i != end(); ++i)
    {
        if (!i->second.data)
        {
            continue;
        }

        nodedata_t *nodedata = (nodedata_t *)i->second.data;

        /* not device */
        if (1 != nodedata->type && 2 != nodedata->type)
        {
            continue;
        }

        if (host == nodedata->host && port == nodedata->port)
        {
            /* not local device, we find the ancestor */
            if (2 == nodedata->type)
            {
                string ances = ancestor(i->first);
                nodedata = (nodedata_t *)get_data(ances);
            }

            Pu *pu = (Pu *)nodedata->data;
            if (!pu)
            {
                continue;
            }

            /* return the first pu as host:port */
            return pu;
        }
    }

    ERRW_MSV("local pu %s:%d is not exist.", host.c_str(), port);

    return NULL;
}

string
PuTree::findDevCode(string host, int port)
{
	Pu *pu = findPu(host, port);
	if (pu != NULL)
	{
		return pu->addrcode();
	}
	return "";
}

Pu *
PuTree::findPu(string addrcode)
{
    string ances = ancestor(addrcode);
    if (ances.empty())
    {
        ERRE_MSV("not found device:%s", addrcode.c_str());
        return NULL;
    }

    nodedata_t *nodedata = (nodedata_t *)get_data(ances);
    if (!nodedata)
    {
        ERRW_MSV("node %s have no data!", addrcode.c_str());
        return NULL;
    }

    if (!nodedata->data || 1 != nodedata->type)
    {
        ERRW_MSV("local pu: %s is not exsit!", addrcode.c_str());
        return NULL;
    }

    return (Pu *)nodedata->data;
}

void
PuTree::findIvaPu(vector<Pu *> &IvaPu)
{
	lock_guard<recursive_mutex> locker(_mutex);
	for (auto i = begin(); i != end(); ++i)
	{
		if (!i->second.data)
		{
			continue;
		}
		nodedata_t *nodedata = (nodedata_t *)i->second.data;
		/* not device */
		if (1 != nodedata->type && 2 != nodedata->type)
		{
			continue;
		}

		Pu *pu = (Pu *)nodedata->data;
		if (pu->type() == Pu::DEV_TYPE_AI)
		{
			IvaPu.push_back((Pu *)nodedata->data);
		}
	}
}


string
PuTree::findCode(string host, int port, int channelNo)
{
    lock_guard<recursive_mutex> locker(_mutex);
    for (auto i = begin(); i != end(); ++i)
    {
        if (!i->second.data)
        {
            continue;
        }

        nodedata_t *nodedata = (nodedata_t *)i->second.data;

        /* not device */
        if (1 != nodedata->type && 2 != nodedata->type)
        {
            continue;
        }

        if (host == nodedata->host && port == nodedata->port)
		{
			string child;
			nodedata_t * gudata = NULL;
			int chidNum = putree->child_num(i->first);
			for (int j = 0; j < chidNum; ++j)
			{
				gudata = (nodedata_t *)putree->get_childata(i->first, j, child);
				if (!gudata)
				{
					continue;
				}

				if (gudata->channel == channelNo)
					return child;
			}


			return "";
        }
    }

    ERRW_MSV("code %s:%d-%d is not exist.", host.c_str(), port, channelNo);

    return "";
}

void
PuTree::addTriggerListener(TriggerListener *listener)
{
    traversal_child(m_root, add_trigger, listener);
}

void
PuTree::delTriggerListener(TriggerListener *listener)
{
    traversal_child(m_root, del_trigger, listener);
}

int
PuTree::syncTime(datetime_s &dt, const string &tzone)
{
    traversal_child(m_root, timer_synctime, (void *)&dt);

    return 0;
}

void
PuTree::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return;
    }

    if (O_ADD == type)
    {
        if (!rvalue.isObject())
        {
            return;
        }

        int devprotocol = rvalue["protocol"].asInt();
        int devtype = rvalue["type"].asInt();
        if (!rvalue["sub_platform"].isNull() ||
            devprotocol == Pu::PROTO_TYPE_GW_B_UT ||
            devprotocol == Pu::PROTO_TYPE_GB28181)
        {
            if (!rvalue["sub_platform"].isNull())
            {
                nodedata_t *data = new nodedata_t(
                    3,
                    NULL,
                    rvalue["camera_name"].asString(),"",0,0,0, 0,0);

                insert_child(rvalue["sub_platform_addrcode"].asString(), rvalue["camera_addrcode"].asString(), data);
            }
            else
            {
                addPu(devprotocol,
                    devtype,
                    rvalue["addrcode"].asString(),
                    rvalue["name"].asString(),
                    rvalue["host"].asString(),
                    rvalue["sipPort"].asInt(),
                    rvalue["rtpPort"].asInt(),
                    rvalue["httpPort"].asInt(),
                    rvalue["rtspPort"].asInt());
            }
            return;
        }

        if (!(Pu::DEV_TYPE_STREAMSERV == devtype ||
              Pu::DEV_TYPE_DVR == devtype ||
              Pu::DEV_TYPE_IPC == devtype ||
              Pu::DEV_TYPE_NVR == devtype ||
              Pu::DEV_TYPE_MENKOU == devtype ||
              Pu::DEV_TYPE_CAR == devtype ||
              Pu::DEV_TYPE_DECODE_6901 == devtype ||
              Pu::DEV_TYPE_DECODE_B20 == devtype ||
              Pu::DEV_TYPE_DECODE == devtype ||
              Pu::DEV_TYPE_MATRIX == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RZ == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RL == devtype ||
              Pu::DEV_TYPE_AI == devtype ||
              Pu::DEV_TYPE_HIK_TERMINAL == devtype||
			  Pu::DEV_TYPE_GATEWAY == devtype||
			  Pu::DEV_TYPE_LAUNCH  == devtype
              ))
        {
            nodedata_t *data = new nodedata_t(
                3,
                NULL,
                rvalue["name"].asString(),
                rvalue["host"].asString(),
                rvalue["port"].asInt(),
                rvalue["chnNo"].asInt(),
                0, 0,
                rvalue["recvCall"].asInt());

            insert_child(rvalue["parentCode"].asString(), rvalue["addrcode"].asString(), data);

            return;
        }


        addPu(rvalue["protocol"].asInt(),
            devtype,
            rvalue["addrcode"].asString(),
            rvalue["name"].asString(),
            rvalue["chnNum"].asInt(),
            rvalue["username"].asString(),
            rvalue["password"].asString(),
            rvalue["host"].asString(),
            rvalue["port"].asInt(),
            rvalue["domainType"].asInt()
            );
    }
    else if (O_MODIFY == type)
    {
        int devprotocol = rvalue["protocol"].asInt();
        int devtype = rvalue["type"].asInt();

        if (!rvalue["sub_platform"].isNull() ||
            devprotocol == Pu::PROTO_TYPE_GW_B_UT ||
            devprotocol == Pu::PROTO_TYPE_GB28181)
        {
            if (!rvalue["sub_platform"].isNull())
            {
                nodedata_t *d = (nodedata_t*)get_data(rvalue["camera_addrcode"].asString());
                if (d)
                {
                    d->name = rvalue["camera_name"].asString();
                }
            }
            else 
            {
                delPu(rvalue["addrcode"].asString());
                addPu(devprotocol,
                    devtype,
                    rvalue["addrcode"].asString(),
                    rvalue["name"].asString(),
                    rvalue["host"].asString(),
                    rvalue["sipPort"].asInt(),
                    rvalue["rtpPort"].asInt(),
                    rvalue["httpPort"].asInt(),
                    rvalue["rtspPort"].asInt());
            }
            return;
        }


        if (!(Pu::DEV_TYPE_STREAMSERV == devtype ||
              Pu::DEV_TYPE_DVR == devtype ||
              Pu::DEV_TYPE_IPC == devtype ||
              Pu::DEV_TYPE_NVR == devtype ||
              Pu::DEV_TYPE_MENKOU == devtype ||
              Pu::DEV_TYPE_CAR == devtype ||
              Pu::DEV_TYPE_DECODE_6901 == devtype ||
              Pu::DEV_TYPE_DECODE_B20 == devtype ||
              Pu::DEV_TYPE_DECODE == devtype ||
              Pu::DEV_TYPE_MATRIX == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RZ == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RL == devtype ||
              Pu::DEV_TYPE_AI == devtype ||
              Pu::DEV_TYPE_HIK_TERMINAL == devtype||
			  Pu::DEV_TYPE_GATEWAY == devtype||
			  Pu::DEV_TYPE_LAUNCH == devtype
              ))
        {
            return;
        }

        delPu(rvalue["addrcode"].asString());

        addPu(rvalue["protocol"].asInt(),
              devtype,
              rvalue["addrcode"].asString(),
              rvalue["name"].asString(),
              rvalue["chnNum"].asInt(),
              rvalue["username"].asString(),
              rvalue["password"].asString(),
              rvalue["host"].asString(),
              rvalue["port"].asInt(),
              rvalue["domainType"].asInt()
             );

    }
    else if (O_DELETE == type)
    {
        int devprotocol = rvalue["protocol"].asInt();
        int devtype = rvalue["type"].asInt();

        if (!rvalue["sub_platform"].isNull() ||
            devprotocol == Pu::PROTO_TYPE_GW_B_UT ||
            devprotocol == Pu::PROTO_TYPE_GB28181)
        {
            if (!rvalue["sub_platform"].isNull())
            {
                delete_sub(rvalue["camera_addrcode"].asString());
            }
            else
            {
                delPu(rvalue["addrcode"].asString());
            }
            return;
        }

        if (!(Pu::DEV_TYPE_STREAMSERV == devtype ||
              Pu::DEV_TYPE_DVR == devtype ||
              Pu::DEV_TYPE_IPC == devtype ||
              Pu::DEV_TYPE_NVR == devtype ||
              Pu::DEV_TYPE_MENKOU == devtype ||
              Pu::DEV_TYPE_CAR == devtype ||
              Pu::DEV_TYPE_DECODE_6901 == devtype ||
              Pu::DEV_TYPE_DECODE_B20 == devtype ||
              Pu::DEV_TYPE_DECODE == devtype ||
              Pu::DEV_TYPE_MATRIX == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RZ == devtype ||
              Pu::DEV_TYPE_FACEAUTH_RL == devtype ||
              Pu::DEV_TYPE_AI == devtype ||
              Pu::DEV_TYPE_HIK_TERMINAL == devtype||
			  Pu::DEV_TYPE_GATEWAY == devtype||
			  Pu::DEV_TYPE_LAUNCH == devtype
              ))
        {
            return;
        }

        delPu(rvalue["addrcode"].asString());
    }
    else if (O_FLUSH == type)
    {}
}

void
PuTree::setUi(OnUiUpdate* ui)
{
    AutoLock locker(m_uimtx);
    m_ui = ui;
}
void
PuTree::setUi(const string &addrcode, OnUiUpdate *ui)
{   

	AutoLock locker(m_uimtx);
	
	m_puuiAddrcode = addrcode;
	m_puui = ui;
}

void
PuTree::setMessageCallback(pfnMsgcallback pfnMsgCb, void *pUser)
{
    m_pfnMsgcallback = pfnMsgCb;
    m_pUser = pUser;
}

void
PuTree::pushMsg(const char *data, int size)
{
    if (m_pfnMsgcallback)
    {
        m_pfnMsgcallback(data, size, m_pUser);
    }
}

void
PuTree::updateUi()
{
    do
    {
        {
            AutoLock locker(m_uimtx);
            if (m_ui == NULL)
            {
                break;
            }
        }
    
	
		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
        if (child_num(m_root) > 0)
        {
            SHL_Json::StreamWriterBuilder writer;
            SHL_Json::Value               value;

            traversal_child(m_root, get_nodedata, (void *)&value);
#if 0
            lock_guard<mutex> locker(_mutex);
            string sibling = (*this)[m_root].child;
            int i = 0;
            while (!sibling.empty())
            {
                nodedata_t *nodedata = (nodedata_t *)(*this)[sibling].data;
                if (!nodedata)
                {
                    continue;
                }

                value[i]["code"]   = sibling;
                value[i]["name"]   = nodedata->name;
                value[i]["host"]   = nodedata->host;
                value[i]["port"]   = nodedata->port;
                value[i]["status"] = nodedata->status;
                value[i]["subnum"] = child_num(sibling);

                sibling = (*this)[sibling].sibling;
                i++;
            }
#endif
            AutoLock locker(m_uimtx);
            if (m_ui)
            {
                m_ui->onUpdate(SHL_Json::writeString(writer, value), NULL, 0);
            }
        }    
		if (m_puui != NULL) 
		{			
			SHL_Json::StreamWriterBuilder puwriter;
			SHL_Json::Value               puvalue;

			traversal_child(m_puuiAddrcode, get_nodedata, (void *)&puvalue);

			m_puui->onUpdate(SHL_Json::writeString(puwriter, puvalue), NULL, 0);			
		}
    } while (0);

    if (realGuMgr)
    {
        realGuMgr->updateUi();
    }

    if (recordGuMgr)
    {
        recordGuMgr->updateUi();
    }
}


void
PuTree::onTimer()
{
#ifdef TIMERITSELF
    uint64_t cnt = 0;
    uint32_t len = 25;

    while (m_timerThread.active())
    {
        {
            AutoLock locker(_mtx);
            for (list<Pu *>::const_iterator i = begin(); i != end(); ++i)
            {
                (*i)->onTimer();
            }
        }

        sys->sleep(len);
        cnt += len;
    }
#else
    traversal_child(m_root, timer_nodedata, this);


    uint64_t current = sys->currentTimeMillis();

    //update channel status
    static uint64_t slastupdate = current;
    if (current - slastupdate > 3000)
    {
        traversal(m_root, update_status, this);
        slastupdate = current;
    }
    //end update channel status


    /* TODO:  <08-05-18, yourname> */
    /* timeCfg.doTime */

    if (ntpTime->doTime())
    {
        do
        {
            
            if (0 == m_lastSync)
            {
                datetime_s curTime = { 0 };
                string strCurTime = sys->strftime();
                sys->strptime(strCurTime, curTime);
                traversal_child(m_root, timer_synctime, &curTime);
                m_lastSync = current;
                break;
            }

            if(current - m_lastSync < ntpTime->timeInterval() * 60 * 1000) 
            {
                break;
            }

            datetime_s curTime = { 0 };
            string strCurTime = sys->strftime();
            sys->strptime(strCurTime, curTime);
            traversal_child(m_root, timer_synctime, &curTime);
            m_lastSync = current;
        }
        while (0);
    }

#endif
}

void *
PuTree::threadEntry(ThreadInfo * info)
{
    PuTree * mgr = (PuTree*)(info->data);

    switch (info->id)
    {
    case T_TIMER:
        mgr->onTimer();
        break;

    default:
        break;
    }

    return NULL;
}
