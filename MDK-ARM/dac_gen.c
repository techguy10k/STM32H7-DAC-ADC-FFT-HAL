#include "dac_gen.h"
#include "math.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_dac.h"
#include "dac.h"

/*
	int16类型的数据 意思是DMA传输模式应设置为halfword模式
*/

int16_t Plot_Bank0[250] = {0};
int16_t Plot_Bank1[250] = {0};


DACStatus_Typedef DAC_Status = {0};
/*
	callback函数就是HAL库的中断服务函数
*/
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac);


/*
	此文件用于测试STM32H7系列DAC的任意波发生功能 此branch的波形产生采用连续生成方式
  此branch的波形不需要整数倍的打表
	此branch需要两个bank A 以及bank B的位置 先计算好两个bank的打表数据
	然后开启bankA的DAC 当bankA完成转换触发DMA中断时开启bankB的转换 同时根据时基update bankA内容
	如此往复
	
	此branch需要DAC_DMA工作于正常模式而非循环模式 详细DAC_DMA配置交给CubeMX 请正确配置CubeMX以便正确初始化
	
	调整基本定时器TIM6的溢出率为50K 以为dac提供时基 此时确定dac刷新率50k
	开启dac输出缓冲 ok
	
	20us一次的更新周期 假设打表周期为10ms则 需要存储int16类型数据500个

	DAC在DMA模式下 DMA运行于循环模式中
	
	进度： DAC时间基准刷新率50k    搞定
	       500Hz + 1200Hz打表程序  搞定
				 初始化函数              搞定
				 开始函数                搞定
*/



/*
	函数名称：dac_gomid
	函数作用：将DAC输出设置到中点电位(以DMA模式调用DAC)
	函数输入：DAC_Handle结构体变量 指向一确定DAC
	函数输出：无
	函数说明：由于使用了DMA 必须从储存中的一个位置向外设送数据 所以这里定义了一个code区位置
*/
void dac_gomid(DAC_HandleTypeDef * dac)  //dac回到2048的位置
{
	const uint16_t midval = 2048;
	
	if(dac->Instance == DAC1)
	{
		HAL_DAC_Start_DMA(dac,DAC_CHANNEL_1,(uint32_t*)&midval,0x01,DAC_ALIGN_12B_R);
	}
	else
	{
		return ;
	}
}

/*
	函数名称：dac_run
	函数作用：以DMA的方式根据生成的DAC波形开启DAC输出 DAC采样率50KHz
	函数输入：int16类型的DAC数组地址
	函数输出：无
	函数说明：此函数为master branch的legacy函数 在此工程中不使用
*/
void dac_run(int16_t* addr)
{
	HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)addr,500,DAC_ALIGN_12B_R);
}


/*
	函数名称：HAL_DAC_ConvCpltCallbackCh1
	函数作用：DAC的DMA传输完成中断函数 完成一次DMA传输调用
	函数输入：DAC_Handle结构变量 此变量由底层传递上来
	函数输出：无
	函数说明：此函数为master branch的legacy函数 在此工程中不使用
*/
//void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef * hdac)
//{
//	if(hdac->Instance == DAC1)
//	{
//		HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)plot_table,500,DAC_ALIGN_12B_R);
//	}
//}



/*
	函数名称：HAL_DAC_ConvCpltCallbackCh1
	函数作用：DAC的DMA传输完成中断函数 完成一次DMA传输调用
	函数输入：DAC_Handle结构变量 此变量由底层传递上来
	函数输出：无
	函数说明：
*/
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	if(hdac->Instance == DAC1)
	{
		if(DAC_Status.DAC_Status == DAC_Status_Idle)
		{
			dac_gomid(&hdac1);
		}
		else if(DAC_Status.DAC_Status == DAC_Status_Busy)
		{
			if(DAC_Status.DAC_Plotting_Bank == DAC_Plotting_Bank_Bank0)
			{
				HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)Plot_Bank1,250,DAC_ALIGN_12B_R);
				DAC_Status.DAC_Plotting_Bank = DAC_Plotting_Bank_Bank1;
				dac_plot_countinue(Plot_Bank0,250,0.00002,&DAC_Status);
				
			}
			else if(DAC_Status.DAC_Plotting_Bank == DAC_Plotting_Bank_Bank1)
			{
				HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)Plot_Bank0,250,DAC_ALIGN_12B_R);
				DAC_Status.DAC_Plotting_Bank = DAC_Plotting_Bank_Bank0;
				dac_plot_countinue(Plot_Bank1,250,0.00002,&DAC_Status);
			}
		}
		
	}
}




/*
	函数名称：dac_plot
	函数作用：DAC的打图函数 按照给信号与时基打图
	函数输入：DAC图首地址 DAC图长度 DAC打图时的时基
	函数输出：打过以后图的首地址
	函数说明：此函数为master branch的legacy函数 在此工程中不使用
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



/*
	函数名称：dac_plot_countinue
	函数作用：DAC的连续打图函数 按照给的信号与时基打图
	函数输入：DAC图首地址 DAC图长度 DAC打图时的时基 DAC_Status结构体(DAC状态结构体)
	函数输出：打过以后图的首地址
	函数说明：此函数不会清零打点计数，同时打点计数由DAC_Status提供
*/
int16_t* dac_plot_countinue(int16_t *addr,uint16_t lenth,double timebase,DACStatus_Typedef* Status)
{
	uint16_t plot_counter;
		
	for(plot_counter = 0;plot_counter < lenth;plot_counter ++)
	{
		addr[plot_counter] = 2048;            //直流偏置电压 小心这玩意坑你
		addr[plot_counter] += (int16_t)(500*sin((double)(Status->DAC_Plot_Counter)*timebase*3142.0));   //500Hz正弦信号
//		addr[plot_counter] += (int16_t)(500*sin((double)(Status->DAC_Plot_Counter)*timebase*7540.5));   //1200Hz正弦信号
		Status->DAC_Plot_Counter ++;
	}
	
	
	
	return addr;
}

