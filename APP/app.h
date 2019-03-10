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
#include "Data_Math.h"

#define Thread_Debug 1

/* 任务使用到的全局变量定义 */
extern char Global_str[30][20];		//字符串二维缓存数组
extern rt_uint8_t Standard_val; 	//测试标准序号
extern TestParameters_Type TestParameters_Structure[4];	//测试指标存放结构体,应用于修改指标和测试时加载指标
static ReadTimeData_Type Global_ReadTimeData_structure;			//实时数据存储结构体
static MeasuredData_Type Global_Measured_Structure;	//实时检测数据结构体
static char HMI_Info[100];				//HMI消息缓存数组
static rt_int16_t SysError_Code;	//系统运行情况代码
static char Batch_str[10];				//测试批量缓存
static rt_uint8_t Entry_Code_Old;	//缓存上一个执行的进程代码以实现线程复用
static rt_uint8_t Current_HMIEvent;	//记录当前的HMI事件
static rt_uint8_t Batch_val;			//批量序号

/* 邮箱控制块 */
static rt_mailbox_t HMI_Response_mb=RT_NULL;		//HMI串口响应邮箱
static rt_mailbox_t GetData_mb=RT_NULL;					//数据采集邮箱
static rt_mailbox_t Event_mb=RT_NULL;						//按键动作邮箱

/* 软件定时器控制块 */
static rt_timer_t GetData_timer=RT_NULL;				//数据采集周期定时器
static struct rt_timer timer2;

/* 用于放邮件的内存池 */
static char mb_pool[32];
static rt_uint8_t Event_Flag;


/* 线程控制块 */
static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;
static rt_thread_t CollectData_thread=RT_NULL;
static rt_thread_t HMI_FastTest_thread=RT_NULL;
static rt_thread_t HMI_SelectBatch_thread=RT_NULL;
static rt_thread_t HMI_SelectStandard_thread=RT_NULL;
static rt_thread_t EventProcessing_thread=RT_NULL;

/* 线程函数 */
void led1_thread_entry(void* parameter);					//LED闪烁线程
void HMIMonitor_thread_entry(void* parameter);		//HMI系统信息监控进程
void Master_thread_entry(void* parameter);				//线程控制主线程
void CollectData_thread_entry(void* parameter);		//实时数据采集进程
void HMI_FastTest_thread_entry(void* parameter);	//快速检测HMI界面线程
void HMI_SelectBatch_thread_entry(void* parameter);	//批量选择线程
void HMI_SelectStandard_thread_entry(void* parameter); //测试标准选择线程
void EventProcessing_thread_entry(void* parameter);			//界面交互事件处理线程
void cpu_usage_idle_hook(void);
void Main_entry(void);
/* 回调函数 */
void GetData_timerout(void* parameter);		//数据采集软件定时器回调函数



#endif

