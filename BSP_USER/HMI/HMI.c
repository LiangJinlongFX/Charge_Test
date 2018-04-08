#include <HMI.h>
#include "stdio.h"
#include "string.h"
#include "sys.h"
#include "usart1.h"
#include "CSV_Database.h"
#include "rtthread.h"

/*
 * 批量检测条目限制变量 bit7:转换效率 bit6:上电时间 bit5:电路保护 bit4:OCP bit3:OVP bit2:cmax bit1:vmax bit0:纹波  1：为有效
 */
u8 HMI_TestLimit=0x88;

/**
 * HMI响应处理函数
 * @param  HMI_Type HMI_Rx_String [页面类型指针 有效内容字符类型指针]
 * @return         [description]
 */
HMI_Error USART_Solution(u8 HMI_Type,char* HMI_Rx_String)
{
	u16 i=0;
	
	//等待设备响应
	do{
		if(HMI_RX_FLAG) break;
		i++;
	}while(i<5000);
	//仍不能接收到标志位，返回无响应错误
	if(!HMI_RX_FLAG) return HMI_NoResponse;
	
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
	//加上结尾符号以方便识别
	HMI_Rx_String[i]='\0';
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
	char str2[10];
	u8 res=HMI_OK;
	
	my_itoa(Page_ID,str2);
	strcpy(str,"page ");
	strcat(str,str2);
	HMI_Print(str);
	
	//HMI_Page_ACK(Page_ID);
//	if(res!=HMI_OK) return res;

	return HMI_OK;
}
/**
* 发送字符串至HMI对象
 * @param  str [要发送的字符串]
 * @return     [description]
 */
HMI_Error HMI_Print_Str(char* Object_ID,char* fmt)
{
	char str[60];
	
	strcpy(str,Object_ID);
	strcat(str,".txt=\"");
	strcat(str,fmt);
	strcat(str,"\"");
	HMI_Print(str);

	return HMI_OK;
}

/**
* 发送数组至HMI对象   !!!必须是无符号整数
 * @param  str [要发送的数值]
 * @return     [description]
 */
HMI_Error HMI_Print_Val(char* Object_ID,u16 varible)
{
	char str[60];
	char temp_str[10];
	
	strcpy(str,Object_ID);
	strcat(str,".val=");
	my_itoa(varible,temp_str);
	strcat(str,temp_str);
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
	if(!USART_Solution(Type,str)) return HMI_NoResponse;
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

HMI_Error HMI_StandardPage_Show(void)
{
	char Num_str[10];
	char temp;
	char temp_str[10];
	
//	TestStandard_Arrary[0].Cout_Max=244;
//	TestStandard_Arrary[0].Vout_Max=50;
//	TestStandard_Arrary[0].Ripple_Voltage=100;
//	TestStandard_Arrary[0].Poweron_Time=10;
//	TestStandard_Arrary[0].Efficiency=89;
//	TestStandard_Arrary[0].Over_Voltage_Protection=1;
//	TestStandard_Arrary[0].Short_Current=0;
//	TestStandard_Arrary[0].Over_Current_Protection=1;
//	TestStandard_Arrary[0].Quick_Charge=2;
	
	my_itoa(TestStandard_Arrary[Current_event].Vout_Max/10,Num_str);
	strcat(Num_str,".");
	temp=TestStandard_Arrary[Current_event].Vout_Max%10+'0';
	strcat(Num_str,&temp);
	HMI_Print_Str("t0",Num_str);
	
	my_itoa(TestStandard_Arrary[Current_event].Cout_Max/100,Num_str);
	strcat(Num_str,".");
	my_itoa(TestStandard_Arrary[Current_event].Cout_Max%100,temp_str);
	strcat(Num_str,temp_str);
	HMI_Print_Str("t1",Num_str);
	
	my_itoa(TestStandard_Arrary[Current_event].Ripple_Voltage,Num_str);
	HMI_Print_Str("t2",Num_str);
	
	my_itoa(TestStandard_Arrary[Current_event].Poweron_Time,Num_str);
	HMI_Print_Str("t3",Num_str);
	
	my_itoa(TestStandard_Arrary[Current_event].Efficiency,Num_str);
	HMI_Print_Str("t4",Num_str);
	
	HMI_Print_Val("bt0",TestStandard_Arrary[Current_event].Over_Voltage_Protection);
	HMI_Print_Val("bt2",TestStandard_Arrary[Current_event].Short_Current);
	HMI_Print_Val("bt1",TestStandard_Arrary[Current_event].Over_Current_Protection);
	
	HMI_Print_Val("FastCharg_STA",TestStandard_Arrary[Current_event].Quick_Charge);

	return HMI_OK;
}

/**
* 获取HMI控件的值  获取的值都为字符类型，如获取为数值需atoi函数
 * @param  Page_ID [页面ID]
 * @return     [description]
 */
HMI_Error HMI_Get(u8 Object_Type,char* Object_ID,char* fmt)
{
	char str[20];
	char str2[10];
	u16 temp;
	u8 i=0,n=0;
	u8 res=HMI_OK;
	
	//要获取的内容为字符串类型
	if(Object_Type==HMI_String_Type)
	{
		strcpy(str,"get ");
		strcat(str,Object_ID);
		strcat(str,".txt");
		HMI_Print(str);
		//等待HMI响应
		res=USART_Solution(HMI_String_Type,fmt);
	}
	//要获取的内容为数值类型
	else if(Object_Type==HMI_Vaule_Type)
	{
		strcpy(str,"get ");
		strcat(str,Object_ID);
		strcat(str,".val");
		HMI_Print(str);
		//等待HMI响应
		res=USART_Solution(HMI_Vaule_Type,str2);
		while(str2[i]!='\0')
		{
			if(str2[i]>=0&&str2[i]<=9) n=10*n+str2[i];
			i++;
		}
		my_itoa(n,fmt);			
	}
	
	return res;
}
/**
* 将标准设置界面的参数文本写进全局标准结构体
 * @param   []
 * @return     [description]
 */
HMI_Error HMI_Standard_Atoi(void)
{
	char str[20];
	u8 res=HMI_OK;
	res=HMI_Get(HMI_String_Type,"t0",str);
	TestStandard_Arrary[Current_event].Vout_Max=my_atoi(str);
	res=HMI_Get(HMI_String_Type,"t1",str);
	TestStandard_Arrary[Current_event].Cout_Max=my_atoi(str);
	res=HMI_Get(HMI_String_Type,"t2",str);
	TestStandard_Arrary[Current_event].Ripple_Voltage=my_atoi(str);
	res=HMI_Get(HMI_String_Type,"t3",str);
	TestStandard_Arrary[Current_event].Poweron_Time=my_atoi(str);
	res=HMI_Get(HMI_String_Type,"t4",str);
	TestStandard_Arrary[Current_event].Efficiency=my_atoi(str);
	res=HMI_Get(HMI_Vaule_Type,"bt0",str);
	TestStandard_Arrary[Current_event].Over_Voltage_Protection=my_atoi(str);
	res=HMI_Get(HMI_Vaule_Type,"bt1",str);
	TestStandard_Arrary[Current_event].Over_Current_Protection=my_atoi(str);
	res=HMI_Get(HMI_Vaule_Type,"bt2",str);
	TestStandard_Arrary[Current_event].Short_Current=my_atoi(str);
	res=HMI_Get(HMI_Vaule_Type,"FastCharg_STA",str);
	TestStandard_Arrary[Current_event].Quick_Charge=my_atoi(str);
	
	return res;
}

/**
 * 将测试门限设置呈现给测试门限设置界面  批量检测条目限制变量 bit7:转换效率 bit6:上电时间 bit5:电路保护 bit4:OCP bit3:OVP bit2:cmax bit1:vmax bit0:纹波  1：为有效
 * @param   []
 * @return     [description]
 */
HMI_Error HMI_TestLimit_Itoa(void)
{
	u8 temp=0;
	u8 temp2=0;
	u8 res=HMI_OK;
	
	temp2=HMI_TestLimit;
	
	if(temp2&0x01) temp=1; else temp=0;
	HMI_Print_Val("bt0",temp);
	if(temp2&0x02) temp=1; else temp=0;
	HMI_Print_Val("bt1",temp);
	if(temp2&0x04) temp=1; else temp=0;
	HMI_Print_Val("bt2",temp);
	if(temp2&0x08) temp=1; else temp=0;
	HMI_Print_Val("bt3",temp);
	if(temp2&0x10) temp=1; else temp=0;
	HMI_Print_Val("bt4",temp);
	if(temp2&0x20) temp=1; else temp=0;
	HMI_Print_Val("bt5",temp);
	if(temp2&0x40) temp=1; else temp=0;
	HMI_Print_Val("bt6",temp);
	if(temp2&0x80) temp=1; else temp=0;
	HMI_Print_Val("bt7",temp);
	
	return res;
}

/**
 * 测试门限设置界面数值赋予全局测试门限变量
 * @param   []
 * @return     [description]
 */
HMI_Error HMI_TestLimit_Atoi(u8 * LimitVal)
{
	char str[5];
	u8 temp1=0;
	u8 temp2=0;
	u8 res=HMI_OK;
	
	res=HMI_Get(HMI_Vaule_Type,"bt0",str);
	temp1=my_atoi(str);
	temp2=temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt1",str);
	temp1=my_atoi(str);
	temp1=temp1<<1;
	temp2=temp2|temp1;
	rt_kprintf("res1=%d\r\n",temp2);
	res=HMI_Get(HMI_Vaule_Type,"bt2",str);
	temp1=my_atoi(str);
	temp1=temp1<<2;
	temp2=temp2|temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt3",str);
	temp1=my_atoi(str);
	temp1=temp1<<3;
	temp2=temp2|temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt4",str);
	temp1=my_atoi(str);
	temp1=temp1<<4;
	temp2=temp2|temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt5",str);
	temp1=my_atoi(str);
	temp1=temp1<<5;
	temp2=temp2|temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt6",str);
	temp1=my_atoi(str);
	temp1=temp1<<6;
	temp2=temp2|temp1;
	res=HMI_Get(HMI_Vaule_Type,"bt7",str);
	temp1=my_atoi(str);
	temp1=temp1<<7;
	temp2=temp2|temp1;
	
	rt_kprintf("res1=%d\r\n",temp1);
	
	
	*LimitVal = temp2;   //将参数传给全局变量  
	
	return res;
}

/**
 * 将时间参数呈现给HMI界面
 * @param   []
 * @return     [description]
 */
HMI_Error HMI_RTC_Show(void)
{
	char str1[20];
	char str2[10];
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct); //获取当前日期
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);  //获取当前时间
	
	my_itoa(RTC_DateStruct.RTC_Year,str2);
	strcpy(str1,str2);
	strcat(str1,"-");
	my_itoa(RTC_DateStruct.RTC_Month,str2);
	strcat(str1,str2);
	strcat(str1,"-");
	my_itoa(RTC_DateStruct.RTC_Date,str2);
	strcat(str1,str2);
	strcat(str1," ");
	my_itoa(RTC_TimeStruct.RTC_Hours,str2);
	strcat(str1,str2);
	strcat(str1,":");
	my_itoa(RTC_TimeStruct.RTC_Minutes,str2);
	strcat(str1,str2);
	
	HMI_Print_Str("t1",str1);
	
	return HMI_OK;
}

HMI_Error HMI_RTC_Atoi(void)
{
	char str1[20];
	char str2[10];
	char str_temp;
	u8 year,month,day,hour,min;
	u8 res1,res2;
	HMI_Get(HMI_String_Type,"t3",str1);
	str2[0]=str1[0];str2[1]=str1[1];str2[2]='\0';
	year=my_atoi(str2);
	str2[0]=str1[2];str2[1]=str1[3];str2[2]='\0';
	month=my_atoi(str2);
	str2[0]=str1[4];str2[1]=str1[5];str2[2]='\0';
	day=my_atoi(str2);
	str2[0]=str1[6];str2[1]=str1[7];str2[2]='\0';
	hour=my_atoi(str2);
	str2[0]=str1[8];str2[1]=str1[9];str2[2]='\0';
	min=my_atoi(str2);
	rt_kprintf("str=%s\r\n",str1);
	rt_kprintf("%d %d %d %d %d\r\n",year,month,day,hour,min);
	if(month>12||month<1||day>31||day<1||day>31||hour>24||hour<1||min>60)
		HMI_File_Page(16);
	else HMI_File_Page(1);
	return res1;
}

HMI_Error HMI_ShowBatch(void)
{
	char str[5][10];
	char Event_Info[3];
	u8 Page_Flag;
	u8 Exit_Flag=0;
	while(1)
	{
		HMI_Print_Str("t0",str[0]);
		HMI_Print_Str("t1",str[1]);
		HMI_Print_Str("t2",str[2]);
		HMI_Print_Str("t3",str[3]);
		HMI_Print_Str("t4",str[4]);
		while(USART_Solution(HMI_Vaule_Type,Event_Info));
		switch(Event_Info[0])
		{
			case 0x01: break;//进入第一条目
			case 0x02: break;//进入第二条目
			case 0x03: break;//进入第三条目
			case 0x04: break;//进入第四条目
			case 0x05: break;//进入第五条目
			case 0x06: break;//上翻
			case 0x07: break;//下翻
			case 0x08: Exit_Flag=1;break;//退出
		}
		if(Exit_Flag) break;
	}
	HMI_File_Page(1);
	
	return HMI_OK;
}

