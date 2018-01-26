#include <HMI.h>
#include "stdio.h"
#include "string.h"
#include "sys.h"
#include "usart1.h"

u8 HMI_Type;		//当前接收处理的HMI对象类型	
u8 HMI_Rx_String[100];	//HMI设备返回的数据内容


HMI_Error USART_Solution(void)
{
	u8 i;

	//判断当前返回的数据类型
	switch(USART_RX_BUF[0])
	{
		case HMI_Touch_Type :	HMI_Type = HMI_Touch_Type; break;
		case HMI_Page_Type :	HMI_Type = HMI_Page_Type; break;
		case HMI_Touchxy_Type :	HMI_Type = HMI_Touchxy_Type; break;
		case HMI_SleepThing_Type :	HMI_Type = HMI_SleepThing_Type; break;
		case HMI_String_Type :	HMI_Type = HMI_String_Type; break;
		case HMI_Vaule_Type :	HMI_Type = HMI_Vaule_Type; break;
		case HMI_Instr_OK :	HMI_Type = HMI_Instr_OK; break;
		default : break;
	}
	
	//从串口接收到的字符串提取有效数据
	for(i=1;i<=UASRT1_RX_BUFFER_LEN-4;i++)
	{
		HMI_Rx_String[i-1]=USART_RX_BUF[i];
	}
	
	return HMI_OK;
}

/**
 * HMI跳转页面指令
 * @param  Page_ID [要跳转的页面名称]
 * @return         [description]
 */
HMI_Error HMI_File_Page(char* Page_ID)
{
	char str[30];
	strcpy(str,"page ");
	strcat(str,Page_ID);
	HMI_Print(str);

	return HMI_OK;
}
/**
 * 发送字符串至HMI
 * @param  str [要发送的字符串]
 * @return     [description]
 */
HMI_Error HMI_Print_Str(char* Str_ID,char* fmt)
{
	char str[60];
	strcpy(str,Str_ID);
	strcat(str,".txt=\"");
	strcat(str,fmt);
	//strcat(str,"\"");
	HMI_Print(str);

	return HMI_OK;
}
/**
 * HMI发送
 * @param  str [description]
 * @return     [description]
 */
HMI_Error HMI_Print(char* str)
{
	printf("%s\n", str);
	HMI_Send_TXEND();
	
	return HMI_OK;
}