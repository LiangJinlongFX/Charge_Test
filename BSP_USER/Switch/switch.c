/**********************************************************************************
* 文件名称: led.c                                                                *
* 文件简述：用于开关的IO口                                                            *
* 创建日期：2015.10.03                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：用于开关的IO口初始化                                                    * 
**********************************************************************************
//适用于实验室开发板LED接口
*********************************************************************************/
#include "sys.h"

//LED对应IO初始化
void Switch_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);//使能GPIOD时钟

  //PG13、PG14和PG15初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;         //LED0、LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);                         //初始化GPIO
	
	GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1);   	
}