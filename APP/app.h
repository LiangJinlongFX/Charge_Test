#ifndef _APP_H
#define _APP_H
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

#include <rtthread.h>

static struct rt_thread led0_thread;//线程控制块
static struct rt_thread led1_thread;//线程控制块
static struct rt_thread usb_thread;	//线程控制块
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led0_thread_stack[256];//线程栈
static rt_uint8_t rt_led1_thread_stack[256];//线程栈
static rt_uint8_t rt_usb_thread_stack[256];//线程栈

extern void led0_thread_entry(void* parameter);
extern void usb_thread_entry(void* parameter);
extern void led1_thread_entry(void* parameter);


#endif

