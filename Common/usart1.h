#ifndef __USART1_H
#define __USART1_H
/**
  ******************************************************************************
  * @file    STM32F407DEMO/usart1.c 
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief   
  ******************************************************************************
  * @attention
	* �����Ƿ�֧��UCOS�궨��
	* ֧����Ӧ��ͬƵ���µĴ��ڲ���������.
	*	�����˶�printf��֧��
	* �����˴��ڽ��������.
	* ������printf��һ���ַ���ʧ��bug
	* �޸Ĵ��ڳ�ʼ��IO��bug
	* �޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
	* ������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
	* �޸���EN_USART1_RX��ʹ�ܷ�ʽ
	* �����˶�UCOSII��֧��
  ******************************************************************************
**/

/* include-------------------------------------------------- */
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

/* define--------------------------------------------------- */
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 				1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         		//����״̬���
extern u8 UASRT1_RX_BUFFER_LEN;  //��Ч�ַ�����
extern u8 USART1_0XFF_FLAG;	//oxff����

void uart1_init(u32 bound);
void HMI_Send_TXEND(void);

#endif


