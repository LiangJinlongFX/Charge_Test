/**
  ******************************************************************************
  * File Name          : mcp3421.c
  * Description        : 基于HAL库硬件IIC的MCP3421驱动程序          
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  *	具有12，16，18位读写函数
	* 默认片内增益为1 连续转换模式 正向输入电压
	* 支持多iic
  ******************************************************************************
  */

#ifndef _MCP3421_H
#define _MCP3421_H
#include "stm32f4xx.h"

#define MCP3421R_Addr 0xd1
#define MCP3421W_Addr 0xd0

#define MCP3421_CMD_18B 0x9c  //18位连续转换 PGA*1 3.75sps
#define MCP3421_CMD_16B 0x98	//16位连续转换 PGA*1 15sps
#define MCP3421_CMD_12B 0x90	//12位连续转换 PGA*1 240sps (默认)

//u16 Get_Mcp3421_12BAdc(I2C_HandleTypeDef hi2c);
//u16 Get_Mcp3421_16BAdc(I2C_HandleTypeDef hi2c);
u32 Get_Mcp3421_18BAdc(void);
#endif

