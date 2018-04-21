#include "sys.h"
#include "Data_Math.h"
#include "math.h" 
#include "arm_math.h"
#include "adc.h"
#include "switch.h"
#include "delay.h"
#include "rtthread.h"


float Get_PowerVoltage(void)
{
	u16 Adc_Val;
	float PowerVoltage=0;
	
	Adc_Val=Get_Adc_Average(1,8,20);
	rt_kprintf("res=%d\r\n",Adc_Val);
	PowerVoltage=(float)Adc_Val*3.3f/4096;
	
	
	return PowerVoltage;	
}

float Get_PowerCurrent(void)
{
	u16 Adc_Val;
	float PowerCurrent=0;
	Adc_Val=Get_Adc_Average(2,9,30);
	PowerCurrent=(float_t)Adc_Val*3.3f/4096;
	
	return PowerCurrent;
}

float Get_PowerRipple(void)
{
	u16 Adc_Val;
	u16 PowerRipple;
	
	Adc_Val=Get_Adc_Average(1,1,30);
	PowerRipple=(float)Adc_Val*3.3f/4096;
	
	
	return PowerRipple;
}

/*
 * MTK-PE�յ�  ���������ѹ
 */
void MTK_IncreaseVoltage(u8 Voltage_level)
{
	u8 i;

	if(Voltage_level==0||Voltage_level>3) return;
	
	for(i=0;i<Voltage_level;i++)
	{
		SW=0;
		delay_ms(200);
		SW=1;
		delay_ms(100);
		
		/* 1 */
		SW=0;
		delay_ms(100);
		SW=1;
		delay_ms(100);
		/* 2 */
		SW=0;
		delay_ms(100);
		SW=1;
		delay_ms(100);
		/* 3 */
		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);
		/* 4 */
		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);
		/* 5 */
		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);
		/* 6 */
		SW=0;
		delay_ms(500);
		SW=1;
		delay_ms(100);
	}
		/* keep */
	SW=0;		
}

/*
 * MTK-PE�յ�  ���������ѹ
 */
void MTK_DecreaseVoltage(u8 Voltage_level)
{
	u8 i=0;
	
	for(i=0;i<Voltage_level;i++)
	{
		SW=0;
		delay_ms(200);
		SW=1;
		delay_ms(100);
		
		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);
		
		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);

		SW=0;
		delay_ms(300);
		SW=1;
		delay_ms(100);
		
		/* 1 */
		SW=0;
		delay_ms(100);
		SW=1;
		delay_ms(100);
		/* 2 */
		SW=0;
		delay_ms(100);
		SW=1;
		delay_ms(100);		
		
		SW=0;
		delay_ms(500);
		SW=1;
		delay_ms(100);
	}
	/* keep */
	SW=0;
}


/*
 * QC2.0 QC3.0ʶ��
 * return 0:֧��QC2.0
 */
u8 QC_Init()
{
	u16 temp_Val;
	
	USB_DP_SetVol(600);	//D+��0.6V��ѹ
	temp_Val=Get_Adc2(6);	//��ȡD-��ѹ
	if(temp_Val<740||temp_Val>750) return 1;	//D-����D+ʧ��
	delay_ms(1300);	//�ȴ�������Ͽ�D+ D-����
	temp_Val=Get_Adc2(6);	//��ȡD-��ѹ
	if(temp_Val>100) return 2;	//D-����Ϊ0,��֧��QC2.0
	
	return 0;
}

/*
 * QC��ѹ����
 */
void QC20_AdjustVoltage(u8 Voltage_level)
{
	switch(Voltage_level)
	{
		case 0:{USB_DP_SetVol(600);USB_DM_SetVol(0);}break;				//5V
		case 1:{USB_DP_SetVol(3299);USB_DM_SetVol(600);}break;		//9V
		case 2:{USB_DP_SetVol(600);USB_DM_SetVol(600);}break;			//12V
		case 3:{USB_DP_SetVol(3299);USB_DM_SetVol(3299);}break;		//20V
		case 4:{USB_DP_SetVol(600);USB_DM_SetVol(3299);}break;		//continuous mode
		case 5:{USB_DP_SetVol(600);delay_ms(100);USB_DP_SetVol(3299);delay_ms(100);USB_DP_SetVol(600);}break;			//continuous mode  V+
		case 6:{USB_DM_SetVol(3299);delay_ms(100);USB_DM_SetVol(600);delay_ms(100);USB_DM_SetVol(3299);}break;		//continuous mode  V-
		default:break;
	}
}
