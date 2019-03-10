#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)
#define USB_DM_SetVol(vol) Dac1_Set_Vol(vol)

/* �������ṹ�� */
typedef struct
{
	float Measured_USB_Voltage;	//��⵽��USB�ڵ�ѹ
	float Measured_USB_Current;	//��⵽��USB�ڵ���
	float Measured_USB_Power;		//��⵽��USB�ڹ���
	float Measured_AC_Voltage;	//��⵽�Ľ��������ѹ
	float Measured_AC_Current;	//��⵽�Ľ����������
	float Measured_AC_Power;		//��⵽�Ľ������빦��
}MeasuredData_Type;

/* �����������ṹ�� */
typedef struct
{
	float Drop_Voltage;	//�����ѹ���� 0-3300
	float DAC_Step;			//DAC��������ֵ
	float Current_Max;	//����������
}OCP_Type;

#define DropVoltage_Proportion 0.8f	//���������ѹռ��
#define OCP_DAC_Step 10	//�������DAC���ò���

/*
	����У׼��ʽ���� Vol = K*Measured_Vol + Offset 
*/
/* ����ϵ�� */
#define Measured_Volteage_K 10.0f
#define Measured_Current_K 1.0f
#define Measured_Ripple_K 1.0f
/* ƫ��ֵ */
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
 * MTK-PE�յ�
 * Voltage_level ���������ڵ�ѹ��λ 1:2:3		5->7	5-9		5->12
 * һ�㽨�� Voltage_level=1  ���е�������
 * ֧�ֵ�ѹ 5V 7V 9V 12V
 */
void MTK_IncreaseVoltage(u8 Voltage_level);
void MTK_DecreaseVoltage(u8 Voltage_level);

u8 QC_Init(void);
void QC_DisInit(void);

void QC20_AdjustVoltage(u8 Voltage_level);
u8 QuickCharge_Induction(u8 QuickCharge_Type);

#endif

