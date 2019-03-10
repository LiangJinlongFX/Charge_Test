#ifndef _LOAD_PID_H
#define _LOAD_PID_H

#include "sys.h"

#define Current_DAC_Set 

/* �������Ӹ��ز������ýṹ�� */
typedef struct
{
	float SetPoint;           //  �趨Ŀ��ֵ Desired Value

	uint16_t DAC_Val;					//  ��ǰDAC����ֵ
	uint16_t DAC_Step;				//	DAC��������ֵ
	
	float Error;							//	��ǰADC���ֵ��Ŀ��ֵ��ƫ��
	float Bias;								//	����ĵ���ֵ��Ŀ��ֵ��ƫ��
	float Return_Value;       //  ���ص���һ��DAC����ֵ
} LoadCtrl_TypeDef;


void TIM4_Init(u16 arr,u16 psc);
void Set_LoadCurrent(float Current_set);
float OverCurrent_Detection(void);

#endif




