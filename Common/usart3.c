#include "usart3.h"
#include "sys.h"
#include "dma.h"
#include "rtthread.h"
#include "string.h"


u8 USART3_RX_BUF[100];	//串口3接收缓存数组
u8 USART3_RX_Size;			//串口3当前接收的字节数
u8 USART3_RX_Flag;			//串口3接收完成标志


void uart3_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	
	//USART3端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PA9，PA10

  //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;	//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//只接收模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3	
	
	USART_ClearFlag(USART3, USART_FLAG_TC);	
	
	/* 配置串口3中断 */
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 中断开关设置 */
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART3,USART_IT_TXE,DISABLE);  
	USART_ITConfig(USART3,USART_IT_IDLE,DISABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);    

	USART_Cmd(USART3, ENABLE);  //使能串口3
	
	//配置串口3DMA接收
	//MYDMA_Config(DMA1_Stream1,DMA_Channel_4,(u32)&USART3->DR,(u32)USART3_RX_BUF,50);
	
	//USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	//使能串口3DMA接收
	
}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	/* 通知RTT进入中断 */
	rt_interrupt_enter();
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!= RESET)
	{       
			if(USART3_RX_Size<=24)
			{
					USART3_RX_BUF[USART3_RX_Size++]=USART_ReceiveData(USART3);    //接收一字节数据存入数组中
			}
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  //空闲中断
	{
		USART_ReceiveData(USART3);//读取数据 注意：这句必须要,否则不能够清除中断标志位。
		USART_ClearFlag(USART3,USART_FLAG_IDLE);
		if(USART3_RX_Size>=24) USART3_RX_Flag=1;
	}
	/* 通知RTT退出中断 */
	rt_interrupt_leave();
}