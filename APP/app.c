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

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include "led.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "app.h"
#include "HMI.h"
#include "usart1.h"
#include "CSV_Database.h"
#include "adc.h"
#include "ds18b20.h"
#include "ff.h"  
#include "exfuns.h"
#include "Data_Math.h"


/* Private functions ---------------------------------------------------------*/

u8 Device_STA=0;
u8 HMI_Event;
char Global_str[10][10];
rt_uint8_t Standard_val; //测试标准序号
TestParameters_Type TestParameters_Structure;	//测试指标存放结构体,应用于修改指标和测试时加载指标


void Master_thread_entry(void* parameter)
{
	u8 res;
	while(1)
	{
		/* 等待HMI串口监控线程的触发邮件 */
		if(rt_mb_recv(&HMI_Response_mb,(rt_uint32_t*)HMI_Info,RT_WAITING_FOREVER)==RT_EOK)
		{
			switch(HMI_Info[0])
			{
				case 0x01 : 
				{
					//创建线程1 
					CollectData_thread = rt_thread_create("GetData",CollectData_thread_entry, RT_NULL,512, 2,20);
					//创建线程1 
					HMI_FastTest_thread = rt_thread_create("HMI_1",HMI_FastTest_thread_entry, RT_NULL,512,3,20);
					
					HMI_File_Page(20);	//跳转到快速测试界面
					rt_thread_startup(CollectData_thread);	//启动数据采集线程
					rt_thread_startup(HMI_FastTest_thread);	//启动快速界面检测线程 
				}break;
				case 0x02 : 
				{
					rt_thread_delete(CollectData_thread);	//删除数据采集线程
					rt_thread_delete(HMI_FastTest_thread);	//删除快速界面检测线程
					HMI_File_Page(1); 
				}break;
				case 0x03 : 
				{
					Entry_Code_Old=0x03;
					//创建线程1 
					HMI_SelectBatch_thread = rt_thread_create("HMI_Batch",HMI_SelectBatch_thread_entry, RT_NULL,512,3,20);					
					rt_enter_critical();	//进入临界区
					f_mount(&fat,"0:",1);	//挂载工作区
					First_writeTestParameters();
					rt_exit_critical();		//退出临界区
					HMI_File_Page(8);	//跳转到列表显示界面
					rt_thread_startup(HMI_SelectBatch_thread);	//启动线程
				}break;
				case 0x04 :
				{
					Entry_Code_Old=0x04;
					//创建模式选择线程
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					rt_thread_delete(HMI_SelectBatch_thread);	//删除批量选择线程
					rt_enter_critical();	//进入临界区
					f_mount(NULL,"0:",1);	//挂载工作区
					rt_exit_critical();		//退出临界区
					HMI_File_Page(25);	//跳转到测试标准选择界面
					rt_thread_startup(HMI_SelectStandard_thread);	//启动线程
				}break;
				case 0x05 :
				{	
					rt_thread_delete(HMI_SelectBatch_thread);	//删除批量选择线程
					rt_enter_critical();	//进入临界区
					f_mount(NULL,"0:",1);	//挂载工作区
					rt_exit_critical();		//退出临界区
					HMI_File_Page(1);	//跳转到主界面界面
				}break;
				case 0x06 :  break;
				case 0x07 : 
				{
					Entry_Code_Old=0x07;
					//创建模式选择线程
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					/* 获取批量目录名称并将其写入SD卡中 */
					
					HMI_File_Page(25);	//跳转到测试标准选择界面
					rt_thread_startup(HMI_SelectStandard_thread);	//启动线程
				}break;
				case 0x08 : 
				{
					//创建列表查看线程
					HMI_ShowBatchList_thread = rt_thread_create("Standard",HMI_ShowBatchList_thread_entry, RT_NULL,512,3,20);
					HMI_File_Page(8);	//跳转到列表显示界面
					rt_thread_startup(HMI_ShowBatchList_thread);	//启动线程
				}break;
				case 0x09 : 
				{
					rt_thread_delete(HMI_ShowBatchList_thread);	//删除批量列表线程
					HMI_File_Page(1);	//跳转到列表显示界面
				}break;
				case 0x0a : 
				{
					Entry_Code_Old=0x0a;
					//创建模式选择线程
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);					
					HMI_File_Page(14);	//跳转到测试标准选择界面
					rt_thread_startup(HMI_SelectStandard_thread);	//启动线程					
				}break;
				case 0x0b : 
				{
					rt_thread_delete(HMI_SelectStandard_thread);
					HMI_File_Page(10);				//跳转到测试标准设置界面
					HMI_StandardPage_Show();	//显示当前模式测试标准
				}break;
				case 0x0c : 
				{
					/* 禁止调度以防止干扰串口接收 */
					rt_enter_critical();	//进入临界区
					res=HMI_Standard_Atoi();	//获取界面标准参数并装载进结构体
					rt_exit_critical();		//退出临界区
					#if	Thread_Debug
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Vout_Max);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Cout_Max);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.V_Ripple);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Poweron_Time);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Efficiency);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Safety_Code);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Quick_Charge);
					#endif
					HMI_File_Page(1);	//跳转到主界面
				}break;
				case 0x0d	:
				{
					HMI_File_Page(12);	//跳转到开关界面
					HMI_TestLimit_Itoa();
				}break;
				case 0x0e	:
				{
					/* 禁止调度以防止干扰串口接收 */
					rt_enter_critical();	//进入临界区
					HMI_TestLimit_Atoi(&HMI_TestLimit);
					rt_exit_critical();		//退出临界区
					#if	Thread_Debug
					rt_kprintf("HMI_TestLimit=%d\r\n",HMI_TestLimit);
					#endif
					HMI_File_Page(1);	//跳转到主界面
				}break;
				case 0x0f	:
				{
					HMI_File_Page(17);	//跳转到时间界面
					HMI_RTC_Show();
				}break;
				case 0x10	:
				{
					
				}break;
				/* 界面按键动作触发(通用) */
				case 0x51 :{Event_Flag=1;rt_mb_send(Event_mb,Event_Flag);}break;	//上翻
				case 0x52 :{Event_Flag=2;rt_mb_send(Event_mb,Event_Flag);}break;	//下翻
				case 0x53 :{Event_Flag=3;rt_mb_send(Event_mb,Event_Flag);}break;	//条目1
				case 0x54 :{Event_Flag=4;rt_mb_send(Event_mb,Event_Flag);}break;	//条目2
				case 0x55 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//条目3
				case 0x56 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//条目4
				case 0x57 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//条目5
				case 0x58 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//条目6
				default:break;
			}
			HMI_Info[0]=0;
		}
	}
}


/*
 * HMI监听进程 空闲状态监听HMI串口 一进入特定页面立即触发主进程，并将hmi串口交由主进程控制
 */
void HMIMonitor_thread_entry(void* parameter)
{
	while(1)
	{
		if(!USART_Solution(HMI_System_Type,HMI_Info)) 
		{
			rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
		}
		rt_thread_delay(100);
	}
}


/*
 * 数据采集线程
 */
void CollectData_thread_entry(void* parameter)
{
	while (1)
	{
		rt_enter_critical();	//进入临界区
		ReadTimeData_structure.V_OUT=Get_PowerVoltage();	//采集电压
		ReadTimeData_structure.C_OUT=Get_PowerCurrent();	//采集电流
		ReadTimeData_structure.V_Ripple=Get_PowerRipple();	//采集纹波电压
		rt_exit_critical();		//退出临界区
		rt_mb_send(GetData_mb,(rt_uint32_t)&ReadTimeData_structure);
		rt_thread_delay(100);
	}
}

/*
 * 快速检测HMI界面线程
 */
void HMI_FastTest_thread_entry(void* parameter)
{
	char str[10];
	ReadTimeData_Type* Showdata_Structure;
	while(1)
	{
		if(rt_mb_recv(GetData_mb, (rt_uint32_t*)&Showdata_Structure, RT_WAITING_FOREVER)== RT_EOK)
		{
			HMI_Print_Str("t6",str);
			HMI_Print_Str("t7",str);
			sprintf(str,"%.3f",(*Showdata_Structure).V_OUT);
			HMI_Print_Str("t9",str);
			sprintf(str,"%.3f",(*Showdata_Structure).C_OUT);
			HMI_Print_Str("t10",str);
			str[0]='\0';
			HMI_Print_Str("t11",str);
			sprintf(str,"%.3f",(*Showdata_Structure).V_Ripple);
			HMI_Print_Str("t12",str);
			str[0]='\0';
			HMI_Print_Str("t8",str);
		}
		rt_thread_delay(100);
	}
}

/*
 * 批量列表显示进程
 */
void HMI_ShowBatchList_thread_entry(void* parameter)
{
	int Page_Flag=0;
	u8 count=0;
	
	count=Scan_BatchDir(Page_Flag,Page_Flag+4);
	HMI_Print_Str("t0",Global_str[0]);
	HMI_Print_Str("t1",Global_str[1]);
	HMI_Print_Str("t2",Global_str[2]);
	HMI_Print_Str("t3",Global_str[3]);
	HMI_Print_Str("t4",Global_str[4]);
	while(1)
	{
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			rt_enter_critical(); /* 进入临界区*/
			/* 上翻 */
			if(Event_Flag==1)
			{
				Page_Flag=Page_Flag-1;
				if(Page_Flag<0) Page_Flag=0;
			}
			/* 下翻 */
			if(Event_Flag==2)
			{
				if(count==5)
					Page_Flag=Page_Flag+1;
			}
			/* 读取批量目录信息 */
			count=Scan_BatchDir(Page_Flag,Page_Flag+4);
			rt_exit_critical(); /* 退出临界区*/
			HMI_Print_Str("t0",Global_str[0]);
			HMI_Print_Str("t1",Global_str[1]);
			HMI_Print_Str("t2",Global_str[2]);
			HMI_Print_Str("t3",Global_str[3]);
			HMI_Print_Str("t4",Global_str[4]);
		}
	}
}

/*
 * 批量列表选择进程
 */
void HMI_SelectBatch_thread_entry(void* parameter)
{
	int Page_Flag=0;
	u8 count=0;
	
	/* 显示目录 */
	count=Scan_BatchDir(Page_Flag,Page_Flag+4);
	rt_kprintf("count=%d\r\n",count);
	rt_kprintf("str=%s\r\n",Global_str[0]);
	HMI_Print_Str("t0",Global_str[0]);
	HMI_Print_Str("t1",Global_str[1]);
	HMI_Print_Str("t2",Global_str[2]);
	HMI_Print_Str("t3",Global_str[3]);
	HMI_Print_Str("t4",Global_str[4]);
	while(1)
	{
		/* 接收按键时间 */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			switch(Event_Flag)
			{
				/* 上翻 */
				case 1:
				{
					Page_Flag=Page_Flag-1;
					if(Page_Flag<0) Page_Flag=0;
				}break;
				/* 下翻 */
				case 2:
				{
					if(count==5)
						Page_Flag=Page_Flag+1;
				}break;
				/* 选择条目1 */
				case 3:
				{
					Batch_val=Page_Flag;
					HMI_Info[0]=4;
					strcpy(Batch_str,Global_str[0]);
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目2 */
				case 4:
				{
					Batch_val=Page_Flag+1;
					strcpy(Batch_str,Global_str[1]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目3 */
				case 5:
				{
					Batch_val=Page_Flag+2;
					strcpy(Batch_str,Global_str[2]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目4 */
				case 6:
				{
					Batch_val=Page_Flag+3;
					strcpy(Batch_str,Global_str[3]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目5 */
				case 7:
				{
					Batch_val=Page_Flag+4;
					strcpy(Batch_str,Global_str[4]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				default:break;
			}
			rt_enter_critical(); /* 进入临界区*/
			/* 读取批量目录信息 */
			count=Scan_BatchDir(Page_Flag,Page_Flag+4);
			rt_exit_critical(); /* 退出临界区*/
			HMI_Print_Str("t0",Global_str[0]);
			HMI_Print_Str("t1",Global_str[1]);
			HMI_Print_Str("t2",Global_str[2]);
			HMI_Print_Str("t3",Global_str[3]);
			HMI_Print_Str("t4",Global_str[4]);
		}
	}
}

/*
 * 测试标准选择界面
 */
void HMI_SelectStandard_thread_entry(void* parameter)
{
	while(1)
	{
		/* 获取测试标准序号 */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			switch(Event_Flag)
			{
				case 3: Standard_val=0;break;
				case 4: Standard_val=1;break;
				case 5: Standard_val=2;break;
				default:break;
			}
		}
		/*批量检测界面部分*/
		if(Entry_Code_Old==0x04||Entry_Code_Old==0x07)
		{
			rt_thread_delay(100);
			HMI_File_Page(19);
		}
		/*测试标准设置部分*/
		else
		{
			HMI_Info[0]=0x0b;
			rt_mb_send(&HMI_Response_mb,HMI_Info[0]);	
		}
	}
}


  
//线程LED1
void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		LED1 =~LED1;
		rt_thread_delay(1000);
	}
}

/*
 * 空闲任务线程 系统运行指示灯闪烁
 */
void cpu_usage_idle_hook(void)
{
	while (1)
	{
		LED0 =~LED0;
		rt_thread_delay(1000);
	}
}





void Main_entry(void)
{
	/* set idle thread hook */
//    rt_thread_idle_sethook(cpu_usage_idle_hook);
	
	/* 初始化mailbox */
	rt_mb_init(&HMI_Response_mb,"HMI_mb", /* 名称是HMI_mb */
			   &mb_pool[0],
				 sizeof(mb_pool)/4, /* 每封邮件的大小是4字节 */
			   RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */

	GetData_mb=rt_mb_create("Data_mb", /* 名称是HMI_mb */
			   32, /* 每封邮件的大小是4字节 */
			   RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */
	if(GetData_mb==RT_NULL)
		rt_kprintf("create mb failed \r\n");
	
		Event_mb=rt_mb_create("Event_mb",32,RT_IPC_FLAG_FIFO);
	if(Event_mb==RT_NULL) rt_kprintf("create mb failed \r\n");
	
  delay_ms(800);
 	HMI_File_Page(3);		 
				 
				 
				   
		//创建线程1
   led1_thread = rt_thread_create("led1", //线程1的名称是t1 
							led1_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick

	if (led1_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(led1_thread); 


	//创建线程1 
    HMIMonitor_thread = rt_thread_create("HMIMonitor", //线程1的名称是t1 
							HMIMonitor_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							1, //线程优先级
							20);//时间片tick

	if (HMIMonitor_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(HMIMonitor_thread); 
	
	//创建线程1 
    Master_thread = rt_thread_create("Master", //线程1的名称是t1 
							Master_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick

	if (Master_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(Master_thread); 
		
}


