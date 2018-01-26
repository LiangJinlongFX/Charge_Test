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
extern u8 HMI_Rx_String[100];

/* Private function prototypes -----------------------------------------------*/
//�߳�LED0
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

//�߳�USB
static void usb_thread_entry(void* parameter)
{

	usbapp_init();
	//usbapp_mode_set(USBD_MSC_MODE);
	while(1)
	{
//		rt_kprintf("OK\r\n");
//		if(usbx.bDeviceState&0x01) printf("USB Writing...\r\n");
//		if(usbx.bDeviceState&0x02) printf("USB Reading...\r\n");
//		if(usbx.bDeviceState&0x04) printf("USB Write Err...\r\n");
//		if(usbx.bDeviceState&0x08) printf("USB Read  Err...\r\n");
//		if(usbx.bDeviceState&0x10) printf("USB Connecting...\r\n");
//		if(usbx.bDeviceState&0x40) printf("USB Connected...\r\n");
//		else printf("USB DisConnected...\r\n");
		rt_thread_delay(1000);

		HMI_File_Page("start");
		//HMI_Print_Str("t0","123");
		//HMI_Print("t0.txt=\"123\"");
		if(HMI_RX_FLAG)
		{
			printf("OK\r\n");
			USART_Solution();
			//printf("%s\r\n",HMI_Rx_String);
			HMI_RX_FLAG=0;
			UASRT1_RX_BUFFER_LEN=0;
		}
	}
}
  
//�߳�LED1
static void led1_thread_entry(void* parameter)
{
//		u32 total,free;
//		FIL fsrc;	  		//�ļ�1
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
//		if(f_mount(fs[0],"0:",1)) printf("����ʧ��");
//		printf("���سɹ�\r\n");
//		if(exf_getfree("0:",&total,&free)) printf("��ȡ����");
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




/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main()
{
		
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
		
		return 0;
}



