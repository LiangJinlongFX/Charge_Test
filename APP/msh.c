#include <finsh.h>
#include "HMI.h"
#include "CSV_Database.h"
#include "dac.h"
#include "adc.h"
#include "switch.h"

int mycmd(void)
{
	rt_kprintf("hello world!\r\n");
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
	rt_kprintf("dac1=");
	
	if(argc>1)	
	{
		rt_kprintf("%s\r\n",argv[1]);
		Dac1_Set_Vol(my_atoi(argv[1]));
		rt_kprintf("dac=%d\r\n",my_atoi(argv[1]));
	}
	return 0;
}

MSH_CMD_EXPORT(dac,my command test);

int showadc(void)
{
	u16 res;
	
	res=Get_Adc_Average(1,ADC_Channel_5,20);
	rt_kprintf("ADC1=%d\r\n",res);
	res=Get_Adc_Average(2,ADC_Channel_0,20);
	rt_kprintf("ADC2=%d\r\n",res);
	return 0;
}

MSH_CMD_EXPORT(showadc,my command test);

int fan(void)
{
	u16 res;
	
	SW0=~SW0;
	
	return 0;
}

MSH_CMD_EXPORT(fan,my command test);

int sw(void)
{
	u16 res;
	
	SW1=~SW1;
	
	return 0;
}

MSH_CMD_EXPORT(sw,my command test);
