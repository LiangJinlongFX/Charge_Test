#ifndef __HLW8032_H
#define __HLW8032_H

#include "sys.h"


/*
 * HLW8032�Ĵ����ṹ��
 */
typedef struct 
{
	u8 StateREG_Val;
	u8 CheckREG_Val;
	u32 VoltageparameterREG_Val;
	u32 VoltageREG_Val;
	u32 CurrentParameterREG_Val;
	u32 CurrentREG_Val;
	u32 PowerparameterREG_Val;
	u32 PowerREG_Val;
	u8 DataUpdataREG_Val;
	u16 PFREG_Val;
	u8 CheckSumREG_Val;		
}HLW8032REG_Type;


/*
 * HLW8032�������ݽṹ��
 */
typedef struct 
{
	float AC_Voltage;
	float AC_Current;
	float AC_Power;
}HLW8032Data_Type;


/* HLW8032Ĭ�ϲ������� */
#define default_VoltageparameterREG  	 0x2d050		//Ĭ�ϵ�ѹ����
#define default_CurrentParameterREG  	 0x3cdc			//Ĭ�ϵ�������
#define default_PowerparameterREG_Val  0x4c2fe8		//Ĭ�Ϲ��ʲ���


#define V_K1 1.93f		//��ѹ�������ϵ��
#define C_K2 1.975f		//�����������ϵ��
#define P_K3 3.914f		//���ʼ������ϵ��

u8 UsartToStruct(HLW8032REG_Type *HLW8032REG_Structure);
u8 HLW8032Get_Data(HLW8032Data_Type *HLW8032Data_Struct);

#endif

