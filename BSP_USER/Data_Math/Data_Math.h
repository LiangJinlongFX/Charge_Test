#ifndef _DATA_MATH_H
#define _DATA_MATH_H
#include "sys.h"
#include "dac.h"

#define USB_DP_SetVol(vol) Dac2_Set_Vol(vol)	//����USB_DP��ѹֵ
#define USB_DM_SetVol(vol) Dac1_Set_Vol(vol)	//����USB_DM��ѹֵ

/* �������ṹ�� */
typedef struct
{
	float Measured_USB_Voltage;	//��⵽��USB�ڵ�ѹ
	float Measured_USB_Current;	//��⵽��USB�ڵ���
	float Measured_USB_Power;		//��⵽��USB�ڹ���
	float Measured_Ripple_Val;	//��⵽��USB�Ʋ���ѹƽ��ֵ
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

#define DropVoltage_Proportion 0.3f		//���������ѹռ��
#define OCP_DAC_Step 100							//�������DAC���ò���

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


/**
 * ��ȡADC����ֵ
 * @param  u32[3]
 * @return
 * @brief 
 **/
void ADCValue_Dealwith(u32 Measure_Vol[3]);

/**
 * ��ȡ�豸���в���ֵ
 * @param  MeasuredData_Type*
 * @return
 * @brief 
 **/
void Get_Power_Val_All(MeasuredData_Type *pp);

/**
 * ��ȡ��Դ��ѹֵ
 * @param   
 * @return float[0-30000]
 * @brief 
 **/
float Get_PowerVoltage(void);

/**
 * ��ȡ��Դ����ֵ
 * @param   
 * @return float[0-3000]
 * @brief 
 **/
float Get_PowerCurrent(void);

/**
 * ��ȡ��Դ�Ʋ���ѹֵ
 * @param   
 * @return float[0-3000]
 * @brief 
 **/
float Get_PowerRipple(void);

/**
 * ��ȡADCУ��ƫ��ֵ
 * @param   
 * @return
 * @brief 
 **/
void Get_Measured_Offset(void);

/**
 * ��Դ��ѹ������,���ڲⶨ����������
 * @param   
 * @return
 * @brief 
 **/
u8 OverCurrent_DetectionVoltage(OCP_Type *pp);

/**
 * MTK-PE�յ� ���������ѹ
 * @param  Voltage_level ���������ڵ�ѹ��λ 1:2:3		5->7	5-9		5->12
 * @return
 * @brief ֧�ֵ�ѹ 5V 7V 9V 12V
 **/
void MTK_IncreaseVoltage(u8 Voltage_level);
/**
 * MTK-PE�յ� ���������ѹ
 * @param Voltage_level ���������ڵ�ѹ��λ 1:2:3		5->7	5-9		5->12
 * @return
 * @brief ֧�ֵ�ѹ 5V 7V 9V 12V
 **/
void MTK_DecreaseVoltage(u8 Voltage_level);

/**
 * QCЭ���յ���ʼ��
 * @param   
 * @return 
 * @brief 
 **/
u8 QC_Init(void);

/**
 * ʧ��QCЭ���յ�
 * @param   
 * @return 
 * @brief 
 **/
void QC_DisInit(void);

/**
 * QC-2.0 �յ�
 * @param  Voltage_level[0-6]
 * @return 
 * @brief 
 **/
void QC20_AdjustVoltage(u8 Voltage_level);

/**
 * ����յ�����
 * @param  Detected_level[�յ����� |0|0|0|0|MTK-PE��Ч��־|QC2.0��Ч��־|QC3.0��Ч��־|��ϸ�յ���ѹ| ��1Ϊ����]
 * @return Results[����� |MTK-PE��Ч��־|QC2.0��Ч��־|QC3.0��Ч��־|PE-7V|PE-9V|PE-12V|QC-9V|QC-12V| ]
 * @brief �յ�����˵�� |���յ� Detected_level=0|������Ƿ��п�书�� Detected_level=0x0E|���MTK-PE������յ�������ĵ�ѹֵ Detected_level=0x09
 */
u8 QuickCharge_Induction(u8 QuickCharge_Type);

#endif

