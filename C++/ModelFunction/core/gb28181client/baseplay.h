#ifndef  _BASE_PLAY_H_
#define  _BASE_PLAY_H_

#include "rtptriggerlistener.h"

class BasePlay :public RTPTriggerListener
{
public:
    virtual void        changeWinSize()=0;
};

#endif
