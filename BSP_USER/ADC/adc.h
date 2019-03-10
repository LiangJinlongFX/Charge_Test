#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 



#define ADC_CCR_ADDRESS ((uint32_t)0x40012308)
#define ADC12_BUFFER_SIZE 120
#define ADC_SAMPLE_FREQUENCY 10

extern uint8_t ADC12_Convert_Flag;
extern __IO uint32_t ADC12ConvertedValue[ADC12_BUFFER_SIZE];	//ADC_DMA��������

void ADC_DualMode_Init(void);
void ADCIndependent_Init(void);
u8 Get_ADC_DualMode(void);
u16 Get_Adc_Average(u8 ADC_ID,u8 ch,u8 times);	//�õ�ĳ��ͨ����������������ƽ��ֵ

#endif















