#include <finsh.h>
#include "HMI.h"
#include "CSV_Database.h"
#include "dac.h"
#include "adc.h"
#include "switch.h"
#include "rtc.h"
#include "HMI.h"
#include "Data_Math.h"

int mycmd(void)
{
	rt_kprintf("hello world!\r\n");
	QC20_Induced();
	return 0;
}

MSH_CMD_EXPORT(mycmd,my command test);

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
	USB_DP_SetVol(my_atoi(argv[1]));
	USB_DM_SetVol(my_atoi(argv[1]));
	rt_kprintf("dac=%d\r\n",my_atoi(argv[1]));
	
	return 0;
}

MSH_CMD_EXPORT(dac,my command test);

int showadc(void)
{
	u16 res;
	
	res=Get_Adc_Average(1,ADC_Channel_5,20);
	rt_kprintf("ADC1=%d\r\n",res);
	res=Get_Adc_Average(2,ADC_Channel_0,1);
	rt_kprintf("ADC2=%d\r\n",res);
	return 0;
}

MSH_CMD_EXPORT(showadc,my command test);

int fan(void)
{
	
	SW=~SW;
	
	return 0;
}

MSH_CMD_EXPORT(fan,my command test);

int sw(void)
{	
	SW=~SW;	
	return 0;
}

MSH_CMD_EXPORT(sw,my command test);

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

int page(int argc,char** argv)
{
	
	HMI_File_Page(*argv[1]); 
	
	return 0;
}

MSH_CMD_EXPORT(page,my command test);

int mtk(int argc,char** argv)
{
	if(my_atoi(argv[1]))
	MTK_IncreaseVoltage(1);
	else
	MTK_DecreaseVoltage(1);	
	return 0;
}

MSH_CMD_EXPORT(mtk,my command test);




