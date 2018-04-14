/**
  ******************************************************************************
  * @file    STM32F407DEMO/app.h 
  * @author  Liang
  * @version V1.0.0
  * @date    2018-1-22
  * @brief   
  ******************************************************************************
  * @attention
	* �����rt-thread��֧��
	*	����rt-thread msh�Ĵ�������
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
rt_uint8_t Standard_val; //���Ա�׼���
TestParameters_Type TestParameters_Structure;	//����ָ���Žṹ��,Ӧ�����޸�ָ��Ͳ���ʱ����ָ��


void Master_thread_entry(void* parameter)
{
	u8 res;
	while(1)
	{
		/* �ȴ�HMI���ڼ���̵߳Ĵ����ʼ� */
		if(rt_mb_recv(&HMI_Response_mb,(rt_uint32_t*)HMI_Info,RT_WAITING_FOREVER)==RT_EOK)
		{
			switch(HMI_Info[0])
			{
				case 0x01 : 
				{
					//�����߳�1 
					CollectData_thread = rt_thread_create("GetData",CollectData_thread_entry, RT_NULL,512, 2,20);
					//�����߳�1 
					HMI_FastTest_thread = rt_thread_create("HMI_1",HMI_FastTest_thread_entry, RT_NULL,512,3,20);
					
					HMI_File_Page(20);	//��ת�����ٲ��Խ���
					rt_thread_startup(CollectData_thread);	//�������ݲɼ��߳�
					rt_thread_startup(HMI_FastTest_thread);	//�������ٽ������߳� 
				}break;
				case 0x02 : 
				{
					rt_thread_delete(CollectData_thread);	//ɾ�����ݲɼ��߳�
					rt_thread_delete(HMI_FastTest_thread);	//ɾ�����ٽ������߳�
					HMI_File_Page(1); 
				}break;
				case 0x03 : 
				{
					Entry_Code_Old=0x03;
					//�����߳�1 
					HMI_SelectBatch_thread = rt_thread_create("HMI_Batch",HMI_SelectBatch_thread_entry, RT_NULL,512,3,20);					
					rt_enter_critical();	//�����ٽ���
					f_mount(&fat,"0:",1);	//���ع�����
					First_writeTestParameters();
					rt_exit_critical();		//�˳��ٽ���
					HMI_File_Page(8);	//��ת���б���ʾ����
					rt_thread_startup(HMI_SelectBatch_thread);	//�����߳�
				}break;
				case 0x04 :
				{
					Entry_Code_Old=0x04;
					//����ģʽѡ���߳�
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					rt_thread_delete(HMI_SelectBatch_thread);	//ɾ������ѡ���߳�
					rt_enter_critical();	//�����ٽ���
					f_mount(NULL,"0:",1);	//���ع�����
					rt_exit_critical();		//�˳��ٽ���
					HMI_File_Page(25);	//��ת�����Ա�׼ѡ�����
					rt_thread_startup(HMI_SelectStandard_thread);	//�����߳�
				}break;
				case 0x05 :
				{	
					rt_thread_delete(HMI_SelectBatch_thread);	//ɾ������ѡ���߳�
					rt_enter_critical();	//�����ٽ���
					f_mount(NULL,"0:",1);	//���ع�����
					rt_exit_critical();		//�˳��ٽ���
					HMI_File_Page(1);	//��ת�����������
				}break;
				case 0x06 :  break;
				case 0x07 : 
				{
					Entry_Code_Old=0x07;
					//����ģʽѡ���߳�
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					/* ��ȡ����Ŀ¼���Ʋ�����д��SD���� */
					
					HMI_File_Page(25);	//��ת�����Ա�׼ѡ�����
					rt_thread_startup(HMI_SelectStandard_thread);	//�����߳�
				}break;
				case 0x08 : 
				{
					//�����б�鿴�߳�
					HMI_ShowBatchList_thread = rt_thread_create("Standard",HMI_ShowBatchList_thread_entry, RT_NULL,512,3,20);
					HMI_File_Page(8);	//��ת���б���ʾ����
					rt_thread_startup(HMI_ShowBatchList_thread);	//�����߳�
				}break;
				case 0x09 : 
				{
					rt_thread_delete(HMI_ShowBatchList_thread);	//ɾ�������б��߳�
					HMI_File_Page(1);	//��ת���б���ʾ����
				}break;
				case 0x0a : 
				{
					Entry_Code_Old=0x0a;
					//����ģʽѡ���߳�
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);					
					HMI_File_Page(14);	//��ת�����Ա�׼ѡ�����
					rt_thread_startup(HMI_SelectStandard_thread);	//�����߳�					
				}break;
				case 0x0b : 
				{
					rt_thread_delete(HMI_SelectStandard_thread);
					HMI_File_Page(10);				//��ת�����Ա�׼���ý���
					HMI_StandardPage_Show();	//��ʾ��ǰģʽ���Ա�׼
				}break;
				case 0x0c : 
				{
					/* ��ֹ�����Է�ֹ���Ŵ��ڽ��� */
					rt_enter_critical();	//�����ٽ���
					res=HMI_Standard_Atoi();	//��ȡ�����׼������װ�ؽ��ṹ��
					rt_exit_critical();		//�˳��ٽ���
					#if	Thread_Debug
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Vout_Max);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Cout_Max);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.V_Ripple);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Poweron_Time);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Efficiency);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Safety_Code);
					rt_kprintf("res=%d\r\n",TestParameters_Structure.Quick_Charge);
					#endif
					HMI_File_Page(1);	//��ת��������
				}break;
				case 0x0d	:
				{
					HMI_File_Page(12);	//��ת�����ؽ���
					HMI_TestLimit_Itoa();
				}break;
				case 0x0e	:
				{
					/* ��ֹ�����Է�ֹ���Ŵ��ڽ��� */
					rt_enter_critical();	//�����ٽ���
					HMI_TestLimit_Atoi(&HMI_TestLimit);
					rt_exit_critical();		//�˳��ٽ���
					#if	Thread_Debug
					rt_kprintf("HMI_TestLimit=%d\r\n",HMI_TestLimit);
					#endif
					HMI_File_Page(1);	//��ת��������
				}break;
				case 0x0f	:
				{
					HMI_File_Page(17);	//��ת��ʱ�����
					HMI_RTC_Show();
				}break;
				case 0x10	:
				{
					
				}break;
				/* ���水����������(ͨ��) */
				case 0x51 :{Event_Flag=1;rt_mb_send(Event_mb,Event_Flag);}break;	//�Ϸ�
				case 0x52 :{Event_Flag=2;rt_mb_send(Event_mb,Event_Flag);}break;	//�·�
				case 0x53 :{Event_Flag=3;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ1
				case 0x54 :{Event_Flag=4;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ2
				case 0x55 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ3
				case 0x56 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ4
				case 0x57 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ5
				case 0x58 :{Event_Flag=5;rt_mb_send(Event_mb,Event_Flag);}break;	//��Ŀ6
				default:break;
			}
			HMI_Info[0]=0;
		}
	}
}


/*
 * HMI�������� ����״̬����HMI���� һ�����ض�ҳ���������������̣�����hmi���ڽ��������̿���
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
 * ���ݲɼ��߳�
 */
void CollectData_thread_entry(void* parameter)
{
	while (1)
	{
		rt_enter_critical();	//�����ٽ���
		ReadTimeData_structure.V_OUT=Get_PowerVoltage();	//�ɼ���ѹ
		ReadTimeData_structure.C_OUT=Get_PowerCurrent();	//�ɼ�����
		ReadTimeData_structure.V_Ripple=Get_PowerRipple();	//�ɼ��Ʋ���ѹ
		rt_exit_critical();		//�˳��ٽ���
		rt_mb_send(GetData_mb,(rt_uint32_t)&ReadTimeData_structure);
		rt_thread_delay(100);
	}
}

/*
 * ���ټ��HMI�����߳�
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
 * �����б���ʾ����
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
			rt_enter_critical(); /* �����ٽ���*/
			/* �Ϸ� */
			if(Event_Flag==1)
			{
				Page_Flag=Page_Flag-1;
				if(Page_Flag<0) Page_Flag=0;
			}
			/* �·� */
			if(Event_Flag==2)
			{
				if(count==5)
					Page_Flag=Page_Flag+1;
			}
			/* ��ȡ����Ŀ¼��Ϣ */
			count=Scan_BatchDir(Page_Flag,Page_Flag+4);
			rt_exit_critical(); /* �˳��ٽ���*/
			HMI_Print_Str("t0",Global_str[0]);
			HMI_Print_Str("t1",Global_str[1]);
			HMI_Print_Str("t2",Global_str[2]);
			HMI_Print_Str("t3",Global_str[3]);
			HMI_Print_Str("t4",Global_str[4]);
		}
	}
}

/*
 * �����б�ѡ�����
 */
void HMI_SelectBatch_thread_entry(void* parameter)
{
	int Page_Flag=0;
	u8 count=0;
	
	/* ��ʾĿ¼ */
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
		/* ���հ���ʱ�� */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			switch(Event_Flag)
			{
				/* �Ϸ� */
				case 1:
				{
					Page_Flag=Page_Flag-1;
					if(Page_Flag<0) Page_Flag=0;
				}break;
				/* �·� */
				case 2:
				{
					if(count==5)
						Page_Flag=Page_Flag+1;
				}break;
				/* ѡ����Ŀ1 */
				case 3:
				{
					Batch_val=Page_Flag;
					HMI_Info[0]=4;
					strcpy(Batch_str,Global_str[0]);
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ2 */
				case 4:
				{
					Batch_val=Page_Flag+1;
					strcpy(Batch_str,Global_str[1]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ3 */
				case 5:
				{
					Batch_val=Page_Flag+2;
					strcpy(Batch_str,Global_str[2]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ4 */
				case 6:
				{
					Batch_val=Page_Flag+3;
					strcpy(Batch_str,Global_str[3]);
					HMI_Info[0]=4;
					rt_mb_send(&HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ5 */
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
			rt_enter_critical(); /* �����ٽ���*/
			/* ��ȡ����Ŀ¼��Ϣ */
			count=Scan_BatchDir(Page_Flag,Page_Flag+4);
			rt_exit_critical(); /* �˳��ٽ���*/
			HMI_Print_Str("t0",Global_str[0]);
			HMI_Print_Str("t1",Global_str[1]);
			HMI_Print_Str("t2",Global_str[2]);
			HMI_Print_Str("t3",Global_str[3]);
			HMI_Print_Str("t4",Global_str[4]);
		}
	}
}

/*
 * ���Ա�׼ѡ�����
 */
void HMI_SelectStandard_thread_entry(void* parameter)
{
	while(1)
	{
		/* ��ȡ���Ա�׼��� */
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
		/*���������沿��*/
		if(Entry_Code_Old==0x04||Entry_Code_Old==0x07)
		{
			rt_thread_delay(100);
			HMI_File_Page(19);
		}
		/*���Ա�׼���ò���*/
		else
		{
			HMI_Info[0]=0x0b;
			rt_mb_send(&HMI_Response_mb,HMI_Info[0]);	
		}
	}
}


  
//�߳�LED1
void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		LED1 =~LED1;
		rt_thread_delay(1000);
	}
}

/*
 * ���������߳� ϵͳ����ָʾ����˸
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
	
	/* ��ʼ��mailbox */
	rt_mb_init(&HMI_Response_mb,"HMI_mb", /* ������HMI_mb */
			   &mb_pool[0],
				 sizeof(mb_pool)/4, /* ÿ���ʼ��Ĵ�С��4�ֽ� */
			   RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ� */

	GetData_mb=rt_mb_create("Data_mb", /* ������HMI_mb */
			   32, /* ÿ���ʼ��Ĵ�С��4�ֽ� */
			   RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ� */
	if(GetData_mb==RT_NULL)
		rt_kprintf("create mb failed \r\n");
	
		Event_mb=rt_mb_create("Event_mb",32,RT_IPC_FLAG_FIFO);
	if(Event_mb==RT_NULL) rt_kprintf("create mb failed \r\n");
	
  delay_ms(800);
 	HMI_File_Page(3);		 
				 
				 
				   
		//�����߳�1
   led1_thread = rt_thread_create("led1", //�߳�1��������t1 
							led1_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							3, //�߳����ȼ�
							20);//ʱ��Ƭtick

	if (led1_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(led1_thread); 


	//�����߳�1 
    HMIMonitor_thread = rt_thread_create("HMIMonitor", //�߳�1��������t1 
							HMIMonitor_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							1, //�߳����ȼ�
							20);//ʱ��Ƭtick

	if (HMIMonitor_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(HMIMonitor_thread); 
	
	//�����߳�1 
    Master_thread = rt_thread_create("Master", //�߳�1��������t1 
							Master_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							3, //�߳����ȼ�
							20);//ʱ��Ƭtick

	if (Master_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(Master_thread); 
		
}


