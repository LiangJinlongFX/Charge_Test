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
#include "usart2.h"
#include <rtthread.h>
#include "CSV_Database.h"
#include "rtc.h"
#include "app.h"
#include "HMI.h"
#include <rthw.h>
#include "ff.h"
#include "exfuns.h"
#include <stdlib.h>
#include <stdio.h>
#include "usart3.h"
#include "dma.h"
#include "Data_Math.h"
#include "hlw8032.h"

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
	u8 i;
	u8 res;
	float AC_V;
//	
//	TestData_Type Date;
//	
//	Date.Vout_Max=5.17;
//	Date.Vout_Min=4.98;
//	Date.Cout_Max=2.10;
//	Date.Cout_short=0.056;
//	Date.Noload_Power=0.078;
//	Date.Over_Current_Protection=1;
//	Date.Efficiency=89;
//	Date.Ripple_Voltage=0.067;
//	f_mount(&fat,"0",1);
	
//	res=Creat_NewBatchDir("123");
//	
//	res=Test_WriteData(Date,"2");
////	res=Creat_FileHeader("123.csv");
//	rt_kprintf("res=%d\r\n",res);
//	f_mount(NULL,"0",1);
	while(1)
	{
		if(USART3_RX_Flag)
		{
			USART3_RX_Flag=0;
			res=UsartToStruct();
			AC_V=((float)default_VoltageparameterREG/(float)HLW8032REG_Structure.VoltageREG_Val)*V_K1;
			printf("V=%f\r\n",AC_V);
			AC_V=((float)default_CurrentParameterREG/(float)HLW8032REG_Structure.CurrentREG_Val)*C_K2;
			printf("C=%f\r\n",AC_V);
			USART3_RX_Size=0;
		}
	}
	Main_entry();
	return 0;
}


