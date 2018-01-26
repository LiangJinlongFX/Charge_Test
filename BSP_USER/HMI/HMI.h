#ifndef _HMI_H
#define _HMI_H

#include "sys.h"

/**
 * HMI数据返回格式（都是以0XFF 0XFF 0XFF 作为结束符）
 * 页面返回格式	0x66+page_ID+0XFF
 * 
 */



/*
 * 指令执行结果返回
 */
#define HMI_Instr_Invalid	0x00	//无效指令
#define HMI_Instr_OK		0x01	//成功执行
#define HMI_Instr_Contr_Invalid	0x02
#define HMI_Instr_Page_Invalid	0x03
#define HMI_Instr_Pic_Invalid	0x04
#define HMI_Instr_Font_Invalid	0x05
#define HMI_Instr_Bund_Invalid	0x11
#define HMI_Instr_Curve_Invalid	0x12
#define HMI_Instr_VarSum_Invalid	0x1b
#define HMI_Instr_VarNum_Invalid 0x1e
#define HMI_Instr_VarAss_Invalid 0x1c
#define HMI_Instr_VarLen_Invalid 0x23
#define HMI_Instr_Str_Invalid 0x20
#define HMI_Instr_SerialBuff_Invalid 0x24

/*
 * 返回数据的类型
 */
#define HMI_Touch_Type 0x65		//触摸热区事件
#define HMI_Page_Type	0x66		//返回页面ID 	sendme
#define HMI_Touchxy_Type	0x67	//返回触摸坐标	发送键值
#define HMI_SleepThing_Type	0x68	//睡眠模式触摸事件
#define HMI_String_Type	0x70	//返回字符串		get
#define HMI_Vaule_Type	0x71	//返回数值		get
#define HMI_Sleep_Type	0x86	//设备进入触摸模式
#define HMI_Wake_Type		0x87	//设备唤醒
#define	HMI_Update_Type			0x89	//sd卡升级
#define	HMI_SendData_Type		0xfd	//透传模式完成
#define HMI_ENDData_Type		0xfe	//透传模式就绪

/**
 * 指令执行情况枚举
 */
typedef enum 
{
	HMI_Page_Error		=(1),		//页面跳转错误
	HMI_OK = 0,						//成功执行
}HMI_Error;

extern u8 HMI_Rx_String[100];

HMI_Error USART_Solution(void);



HMI_Error HMI_File_Page(char* Page_ID);	//页面跳转
HMI_Error HMI_Print_Str(char* Str_ID,char* fmt);	//发送字符串至串口设备
HMI_Error HMI_Print(char* str);







#endif
