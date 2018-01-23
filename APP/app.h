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
	* �����rt-thread��֧��
	*	����rt-thread msh�Ĵ�������
  ******************************************************************************
**/

#include <rtthread.h>

static struct rt_thread led0_thread;//�߳̿��ƿ�
static struct rt_thread led1_thread;//�߳̿��ƿ�
static struct rt_thread usb_thread;	//�߳̿��ƿ�
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led0_thread_stack[256];//�߳�ջ
static rt_uint8_t rt_led1_thread_stack[256];//�߳�ջ
static rt_uint8_t rt_usb_thread_stack[256];//�߳�ջ

extern void led0_thread_entry(void* parameter);
extern void usb_thread_entry(void* parameter);
extern void led1_thread_entry(void* parameter);


#endif

