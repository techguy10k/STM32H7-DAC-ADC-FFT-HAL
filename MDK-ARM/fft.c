#include "stdlib.h"
#include "stdio.h"
#include "fft.h"
#include "arm_math.h"

float magoutput[32] = {0};


void fast_rfft_64(int16_t* adc_input,float32_t* fft_output)
{
	uint16_t input_counter = 0;
	float32_t realval[64] = {0};
	arm_rfft_fast_instance_f32 S;
	
	//FFT初始化函数
	arm_rfft_fast_init_f32(&S,64);
	
	//将ADC加窗后的16位uint转成float32传进来
	for(input_counter = 0;input_counter < 64;input_counter ++)
	{
		realval[input_counter] = adc_input[input_counter];
	}
	
	arm_rfft_fast_f32(&S,realval,fft_output,0);
	
	arm_cmplx_mag_f32(fft_output,magoutput,64);
		
}