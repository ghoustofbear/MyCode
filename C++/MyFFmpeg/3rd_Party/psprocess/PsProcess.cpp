#include "PsProcess.h"
PsProcess::PsProcess()
{
	m_spslen = 0;
	m_ppslen = 0;
	memset(m_spsbuf, 0, sizeof(m_spsbuf));
	memset(m_ppsbuf, 0, sizeof(m_ppsbuf));

	do
	{
		workspace = (uint8_t*)malloc(1024 * 1024);
	}
	while (NULL == workspace);
	memset(workspace, 0, 1024 * 1024);
    m_block = true;
}

PsProcess::~PsProcess()
{
	if (NULL != workspace)
	{
		free(workspace);
		workspace = NULL;
	}
}


int 
PsProcess::start()
{
    if (m_block == false)
    {
        m_thread.setEntry(threadEntry, this, 0);
        return m_thread.start();
    }
}


void 
PsProcess::stop()
{
    if (m_block == false)
    {
        m_thread.stop();

        AutoLock locker(m_dataQue._mtx);
        while (m_dataQue.size())
        {
            dataInfo* infod = m_dataQue.front();
            delete infod->data;
            delete infod;
            m_dataQue.pop();
        }
    }
    


}

void*
PsProcess::threadEntry(ThreadInfo *arg)
{
	PsProcess *base = (PsProcess*)arg->data;
	while (base->m_thread.active())
	{
		int sz = 0;
		{
            AutoLock locker(base->m_dataQue._mtx);
            sz = base->m_dataQue.size();
		}

		if (0 == sz)
		{
			sys->sleep(10);
			continue;
		}

		{
            AutoLock locker(base->m_dataQue._mtx);            
            dataInfo* infod = base->m_dataQue.front();
            base->process(infod);
            delete infod->data;
            delete infod;
            base->m_dataQue.pop();
            
		}

		sys->sleep(1);
	}
	return NULL;
}

void 
PsProcess::setstreamcb(pfnstreamdatacallback fnstreamcb, void * puser)
{
	m_fnstreamcb = fnstreamcb;
	m_puser      = puser;
}

int
PsProcess::getNextNalu(uint8_t* data, int data_Len ,uint8_t*buff, int &size, int &type)
{
	if (NULL == data || data_Len <= 5 ||NULL == buff)
	{
		return -1;
	}
	int First_Nalu_Index  = -1;
	int Second_Nalu_Index = -1;
	for (int i = 0; i < data_Len - 4; i++)
	{
		if ((0x00 == data[i]) && (0x00 == data[i + 1]) &&
			(0x00 == data[i + 2]) && (0x01 == data[i + 3]))
		{
			if (-1 == First_Nalu_Index)
			{
				First_Nalu_Index = i;
				type = data[i + 4] & 31;
				continue;
			}
			else if (-1 == Second_Nalu_Index)
			{
				//condition 1:more than one nalu
				Second_Nalu_Index = i;
				data += First_Nalu_Index;
				size = Second_Nalu_Index - First_Nalu_Index;
				memcpy(buff, data, size);
				return Second_Nalu_Index;
			}
		}
	}
	if (-1 == First_Nalu_Index)
	{
		//condition 2:no  nlau , as Audio Frame , length 320
		size = data_Len;
		type = -1;//-1 as Audio Frame Type
		memcpy(buff, data, size);
		return data_Len;
	}
	else if (-1 == Second_Nalu_Index)
	{
		//condition 3:only one nalu
		data += First_Nalu_Index;
		size = data_Len - First_Nalu_Index;
		memcpy(buff, data, size);
		return data_Len;
	}
	
}
void 
PsProcess::inputstreamdata_ex(uint8_t*data, int size, int type, uint64_t pts, uint64_t frameTime)
{
	if (NULL == data || size < 5)
	{
		return;
	}
	if (m_beFirstPacket)//在第一个包中计算I帧和P帧的数目判定是否为双场
	{
        int NaluNum = 0;
		for (int i = 0; i < size - 4; i++)
		{
			if ((0x00 == data[i]) && (0x00 == data[i + 1]) &&
				(0x00 == data[i + 2]) && (0x01 == data[i + 3]))
			{
				int ntype = data[i + 4] & 31;
				if (1 == ntype || 5 == ntype)
				{
                    NaluNum++;
				}
			}
		}
        if (NaluNum >= 2)//I帧+P帧>=2
		{
			m_doubleField = 1;//开启双场
		}
		m_beFirstPacket = false;
        NaluNum = 0;
	}
	int nalu_size = -1;
	int nalu_type = -1;
	do 
	{
		int ret = getNextNalu(data, size,workspace, nalu_size, nalu_type);//to get one nalu every time
		if (-1 == ret)
		{
			break;
		}
		else
		{
			data += ret;
			size -= ret;
		}
		dataInfo *info = new dataInfo;
		if (NULL == info)
		{
		    return;
		}
		info->data = new uint8_t[nalu_size];
		if (NULL == info->data)
		{
		    delete info;
		    return;
		}
		memcpy(info->data, workspace, nalu_size);
		info->size = nalu_size;
		info->type = nalu_type;
		info->pts = pts;
        info->frameTime = frameTime;

        if (m_block)
        {
            process(info);
        }
        else
        {
            AutoLock locker(m_dataQue._mtx);
            m_dataQue.push(info);
        }
	} while (true);
}
void 
PsProcess::inputstreamdata(uint8_t*data, int size, int type, uint64_t pts, uint64_t frameTime)
{
	if (NULL == data || size < 5)
	{
		return;
	}
	dataInfo *info = new dataInfo;
	if (NULL == info)
	{
		delete info;
		return;
	}

	info->data = new uint8_t[size];
	if (NULL == info->data)
	{
		delete[] info->data;
		delete info;
		return;
	}
	memcpy(info->data, data, size);
	info->size = size;
	info->type = type;
	info->pts = pts;
    info->frameTime = frameTime;
	{
        AutoLock locker(m_dataQue._mtx);
        m_dataQue.push(info);
	}
}
