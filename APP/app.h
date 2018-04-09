#ifndef _APP_H
#define _APP_H
/**
  ******************************************************************************
  * @file    STM32F407DEMO/app.h 
  * @author  Liang
  * @version V1.0.0
  * @date    2018-1-22
  * @brief   
  ******************************************************************************
  * @attention
	* 加入对rt-thread的支持
	*	用于rt-thread msh的串口驱动
  ******************************************************************************
**/

#include <rtthread.h>
#include "CSV_Database.h"

/* 任务使用到的全局变量定义 */
static ReadTimeData_Type ReadTimeData_structure;	//实时数据存储结构体
static char Global_str[10][10];	//字符串二维缓存数组
static char HMI_Info[100];	//HMI消息缓存数组
static TestParameters_Type TestParameters_Structure;	//测试指标存放结构体,应用于修改指标和测试时加载指标
static rt_int16_t SysError_Code;	//系统运行情况代码
static char Batch_str[10];	//测试批量缓存
static rt_uint8_t Entry_Code_Old;	//缓存上一个执行的进程代码以实现线程复用

/* 邮箱控制块 */
static struct rt_mailbox HMI_Response_mb;		//HMI串口响应邮箱
static rt_mailbox_t GetData_mb=RT_NULL;					//数据采集邮箱
static rt_mailbox_t Event_mb=RT_NULL;						//按键动作邮箱

/* 用于放邮件的内存池 */
static char mb_pool[32];
static rt_uint8_t Event_Flag;

static rt_uint8_t Batch_val;	//批量序号
static rt_uint8_t Standard_val; //测试标准序号

static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t usb_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;
static rt_thread_t CollectData_thread=RT_NULL;
static rt_thread_t HMI_FastTest_thread=RT_NULL;
static rt_thread_t HMI_ShowBatchList_thread=RT_NULL;
static rt_thread_t HMI_SelectBatch_thread=RT_NULL;
static rt_thread_t HMI_SelectStandard_thread=RT_NULL;	

void usb_thread_entry(void* parameter);						//USB线程
void led1_thread_entry(void* parameter);					//LED闪烁线程
void HMIMonitor_thread_entry(void* parameter);		//HMI系统信息监控进程
void Master_thread_entry(void* parameter);				//线程控制主线程
void CollectData_thread_entry(void* parameter);		//实时数据采集进程
void HMI_FastTest_thread_entry(void* parameter);	//快速检测HMI界面线程
void HMI_ShowBatchList_thread_entry(void* parameter); //批量列表显示界面线程
void HMI_SelectBatch_thread_entry(void* parameter);	//批量选择线程
void HMI_SelectStandard_thread_entry(void* parameter); //测试标准选择线程
void cpu_usage_idle_hook(void);
void Main_entry(void);



#endif

