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
#include "mcp3421.h"
#include "myiic.h"
#include "dac.h"
#include "ff.h"  
#include "exfuns.h"
#include "malloc.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main()
{
	u32 res;
	u8 sd_res;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);
	uart_init(115200);
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	LED_Init();
	IIC_Init();
	Dac1_Init();
	exfuns_init();
	LED0=0;
	sd_res=SD_Init();
	res=f_mount(fs[0],"0:",1);
	printf("sd_res=%d\n",res);
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	Dac1_Set_Vol(1700);
	while(1)
	{
//		while(!USART_RX_STA);
//		USART_RX_STA=0;
//		for(a=0;a<=UASRT1_RX_BUFFER_LEN-4;a++)
//			printf("%x",USART_RX_BUF[a]);
//		USART1_0XFF_FLAG=0;
//		UASRT1_RX_BUFFER_LEN=0;
//		printf("\r\n");
		res=Get_Mcp3421_18BAdc();
		printf("res=%d\n",res);
		delay_ms(300);
	}
}


