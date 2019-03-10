#ifndef __HLW8032_H
#define __HLW8032_H

#include "sys.h"


/*
 * HLW8032寄存器结构体
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
 * HLW8032测量数据结构体
 */
typedef struct 
{
	float AC_Voltage;
	float AC_Current;
	float AC_Power;
}HLW8032Data_Type;


/* HLW8032默认参数定义 */
#define default_VoltageparameterREG  	 0x2d050		//默认电压参数
#define default_CurrentParameterREG  	 0x3cdc			//默认电流参数
#define default_PowerparameterREG_Val  0x4c2fe8		//默认功率参数


#define V_K1 1.93f		//电压计算比例系数
#define C_K2 1.975f		//电流计算比例系数
#define P_K3 3.914f		//功率计算比例系数

u8 UsartToStruct(HLW8032REG_Type *HLW8032REG_Structure);
u8 HLW8032Get_Data(HLW8032Data_Type *HLW8032Data_Struct);

#endif

