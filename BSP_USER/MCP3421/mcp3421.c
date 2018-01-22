/**
  ******************************************************************************
  * File Name          : mcp3421.c
  * Description        : 基于STD库硬件IIC的MCP3421驱动程序          
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  *	具有12，16，18位读写函数
	* 默认片内增益为1 连续转换模式 正向输入电压
	* 支持多iic
  ******************************************************************************
  */
#include "myiic.h"
#include "stm32f4xx.h"
#include "mcp3421.h"


u32 Get_Mcp3421_18BAdc(void)
{
	u8 i;
	u8 MCP3421_Data[3];
	u32 VoltageD=0;
	u8 MCP3421_CMD = MCP3421_CMD_18B;
	
	IIC_Start();
	IIC_Send_Byte(MCP3421R_Addr);  //读地址
	IIC_Send_Byte(MCP3421_CMD);	//命令
	IIC_Wait_Ack();
	
	for(i=0;i<3;i++)
	{
		MCP3421_Data[i] = IIC_Read_Byte(1);
	}
	IIC_Stop();
	VoltageD = VoltageD|MCP3421_Data[0];
	VoltageD = VoltageD << 16;
	VoltageD = VoltageD|MCP3421_Data[1];
	VoltageD = VoltageD << 8;
	VoltageD = VoltageD|MCP3421_Data[2];
	VoltageD &= 0x03ffff;
	//VoltageD+=1;
	return VoltageD;
}

//u16 Get_Mcp3421_16BAdc(I2C_HandleTypeDef hi2c)
//{
//	u8 res;
//	u8 MCP3421_Data[2];
//	u16 VoltageD=0;
//	u8 MCP3421_CMD = MCP3421_CMD_16B;
//	
//	res=HAL_I2C_Master_Transmit(&hi2c,MCP3421W_Addr,&MCP3421_CMD,1,10000);
//	if(res) return res;
//	HAL_Delay(100);
//	res=HAL_I2C_Master_Receive(&hi2c,MCP3421R_Addr,MCP3421_Data,3,10000);
//	if(res) return res;
//	VoltageD = VoltageD|MCP3421_Data[0];
//	VoltageD = VoltageD << 8;
//	VoltageD = VoltageD|MCP3421_Data[1];
//	//VoltageD+=1;
//	return VoltageD;
//}

//u16 Get_Mcp3421_12BAdc(I2C_HandleTypeDef hi2c)
//{
//	u8 res;
//	u8 MCP3421_Data[2];
//	u16 VoltageD=0;
//	u8 MCP3421_CMD = MCP3421_CMD_12B;
//	
//	res=HAL_I2C_Master_Transmit(&hi2c,MCP3421W_Addr,&MCP3421_CMD,1,10000);
//	if(res) return 0;
//	HAL_Delay(10);
//	res=HAL_I2C_Master_Receive(&hi2c,MCP3421R_Addr,MCP3421_Data,3,10000);
//	if(res) return 0;
//	VoltageD = VoltageD|MCP3421_Data[0];
//	VoltageD = VoltageD << 8;
//	VoltageD = VoltageD|MCP3421_Data[1];
//	VoltageD &= 0xfff;
//	//VoltageD+=1;
//	return VoltageD;
//}

