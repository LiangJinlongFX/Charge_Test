#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)
#define USB_DM_SetVol(vol) Dac1_Set_Vol(vol)

/* 检测参数结构体 */
typedef struct
{
	float Measured_USB_Voltage;	//检测到的USB口电压
	float Measured_USB_Current;	//检测到的USB口电流
	float Measured_USB_Power;		//检测到的USB口功率
	float Measured_AC_Voltage;	//检测到的交流输入电压
	float Measured_AC_Current;	//检测到的交流输入电流
	float Measured_AC_Power;		//检测到的交流输入功率
}MeasuredData_Type;

/* 过流检测参数结构体 */
typedef struct
{
	float Drop_Voltage;	//跌落电压门限 0-3300
	float DAC_Step;			//DAC调整步进值
	float Current_Max;	//最大输出电流
}OCP_Type;

#define DropVoltage_Proportion 0.8f	//过流跌落电压占比
#define OCP_DAC_Step 10	//过流检测DAC设置步进

/*
	测量校准公式参数 Vol = K*Measured_Vol + Offset 
*/
/* 比例系数 */
#define Measured_Volteage_K 10.0f
#define Measured_Current_K 1.0f
#define Measured_Ripple_K 1.0f
/* 偏差值 */
extern float Measured_Voltage_Offset;
extern float Measured_Current_Offset;
extern float Measured_Ripple_Offset;

void ADCValue_Dealwith(u32 Measure_Vol[3]);

void Get_Power_Val_All(MeasuredData_Type *pp);
float Get_PowerVoltage(void);
float Get_PowerCurrent(void);
float Get_PowerRipple(void);


void Get_Measured_Offset(void);

u8 OverCurrent_DetectionVoltage(OCP_Type *pp);


/*
 * MTK-PE诱导
 * Voltage_level ：连续调节电压挡位 1:2:3		5->7	5-9		5->12
 * 一般建议 Voltage_level=1  进行单级调节
 * 支持电压 5V 7V 9V 12V
 */
void MTK_IncreaseVoltage(u8 Voltage_level);
void MTK_DecreaseVoltage(u8 Voltage_level);

u8 QC_Init(void);
void QC_DisInit(void);

void QC20_AdjustVoltage(u8 Voltage_level);
u8 QuickCharge_Induction(u8 QuickCharge_Type);

#endif

