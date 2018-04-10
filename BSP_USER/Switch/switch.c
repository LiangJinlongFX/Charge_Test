/**********************************************************************************
* �ļ�����: led.c                                                                *
* �ļ����������ڿ��ص�IO��                                                            *
* �������ڣ�2015.10.03                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    �������ڿ��ص�IO�ڳ�ʼ��                                                    * 
**********************************************************************************
//������ʵ���ҿ�����LED�ӿ�
*********************************************************************************/
#include "sys.h"
#include "switch.h"

//���ؿ��ض�ӦIO��ʼ��
void Switch_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//ʹ��GPIOBʱ��

  //PB13��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
  GPIO_Init(GPIOC, &GPIO_InitStructure);                         //��ʼ��GPIO
	
	GPIO_SetBits(GPIOC, GPIO_Pin_0);   	
}

