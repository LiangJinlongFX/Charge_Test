/**
  ******************************************************************************
  * @file    STM32F407DEMO/app.h 
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

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include "led.h"
#include "delay.h"
#include "stm32f4xx.h"
/* Private functions ---------------------------------------------------------*/
static struct rt_thread led0_thread;//线程控制块
static struct rt_thread led1_thread;//线程控制块
static struct rt_thread usb_thread;	//线程控制块
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led0_thread_stack[256];//线程栈
static rt_uint8_t rt_led1_thread_stack[256];//线程栈
static rt_uint8_t rt_usb_thread_stack[256];//线程栈


//线程LED0
static void led0_thread_entry(void* parameter)
{
	while(1)
	{
		LED0=~LED0;
		delay_ms(300);	
		delay_us(100000);
		rt_thread_delay(100);
	}
}

//线程USB
static void usb_thread_entry(void* parameter)
{
	//USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	while(1)
	{
	}
}
  
//线程LED1
static void led1_thread_entry(void* parameter)
{
//		u32 total,free;
//		FIL fsrc;	  		//文件1
//		u8 res;
//		char str[20];
//	
//		TestData_Type Temp_struct;
//	
//		Temp_struct.Test_Time[0]=18;
//		Temp_struct.Test_Time[1]=1;
//		Temp_struct.Test_Time[2]=22;
//		Temp_struct.Test_Time[3]=13;
//		Temp_struct.Test_Time[4]=55;
//		Temp_struct.Test_Time[5]=18;
//		Temp_struct.Ripple_Voltage=200;
//		Temp_struct.Vout_Max=200;
//		Temp_struct.Cout_Max=150;
//		Temp_struct.Over_Current_Protection=1;
//		Temp_struct.Over_Voltage_Protection=1;
//		Temp_struct.Poweron_Time=20;
//		Temp_struct.Quick_Charge=3;
//		Temp_struct.Short_Current=1;
//		Temp_struct.Efficiency=67;
//		Temp_struct.Test_Subsequence=9;
		
//		printf("OK");
//		exfuns_init();
//		if(f_mount(fs[0],"0:",1)) printf("挂载失败");
//		printf("挂载成功\r\n");
//		if(exf_getfree("0:",&total,&free)) printf("获取错误");
//		printf("total=%dMB\r\nfree=%dMB\r\n",total>>10,free>>10);
//		
//		///my_itoa(10,str);
//		rt_kprintf("\r\nOK");
//		printf("rse=%d",res);
	while(1)
	{
		LED1=~LED1; 
		delay_ms(10);
		delay_us(50);
		rt_thread_delay(30);
	}
}