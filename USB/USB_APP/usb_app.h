#ifndef __USB_APP_H
#define __USB_APP_H	 
#include "sys.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usb_dcd_int.h"
#include "string.h" 
//////////////////////////////////////////////////////////////////////////////////	 


////////////////////////////////////////////////////////////////////////////////// 	   



extern USB_OTG_CORE_HANDLE  USB_OTG_Core_dev;	//����һ��USB OTG ���

extern u8 USB_DeviceState;		//USB�豸״̬��ʶ

extern uint8_t *MSC_BOT_Data;	
	
void USB_running(void);

#endif

















