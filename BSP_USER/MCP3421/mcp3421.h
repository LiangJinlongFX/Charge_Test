/**
  ******************************************************************************
  * File Name          : mcp3421.c
  * Description        : ����HAL��Ӳ��IIC��MCP3421��������          
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  *	����12��16��18λ��д����
	* Ĭ��Ƭ������Ϊ1 ����ת��ģʽ ���������ѹ
	* ֧�ֶ�iic
  ******************************************************************************
  */

#ifndef _MCP3421_H
#define _MCP3421_H
#include "stm32f4xx.h"

#define MCP3421R_Addr 0xd1
#define MCP3421W_Addr 0xd0

#define MCP3421_CMD_18B 0x9c  //18λ����ת�� PGA*1 3.75sps
#define MCP3421_CMD_16B 0x98	//16λ����ת�� PGA*1 15sps
#define MCP3421_CMD_12B 0x90	//12λ����ת�� PGA*1 240sps (Ĭ��)

//u16 Get_Mcp3421_12BAdc(I2C_HandleTypeDef hi2c);
//u16 Get_Mcp3421_16BAdc(I2C_HandleTypeDef hi2c);
u32 Get_Mcp3421_18BAdc(void);
#endif

