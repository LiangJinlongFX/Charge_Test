#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)
float Get_PowerVoltage(void);
float Get_PowerCurrent(void);
float Get_PowerRipple(void);


/*
 * MTK-PE�յ�
 * Voltage_level ���������ڵ�ѹ��λ 1:2:3		5->7	5-9		5->12
 * һ�㽨�� Voltage_level=1  ���е�������
 * ֧�ֵ�ѹ 5V 7V 9V 12V
 */
void MTK_IncreaseVoltage(u8 Voltage_level);
void MTK_DecreaseVoltage(u8 Voltage_level);

void QC20_Induced(void);

#endif

