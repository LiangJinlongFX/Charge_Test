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
 * 获取USB电源电压
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
 * 获取USB电源电流值
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
 * 获取USB电源纹波电压
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
 * MTK-PE诱导 增大输出电压
 * @param Voltage_level[电压级别]
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
 * MTK-PE诱导  减少输出电压
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
 * QC2.0 QC3.0识别
 * return 0:支持QC2.0
 */
u8 QC_Init()
{
	u16 temp_Val;
	u16 i=0;
	
	USB_DP_SetVol(600);				//D+加0.6V电压
	do{
		temp_Val=Get_Adc2(7);		//获取D-电压
		if(temp_Val>730&&temp_Val<760) break;		//D-与D+断开, D-恢复0电压
		rt_thread_delay(2);
		i++;
	}while(i<500);
	if(temp_Val<730||temp_Val>760) return 1;	//D-跟随D+失败
	temp_Val=0;
	i=0;
	do{
		temp_Val=Get_Adc2(7);	//获取D-电压
		if(temp_Val<100) return 0;	//D-与D+断开, D-恢复0电压
		rt_thread_delay(2);
		i++;
	}while(i<500);
	
	return 2;
}

/**
 * 去初始化QCD+/D-设置的电压
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
 * QC电压调整
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
 * 快充诱导函数
* @param  Detected_level[诱导级别 |0|0|0|0|MTK-PE有效标志|QC2.0有效标志|QC3.0有效标志|详细诱导电压| 置1为激活]
 * @return Results[检测结果 |MTK-PE有效标志|QC2.0有效标志|QC3.0有效标志|PE-7V|PE-9V|PE-12V|QC-9V|QC-12V| ]
 * @brief 诱导设置说明 |不诱导 Detected_level=0|仅检测是否有快充功能 Detected_level=0x0E|检测MTK-PE并检测诱导出具体的电压值 Detected_level=0x09
 */
u8 QuickCharge_Induction(u8 Detected_level)
{
	float PowerVoltage=0;
	u8 Results=0;
	u16 i=0;
	
	if(Detected_level==0) return Results;
	
	if(Detected_level&0x08)
	{
		//MTK-PE诱导
		logging_debug("detected PE-7V");
		MTK_IncreaseVoltage(1);
		do{
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>6.5&&PowerVoltage<7.5)
			{
				logging_debug("MTK-PE - 7V");
				Results = Results|0x80;	//诱导MTK-PE有效
				Results = Results|0x10; //将QC-9V有效位置位
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
					Results = Results|0x08; //将QC-9V有效位置位
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
					Results = Results|0x04; //将QC-9V有效位置位
					break;
				}
				rt_thread_delay(20);
				i++;
			}while(i<50);
		}
		SW=1;	//断开电子负载
	}
	
	//QC诱导
	if(Detected_level&0x04||Detected_level&0x02)
	{
		if(QC_Init())
		{
			logging_debug("QC cannot be detected or maybe QC_Init!!!");
			Results = Results & 0xbf;		//将QC2.0有效位至零
		}
		logging_debug("detected QC2.0-9V");
		rt_thread_delay(500);
		QC20_AdjustVoltage(1);	//QC2.0诱导9V
		i=0;
		do{
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>8.5&&PowerVoltage<9.5)
			{
				logging_debug("QC2.0 - 9V");
				Results = Results|0x40; //将QC2.0有效位置位
				Results = Results|0x02; //将QC-9V有效位置位
				break;
			}
			rt_thread_delay(20);
			i++;
		}while(i<50);
		if((Detected_level&0x01)&&(Results&0x40))
		{
			logging_debug("detected QC2.0-12V");
			QC20_AdjustVoltage(2);	//QC2.0诱导12V
			i=0;
			do{
				PowerVoltage = Get_PowerVoltage();
				if(PowerVoltage>11.5&&PowerVoltage<12.5)
				{
					logging_debug("QC2.0 - 12V");
					Results = Results|0x01; //将QC-9V有效位置位
					break;
				}
				rt_thread_delay(20);
				i++;
			}while(i<50);
		}
		if((Detected_level&0x02)&&(Results&0x40))
		{
			logging_debug("detected QC3.0");
			QC20_AdjustVoltage(0);	//复位至5V
			rt_thread_delay(100);
			QC20_AdjustVoltage(4);	//进入QC连续调整模式(QC3.0)
			rt_thread_delay(100);
			//增加电压至6V
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			QC20_AdjustVoltage(5);
			PowerVoltage = Get_PowerVoltage();
			if(PowerVoltage>5.5&&PowerVoltage<6.5)
			{
				logging_debug("QC3.0");
				Results = Results|0x20; //将QC3.0有效位置位
			}
		}
		QC_DisInit();
	}
		
	return Results;
}
