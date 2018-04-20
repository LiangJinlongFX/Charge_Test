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
	* 用于接收HLW8032测量数据
	*	HLW8032每隔50ms发送一次数据  每组数据包括11组寄存器,共24字节
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

extern u8 USART3_RX_BUF[100];	//串口3接收缓存数组
extern u8 USART3_RX_Size;			//串口3当前接收的字节数
extern u8 USART3_RX_Flag;			//串口3接收完成标志

void uart3_init(u32 bound);

#endif



