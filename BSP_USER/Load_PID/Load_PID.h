#ifndef _LOAD_PID_H
#define _LOAD_PID_H

#include "sys.h"

#define Current_DAC_Set 

/* 恒流电子负载参数设置结构体 */
typedef struct
{
	float SetPoint;           //  设定目标值 Desired Value

	uint16_t DAC_Val;					//  当前DAC设置值
	uint16_t DAC_Step;				//	DAC调整步进值
	
	float Error;							//	当前ADC检测值与目标值的偏差
	float Bias;								//	允许的调整值与目标值的偏差
	float Return_Value;       //  返回的下一次DAC设置值
} LoadCtrl_TypeDef;


void TIM4_Init(u16 arr,u16 psc);
void Set_LoadCurrent(float Current_set);
float OverCurrent_Detection(void);

#endif




