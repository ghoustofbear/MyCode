#include "PsProcessApi.h"
#include "PsPacker.h"
#include "ps_parser.h"
#include "PsProcess.h"

PSPROCESS_API void* 
createpsinstance(enPsType type)
{
	PsProcess *base = NULL;
	if (type == PS_PACK)
	{
		base = new PsPacker();
		base->flag = 0;//pack
	}
	else if (type == PS_UNPACK)
	{
		base = new PsParser();
		base->flag = 1;//unpack
	}
	base->start();
	return base;
}

PSPROCESS_API void  
destorypsinstance(void *obj)
{
	if (obj != NULL)
	{
		PsProcess * base = (PsProcess*)obj;
		base->stop();
		delete base;
		obj = NULL;
	}
}

PSPROCESS_API void  
setpsstreamcallback(void *obj, pfnstreamdatacallback fnstreamcb, void *puser)
{
	PsProcess * base = (PsProcess*)obj;
	if (base)
	{
		base->setstreamcb(fnstreamcb, puser);
	}
}

PSPROCESS_API void  
inputstreamdata(void *obj, uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime)
{
	PsProcess * base = (PsProcess*)obj;
	if (base)
	{
		if (0 == base->flag)//pack
		{
			base->inputstreamdata_ex(data, size, type, pts, frameTime);
		}
		else if (1 == base->flag)//unpack
		{
			base->inputstreamdata(data, size, type, pts, frameTime);
		}
	}
}

PSPROCESS_API void  
setPtsFlag(void*obj,bool flag)
{
	PsProcess * base = (PsProcess*)obj;
	if (base)
	{
		base->m_ptsflag = flag;
	}
}
