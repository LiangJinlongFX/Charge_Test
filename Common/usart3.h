#ifndef __USART3_H
#define __USART3_H
/**
  ******************************************************************************
  * @file    STM32F407DEMO/usart3.h 
  * @author  Liang
  * @version V1.0.0
  * @date    2018-1-22
  * @brief   
  ******************************************************************************
  * @attention
	* ���ڽ���HLW8032��������
	*	HLW8032ÿ��50ms����һ������  ÿ�����ݰ���11��Ĵ���,��24�ֽ�
	* Stage REG 1Byte
	* Check REG 1Byte
	* Voltage parameter REG 3Byte
	* Voltage REG 3Byte
	* Current Parameter REG 3Byte
	* Current REG 3Byte
	* Power parameter REG 3Byte
	* Power REG 3Byte
	* Data Updata REG 1Byte
	* PE REG 2Byte
	* CheckSum REG 1Byte
  ******************************************************************************
**/

#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

extern u8 USART3_RX_BUF[100];	//����3���ջ�������
extern u8 USART3_RX_Size;			//����3��ǰ���յ��ֽ���
extern u8 USART3_RX_Flag;			//����3������ɱ�־

void uart3_init(u32 bound);

#endif



