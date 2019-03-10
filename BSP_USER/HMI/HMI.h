#ifndef _HMI_H
#define _HMI_H

#include "CSV_Database.h"


/*
 * 页面ID定义
 */
#define Page_start 0
#define Page_main	 1
#define Page_quick_test_ACK 2
#define Page_Init 3
#define Page_keyboard 4
#define Page_batch_test 5
#define Page_Error 6
#define Page_database_sel 7
#define Page_list 8
#define Page_setting 9
#define Page_standard 10		//标准参数设置界面
#define Page_fast_charge 11
#define Page_select_test 12
#define Page_system_setting 13
#define Page_standard_sel 14	//标准参数序号选择界面
#define Page_batch_testing 15
#define Page_warning 16
#define Page_time_setting 17
#define Page_light_setting 18
#define Page_connection 19
#define Page_fast_testing 20
#define Page_new_batch 21
#define Page_format 22
#define Page_system_version 23
#define Page_USB 24
#define Page_test_selstand 25

/*
 * HMI屏幕事件定义
 */
#define HMI_Event_FastTest_In  0x01		//进入了快速检测界面
#define HMI_Event_FastTest_Out 0x02		//退出了快速检测界面
#define HMI_Event_SelList_IN   0x03		//进入了批量选择交互界面
#define HMI_Event_ListSelstand_IN 0x04		//选中批量后进入标准选择界面
#define HMI_Event_SelList_OUT  0x05		//进入批量选择但取消选择
#define HMI_Event_ListSelstand_OUT  0x06		//选择批量标准中中途放弃
#define HMI_Event_SaveDataToSD 0x07	//创建了新批量并将其写入SD卡
#define HMI_Event_SetStandardNum_IN   0x0a	//标准设置交互中选择标准序号
#define HMI_Event_SetStandardNum_OUT  0x0b	//标准设置交互中选择标准序号事件结束
#define HMI_Event_SetStandard_IN    0x0c	    //标准设置交互中设置标准
#define HMI_Event_Limit_IN   0x0d	    	//进入测试开关设置界面
#define HMI_Event_Limit_OUT   0x0e	    //退出测试开关设置界面
#define HMI_Event_SetTime_IN   0x0f	    //进入时间设置界面
#define HMI_Event_SetTime_OUT   0x10	    //退出时间设置界面



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
#define HMI_System_Type		0x88	//系统事件  自定义


/**
 * 指令执行情况枚举
 */
typedef enum 
{
	HMI_NoResponse		=(1),		//无响应
	HMI_Parse_Error		=(2),		//解析错误
	HMI_Page_Error		=(3),		//页面跳转错误
	HMI_OK = 0,						//成功执行
}HMI_Error;


extern u8 HMI_TestLimit;

HMI_Error USART_Solution(u8 HMI_Type,char* HMI_Rx_String);



HMI_Error HMI_File_Page(u8 Page_ID);	//页面跳转
HMI_Error HMI_Print_Str(char* Str_ID,char* fmt);	//发送字符串至串口设备
HMI_Error HMI_Print(char* str);		//发送字符串给HMI设备
HMI_Error HMI_Page_ACK(u8 Page_ID);		//HMI当前页面确认
HMI_Error HMI_StandardPage_Show(TestParameters_Type *Parameters);	//HMI标准设置参数显示
HMI_Error HMI_Print_Val(char* Object_ID,u16 varible);		//发送控件val值给HMI设备
HMI_Error HMI_Get(u8 Object_Type,char* Object_ID,char* fmt);	//获取控件参数
HMI_Error HMI_Standard_Atoi(void);		//
HMI_Error HMI_TestLimit_Itoa(void);
HMI_Error HMI_TestLimit_Atoi(u8* LimitVal);
HMI_Error HMI_SetStandard_Interface(void);
HMI_Error HMI_RTC_Show(void);
HMI_Error HMI_ShowBatch(void);
HMI_Error HMI_ShowBatchList(void);
HMI_Error HMI_RTC_Atoi(void);
HMI_Error HMI_FastTest(void);
HMI_Error HMI_Creat_NewBatch(void);
void HMI_ShowError(u16 Running_Code);


#endif
