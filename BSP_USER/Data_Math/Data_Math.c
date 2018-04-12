#include "sys.h"
#include "Data_Math.h"
#include "math.h" 
#include "arm_math.h"
#include "adc.h"



float Get_PowerVoltage(void)
{
	u16 Adc_Val;
	float PowerVoltage=0;
	
	Adc_Val=Get_Adc_Average(1,8,30);
	PowerVoltage=(float)Adc_Val*10*3.3f/4096;
	
	
	return PowerVoltage;	
}

float Get_PowerCurrent(void)
{
	u16 Adc_Val;
	float PowerCurrent=0;
	Adc_Val=Get_Adc_Average(2,9,30);
	PowerCurrent=(float_t)Adc_Val*3.3f/4096;
	
	return PowerCurrent;
}

float Get_PowerRipple(void)
{
	u16 Adc_Val;
	u16 PowerRipple;
	
	Adc_Val=Get_Adc_Average(1,1,30);
	PowerRipple=(float)Adc_Val*3.3f/4096;
	
	return PowerRipple;
}