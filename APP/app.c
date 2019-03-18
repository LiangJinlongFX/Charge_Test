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
#include "sys.h"
#include "Load_PID.h"
#include "switch.h"
#include "led.h"
#include "delay.h"
#include "app.h"
#include "HMI.h"
#include "usart1.h"
#include "adc.h"
#include "ff.h"  
#include "exfuns.h"
#include "Data_Math.h"
#include "usart3.h"
#include "hlw8032.h"
#include "debug.h"
#include "string.h"


/* Private functions ---------------------------------------------------------*/
u8 HMI_Event;
char Global_str[30][20];
rt_uint8_t Standard_val; //测试标准序号
TestParameters_Type TestParameters_Structure[4];	//测试指标存放结构体,应用于修改指标和测试时加载指标

/**
 * HMI控制主线程
 * @param
 * @return
 * 用于初始化以及对其他线程的控制
 */
void Master_thread_entry(void* parameter)
{
	u8 res;	
	
	/* Run Forever */
	while(1)
	{
		/* 等待HMI串口监控线程的触发邮件 */
		if(rt_mb_recv(HMI_Response_mb,(rt_uint32_t*)HMI_Info,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("case %d",HMI_Info[0]);
			switch(HMI_Info[0])
			{
				/* 进入快速检测交互界面 */
				case 0x01 : 
				{
					/* 创建快速测试界面显示线程 */
					HMI_FastTest_thread = rt_thread_create("HMI_1",HMI_FastTest_thread_entry, RT_NULL,1024,2,20);
					/* 跳转到快速测试界面 */
					HMI_File_Page(Page_fast_testing);
					/* 使能串口3中断响应开关 */
					USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
					USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
					Current_HMIEvent = 0x01;
					rt_thread_startup(HMI_FastTest_thread);		//启动快速界面检测线程
				}break;
				
				/* 退出快速检测交互界面 */
				case 0x02 : 
				{
					/* 关闭串口3中断响应开关 */
					USART_ITConfig(USART3,USART_IT_IDLE,DISABLE);
					USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
					rt_thread_delete(HMI_FastTest_thread);	//删除快速界面检测线程
					HMI_File_Page(Page_main); 							//跳转到主界面
				}break;
				
				/* 进入批量选择交互界面 */
				case 0x03 : 
				{
					Entry_Code_Old=0x03;
					/* 创建线程：批量列表线程 */
					HMI_SelectBatch_thread = rt_thread_create("HMI_Batch",HMI_SelectBatch_thread_entry, RT_NULL,1024,3,20);					
					HMI_File_Page(8);			//跳转到列表显示界面
					rt_thread_startup(HMI_SelectBatch_thread);	//启动线程
				}break;
				
				/* 选中批量后进入批量标准选择线程 */
				case 0x04 :
				{
					Entry_Code_Old=0x04;
					//创建模式选择线程
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					rt_thread_delete(HMI_SelectBatch_thread);	//删除批量选择线程
					HMI_File_Page(Page_test_selstand);	//跳转到测试标准选择界面
					rt_thread_startup(HMI_SelectStandard_thread);	//启动线程
				}break;
				/* 进入批量选择但取消选择 */
				case 0x05 :
				{	
					rt_thread_delete(HMI_SelectBatch_thread);	//删除批量选择线程
					HMI_File_Page(Page_main);	//跳转到主界面界面
				}break;
				/* 选择批量标准中中途放弃 */
				case 0x06 :
				{
					rt_thread_delete(HMI_SelectStandard_thread);	//删除批量选择线程
					HMI_File_Page(Page_main);	//跳转到主界面界面
				}break;
				/* 创建了新批量并将其写入SD卡 */
				case 0x07 : 
				{
					Entry_Code_Old=0x07;
					/* 获取批量目录名称并将其写入SD卡中 */
					f_mount(&fat,"0",1);
					HMI_Creat_NewBatch();
					f_mount(NULL,"0",1);
					HMI_File_Page(Page_main);	//跳转到测试标准选择界面
				}break;
				/* 标准设置交互中选择标准序号 */
				case 0x0a : 
				{
					Entry_Code_Old=0x0a;
					//创建模式选择线程
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);					
					HMI_File_Page(Page_standard_sel);	//跳转到设置测试标准选择界面
					rt_thread_startup(HMI_SelectStandard_thread);	//启动线程					
				}break;
				/* 标准设置交互中选择标准序号事件结束 */
				case 0x0b : 
				{
					rt_thread_delete(HMI_SelectStandard_thread);
					HMI_File_Page(Page_standard);		//跳转到测试标准设置界面
					#if	Thread_Debug
					logging_debug("Standard_val: %d",Standard_val);
					logging_debug("Vout=%d",TestParameters_Structure[Standard_val].Vout);
					logging_debug("Vout_Tolerance=%d",TestParameters_Structure[Standard_val].Vout_Tolerance);
					logging_debug("Cout_Max=%d",TestParameters_Structure[Standard_val].Cout_Max);
					logging_debug("V_Ripple=%d",TestParameters_Structure[Standard_val].V_Ripple);
					logging_debug("Efficiency=%d",TestParameters_Structure[Standard_val].Efficiency);
					logging_debug("Safety_Code=%d",TestParameters_Structure[Standard_val].Safety_Code);
					logging_debug("Quick_Charge=%d",TestParameters_Structure[Standard_val].Quick_Charge);	
					HMI_StandardPage_Show(&TestParameters_Structure[Standard_val]);	//显示当前模式测试标准
					#endif
				}break;
				/* 标准设置交互中设置标准 */
				case 0x0c : 
				{
					/* 禁止调度以防止干扰串口接收 */
					res=HMI_Standard_Atoi();	//获取界面标准参数并装载进结构体
					rt_enter_critical();	//进入临界区
					f_mount(&fat,"0:",1);	//挂载工作区
					if(Modify_TestParameters(&TestParameters_Structure[Standard_val],Standard_val)) rt_kprintf("WriteData_Error!\r\n");
					f_mount(NULL,"0:",1);	//注销工作区
					rt_exit_critical();		//退出临界区
					#if	Thread_Debug
					logging_debug("Standard_val: %d",Standard_val);
					logging_debug("Vout=%d",TestParameters_Structure[Standard_val].Vout);
					logging_debug("Vout_Tolerance=%d",TestParameters_Structure[Standard_val].Vout_Tolerance);
					logging_debug("Cout_Max=%d",TestParameters_Structure[Standard_val].Cout_Max);
					logging_debug("V_Ripple=%d",TestParameters_Structure[Standard_val].V_Ripple);
					logging_debug("Efficiency=%d",TestParameters_Structure[Standard_val].Efficiency);
					logging_debug("Safety_Code=%d",TestParameters_Structure[Standard_val].Safety_Code);
					logging_debug("Quick_Charge=%d",TestParameters_Structure[Standard_val].Quick_Charge);	
					#endif
					HMI_File_Page(Page_main);	//跳转到主界面
				}break;
				/* 测试开关设置进入界面 */
				case 0x0d	:
				{
					HMI_File_Page(12);	//跳转到开关界面
					HMI_TestLimit_Itoa();
				}break;
				/* 测试开关设置退出界面 */
				case 0x0e	:
				{
					/* 禁止调度以防止干扰串口接收 */
					rt_enter_critical();	//进入临界区
					HMI_TestLimit_Atoi(&HMI_TestLimit);
					rt_exit_critical();		//退出临界区
					HMI_File_Page(1);	//跳转到主界面
				}break;
				/* 进入时间设置界面 */
				case 0x0f	:
				{
					HMI_File_Page(17);	//跳转到时间界面
					HMI_RTC_Show();
				}break;
				/* 退出时间设置界面 */
				case 0x10	:
				{
					HMI_RTC_Atoi();
					HMI_File_Page(1);
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

/**
 * HMI事件监听线程
 * @param
 * @return
 * 空闲状态监听HMI串口 一进入特定页面立即触发主进程，并将hmi串口交由主进程控制
 */
void HMIMonitor_thread_entry(void* parameter)
{
	while(1)
	{
		/* 等待接收到HMI系统事件信息 */
		if(!USART_Solution(HMI_System_Type,HMI_Info)) 
		{
			/* 接收到HMI系统事件信息,发送邮箱通知别的线程 */
			rt_mb_send(HMI_Response_mb,HMI_Info[0]);
		}
		LED2=~LED2;
		rt_thread_delay(100);
	}
}

/**
 * 检测数据采集回调函数
 * @param
 * @return
 * 采集USB端口的电源电压/电流/纹波电压
 * 使用软件定时器回调函数
 */
void GetData_timerout(void* parameter)
{
	rt_enter_critical();	//进入临界区
	Get_Power_Val_All(&Global_Measured_Structure);
	rt_exit_critical();		//退出临界区
	/* 发送数据地址给别的线程 */
	rt_mb_send(GetData_mb,(rt_uint32_t)&Global_ReadTimeData_structure);
}


/**
 * HMI交互界面事件处理线程
 * @param
 * @return
 * 
 */
void EventProcessing_thread_entry(void* parameter)
{
	u8 res;
	float Val;
	char str[10];
	
	while(1)
	{
		/* 接收虚拟按键事件邮箱 */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("Receive Interface Event:%d",Event_Flag);
			if(Current_HMIEvent == 0x01)
			{
				/* 接收到快充诱导事件 */
				if(Event_Flag==1)
				{
					logging_debug("Detected QuickCharge...");
					/* 对所有快充协议进行诱导 */
					rt_enter_critical();	//进入临界区
					res=QuickCharge_Induction(0x0f);
					sprintf(str,"QC: %x",res);
					HMI_Print_Str("t8",str);
					rt_exit_critical();		//退出临界区
				}
				else if(Event_Flag==2)
				{
					logging_debug("Start OCP TEST...");
					/* 进行过流保护测试 */
					rt_enter_critical();	//进入临界区
					Val=OverCurrent_Detection();
					if(Val == 0)
					{
						sprintf(str,"NO LOAD!");
						HMI_Print_Str("t8",str);
					}
					else
					{
						sprintf(str,"OCP: %d",(u32)Val);
						HMI_Print_Str("t8",str);
					}
					rt_exit_critical();		//退出临界区				
				}
			}
		}
		rt_thread_delay(500);
	}	
}



/**
 * 快速检测参数显示进程
 * @param   
 * @return 
 * @brief 
 */
void HMI_FastTest_thread_entry(void* parameter)
{
	char str[20];
	
	while(1)
	{
		Get_Power_Val_All(&Global_Measured_Structure);
		sprintf(str,"%.3f",Global_Measured_Structure.Measured_AC_Voltage);
		HMI_Print_Str("t6",str);	//显示AC输入电压
		str[0]='\0';
		sprintf(str,"%.3f",Global_Measured_Structure.Measured_AC_Current);
		HMI_Print_Str("t7",str);	//显示AC输入电流
		str[0]='\0';
		sprintf(str,"%.3f",(Global_Measured_Structure.Measured_USB_Voltage)/1000);
		HMI_Print_Str("t9",str);	//显示DC输出电压
		str[0]='\0';
		sprintf(str,"%.3f",Global_Measured_Structure.Measured_AC_Current);
		HMI_Print_Str("t10",str);	//显示DC输出电流
		str[0]='\0';
		HMI_Print_Str("t11",str);	//显示转换效率
		str[0]='\0';
		sprintf(str,"%.3f",0.00f);
		HMI_Print_Str("t12",str);	//显示纹波电压
		str[0]='\0';
		rt_thread_delay(200);
	}
}

/**
 * 批量选择列表线程
 * @param   
 * @return 
 * @brief 
 */
void HMI_SelectBatch_thread_entry(void* parameter)
{
	char *Str[30];
	int Page_Flag=0;
	u8 count=0;
	u8 i;
	
	for(i=0;i<30;i++)
		Str[i]=Global_str[i];
	
	logging_debug("join HMI_SelectBatch_thread_entry",count);
	/* 显示目录 */
	rt_enter_critical();	//进入临界区
	f_mount(&fat,"0:",1);	//挂载工作区
	count=Scan_BatchDir(0,29,Str);
	f_mount(NULL,"0:",1);	//注销工作区
	rt_exit_critical();		//退出临界区
	logging_debug("Get the number of entries: %d",count);
	
	for(i=0;i<count;i++)
		logging_debug("list %d: %s",i,Global_str[i]);
	HMI_Print_Str("t0",Global_str[0]);
	HMI_Print_Str("t1",Global_str[1]);
	HMI_Print_Str("t2",Global_str[2]);
	HMI_Print_Str("t3",Global_str[3]);
	HMI_Print_Str("t4",Global_str[4]);
	/* forever */
	while(1)
	{
		/* 接收按键时间 */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("Receive button! %d",Event_Flag);
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
					if((Page_Flag+1)*5<count)
						Page_Flag=Page_Flag+1;
				}break;
				/* 选择条目1 */
				case 3:
				{
					Batch_val=Page_Flag;
					HMI_Info[0]=4;
					strcpy(Batch_str,Global_str[0]);
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目2 */
				case 4:
				{
					Batch_val=Page_Flag+1;
					strcpy(Batch_str,Global_str[1]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目3 */
				case 5:
				{
					Batch_val=Page_Flag+2;
					strcpy(Batch_str,Global_str[2]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目4 */
				case 6:
				{
					Batch_val=Page_Flag+3;
					strcpy(Batch_str,Global_str[3]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* 选择条目5 */
				case 7:
				{
					Batch_val=Page_Flag+4;
					strcpy(Batch_str,Global_str[4]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				default:break;
			}
			logging_debug("Page_Flag: %d",Page_Flag);
			HMI_Print_Str("t0",Global_str[Page_Flag*5+0]);
			HMI_Print_Str("t1",Global_str[Page_Flag*5+1]);
			HMI_Print_Str("t2",Global_str[Page_Flag*5+2]);
			HMI_Print_Str("t3",Global_str[Page_Flag*5+3]);
			HMI_Print_Str("t4",Global_str[Page_Flag*5+4]);
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
				case 1: Standard_val=0;break;
				case 2: Standard_val=1;break;
				case 3: Standard_val=2;break;
				case 4: Standard_val=3;break;
				default:break;
			}
		}
		/*批量检测界面交互部分*/
		if(Entry_Code_Old==0x04||Entry_Code_Old==0x07)
		{
			rt_thread_delay(100);
			HMI_File_Page(19);
		}
		/*测试标准设置交互部分*/
		else
		{
			HMI_Info[0]=0x0b;
			rt_mb_send(HMI_Response_mb,HMI_Info[0]);
		}
	}
}


  
/**
 * 系统指示灯闪烁线程
 * @param   
 * @return 
 * @brief 
 **/
void led_thread_entry(void* parameter)
{	
	while (1)
	{
		SYS_LED = ~SYS_LED;
		rt_thread_delay(1000);
	}
}



/**
 * APP入口主函数
 * @param   
 * @return 
 * @brief 
 **/
void Main_entry(void)
{

	//Get_Measured_Offset();
	
	/* 读取存储体内的设置参数 */
	if(Poweron_ReadTestParameters())
		logging_error("Read set.dat ERROR!");
	else
		logging_debug("Read set.dat OK!");
	
	
	/* 创建mailbox */
	HMI_Response_mb=rt_mb_create("HMI_mb", /* 名称是HMI_mb */
			   32, /* 每封邮件的大小是4字节 */
			   RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */
	GetData_mb=rt_mb_create("Data_mb", /* 名称是HMI_mb */
			   32, /* 每封邮件的大小是4字节 */
			   RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */			 
	Event_mb=rt_mb_create("Event_mb", /* 名称是HMI_mb */
			   32, /* 每封邮件的大小是4字节 */
			   RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */
	
	/* 检查创建邮箱是否成功 */
	if(HMI_Response_mb==RT_NULL)
		logging_error("create GetData_mb failed \r\n");
	if(GetData_mb==RT_NULL)
		logging_error("create GetData_mb failed \r\n");	
	if(Event_mb==RT_NULL)
		logging_error("create Event_mb failed \r\n");
	
	/* create timer*/
	GetData_timer = rt_timer_create("timer1", /* 定时器名字是timer1 */
														GetData_timerout, /* 超时时回调的处理函数*/
														RT_NULL, /* 超时函数的入口参数*/
														500, /* 定时长度，以OS Tick为单位，即10个OS Tick */
														RT_TIMER_FLAG_PERIODIC); /* 周期性定时器*/
	if(GetData_timer==RT_NULL)
		logging_error("create GetData_timer failed \r\n");
	
  /* 延时一段时间,等待HMI屏幕初始化完毕 */
	delay_ms(800);
	/* 跳转到启动页 */
	logging_debug("Flip Page_Init");
 	HMI_File_Page(Page_Init);		 
				
	/* 创建 led1闪烁 线程 */
	logging_debug("create thread sys_led");
  led_thread = rt_thread_create("led", //线程1的名称是t1 
							led_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick
	if (led_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(led_thread);
	else
		logging_error("create thread led1 error!");

	/* 创建 HMI串口监控 线程 */
	logging_debug("create thread HMIMonitor");
  HMIMonitor_thread = rt_thread_create("HMIMonitor", //线程1的名称是t1 
							HMIMonitor_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							1, //线程优先级
							20);//时间片tick
	if (HMIMonitor_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(HMIMonitor_thread);
	else
		logging_error("create thread HMIMonitor error!");
	
  /* 创建 交互事件检测 线程 */
	logging_debug("create thread Processing");
  EventProcessing_thread = rt_thread_create("Process", //线程1的名称是t1 
							EventProcessing_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							512, //线程堆栈大小
							1, //线程优先级
							20);//时间片tick
	if (EventProcessing_thread != RT_NULL) //如果获得线程控制块，启动这个线程
			rt_thread_startup(EventProcessing_thread);
	else
		logging_error("create thread HMIMonitor error!");
	
	/* 创建 主控制 线程 */
	logging_debug("create thread Master");
  Master_thread = rt_thread_create("Master", //线程1的名称是t1 
							Master_thread_entry, RT_NULL, //入口是thread1_entry，参数是RT_NULL 
							1024, //线程堆栈大小
							3, //线程优先级
							20);//时间片tick
	if (Master_thread != RT_NULL) //如果获得线程控制块，启动这个线程
		rt_thread_startup(Master_thread);
	else
		logging_error("create thread Master error!");
		
}


