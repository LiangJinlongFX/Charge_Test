/**
  * @file    led.c
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 GPIO����LED
  **/
#include "led.h"
#include "sys.h"

/**
 * LED���ų�ʼ��
 * @param   
 * @return 
 * @brief 
 **/
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//ʹ��GPIOCʱ��

  //GPIO��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
  GPIO_Init(GPIOC, &GPIO_InitStructure);                         //��ʼ��GPIO
	
	/* Ĭ��Ϊ����״̬ */
	GPIO_ResetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);   	
}