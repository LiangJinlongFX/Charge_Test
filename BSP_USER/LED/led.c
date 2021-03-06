/**
  * @file    led.c
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 GPIO控制LED
  **/
#include "led.h"
#include "sys.h"

/**
 * LED引脚初始化
 * @param   
 * @return 
 * @brief 
 **/
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//使能GPIOC时钟

  //GPIO初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);                         //初始化GPIO
	
	/* 默认为点亮状态 */
	GPIO_SetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);   	
}