#ifndef __LED_H
#define __LED_H

/**
  * @file    led.c 
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 指示灯驱动
  **/

//LED端口定义
#define LED0 PCout(2)	 
#define LED1 PCout(3)
#define LED2 PCout(4)
#define LED3 PCout(5)	

//函数声明
void LED_Init(void);//初始化

#endif
