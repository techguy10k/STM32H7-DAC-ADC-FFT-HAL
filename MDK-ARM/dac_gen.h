#ifndef DAC_GEN_H_
#define DAC_GEN_H_

#include "stdint.h"

void dac_run(int16_t* addr);
void dac_gomid(void);
int16_t* dac_plot(int16_t *addr,uint16_t lenth,double timebase);



#endif