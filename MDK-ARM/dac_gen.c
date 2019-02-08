#include "dac_gen.h"
#include "math.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_dac.h"
#include "dac.h"

int16_t plot_table[500] = {0};
void HAL_DAC_ConvCpltCallbackCh1 (DAC_HandleTypeDef * hdac);


/*
	此文件用于测试STM32H7系列DAC的任意波发生功能
	调整基本定时器的溢出率为50K 以为dac提供时基 此时确定dac刷新率50k
	开启dac输出缓冲
	
	20us一次的更新周期 假设打表周期为10ms则 需要存储int16类型数据500个

	DAC在DMA模式下 DMA运行于循环模式中
	
	进度： DAC时间基准刷新率50k    搞定
	       500Hz + 1200Hz打表程序  搞定
				 初始化函数              搞定
				 开始函数                搞定
*/



void dac_gomid(void)  //dac回到2048的位置
{
	HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)2048,sizeof(uint32_t),DAC_ALIGN_12B_R);
}

void dac_run(int16_t* addr)
{
	HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)addr,500,DAC_ALIGN_12B_R);
}


//void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef * hdac)
//{
//	if(hdac->Instance == DAC1)
//	{
//		HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)plot_table,500,DAC_ALIGN_12B_R);
//	}
//}


/*
	采用12bitR对齐模式
*/
int16_t* dac_plot(int16_t *addr,uint16_t lenth,double timebase)
{
	uint16_t plot_counter = 0;
	
	for(plot_counter = 0;plot_counter < lenth;plot_counter ++)
	{
		addr[plot_counter] += 2048;            //直流偏置电压
		addr[plot_counter] += (int16_t)(500*sin((double)plot_counter*timebase*3142.0));   //500Hz正弦信号
		addr[plot_counter] += (int16_t)(500*sin((double)plot_counter*timebase*7540.5));   //1200Hz正弦信号
	}
	
	
	
	return addr;
}
