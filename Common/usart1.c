/**
  ******************************************************************************
  * @file    STM32F407DEMO/usart1.c 
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief   
  ******************************************************************************
  * @attention
	* 加入是否支持UCOS宏定义
	* 支持适应不同频率下的串口波特率设置.
	*	加入了对printf的支持
	* 增加了串口接收命令功能.
	* 修正了printf第一个字符丢失的bug
	* 修改串口初始化IO的bug
	* 修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
	* 增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
	* 修改了EN_USART1_RX的使能方式
	* 
  ******************************************************************************
**/

/* include-------------------------------------------------- */
#include "sys.h"
#include "usart1.h"	
#include "rtthread.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
}  
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.


//接收状态
u8 HMI_RX_FLAG=0;       //接收状态标记
u8 UASRT1_RX_BUFFER_LEN=0;  //有效字符长度
u8 USART1_0XFF_FLAG=0;	//oxff计数
//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


void HMI_Send_TXEND(void)
{
	USART_SendData(USART1,0xff);
	while(RESET == USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	USART_SendData(USART1,0xff);
	while(RESET == USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	USART_SendData(USART1,0xff);
	while(RESET == USART_GetFlagStatus(USART1,USART_FLAG_TXE));
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0xff 0xff 0xff结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		//rt_kprintf("%x\r\n",Res);
			if(Res==0xff)
			{
				//成功接收到3个0XFF,该语句接收完毕
				if(USART1_0XFF_FLAG==2) 
				{

					HMI_RX_FLAG=1;	//语句接收结束标志置位
					//rt_kprintf("OK\r\n");
				}
				//非终止情况下接收到0xff
				USART_RX_BUF[UASRT1_RX_BUFFER_LEN]=Res;	//放入缓存数组
				USART1_0XFF_FLAG=1;	//0xff计数
				//当前一个数值也为0xff,计数为2
				if(USART_RX_BUF[UASRT1_RX_BUFFER_LEN-1]==0xff) USART1_0XFF_FLAG=2;
			}
			else
				USART_RX_BUF[UASRT1_RX_BUFFER_LEN]=Res;
			UASRT1_RX_BUFFER_LEN++;	
  }
} 
#endif	

