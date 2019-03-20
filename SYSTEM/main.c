/**
  ******************************************************************************
  * @file    STM32F407DEMO/main.c 
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "app.h"
#include <rtthread.h>
#include "tlc5615.h"
#include "delay.h"
#include "adc.h"
#include "hlw8032.h"
#include "stdio.h"
#include "switch.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main()
{
	u8 res;
	
	Main_entry();
//	RELAY=1;
//	delay_ms(1000);
//	res=QuickCharge_Induction(0x07);
//	rt_kprintf("res=%x\r\n",res);
//	while(1);

}


