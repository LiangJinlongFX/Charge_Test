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

/* ƫ��ֵ */
float Measured_Voltage_Offset=0;
float	Measured_Current_Offset=0;
float Measured_Ripple_Offset=0;

/**
 * ��ȡ���������ѹ/����ֵ
 * @param   
 * @return 
 * @brief 
 **/
void Get_ACPower(void)
{
	while(1);
}

/**
 * ADC DMA�������з��ദ��
 * @param   
 * @return 
 * @brief ���ν��� USB��ѹ/USB����/USB�Ʋ���ֵ
 * ��60������,��20��
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
	
	/* ADC��Ⲣ�ȴ���� */
	Get_ADC_DualMode();
	while(!ADC12_Convert_Flag);
	/* �ҳ���ֵ����� */
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
	/* ȥ����ֵ����ƽ��ֵ */
	Voltage_Average=(Voltage_Val-(Voltage_Max+Voltage_Min))/(ADC12_BUFFER_SIZE/3-2);
	Current_Average=(Current_Val-(Current_Max+Current_Min))/(ADC12_BUFFER_SIZE/3-2);
	Ripple_Average=Ripple_Val/(ADC12_BUFFER_SIZE/3);
	
	//logging_debug("%d %d %d\r\n",Voltage_Average,Current_Average,Ripple_Average);
	
	/* ��ֵ���ⲿ���� */
	Measure_Vol[0]=Voltage_Average;
	Measure_Vol[1]=Current_Average;
	Measure_Vol[2]=Ripple_Average;
		
}

/**
 * ��ȡ������������
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
	
	// ����USB�ڵ�ѹ
	Val=(float)(Adc_Val[0])*3300/4096;
	pp->Measured_USB_Voltage=Measured_Volteage_K*Val-Measured_Voltage_Offset;
	if(pp->Measured_USB_Voltage<0) pp->Measured_USB_Voltage=0;
	// ����USB�ڵ���
	Val=(float)(Adc_Val[1])*3300/4096;
	pp->Measured_USB_Current=Measured_Current_K*Val-Measured_Current_Offset;
	if(pp->Measured_USB_Current<0) pp->Measured_USB_Current=0;
	// ����USB�ں�ɢ����
	pp->Measured_USB_Power=pp->Measured_USB_Voltage*pp->Measured_USB_Current;
	// ��ȡ����������
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
	// �������
	logging_debug("Measured_USB_Voltage=%dmV\r\n",(u32)pp->Measured_USB_Voltage);
	logging_debug("Measured_USB_Current=%dmA\r\n",(u32)pp->Measured_USB_Current);
	logging_debug("Measured_USB_Power=%dmW\r\n",(u32)pp->Measured_USB_Power);
	logging_debug("Measured_AC_Voltage=%d\r\n",(u32)pp->Measured_AC_Voltage);
	logging_debug("Measured_AC_Current=%d\r\n",(u32)pp->Measured_AC_Current);
	logging_debug("Measured_AC_Power=%d\r\n",(u32)pp->Measured_AC_Power);
	
}


/**
 * ��ȡUSB��Դ��ѹ
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
 * ��ȡUSB��Դ����ֵ
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
 * ��ȡUSB��Դ�Ʋ���ѹ
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
 * ��������ѹ����
 * @param  OCP_Type 
 * @return 0 - ��ѹ����������Χ  1 - ��ѹ�ѵ���
 * @brief 
 **/
u8 OverCurrent_DetectionVoltage(OCP_Type *pp)
{
	u32 Adc_Val[3];
	float Voltage_Value;
	float Current_Value;
	
	// ��ȡADC���ֵ
	ADCValue_Dealwith(Adc_Val);
	// ����USB�ڵ�ѹֵ
	Voltage_Value=(float)Adc_Val[0]*3300/4096;
	Voltage_Value=Measured_Volteage_K*Voltage_Value-Measured_Voltage_Offset;
	if(Voltage_Value<0) Voltage_Value=0;
	// ����USB�ڵ���ֵ
	Current_Value=(float)Adc_Val[1]*3300/4096;
	Current_Value=Measured_Current_K*Current_Value-Measured_Current_Offset;
	if(Current_Value<0) Current_Value=0;
	// ����ѹ�������
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
 * ��ȡ�豸У���õ�ƫ��ֵ(���ʱ��Ӧ�������豸)
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
		//temp_Val=Get_Adc2(7);		//��ȡD-��ѹ
		if(temp_Val>730&&temp_Val<760) break;		//D-��D+�Ͽ�, D-�ָ�0��ѹ
		rt_thread_delay(2);
		i++;
	}while(i<500);
	if(temp_Val<730||temp_Val>760) return 1;	//D-����D+ʧ��
	temp_Val=0;
	i=0;
	do{
		//temp_Val=Get_Adc2(7);	//��ȡD-��ѹ
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
			if((PowerVoltage>6500)&&(PowerVoltage<7500))
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
				if((PowerVoltage>8500)&&(PowerVoltage<9500))
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
				if((PowerVoltage>10500)&&(PowerVoltage<12500))
				{
					logging_debug("MTK-PE - 12V");
					Results = Results|0x04; //��QC-12V��Чλ��λ
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
		logging_debug("Start QC!!!");
		if(QC_Init())
		{
//			logging_debug("QC cannot be detected or maybe QC_Init!!!");
//			Results = Results & 0xbf;		//��QC2.0��Чλ����
//			return Results;
		}
		logging_debug("detected QC2.0-9V");
		rt_thread_delay(500);
		QC20_AdjustVoltage(1);	//QC2.0�յ�9V
		i=0;
		do{
			PowerVoltage = Get_PowerVoltage();
			logging_debug("PowerVoltage: %d",(u32)PowerVoltage);
			if((PowerVoltage>8500)&&(PowerVoltage<9500))
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
				if((PowerVoltage>11500)&&(PowerVoltage<12500))
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
			if((PowerVoltage>5500)&&(PowerVoltage<6500))
			{
				logging_debug("QC3.0");
				Results = Results|0x20; //��QC3.0��Чλ��λ
			}
		}
		QC_DisInit();
	}
		
	return Results;
}
