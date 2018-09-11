#include "sys.h"
#include "Data_Math.h"
#include "math.h" 
#include "arm_math.h"
#include "adc.h"
#include "switch.h"
#include "delay.h"
#include "rtthread.h"
#include "debug.h"


/**
 * ��ȡUSB��Դ��ѹ
 * @param   
 * @return 
 * @brief 
 */
float Get_PowerVoltage(void)
{
	u16 Adc_Val;
	float PowerVoltage=0;
	
	Adc_Val=Get_Adc_Average(2,8,20);
	PowerVoltage=(float)Adc_Val*10*3.3f/4096;
	
	return PowerVoltage;	
}

/**
 * ��ȡUSB��Դ����ֵ
 * @param   
 * @return 
 * @brief 
 */
float Get_PowerCurrent(void)
{
	u16 Adc_Val;
	float PowerCurrent=0;
	Adc_Val=Get_Adc_Average(2,9,30);
	PowerCurrent=(float_t)Adc_Val*3.3f/4096;
	
	return PowerCurrent;
}

/**
 * ��ȡUSB��Դ�Ʋ���ѹ
 * @param   
 * @return 
 * @brief 
 */
float Get_PowerRipple(void)
{
	u16 Adc_Val;
	u16 PowerRipple;
	
	Adc_Val=Get_Adc_Average(1,1,30);
	PowerRipple=(float)Adc_Val*3.3f/4096;
	
	return PowerRipple;
}

/**
 * MTK-PE�յ� ���������ѹ
 * @param Voltage_level[��ѹ����]
 * @return 
 * @brief 
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
	u16 i=0;
	
	USB_DP_SetVol(600);				//D+��0.6V��ѹ
	do{
		temp_Val=Get_Adc2(7);		//��ȡD-��ѹ
		if(temp_Val>730&&temp_Val<760) break;		//D-��D+�Ͽ�, D-�ָ�0��ѹ
		rt_thread_delay(2);
		i++;
	}while(i<500);
	if(temp_Val<730||temp_Val>760) return 1;	//D-����D+ʧ��
	temp_Val=0;
	i=0;
	do{
		temp_Val=Get_Adc2(7);	//��ȡD-��ѹ
		if(temp_Val<100) return 0;	//D-��D+�Ͽ�, D-�ָ�0��ѹ
		rt_thread_delay(2);
		i++;
	}while(i<500);
	
	return 2;
}

/**
 * ȥ��ʼ��QCD+/D-���õĵ�ѹ
 * @param   
 * @return 
 * @brief 
 */
void QC_DisInit(void)
{
	USB_DP_SetVol(0);
	USB_DM_SetVol(0);
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

/**
 * ����յ�����
* @param  Detected_level[�յ����� |0|0|0|0|MTK-PE��Ч��־|QC2.0��Ч��־|QC3.0��Ч��־|��ϸ�յ���ѹ| ��1Ϊ����]
 * @return Results[����� |MTK-PE��Ч��־|QC2.0��Ч��־|QC3.0��Ч��־|PE-7V|PE-9V|PE-12V|QC-9V|QC-12V| ]
 * @brief �յ�����˵�� |���յ� Detected_level=0|������Ƿ��п�书�� Detected_level=0x0E|���MTK-PE������յ�������ĵ�ѹֵ Detected_level=0x09
 */
u8 QuickCharge_Induction(u8 Detected_level)
{
	float PowerVoltage=0;
	u8 Results=0;
	u16 i=0;
	
	if(Detected_level==0) return Results;
	
	if(Detected_level&0x08)
	{
		//MTK-PE�յ�
		logging_debug("detected PE-7V");
		MTK_IncreaseVoltage(1);
		do{
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>6.5&&PowerVoltage<7.5)
			{
				logging_debug("MTK-PE - 7V");
				Results = Results|0x80;	//�յ�MTK-PE��Ч
				Results = Results|0x10; //��QC-9V��Чλ��λ
				break;
			}
			rt_thread_delay(20);
			i++;
		}while(i<50);
		if(Detected_level&0x01)
		{
			logging_debug("detected PE-9V");
			MTK_IncreaseVoltage(1);
			i=0;
			do{
				PowerVoltage = Get_PowerVoltage();
				if(PowerVoltage>8.5&&PowerVoltage<9.5)
				{
					logging_debug("MTK-PE - 9V");
					Results = Results|0x08; //��QC-9V��Чλ��λ
					break;
				}
				rt_thread_delay(20);
				i++;
			}while(i<50);
			logging_debug("detected PE-12V");
			MTK_IncreaseVoltage(1);
			i=0;
			do{
				PowerVoltage = Get_PowerVoltage();
				if(PowerVoltage>11.5&&PowerVoltage<12.5)
				{
					logging_debug("MTK-PE - 12V");
					Results = Results|0x04; //��QC-9V��Чλ��λ
					break;
				}
				rt_thread_delay(20);
				i++;
			}while(i<50);
		}
		SW=1;	//�Ͽ����Ӹ���
	}
	
	//QC�յ�
	if(Detected_level&0x04||Detected_level&0x02)
	{
		if(QC_Init())
		{
			logging_debug("QC cannot be detected or maybe QC_Init!!!");
			Results = Results & 0xbf;		//��QC2.0��Чλ����
		}
		logging_debug("detected QC2.0-9V");
		rt_thread_delay(500);
		QC20_AdjustVoltage(1);	//QC2.0�յ�9V
		i=0;
		do{
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>8.5&&PowerVoltage<9.5)
			{
				logging_debug("QC2.0 - 9V");
				Results = Results|0x40; //��QC2.0��Чλ��λ
				Results = Results|0x02; //��QC-9V��Чλ��λ
				break;
			}
			rt_thread_delay(20);
			i++;
		}while(i<50);
		if((Detected_level&0x01)&&(Results&0x40))
		{
			logging_debug("detected QC2.0-12V");
			QC20_AdjustVoltage(2);	//QC2.0�յ�12V
			i=0;
			do{
				PowerVoltage = Get_PowerVoltage();
				if(PowerVoltage>11.5&&PowerVoltage<12.5)
				{
					logging_debug("QC2.0 - 12V");
					Results = Results|0x01; //��QC-9V��Чλ��λ
					break;
				}
				rt_thread_delay(20);
				i++;
			}while(i<50);
		}
		if((Detected_level&0x02)&&(Results&0x40))
		{
			logging_debug("detected QC3.0");
			QC20_AdjustVoltage(0);	//��λ��5V
			rt_thread_delay(100);
			QC20_AdjustVoltage(4);	//����QC��������ģʽ(QC3.0)
			rt_thread_delay(100);
			//���ӵ�ѹ��6V
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>5.5&&PowerVoltage<6.5)
			{
				logging_debug("QC3.0");
				Results = Results|0x20; //��QC3.0��Чλ��λ
			}
		}
		QC_DisInit();
	}
		
	return Results;
}
