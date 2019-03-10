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
 * 配置定时器4中断
 * @param   
 * @return 
 * @brief 溢出时间计算 Tout = ((arr+1)*(psc+1)) / Ft(us) Ft:定时器工作频率
 **/
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = arr;	//自动重装载数值 arr
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//定时器分频值 psc
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//输入时钟不分频	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);	//使能定时器4更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	//定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;	//响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*************************** 电子负载调节部分 *******************************/

/**
 * 
 * @param   
 * @return 
 * @brief 
 **/
u8 Load_Ctrl(LoadCtrl_TypeDef *pp,float Current_ADCVal)
{
	
	pp->Error = pp->SetPoint - Current_ADCVal;	//计算当前检测值与目标值的误差
	
	// 设置值在误差范围内，返回 0
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
 * 渐进式过流检测函数
 * @param   
 * @return 最大输出电流
 * @brief
 * 检测精度由OCP_DAC_Step步进控制
 * 步进间隔为固定的5ms
 **/
float OverCurrent_Detection(void)
{
	u16 DAC_Val=0;
	float NoLoad_Voltage=0;
	OCP_Type OCP_Structure;
	
	SW = 1;	//确保负载已断开
	// 获取空载电压
	NoLoad_Voltage=Get_PowerVoltage();
	// 计算跌落电压比例
	OCP_Structure.Drop_Voltage=NoLoad_Voltage*DropVoltage_Proportion;
	// 设置DAC步进值
	OCP_Structure.DAC_Step=OCP_DAC_Step;
	// 确保DAC输出电压为0
	TLC5615_SetVoltage(DAC_Val);
	delay_ms(5);
	// 如果没有接入负载,返回0值
	if(NoLoad_Voltage<100)
		return 0;
	// 接通负载,等待上升
	SW = 0;
	delay_ms(5);
	// 逐步加大DAC输出电压,直到电压跌落
	while(1)
	{
		if(OverCurrent_DetectionVoltage(&OCP_Structure))
		{
			SW = 1;	//断开负载
			TLC5615_SetVoltage(0);	//恢复零电压
			return OCP_Structure.Current_Max;
		}
		TLC5615_SetVoltage(DAC_Val+=OCP_Structure.DAC_Step);
		delay_ms(50);
		if(DAC_Val>3000)
			return 0;			
	}
}

/**
 * 定时器4中断服务函数
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

