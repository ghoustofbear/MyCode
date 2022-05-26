#ifndef _GWA_GU_H_
#define _GWA_GU_H_

#include "gwb_gu.h"

class GwaRealGu : public GwbRealGu
{
public:
	GwaRealGu(Pu *pu, string addrcode, int streamType, int stream);
	virtual ~GwaRealGu();

protected:

private:

};

#endif
