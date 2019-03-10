#include "sys.h"
#include "Data_Math.h"
#include "math.h" 
#include "arm_math.h"
#include "adc.h"
#include "switch.h"
#include "delay.h"
#include "rtthread.h"
#include "debug.h"
#include "hlw8032.h"

/* 偏差值 */
float Measured_Voltage_Offset=0;
float	Measured_Current_Offset=0;
float Measured_Ripple_Offset=0;

/**
 * 获取交流输入电压/电流值
 * @param   
 * @return 
 * @brief 
 **/
void Get_ACPower(void)
{
	while(1);
}

/**
 * ADC DMA传输序列分类处理
 * @param   
 * @return 
 * @brief 依次接收 USB电压/USB电流/USB纹波峰值
 * 共60个数据,各20个
 **/
void ADCValue_Dealwith(u32 Measure_Vol[3])
{
	u8 i;
	
	u32 Voltage_Max=0;
	u32 Voltage_Min=0;
	u32 Current_Max=0;
	u32 Current_Min=0;

	u32 Voltage_Val=0;
	u32 Current_Val=0;
	u32 Ripple_Val=0;
	u32 Voltage_Average=0;
	u32 Current_Average=0;
	u32 Ripple_Average=0;
	
	/* ADC检测并等待完成 */
	Get_ADC_DualMode();
	while(!ADC12_Convert_Flag);
	/* 找出最值并求和 */
	for(i=0;i<ADC12_BUFFER_SIZE/3;i++)
	{
		if(i==0)
		{
			Voltage_Max=Voltage_Min = ADC12ConvertedValue[3*i+1];
			Current_Max=Current_Min = ADC12ConvertedValue[3*i+2];
		}
		if(Voltage_Min>ADC12ConvertedValue[3*i+1]) Voltage_Min=ADC12ConvertedValue[3*i+1];
		if(Voltage_Max<ADC12ConvertedValue[3*i+1]) Voltage_Max=ADC12ConvertedValue[3*i+1];
		if(Current_Min>ADC12ConvertedValue[3*i+2]) Current_Min=ADC12ConvertedValue[3*i+2];
		if(Current_Max<ADC12ConvertedValue[3*i+2]) Current_Max=ADC12ConvertedValue[3*i+1];
		Ripple_Val+=ADC12ConvertedValue[3*i];
		Voltage_Val+=ADC12ConvertedValue[3*i+1];
		Current_Val+=ADC12ConvertedValue[3*i+2];
	}
	/* 去掉最值后求平均值 */
	Voltage_Average=(Voltage_Val-(Voltage_Max+Voltage_Min))/(ADC12_BUFFER_SIZE/3-2);
	Current_Average=(Current_Val-(Current_Max+Current_Min))/(ADC12_BUFFER_SIZE/3-2);
	Ripple_Average=Ripple_Val/(ADC12_BUFFER_SIZE/3);
	
	//logging_debug("%d %d %d\r\n",Voltage_Average,Current_Average,Ripple_Average);
	
	/* 赋值给外部参数 */
	Measure_Vol[0]=Voltage_Average;
	Measure_Vol[1]=Current_Average;
	Measure_Vol[2]=Ripple_Average;
		
}

/**
 * 获取测试完整参数
 * @param   
 * @return 
 * @brief 
 **/
void Get_Power_Val_All(MeasuredData_Type *pp)
{
	u32 Adc_Val[3];
	float Val=0;
	HLW8032Data_Type HLW8032Data_StructureData;
	
	ADCValue_Dealwith(Adc_Val);
	
	// 计算USB口电压
	Val=(float)(Adc_Val[0])*3300/4096;
	pp->Measured_USB_Voltage=Measured_Volteage_K*Val-Measured_Voltage_Offset;
	if(pp->Measured_USB_Voltage<0) pp->Measured_USB_Voltage=0;
	// 计算USB口电流
	Val=(float)(Adc_Val[1])*3300/4096;
	pp->Measured_USB_Current=Measured_Current_K*Val-Measured_Current_Offset;
	if(pp->Measured_USB_Current<0) pp->Measured_USB_Current=0;
	// 计算USB口耗散功率
	pp->Measured_USB_Power=pp->Measured_USB_Voltage*pp->Measured_USB_Current;
	// 获取交流检测参数
	if(!HLW8032Get_Data(&HLW8032Data_StructureData))
	{
		pp->Measured_AC_Voltage = HLW8032Data_StructureData.AC_Voltage;
		pp->Measured_AC_Current = HLW8032Data_StructureData.AC_Current;
		pp->Measured_AC_Power = HLW8032Data_StructureData.AC_Power;
	}
	else
	{
		pp->Measured_AC_Voltage = 0;
		pp->Measured_AC_Current = 0;
		pp->Measured_AC_Power = 0;		
	}
	// 调试输出
	logging_debug("Measured_USB_Voltage=%dmV\r\n",(u32)pp->Measured_USB_Voltage);
	logging_debug("Measured_USB_Current=%dmA\r\n",(u32)pp->Measured_USB_Current);
	logging_debug("Measured_USB_Power=%dmW\r\n",(u32)pp->Measured_USB_Power);
	logging_debug("Measured_AC_Voltage=%d\r\n",(u32)pp->Measured_AC_Voltage);
	logging_debug("Measured_AC_Current=%d\r\n",(u32)pp->Measured_AC_Current);
	logging_debug("Measured_AC_Power=%d\r\n",(u32)pp->Measured_AC_Power);
	
}


/**
 * 获取USB电源电压
 * @param   
 * @return 
 * @brief 
 */
float Get_PowerVoltage(void)
{
	u32 Adc_Val[3];
	float PowerVoltage=0;
	
	ADCValue_Dealwith(Adc_Val);
	
	PowerVoltage=(float)Adc_Val[0]*3300/4096;	
	PowerVoltage=Measured_Volteage_K*PowerVoltage-Measured_Voltage_Offset;
	if(PowerVoltage<0) PowerVoltage=0;
		
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
	u32 Adc_Val[3];
	int32_t Calibrated_Value[3]; 
	float PowerCurrent=0;
	
	ADCValue_Dealwith(Adc_Val);
	PowerCurrent=(float)Adc_Val[1]*3300/4096;	
	PowerCurrent=Measured_Current_K*PowerCurrent-Measured_Current_Offset;
	if(PowerCurrent<0) PowerCurrent=0;
	
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
	float PowerRipple;
	
	Adc_Val=Get_Adc_Average(1,1,1);
	PowerRipple=(float)Adc_Val*3.3f/4096;
	
	return PowerRipple;
}

/**
 * 过流检测电压跌落
 * @param  OCP_Type 
 * @return 0 - 电压仍在正常范围  1 - 电压已跌落
 * @brief 
 **/
u8 OverCurrent_DetectionVoltage(OCP_Type *pp)
{
	u32 Adc_Val[3];
	float Voltage_Value;
	float Current_Value;
	
	// 获取ADC检测值
	ADCValue_Dealwith(Adc_Val);
	// 计算USB口电压值
	Voltage_Value=(float)Adc_Val[0]*3300/4096;
	Voltage_Value=Measured_Volteage_K*Voltage_Value-Measured_Voltage_Offset;
	if(Voltage_Value<0) Voltage_Value=0;
	// 计算USB口电流值
	Current_Value=(float)Adc_Val[1]*3300/4096;
	Current_Value=Measured_Current_K*Current_Value-Measured_Current_Offset;
	if(Current_Value<0) Current_Value=0;
	// 检测电压跌落情况
	logging_debug("%d %d",(u32)Voltage_Value,(u32)Current_Value);
	if(Voltage_Value<=(pp->Drop_Voltage))
		return 1;
	else
	{
		pp->Current_Max = Current_Value;
		return 0;
	}
}

/**
 * 获取设备校验用的偏差值(检测时不应接入检测设备)
 * @param   
 * @return 
 * @brief 
 **/
void Get_Measured_Offset(void)
{
	u32 Vol[3];
	
	ADCValue_Dealwith(Vol);
	Measured_Voltage_Offset=Vol[0];
	Measured_Current_Offset=Vol[1];
	Measured_Ripple_Offset=Vol[2];
	
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
		//temp_Val=Get_Adc2(7);		//获取D-电压
		if(temp_Val>730&&temp_Val<760) break;		//D-与D+断开, D-恢复0电压
		rt_thread_delay(2);
		i++;
	}while(i<500);
	if(temp_Val<730||temp_Val>760) return 1;	//D-跟随D+失败
	temp_Val=0;
	i=0;
	do{
		//temp_Val=Get_Adc2(7);	//获取D-电压
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
			if((PowerVoltage>6500)&&(PowerVoltage<7500))
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
				if((PowerVoltage>8500)&&(PowerVoltage<9500))
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
				if((PowerVoltage>10500)&&(PowerVoltage<12500))
				{
					logging_debug("MTK-PE - 12V");
					Results = Results|0x04; //将QC-12V有效位置位
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
		logging_debug("Start QC!!!");
		if(QC_Init())
		{
//			logging_debug("QC cannot be detected or maybe QC_Init!!!");
//			Results = Results & 0xbf;		//将QC2.0有效位至零
//			return Results;
		}
		logging_debug("detected QC2.0-9V");
		rt_thread_delay(500);
		QC20_AdjustVoltage(1);	//QC2.0诱导9V
		i=0;
		do{
			PowerVoltage = Get_PowerVoltage();
			logging_debug("PowerVoltage: %d",(u32)PowerVoltage);
			if((PowerVoltage>8500)&&(PowerVoltage<9500))
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
				if((PowerVoltage>11500)&&(PowerVoltage<12500))
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
			if((PowerVoltage>5500)&&(PowerVoltage<6500))
			{
				logging_debug("QC3.0");
				Results = Results|0x20; //将QC3.0有效位置位
			}
		}
		QC_DisInit();
	}
		
	return Results;
}
