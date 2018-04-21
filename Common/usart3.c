#include "usart3.h"
#include "sys.h"
#include "dma.h"
#include "rtthread.h"
#include "string.h"


u8 USART3_RX_BUF[100];	//����3���ջ�������
u8 USART3_RX_Size;			//����3��ǰ���յ��ֽ���
u8 USART3_RX_Flag;			//����3������ɱ�־


void uart3_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	
	//USART3�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PA9��PA10

  //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;	//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//ֻ����ģʽ
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3	
	
	USART_ClearFlag(USART3, USART_FLAG_TC);	
	
	/* ���ô���3�ж� */
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* �жϿ������� */
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART3,USART_IT_TXE,DISABLE);  
	USART_ITConfig(USART3,USART_IT_IDLE,DISABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);    

	USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
	
	//���ô���3DMA����
	//MYDMA_Config(DMA1_Stream1,DMA_Channel_4,(u32)&USART3->DR,(u32)USART3_RX_BUF,50);
	
	//USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	//ʹ�ܴ���3DMA����
	
}

void USART3_IRQHandler(void)                	//����3�жϷ������
{
	/* ֪ͨRTT�����ж� */
	rt_interrupt_enter();
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!= RESET)
	{       
			if(USART3_RX_Size<=24)
			{
					USART3_RX_BUF[USART3_RX_Size++]=USART_ReceiveData(USART3);    //����һ�ֽ����ݴ���������
			}
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  //�����ж�
	{
		USART_ReceiveData(USART3);//��ȡ���� ע�⣺������Ҫ,�����ܹ�����жϱ�־λ��
		USART_ClearFlag(USART3,USART_FLAG_IDLE);
		if(USART3_RX_Size>=24) USART3_RX_Flag=1;
	}
	/* ֪ͨRTT�˳��ж� */
	rt_interrupt_leave();
}