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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB״̬
extern vu8 bDeviceState;		//USB���� ���

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
		
	// ������̬�߳�
	rt_thread_init(&led0_thread,               	  //�߳̿��ƿ�
								 "led0",                     	  //�߳����֣���shell������Կ���
								 led0_thread_entry,          	  //�߳���ں���
								 RT_NULL,                    	  //�߳���ں�������
								 &rt_led0_thread_stack[0],      //�߳�ջ��ʼ��ַ
								 sizeof(rt_led0_thread_stack),  //�߳�ջ��С
								 3,                          	  //�̵߳����ȼ�
								 20);                           //�߳�ʱ��Ƭ
														 
	rt_thread_startup(&led0_thread);           	  //�����߳�led0_thread����������
				   
    // ������̬�߳�                          
	rt_thread_init(&led1_thread,                  //�߳̿��ƿ�
				   "led1",                        //�߳����֣���shell������Կ���
				   led1_thread_entry,             //�߳���ں���
				   RT_NULL,                       //�߳���ں�������
				   &rt_led1_thread_stack[0],      //�߳�ջ��ʼ��ַ
				   sizeof(rt_led1_thread_stack),  //�߳�ջ��С
				   3,                             //�̵߳����ȼ�
				   20);                           //�߳�ʱ��Ƭ     

	rt_thread_startup(&led1_thread);              //�����߳�led1_thread����������  	

    // ������̬�߳�                          
	rt_thread_init(&usb_thread,                  //�߳̿��ƿ�
				   "usb",                        //�߳����֣���shell������Կ���
				   usb_thread_entry,             //�߳���ں���
				   RT_NULL,                       //�߳���ں�������
				   &rt_usb_thread_stack[0],      //�߳�ջ��ʼ��ַ
				   sizeof(rt_usb_thread_stack),  //�߳�ջ��С
				   3,                             //�̵߳����ȼ�
				   5);                           //�߳�ʱ��Ƭ     

	rt_thread_startup(&usb_thread);              //�����߳�led1_thread����������  	

    // ������̬�߳�                          
	rt_thread_init(&HMI_thread,                  //�߳̿��ƿ�
				   "HMI",                        //�߳����֣���shell������Կ���
				   HMI_thread_entry,             //�߳���ں���
				   RT_NULL,                        //�߳���ں�������
				   &rt_HMI_thread_stack[0],      //�߳�ջ��ʼ��ַ
				   sizeof(rt_HMI_thread_stack),  //�߳�ջ��С
				   3,                              //�̵߳����ȼ�
				   5);                             //�߳�ʱ��Ƭ     

	rt_thread_startup(&HMI_thread);              //�����߳�led1_thread����������  
		return 0;
}



