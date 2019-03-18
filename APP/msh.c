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


int print(int argc,char** argv)
{
	rt_kprintf("print ");
	
	if(argc>1)	
	{
		rt_kprintf("%s\r\n",argv[1]);
		HMI_Print(argv[1]);
	}
	return 0;
}

MSH_CMD_EXPORT(print,my command test);

int dac(int argc,char** argv)
{
	u16 i;
	
	i=(u16)my_atoi(argv[1]);
	TLC5615_SetVoltage(i);
	rt_kprintf("dac=%d\r\n",i);
	
	return 0;
}

MSH_CMD_EXPORT(dac,my command test);

int showadc(void)
{

	;

	return 0;
}

MSH_CMD_EXPORT(showadc,my command test);


/**
 * 检测交流参数
 * @param   
 * @return 
 * @brief 
 */
int ACparameters(void)
{
	u8 res;
	HLW8032Data_Type HLW8032Data_StructureData;
	char str[30];
	
	res=HLW8032Get_Data(&HLW8032Data_StructureData);
	if(res)
	{
		rt_kprintf("AC ERROR!!!\r\n");
		return 0;
	}
	sprintf(str,"V=%.3f\r\nC=%.3f\r\nP=%.3f\r\n",HLW8032Data_StructureData.AC_Voltage,HLW8032Data_StructureData.AC_Current,HLW8032Data_StructureData.AC_Power);
	rt_kprintf("%s",str);
		
	return 0;
}
MSH_CMD_EXPORT(ACparameters,my command test);

/**
 * 负载开关控制
 * @param   
 * @return 
 * @brief 
 **/
int sw(void)
{	
	SW=~SW;	
	return 0;
}
MSH_CMD_EXPORT(sw,my command test);

/**
 * 散热风扇开关控制
 * @param   
 * @return 
 * @brief 
 **/
int fan(void)
{	
	FAN=~FAN;	
	return 0;
}
MSH_CMD_EXPORT(fan,my command test);

/**
 * 继电器开关控制
 * @param   
 * @return 
 * @brief 
 **/
int relay(void)
{	
	RELAY=~RELAY;	
	return 0;
}
MSH_CMD_EXPORT(relay,my command test);

/**
 * 设置电子负载电流
 * @param   
 * @return 
 * @brief 
 **/
int Set_Current(int argc,char** argv)
{	
	u32 vol;
	
	SW=0;
	vol=my_atoi(argv[1]);
	rt_kprintf("set=%d\r\n",vol);
	Set_LoadCurrent((float)vol);
	//TLC5615_SetVoltage(vol);
	return 0;
}
MSH_CMD_EXPORT(Set_Current,my command test);

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

int OCP(void)
{
	float Val;
	
	Val=OverCurrent_Detection();
	rt_kprintf("OCP=%d\r\n",(u32)Val);
	
	return 0;
}

MSH_CMD_EXPORT(OCP,my command test);

int mtk(int argc,char** argv)
{
	if(my_atoi(argv[1]))
	MTK_IncreaseVoltage(1);
	else
	MTK_DecreaseVoltage(1);	
	return 0;
}

MSH_CMD_EXPORT(mtk,my command test);

int qc(int argc,char** argv)
{
	u8 res;
	
	res=QuickCharge_Induction(my_atoi(argv[1]));
	
	rt_kprintf("res=%x\r\n",res);
	
	return 0;
}

MSH_CMD_EXPORT(qc,my command test);

int qc_init(int argc,char** argv)
{	
	QC20_AdjustVoltage(my_atoi(argv[1]));
	return 0;
}

MSH_CMD_EXPORT(qc_init,my command test);


