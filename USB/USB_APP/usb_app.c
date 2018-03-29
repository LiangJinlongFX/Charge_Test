#include "usb_app.h" 
#include "delay.h"   
#include "rtthread.h"    
#include "exfuns.h"      
#include "sys.h"
#include "usbd_msc_core.h"
#include "usb_dcd_int.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 	   

//USB OTG 句柄
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev; 

u8 USB_DeviceState;		//USB设备状态标识



void USB_running(void)
{
	MSC_BOT_Data=rt_malloc(MSC_MEDIA_PACKET);		//给USBMSC缓存分配空间
	USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);	//初始化MSC设备
	
	while(1);
	DCD_DevDisconnect(&USB_OTG_Core_dev);	//断开USB连接
	USB_OTG_StopDevice(&USB_OTG_Core_dev);
	rt_free(MSC_BOT_Data);	//释放内存
	RCC->AHB2RSTR|=1<<7;	//USB OTG FS 复位
	delay_ms(5);
	RCC->AHB2RSTR&=~(1<<7);	//复位结束
	memset(&USB_OTG_Core_dev,0,sizeof(USB_OTG_CORE_HANDLE));	
}







