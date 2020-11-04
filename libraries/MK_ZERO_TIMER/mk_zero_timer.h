#include "Arduino.h"


#ifndef MK_ZERO_TIMER_H
#define MK_ZERO_TIMER_H

class MK_TIMER
{
  public:
    MK_TIMER(double int_interval );
    void Setup_Hardware(void);
    
  private:
    
    int period;
   
};





#endif
