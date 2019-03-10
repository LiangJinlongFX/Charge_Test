/**
  * @file    adc.c
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 STM32ADC��������
	* ʹ��ͨ��	IN1/IN8/IN6/IN7/IN9/�ڲ��¶ȴ�����
  **/
#include "adc.h"
#include "delay.h"
#include "dma.h"
#include "debug.h"
#include "stm32f4xx.h"
#include "rtthread.h"

__IO uint32_t ADC12ConvertedValue[ADC12_BUFFER_SIZE];	//ADC_DMA��������
uint8_t ADC12_Convert_Flag=0;

/**
 * ��ʱ��3��ʼ��
 * @param   
 * @return 
 * @brief 
 **/
static void TIM3_Config(void) 
{  
    TIM_TimeBaseInitTypeDef timer_init_structure;
	  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //ʹ��TIM3ʱ��    
    TIM_DeInit(TIM3);                                               //��λTIM3    
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //ϵͳʱ��
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //���ϼ���ģʽ  
    timer_init_structure.TIM_Period = ( 2000000/(ADC_SAMPLE_FREQUENCY*ADC12_BUFFER_SIZE) )-1;                        
    timer_init_structure.TIM_Prescaler = 36 - 1;                      //����ʱ��Ԥ��Ƶ
    TIM_TimeBaseInit(TIM3, &timer_init_structure);
  	
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

/**
 * ADC˫�ز���ģʽ��ʼ��
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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);  //DMA2ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);											//ʹ��GPIOA/Bʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 											//ʹ��ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 											//ʹ��ADC2ʱ��

	/* ���� DMA2 Stream0 */
	DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  						//ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CCR_ADDRESS;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr =  (uint32_t)&ADC12ConvertedValue;					//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;			//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = ADC12_BUFFER_SIZE;												//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;		 //�洢�����ݳ���:32λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;					//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//��ʼ��DMA Stream
	DMA_Cmd(DMA2_Stream0, ENABLE);	//ʹ��DMA��
	
	/* ʹ��DMA2������0��������ж� */
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	
	/* ����DMA2������0�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* ��ʼ��ADC IOͨ�� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_6|GPIO_Pin_7;		// ͨ��1/6/7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				// ͨ��8/9
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //��ʼ��

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_InjecSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
 
  /* ADC1/ADC2ͨ������ */
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
	
	/* ADC����ͨ������ */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 2, ADC_SampleTime_3Cycles);
	
  /* Enable DMA request after last transfer (Multi-ADC mode) */
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
 
	/* ʹ��ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);
	
	TIM3_Config();

}

/**
 * ADC��������ģʽ��ʼ��
 * @param   
 * @return 
 * @brief 
 **/
void ADCIndependent_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��
	

	//��ʼ��ADC IOͨ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;		// ͨ��8/9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		 //ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //��ʼ��
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
}


/**
 * ���ADC1ֵ[ADC1�ɶ�ͨ���ɼ�]
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

//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
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
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);	//���DMA�жϱ�־
		//DMA_Cmd(DMA2_Stream0, ENABLE);
		TIM_Cmd(TIM3, DISABLE);
		ADC12_Convert_Flag=1;
	}
}







