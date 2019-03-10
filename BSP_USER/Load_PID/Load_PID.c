#include "Load_PID.h"
#include "stm32f4xx.h"
#include "Data_Math.h"
#include "tlc5615.h"
#include "delay.h"
#include "rtthread.h"
#include "switch.h"

LoadCtrl_TypeDef Global_LoadCtrl_Structure;

static u8 Load_Ctrl(LoadCtrl_TypeDef *pp,float Current_ADCVal);

/**
 * ���ö�ʱ��4�ж�
 * @param   
 * @return 
 * @brief ���ʱ����� Tout = ((arr+1)*(psc+1)) / Ft(us) Ft:��ʱ������Ƶ��
 **/
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = arr;	//�Զ���װ����ֵ arr
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//��ʱ����Ƶֵ psc
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//����ʱ�Ӳ���Ƶ	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);	//ʹ�ܶ�ʱ��4�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	//��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;	//��Ӧ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*************************** ���Ӹ��ص��ڲ��� *******************************/

/**
 * 
 * @param   
 * @return 
 * @brief 
 **/
u8 Load_Ctrl(LoadCtrl_TypeDef *pp,float Current_ADCVal)
{
	
	pp->Error = pp->SetPoint - Current_ADCVal;	//���㵱ǰ���ֵ��Ŀ��ֵ�����
	
	// ����ֵ����Χ�ڣ����� 0
	if(((pp->Error)<(pp->Bias))&&((pp->Error)>(-pp->Bias)))
		return 1;
	
	if((pp->Error)>0)
	{
		(pp->DAC_Val)+=(pp->DAC_Step);
		TLC5615_SetVoltage(pp->DAC_Val);
	}
	else if((pp->Error)<0)
	{
		(pp->DAC_Val)-=(pp->DAC_Step);
		TLC5615_SetVoltage(pp->DAC_Val);
	}
	
	return 0;		
}

void Set_LoadCurrent(float Current_set)
{
	
	Global_LoadCtrl_Structure.SetPoint = Current_set;
	Global_LoadCtrl_Structure.DAC_Step = 10;
	Global_LoadCtrl_Structure.DAC_Val = 0;
  Global_LoadCtrl_Structure.Bias = 3;
	TIM4_Init(50,8400-1);
	
	TIM_Cmd(TIM4, ENABLE);
}

/**
 * ����ʽ������⺯��
 * @param   
 * @return ����������
 * @brief
 * ��⾫����OCP_DAC_Step��������
 * �������Ϊ�̶���5ms
 **/
float OverCurrent_Detection(void)
{
	u16 DAC_Val=0;
	float NoLoad_Voltage=0;
	OCP_Type OCP_Structure;
	
	SW = 1;	//ȷ�������ѶϿ�
	// ��ȡ���ص�ѹ
	NoLoad_Voltage=Get_PowerVoltage();
	// ��������ѹ����
	OCP_Structure.Drop_Voltage=NoLoad_Voltage*DropVoltage_Proportion;
	// ����DAC����ֵ
	OCP_Structure.DAC_Step=OCP_DAC_Step;
	// ȷ��DAC�����ѹΪ0
	TLC5615_SetVoltage(DAC_Val);
	delay_ms(5);
	// ���û�н��븺��,����0ֵ
	if(NoLoad_Voltage<100)
		return 0;
	// ��ͨ����,�ȴ�����
	SW = 0;
	delay_ms(5);
	// �𲽼Ӵ�DAC�����ѹ,ֱ����ѹ����
	while(1)
	{
		if(OverCurrent_DetectionVoltage(&OCP_Structure))
		{
			SW = 1;	//�Ͽ�����
			TLC5615_SetVoltage(0);	//�ָ����ѹ
			return OCP_Structure.Current_Max;
		}
		TLC5615_SetVoltage(DAC_Val+=OCP_Structure.DAC_Step);
		delay_ms(50);
		if(DAC_Val>3000)
			return 0;			
	}
}

/**
 * ��ʱ��4�жϷ�����
 * @param   
 * @return 
 * @brief 
 **/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
	{
		Load_Ctrl(&Global_LoadCtrl_Structure,Get_PowerCurrent());
	}
}

