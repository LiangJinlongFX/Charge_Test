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
    rt_thread_idle_sethook(cpu_usage_idle_hook);
		/* 初始化互斥锁 */
	rt_mutex_init(&mutex,"mutex", RT_IPC_FLAG_FIFO);
	rt_sem_init(&sem, "sem", 0, RT_IPC_FLAG_FIFO);
		
	// 创建静态线程
	rt_thread_init(&led0_thread,               	  //线程控制块
								 "led0",                     	  //线程名字，在shell里面可以看到
								 led0_thread_entry,          	  //线程入口函数
								 RT_NULL,                    	  //线程入口函数参数
								 &rt_led0_thread_stack[0],      //线程栈起始地址
								 sizeof(rt_led0_thread_stack),  //线程栈大小
								 3,                          	  //线程的优先级
								 20);                           //线程时间片
														 
	rt_thread_startup(&led0_thread);           	  //启动线程led0_thread，开启调度
				   
    // 创建静态线程                          
	rt_thread_init(&led1_thread,                  //线程控制块
				   "led1",                        //线程名字，在shell里面可以看到
				   led1_thread_entry,             //线程入口函数
				   RT_NULL,                       //线程入口函数参数
				   &rt_led1_thread_stack[0],      //线程栈起始地址
				   sizeof(rt_led1_thread_stack),  //线程栈大小
				   3,                             //线程的优先级
				   20);                           //线程时间片     

	//rt_thread_startup(&led1_thread);              //启动线程led1_thread，开启调度  	

    // 创建静态线程                          
	rt_thread_init(&usb_thread,                  //线程控制块
				   "usb",                        //线程名字，在shell里面可以看到
				   usb_thread_entry,             //线程入口函数
				   RT_NULL,                       //线程入口函数参数
				   &rt_usb_thread_stack[0],      //线程栈起始地址
				   sizeof(rt_usb_thread_stack),  //线程栈大小
				   3,                             //线程的优先级
				   5);                           //线程时间片     

	rt_thread_startup(&usb_thread);              //启动线程led1_thread，开启调度  	

    // 创建静态线程                          
	rt_thread_init(&HMIMonitor_thread,                  //线程控制块
				   "HMIMonitor",                        //线程名字，在shell里面可以看到
				   HMIMonitor_thread_entry,             //线程入口函数
				   RT_NULL,                        //线程入口函数参数
				   &rt_HMIMonitor_thread_stack[0],      //线程栈起始地址
				   sizeof(rt_HMIMonitor_thread_stack),  //线程栈大小
				   3,                              //线程的优先级
				   5);                             //线程时间片     

	rt_thread_startup(&HMIMonitor_thread);              //启动线程led1_thread，开启调度  
	
    // 创建静态线程                          
	rt_thread_init(&Master_thread,                  //线程控制块
				   "Master",                        //线程名字，在shell里面可以看到
				   Master_thread_entry,             //线程入口函数
				   RT_NULL,                        //线程入口函数参数
				   &rt_Master_thread_stack[0],      //线程栈起始地址
				   sizeof(rt_Master_thread_stack),  //线程栈大小
				   2,                              //线程的优先级
				   5);                             //线程时间片     

	rt_thread_startup(&Master_thread);              //启动线程led1_thread，开启调度 
					 
					 return 0;
}



