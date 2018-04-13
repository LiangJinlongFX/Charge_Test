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
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart1.h"
#include "sdio_sdcard.h"
#include "dac.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usart2.h"
#include <rtthread.h>
#include "CSV_Database.h"
#include "rtc.h"
#include "app.h"
#include "usb_app.h"
#include "HMI.h"
#include <rthw.h>
#include "ff.h"
#include "exfuns.h"
#include <stdlib.h>
#include <stdio.h>

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
	TestData_Type Test_da;
	Test_da.Ripple_Voltage=0.20;
	Test_da.Vout_Max=5.120;
	Test_da.Cout_Max=2.20;
	Test_da.Poweron_Time=89;
	Test_da.Efficiency=78;
	Test_da.Over_Current_Protection=Test_da.Over_Voltage_Protection=Test_da.Quick_Charge=Test_da.Short_Current=1;
	Test_da.Test_Subsequence=6;
	f_mount(&fat,"0",1);
	res=Test_WriteData(Test_da,"2");
	//res=Creat_FileHeader("111.csv");
	rt_kprintf("%d\r\n",res);
	f_mount(NULL,"0",1);
	while(1);
	Main_entry();
	return 0;
}


