#include "adc.h"
#include "delay.h"		 
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


//初始化ADC															   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); //使能ADC2时钟

  //初始化ADC1通道1 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA5 通道1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

	//初始化ADC1通道8 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA5 通道8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

  //初始化ADC2通道6 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PA6 通道6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  

  //初始化ADC2通道7 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA7 通道7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化 

  //初始化ADC2通道9 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PB1 通道9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

	ADC_TempSensorVrefintCmd(ENABLE);//使能内部温度传感器
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC2复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//复位结束	
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;											//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 			//DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;										//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);																			//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;												//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;																	//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;														//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;																		//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	ADC_Init(ADC2, &ADC_InitStructure);//ADC初始化
		
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	
	ADC_Cmd(ADC2, ENABLE);//开启AD转换器	

}				  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc1(u8 ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return (ADC_GetConversionValue(ADC1)&0xfff);	//返回最近一次ADC1规则组的转换结果
}
u16 Get_Adc2(u8 ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC2);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC2规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ADC_ID,u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		if(ADC_ID==1)
		temp_val+=Get_Adc1(ch);
		else
		temp_val+=Get_Adc2(ch);	
		delay_ms(5);
	}
	return temp_val/times;
} 
	 
/*
 * 返回值:温度值(扩大了100倍,单位:℃.)
 */
short Get_Temprate(void)
{
	u32 adcx;
	short result;
 	double temperate;
	adcx=Get_Adc_Average(1,ADC_Channel_16,10);	//读取通道16内部温度传感器通道,10次取平均
	temperate=(float)adcx*(3.3/4096);		//电压值
	temperate=(temperate-0.76)/0.0025 + 25; //转换为温度值 
	result=temperate*=100;					//扩大100倍.
	return result;
}








