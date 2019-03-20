#include <finsh.h>
#include "HMI.h"
#include "CSV_Database.h"
#include "dac.h"
#include "adc.h"
#include "switch.h"
#include "rtc.h"
#include "HMI.h"
#include "Data_Math.h"
#include "hlw8032.h"
#include "stdio.h"
#include "usart3.h"
#include "tlc5615.h"
#include "Load_PID.h"
#include "delay.h"

/*
 * 检测交流参数
 */
int ACparameters(void)
{
	u8 res;
	HLW8032Data_Type HLW8032Data_StructureData;
	char str[30];
	
	/* 使能串口3中断响应开关 */
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	res=HLW8032Get_Data(&HLW8032Data_StructureData);
	if(res)
	{
		rt_kprintf("AC ERROR!!!\r\n");
		return 0;
	}
	else
	{
		sprintf(str,"V=%.3f\r\nC=%.3f\r\nP=%.3f\r\n",HLW8032Data_StructureData.AC_Voltage,HLW8032Data_StructureData.AC_Current,HLW8032Data_StructureData.AC_Power);
		rt_kprintf("%s",str);
	}
		
	return 0;
}
MSH_CMD_EXPORT(ACparameters,my command test);

/*
 * 负载开关控制
 */
int sw(void)
{	
	SW=~SW;	
	return 0;
}
MSH_CMD_EXPORT(sw,my command test);

/*
 * 散热风扇控制
 */
int fan(void)
{	
	FAN=~FAN;	
	return 0;
}
MSH_CMD_EXPORT(fan,my command test);

/*
 * 继电器控制
 */
int relay(void)
{	
	RELAY=~RELAY;	
	return 0;
}
MSH_CMD_EXPORT(relay,my command test);

/*
 * 设置电子负载电流
 */
int Set_Current(int argc,char** argv)
{	
	u8 vol;
//	
//	SW=0;
	vol=my_atoi(argv[1]);
//	rt_kprintf("set=%d\r\n",vol);
//	//Set_LoadCurrent((float)vol);
//	TLC5615_SetVoltage(vol);
//	
	QC20_AdjustVoltage(vol);
	
	return 0;
}
MSH_CMD_EXPORT(Set_Current,my command test);

/*
 * 显示当前时间
 */
int showtime(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	
	rt_kprintf("Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	rt_kprintf("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	
	return 0;
}
MSH_CMD_EXPORT(showtime,my command test);

/*
 * 过流检测
 */
int OCP(void)
{
	float Val;
	
	Val=OverCurrent_Detection();
	rt_kprintf("C_MAX=%d\r\n",(u32)Val);
	
	return 0;
}
MSH_CMD_EXPORT(OCP,my command test);

/*
 * 快充诱导
 */
int QC(void)
{
	u8 Val;
	
	rt_enter_critical();	//进入临界区
	Val=QuickCharge_Induction(0x07);
//	QC20_AdjustVoltage(0);
//	delay_ms(1000);
//	QC20_AdjustVoltage(1);
//	delay_ms(1000);
//	QC20_AdjustVoltage(0);
//	delay_ms(1000);
//	QC20_AdjustVoltage(2);
//	delay_ms(1000);
//	QC20_AdjustVoltage(0);
	rt_exit_critical();		//退出临界区
	rt_kprintf("res=%x\r\n",Val);
	
	return 0;
}
MSH_CMD_EXPORT(QC,my command test);



