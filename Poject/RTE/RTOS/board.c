/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 */
#include <rthw.h>
#include <rtthread.h>

// rtthread tick configuration
// 1. include header files
// 2. configure rtos tick and interrupt
// 3. add tick interrupt handler

// rtthread tick configuration
// 1. include some header file as need
#include <stm32f4xx.h>

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define HEAP_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define HEAP_BEGIN    (&__bss_end)
#endif

#define SRAM_SIZE         8
//#define SRAM_END          (0x20000000 + SRAM_SIZE * 1024)
#define SRAM_END          (HEAP_BEGIN + SRAM_SIZE * 1024)


/**
 * This function will initial STM32 board.
 */

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
#include "usb_app.h"
 
void rt_hw_board_init()
{    
	// rtthread tick configuration
	// 2. Configure rtos tick and interrupt
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
	delay_init(168);		//初始化延时函数
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart1_init(9600);	//初始化串口1 HMI用
	uart2_init(115200);	//提前初始化串口2以输出初始化状态
	LED_Init();					//初始化指示灯
	USART2_printf("================================\r\n");
	if(SD_Init())  USART2_printf("SD_Init_Error!\r\n");
	else USART2_printf("SD_Init_OK!\r\n");
	//My_RTC_Init();
	IIC_Init();
	USART2_printf("IIC_Init_OK!\r\n");
	Dac1_Init();
	USART2_printf("DAC1_Init_OK!\r\n");
	USART2_printf("================================\r\n");
	
//	usbapp_init();
//	usbapp_mode_set(USBD_MSC_MODE);
		
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)SRAM_END);
#endif
}

// rtthread tick configuration
// 3. add tick interrupt handler 
 void SysTick_Handler(void)
 {
 	/* enter interrupt */
 	rt_interrupt_enter();
 
 	rt_tick_increase();
 
 	/* leave interrupt */
 	rt_interrupt_leave();
 }
