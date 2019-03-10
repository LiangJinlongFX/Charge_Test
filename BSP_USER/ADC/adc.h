#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 



#define ADC_CCR_ADDRESS ((uint32_t)0x40012308)
#define ADC12_BUFFER_SIZE 120
#define ADC_SAMPLE_FREQUENCY 10

extern uint8_t ADC12_Convert_Flag;
extern __IO uint32_t ADC12ConvertedValue[ADC12_BUFFER_SIZE];	//ADC_DMA缓存数组

void ADC_DualMode_Init(void);
void ADCIndependent_Init(void);
u8 Get_ADC_DualMode(void);
u16 Get_Adc_Average(u8 ADC_ID,u8 ch,u8 times);	//得到某个通道给定次数采样的平均值

#endif















