#include "tlc5615.h"
#include "spi.h"

void TLC5615_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         //LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                         //��ʼ��GPIO
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14); 
	
	SPI3_Init();
	
	SPI3_SetSpeed(6);
}


void TLC5615_SetVoltage(u16 val)
{
	u16 temp;
	
	SPI3_CS = 0;
	
	temp=val<<2;
	temp=temp&0xffc;
	
	SPI3_ReadWriteByte(temp&0xff);
	SPI3_ReadWriteByte(temp<<8);
	
	SPI3_CS = 1;
}
