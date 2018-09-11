/**
  * @file		TLC5615.c   
  * @author  Liang
  * @version V1.0.0
  * @date    2018-9-10
  * @brief		TLC5615驱动程序
  **/
#include "tlc5615.h"
#include "spi.h"

/**
 * TLC5615初始化
 * @param   
 * @return 
 * @brief 初始化TLC5615 SPI端口	使用SPI3
 */
void TLC5615_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         							//CS对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);                         //初始化GPIO
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14); 
	
	SPI3_Init();
	
	SPI3_SetSpeed(6);
}

/**
 * 设置TLC5615输出电压
 * @param  电压值[0-65535]
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
