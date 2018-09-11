/**
  * @file		TLC5615.c   
  * @author  Liang
  * @version V1.0.0
  * @date    2018-9-10
  * @brief		TLC5615��������
  **/
#include "tlc5615.h"
#include "spi.h"

/**
 * TLC5615��ʼ��
 * @param   
 * @return 
 * @brief ��ʼ��TLC5615 SPI�˿�	ʹ��SPI3
 */
void TLC5615_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         							//CS��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                         //��ʼ��GPIO
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14); 
	
	SPI3_Init();
	
	SPI3_SetSpeed(6);
}

/**
 * ����TLC5615�����ѹ
 * @param  ��ѹֵ[0-65535]
 * @return 
 * @brief 
 */
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
