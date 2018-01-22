#ifndef __USART2_H
#define __USART2_H
/**
  ******************************************************************************
  * @file    STM32F407DEMO/usart2.h 
  * @author  Liang
  * @version V1.0.0
  * @date    2018-1-22
  * @brief   
  ******************************************************************************
  * @attention
	* 加入对rt-thread的支持
	*	用于rt-thread msh的串口驱动
  ******************************************************************************
**/

#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

int rt_hw_usart_init(void);
void USART2_printf(char *fmt,...);

#endif


