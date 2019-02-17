#ifndef FFT_H_
#define FFT_H_

#include "stdint.h"
#include "arm_math.h"

void fast_rfft_64(int16_t* adc_input,float32_t* fft_output);

#endif
