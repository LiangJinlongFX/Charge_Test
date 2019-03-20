#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)	//设置USB_DP电压值
#define USB_DM_SetVol(vol) Dac1_Set_Vol(vol)	//设置USB_DM电压值

/* 检测参数结构体 */
typedef struct
{
	float Measured_USB_Voltage;	//检测到的USB口电压
	float Measured_USB_Current;	//检测到的USB口电流
	float Measured_USB_Power;		//检测到的USB口功率
	float Measured_Ripple_Val;	//检测到的USB纹波电压平均值
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

#define DropVoltage_Proportion 0.3f		//过流跌落电压占比
#define OCP_DAC_Step 100							//过流检测DAC设置步进

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


/**
 * 获取ADC采样值
 * @param  u32[3]
 * @return
 * @brief 
 **/
void ADCValue_Dealwith(u32 Measure_Vol[3]);

/**
 * 获取设备所有测量值
 * @param  MeasuredData_Type*
 * @return
 * @brief 
 **/
void Get_Power_Val_All(MeasuredData_Type *pp);

/**
 * 获取电源电压值
 * @param   
 * @return float[0-30000]
 * @brief 
 **/
float Get_PowerVoltage(void);

/**
 * 获取电源电流值
 * @param   
 * @return float[0-3000]
 * @brief 
 **/
float Get_PowerCurrent(void);

/**
 * 获取电源纹波电压值
 * @param   
 * @return float[0-3000]
 * @brief 
 **/
float Get_PowerRipple(void);

/**
 * 获取ADC校验偏差值
 * @param   
 * @return
 * @brief 
 **/
void Get_Measured_Offset(void);

/**
 * 电源电压跌落检测,用于测定最大输出电流
 * @param   
 * @return
 * @brief 
 **/
u8 OverCurrent_DetectionVoltage(OCP_Type *pp);

/**
 * MTK-PE诱导 增加输出电压
 * @param  Voltage_level ：连续调节电压挡位 1:2:3		5->7	5-9		5->12
 * @return
 * @brief 支持电压 5V 7V 9V 12V
 **/
void MTK_IncreaseVoltage(u8 Voltage_level);
/**
 * MTK-PE诱导 减少输出电压
 * @param Voltage_level ：连续调节电压挡位 1:2:3		5->7	5-9		5->12
 * @return
 * @brief 支持电压 5V 7V 9V 12V
 **/
void MTK_DecreaseVoltage(u8 Voltage_level);

/**
 * QC协议诱导初始化
 * @param   
 * @return 
 * @brief 
 **/
u8 QC_Init(void);

/**
 * 失能QC协议诱导
 * @param   
 * @return 
 * @brief 
 **/
void QC_DisInit(void);

/**
 * QC-2.0 诱导
 * @param  Voltage_level[0-6]
 * @return 
 * @brief 
 **/
void QC20_AdjustVoltage(u8 Voltage_level);

/**
 * 快充诱导函数
 * @param  Detected_level[诱导级别 |0|0|0|0|MTK-PE有效标志|QC2.0有效标志|QC3.0有效标志|详细诱导电压| 置1为激活]
 * @return Results[检测结果 |MTK-PE有效标志|QC2.0有效标志|QC3.0有效标志|PE-7V|PE-9V|PE-12V|QC-9V|QC-12V| ]
 * @brief 诱导设置说明 |不诱导 Detected_level=0|仅检测是否有快充功能 Detected_level=0x0E|检测MTK-PE并检测诱导出具体的电压值 Detected_level=0x09
 */
u8 QuickCharge_Induction(u8 QuickCharge_Type);

#endif

