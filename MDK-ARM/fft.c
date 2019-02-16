#include "stdlib.h"
#include "fft.h"
#include "arm_math.h"


void fast_rfft_64(uint32_t* adc_input,float32_t* fft_output)
{
	arm_rfft_fast_instance_f32 S;
	
	//FFT初始化函数
	arm_rfft_fast_init_f32(&S,64);
	
}