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

//������ƿ�
static struct rt_mailbox mb;
//���������ƿ�
static rt_mutex_t mutex = RT_NULL;
//�ź������ƿ�
static struct rt_semaphore sem;
/* �¼����ƿ� */
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

