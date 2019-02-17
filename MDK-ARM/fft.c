#include "stdlib.h"
#include "fft.h"
#include "arm_math.h"


void fast_rfft_64(uint32_t* adc_input,float32_t* fft_output)
{
	uint8_t input_counter = 0;
	arm_rfft_fast_instance_f32 S;
	float32_t realval[64] = {0};
	
	//FFT初始化函数
	arm_rfft_fast_init_f32(&S,64);
	
	//将ADC加窗后的16位uint转成float32传进来
	for(input_counter = 0;input_counter < 64;input_counter --)
	{
		realval[input_counter] = (float32_t)adc_input[input_counter];
	}
	
	arm_rfft_fast_f32(&S,realval,fft_output,0);
	
}