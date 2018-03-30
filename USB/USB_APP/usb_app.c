#include "usb_app.h" 
#include "delay.h"   
#include "rtthread.h"    
#include "exfuns.h"      
#include "sys.h"      
//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 	   

//USB OTG ���
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev; 
//USB APP������
_usb_app usbx;


//USB OTG �жϷ�����  λ�� usbd_usr.c 
//��ʼ��USB
void usbapp_init(void)
{
	usbx.bDeviceState=0;	//USB״̬��λ

	usbx.mode=0XFF; 	//����Ϊһ���Ƿ���ģʽ,�����ȵ���usbapp_mode_set����ģʽ,������������
} 

//USB��ѯ����,���������Եı�����.
void usbapp_pulling(void)
{
	
}

//USB������ǰ����ģʽ
void usbapp_mode_stop(void)
{
	
}

//����USB����ģʽ
//mode:0,USB HOST MSCģʽ(Ĭ��ģʽ,��U��)
//	   1,USB HOST HIDģʽ(���������̵�)
//	   2,USB Device MSCģʽ(USB������) 
void usbapp_mode_set(u8 mode)
{
	usbapp_mode_stop();//��ֹͣ��ǰUSB����ģʽ
	usbx.mode=mode;
	switch(usbx.mode)
	{
		case USBD_MSC_MODE:
			//MSC_BOT_Data=rt_malloc(MSC_MEDIA_PACKET);	//��MSC�����������ڴ�
			USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);	//��ʼ��MSC
			break;	  
	}
}







