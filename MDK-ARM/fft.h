#ifndef FFT_H_
#define FFT_H_

#include "stdint.h"
#include "arm_math.h"

void fast_rfft_64(uint32_t* adc_input,float32_t* fft_output);

#endif
