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

//邮箱控制块
static struct rt_mailbox mb;
//互斥锁控制块
static rt_mutex_t mutex = RT_NULL;
//信号量控制块
static struct rt_semaphore sem;
/* 事件控制块 */
static struct rt_event event;


static rt_thread_t led0_thread=RT_NULL;
static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t usb_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;


extern void led0_thread_entry(void* parameter);
extern void usb_thread_entry(void* parameter);
extern void led1_thread_entry(void* parameter);
extern void HMIMonitor_thread_entry(void* parameter);
extern void Master_thread_entry(void* parameter);
void cpu_usage_idle_hook(void);

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);


#endif

