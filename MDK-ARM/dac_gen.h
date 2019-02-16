#ifndef DAC_GEN_H_
#define DAC_GEN_H_

#include "stdint.h"
#include "dac.h"


typedef struct
{
	uint8_t DAC_Status;
	uint8_t DAC_Plotting_Bank;
	int32_t DAC_Plot_Counter;
}DACStatus_Typedef;

#define DAC_Status_Idle 0
#define DAC_Status_Busy 1

#define DAC_Plotting_Bank_Bank0 0
#define DAC_Plotting_Bank_Bank1 1


void dac_run(int16_t* addr);
void dac_gomid(DAC_HandleTypeDef * dac);
int16_t* dac_plot(int16_t *addr,uint16_t lenth,double timebase);
int16_t* dac_plot_countinue(int16_t *addr,uint16_t lenth,double timebase,DACStatus_Typedef* Status);
int16_t* han_win(int16_t* addr,uint16_t lenth);


#endif