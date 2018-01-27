#include <HMI.h>
#include "stdio.h"
#include "string.h"
#include "sys.h"
#include "usart1.h"
#include "CSV_Database.h"

//u8 HMI_Type;		//当前接收处理的HMI对象类型	
//u8 HMI_Rx_String[100];	//HMI设备返回的数据内容

/**
 * HMI响应处理函数
 * @param  HMI_Type HMI_Rx_String [页面类型指针 有效内容指针]
 * @return         [description]
 */
HMI_Error USART_Solution(u8* HMI_Type,char* HMI_Rx_String)
{
	u16 i=0;
	
	//等待设备响应
	do{
		if(HMI_RX_FLAG) break;
		i++;
	}while(i<1000);
	//仍不能接收到标志位，返回无响应错误
	if(!HMI_RX_FLAG) return HMI_NoResponse;
	
	//判断当前返回的数据类型
	switch(USART_RX_BUF[0])
	{
		case HMI_Touch_Type :	*HMI_Type = HMI_Touch_Type; break;
		case HMI_Page_Type :	*HMI_Type = HMI_Page_Type; break;
		case HMI_Touchxy_Type :	*HMI_Type = HMI_Touchxy_Type; break;
		case HMI_SleepThing_Type :	*HMI_Type = HMI_SleepThing_Type; break;
		case HMI_String_Type :	*HMI_Type = HMI_String_Type; break;
		case HMI_Vaule_Type :	*HMI_Type = HMI_Vaule_Type; break;
		case HMI_Instr_OK :	*HMI_Type = HMI_Instr_OK; break;
		default : break;
	}
	
	//从串口接收到的字符串提取有效数据
	for(i=1;i<=UASRT1_RX_BUFFER_LEN-4;i++)
	{
		HMI_Rx_String[i-1]=USART_RX_BUF[i];
	}
	//清除缓冲变量
	HMI_RX_FLAG=0;
	UASRT1_RX_BUFFER_LEN=0;
	
	return HMI_OK;
}

/**
 * HMI跳转页面指令
 * @param  Page_ID [要跳转的页面名称]
 * @return         [description]
 */
HMI_Error HMI_File_Page(u8 Page_ID)
{
	char str[30];
	u8 res=HMI_OK;
	
	Page_ID=Page_ID+'0';
	strcpy(str,"page ");
	strcat(str,&Page_ID);
	HMI_Print(str);
	
	//HMI_Page_ACK(Page_ID);
//	if(res!=HMI_OK) return res;

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

/**
 * HMI当前页面确认
 * @param  Page_ID [页面ID]
 * @return     [description]
 */
HMI_Error HMI_Page_ACK(u8 Page_ID)
{
	u16 i;
	u8 Type;
	char str[10];
	HMI_Print("sendme");
	if(!USART_Solution(&Type,str)) return HMI_NoResponse;
	if(Type!=HMI_Page_Type) return HMI_Parse_Error;
	if(str[0]!=Page_ID) return HMI_Page_Error;
	
	return HMI_OK;
}

//HMI_Error HMI_Set_Standard()
//{
//	//创建一测试标准结构体对象
//	TestStandard_Type TestStandard_Struct;
//	//判断页面是否真的在标准设置页面
//	if(HMI_Page_ACK(Page_standard));
//	while(1)
//	{
//		
//	}
//}