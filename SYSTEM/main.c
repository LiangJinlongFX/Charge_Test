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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main()
{
	
	/* set idle thread hook */
    //rt_thread_idle_sethook(cpu_usage_idle_hook);
		/* 初始化事件对象 */
	rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
		
	//创建线程1 
    led0_thread = rt_thread_create("led0", //线程1的名称是t1 
							led0_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							2, //线程优先级
							20);//时间片tick

//	if (led0_thread != RT_NULL) //如果获得线程控制块，启动这个线程
//			rt_thread_startup(led0_thread);
				   
	//创建线程1 
    led1_thread = rt_thread_create("led1", //线程1的名称是t1 
							led1_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							2, //线程优先级
							20);//时间片tick

//	if (led1_thread != RT_NULL) //如果获得线程控制块，启动这个线程
//			rt_thread_startup(led1_thread); 

	//创建线程1 
    usb_thread = rt_thread_create("usb", //线程1的名称是t1 
							usb_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							2, //线程优先级
							20);//时间片tick

	if (usb_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(usb_thread);

	//创建线程1 
    HMIMonitor_thread = rt_thread_create("HMIMonitor", //线程1的名称是t1 
							HMIMonitor_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick

	if (HMIMonitor_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(HMIMonitor_thread); 
	
	//创建线程1 
    Master_thread = rt_thread_create("Master", //线程1的名称是t1 
							Master_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick

	if (Master_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(Master_thread); 
	
	return 0;
}



