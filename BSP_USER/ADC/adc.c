/**
  * @file    adc.c
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 STM32ADC外设驱动
	* 使用通道	IN1/IN8/IN6/IN7/IN9/内部温度传感器
  **/
#include "adc.h"
#include "delay.h"
#include "dma.h"
#include "debug.h"
#include "stm32f4xx.h"
#include "rtthread.h"

__IO uint32_t ADC12ConvertedValue[ADC12_BUFFER_SIZE];	//ADC_DMA缓存数组
uint8_t ADC12_Convert_Flag=0;

/**
 * 定时器3初始化
 * @param   
 * @return 
 * @brief 
 **/
static void TIM3_Config(void) 
{  
    TIM_TimeBaseInitTypeDef timer_init_structure;
	  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //使能TIM3时钟    
    TIM_DeInit(TIM3);                                               //复位TIM3    
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //系统时钟
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //向上计数模式  
    timer_init_structure.TIM_Period = ( 2000000/(ADC_SAMPLE_FREQUENCY*ADC12_BUFFER_SIZE) )-1;                        
    timer_init_structure.TIM_Prescaler = 36 - 1;                      //计数时钟预分频
    TIM_TimeBaseInit(TIM3, &timer_init_structure);
  	
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

/**
 * ADC双重采样模式初始化
 * @param   
 * @return 
 * @brief 
 **/
void ADC_DualMode_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef      NVIC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);  //DMA2时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);											//使能GPIOA/B时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 											//使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 											//使能ADC2时钟

	/* 配置 DMA2 Stream0 */
	DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  						//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CCR_ADDRESS;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr =  (uint32_t)&ADC12ConvertedValue;					//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;			//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = ADC12_BUFFER_SIZE;												//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//外设数据长度:32位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;		 //存储器数据长度:32位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;					//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//初始化DMA Stream
	DMA_Cmd(DMA2_Stream0, ENABLE);	//使能DMA流
	
	/* 使能DMA2数据流0发送完成中断 */
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	
	/* 配置DMA2数据流0中断 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* 初始化ADC IO通道 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_6|GPIO_Pin_7;		// 通道1/6/7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				// 通道8/9
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //初始化

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_InjecSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
 
  /* ADC1/ADC2通道配置 */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
	ADC_InitStructure.ADC_NbrOfConversion = 2;
	ADC_Init(ADC2, &ADC_InitStructure);
	
	/* ADC规则通道配置 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 2, ADC_SampleTime_3Cycles);
	
  /* Enable DMA request after last transfer (Multi-ADC mode) */
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
 
	/* 使能ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);
	
	TIM3_Config();

}

/**
 * ADC独立采样模式初始化
 * @param   
 * @return 
 * @brief 
 **/
void ADCIndependent_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	

	//初始化ADC IO通道
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;		// 通道8/9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //初始化
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
}


/**
 * 获得ADC1值[ADC1可多通道采集]
 * @param
 * @return 
 * @brief 
 **/
u8 Get_ADC_DualMode(void)   
{
	u16 j=0;

	TIM_Cmd(TIM3, ENABLE);
	while(!ADC12_Convert_Flag)
	{
		j++;
		if(j>65534)
			return 0;
	}
	if(ADC12_Convert_Flag)
	{
		ADC12_Convert_Flag=0;
	}
	
	return 0;
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
		if(ADC_ID==1);
		//temp_val+=Get_Adc1(ch);
		else
		//temp_val+=Get_Adc2(ch);	
		delay_ms(5);
	}
	return temp_val/times;
}

/**
 * 
 * @param   
 * @return 
 * @brief 
 **/
void DMA2_Stream0_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);	//清除DMA中断标志
		//DMA_Cmd(DMA2_Stream0, ENABLE);
		TIM_Cmd(TIM3, DISABLE);
		ADC12_Convert_Flag=1;
	}
}







