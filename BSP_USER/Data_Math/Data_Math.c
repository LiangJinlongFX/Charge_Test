#include "sys.h"
#include "Data_Math.h"
#include "math.h" 
#include "arm_math.h"
#include "adc.h"



u16 Get_PowerVoltage(void)
{
	u8 res;
	u16 Adc_Val;
	u16 PowerVoltage=0;
	
	Adc_Val=Get_Adc_Average(1,8,1);
	PowerVoltage=Adc_Val*3300*10/4096;
	
	
	return PowerVoltage;	
}

u32 Get_PowerCurrent(void)
{
	u16 Adc_Val;
	u32 PowerCurrent=0;
	Adc_Val=Get_Adc_Average(2,9,1);
	PowerCurrent=(u32)Adc_Val*330000/(4096*50*20);
	
	return Adc_Val;
}

u16 Get_PowerRipple(void)
{
	u16 Adc_Val;
	u16 PowerRipple;
	
	Adc_Val=Get_Adc_Average(1,1,1);
	PowerRipple=Adc_Val*3300*10/4096;
	
	return PowerRipple;
}