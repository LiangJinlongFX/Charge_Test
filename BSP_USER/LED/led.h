#ifndef __LED_H
#define __LED_H
/**
  * @file    led.c
  * @author  Liang
  * @version V1.0.0
  * @date    2017-4-26
  * @brief	 GPIO����LED
  **/

//LED�˿ڶ���
#define SYS_LED PCout(2)	//ϵͳ״̬LED	 
#define LED1 PCout(3)
#define LED2 PCout(4)
#define LED3 PCout(5)	

//��������
void LED_Init(void);//��ʼ��

#endif
