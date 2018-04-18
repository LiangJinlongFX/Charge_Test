#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)
float Get_PowerVoltage(void);
float Get_PowerCurrent(void);
float Get_PowerRipple(void);


/*
 * MTK-PE诱导
 * Voltage_level ：连续调节电压挡位 1:2:3		5->7	5-9		5->12
 * 一般建议 Voltage_level=1  进行单级调节
 * 支持电压 5V 7V 9V 12V
 */
void MTK_IncreaseVoltage(u8 Voltage_level);
void MTK_DecreaseVoltage(u8 Voltage_level);

void QC20_Induced(void);

#endif

