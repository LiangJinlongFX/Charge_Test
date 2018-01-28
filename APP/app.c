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


#define CPU_USAGE_CALC_TICK	10
#define CPU_USAGE_LOOP		  100
rt_uint8_t  cpu_usage_major = 0, cpu_usage_minor= 0;
rt_uint32_t total_count = 0;
/* Private functions ---------------------------------------------------------*/

u8 Device_STA=0;
u8 HMI_Event;
char HMI_Info[100];
u8 HMI_STA;


void Master_thread_entry(void* parameter)
{
	while(1)
	{
		rt_thread_delay(100);
		rt_sem_take(&sem,RT_WAITING_FOREVER);//死等
		rt_kprintf("high task has take mutex!\r\n");
		rt_kprintf("high task running!\r\n");
		rt_sem_release(&sem);
		rt_thread_delay(400); 
	}
}


/*
 * HMI监听进程 空闲状态监听HMI串口 一进入特定页面立即触发主进程，并将hmi串口交由主进程控制
 */
void HMIMonitor_thread_entry(void* parameter)
{
	u8 res;
	while(1)
	{
		//串口监听抢先占有信号量
//		rt_sem_take(&sem,1000);
//		rt_kprintf("res=%d\r\n",sem.value);
		//有页面跳转消息,释放信号量
		if(!USART_Solution(HMI_Page_Type,HMI_Info)) 
		{
			rt_kprintf("master will running\r\n");
			res=rt_sem_release(&sem);
			rt_kprintf("res=%d\r\n",sem.value);
		}
		rt_thread_delay(100);
		LED1=~LED1;
	}
}



//线程LED0
void led0_thread_entry(void* parameter)
{
	while(1)
	{
		LED0=~LED0;
		delay_ms(300);	
		delay_us(100000);
		rt_thread_delay(100);
	}
}

//线程USB
void usb_thread_entry(void* parameter)
{
	u8 res;
	char str[10];
	//HMI_StandardPage_Show();
//	res=HMI_Get(HMI_Vaule_Type,"bt1",str);
//	if(res) rt_kprintf("res=%d\r\n",res);
//	else rt_kprintf("str=%s\r\n",str);
//	HMI_Standard_Atoi();
//	rt_kprintf("Vout_Max=%d\r\n",TestStandard_Arrary[Current_event].Vout_Max);
//	rt_kprintf("Cout_Max=%d\r\n",TestStandard_Arrary[Current_event].Cout_Max);
//	rt_kprintf("Ripple_Voltage=%d\r\n",TestStandard_Arrary[Current_event].Ripple_Voltage);
//	rt_kprintf("Poweron_Time=%d\r\n",TestStandard_Arrary[Current_event].Poweron_Time);
//	rt_kprintf("Efficiency=%d\r\n",TestStandard_Arrary[Current_event].Efficiency);
//	rt_kprintf("Over_Voltage_Protection=%d\r\n",TestStandard_Arrary[Current_event].Over_Voltage_Protection);
//	rt_kprintf("Over_Current_Protection=%d\r\n",TestStandard_Arrary[Current_event].Over_Current_Protection);
//	rt_kprintf("Quick_Charge=%d\r\n",TestStandard_Arrary[Current_event].Quick_Charge);
//	HMI_TestLimit_Atoi();
//	rt_kprintf("res=%x\r\n",HMI_TestLimit);
	while(1)
	{
		//HMI_Print_Str("t0","123");
		rt_thread_delay(1000);
	}
}
  
//线程LED1
void led1_thread_entry(void* parameter)
{
//		u32 total,free;
//		FIL fsrc;	  		//文件1
//		u8 res;
//		char str[20];
//	
//		TestData_Type Temp_struct;
//	
//		Temp_struct.Test_Time[0]=18;
//		Temp_struct.Test_Time[1]=1;
//		Temp_struct.Test_Time[2]=22;
//		Temp_struct.Test_Time[3]=13;
//		Temp_struct.Test_Time[4]=55;
//		Temp_struct.Test_Time[5]=18;
//		Temp_struct.Ripple_Voltage=200;
//		Temp_struct.Vout_Max=200;
//		Temp_struct.Cout_Max=150;
//		Temp_struct.Over_Current_Protection=1;
//		Temp_struct.Over_Voltage_Protection=1;
//		Temp_struct.Poweron_Time=20;
//		Temp_struct.Quick_Charge=3;
//		Temp_struct.Short_Current=1;
//		Temp_struct.Efficiency=67;
//		Temp_struct.Test_Subsequence=9;
		
//		printf("OK");
//		exfuns_init();
//		if(f_mount(fs[0],"0:",1)) printf("挂载失败");
//		printf("挂载成功\r\n");
//		if(exf_getfree("0:",&total,&free)) printf("获取错误");
//		printf("total=%dMB\r\nfree=%dMB\r\n",total>>10,free>>10);
//		
//		///my_itoa(10,str);
//		rt_kprintf("\r\nOK");
//		printf("rse=%d",res);
	while(1)
	{
		LED1=~LED1; 
		delay_ms(10);
		delay_us(50);
		rt_thread_delay(30);
	}
}

//利用空闲任务钩子计算CPU利用率
void cpu_usage_idle_hook(void)
{
    rt_tick_t tick;
    rt_uint32_t count;
    volatile rt_uint32_t loop;

    if (total_count == 0)
    {
        rt_enter_critical();
			
		/* get total count */
        tick = rt_tick_get();
        while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
        {
            total_count ++;
            loop = 0;

            while (loop < CPU_USAGE_LOOP) loop ++;
        }
				
        rt_exit_critical();
    }

    count = 0;
    /* get CPU usage */
    tick = rt_tick_get();
	
    while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
        count ++;
        loop  = 0;
        while (loop < CPU_USAGE_LOOP) loop ++;
    }

    /* calculate major and minor */
    if (count < total_count)
    {
        count = total_count - count;
        cpu_usage_major = (count * 100) / total_count;
        cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
			
    }
    else
    {
        total_count = count;

        /* no CPU usage */
        cpu_usage_major = 0;
        cpu_usage_minor = 0;
    }
}

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor)
{
    RT_ASSERT(major != RT_NULL);
    RT_ASSERT(minor != RT_NULL);

    *major = cpu_usage_major;
    *minor = cpu_usage_minor;
}


