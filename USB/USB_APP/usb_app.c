#include "usb_app.h" 
#include "delay.h"   
#include "rtthread.h"    
#include "exfuns.h"      
#include "sys.h"      
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USB-APP 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

//USB HOST
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev; 
_usb_app usbx;//USB APP控制器 


//USB OTG 中断服务函数  位于 usbd_usr.c 
//初始化USB
void usbapp_init(void)
{
	usbx.bDeviceState=0;	//USB状态复位

	usbx.mode=0XFF; 	//设置为一个非法的模式,必须先调用usbapp_mode_set设置模式,才能正常工作
} 

//USB轮询函数,必须周期性的被调用.
void usbapp_pulling(void)
{
	
}

//USB结束当前工作模式
void usbapp_mode_stop(void)
{
	
}

//设置USB工作模式
//mode:0,USB HOST MSC模式(默认模式,接U盘)
//	   1,USB HOST HID模式(驱动鼠标键盘等)
//	   2,USB Device MSC模式(USB读卡器) 
void usbapp_mode_set(u8 mode)
{
	usbapp_mode_stop();//先停止当前USB工作模式
	usbx.mode=mode;
	switch(usbx.mode)
	{
		case USBD_MSC_MODE:
			MSC_BOT_Data=rt_malloc(MSC_MEDIA_PACKET);	//给MSC缓冲区申请内存
			USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);	//初始化MSC
			break;	  
	}
}







