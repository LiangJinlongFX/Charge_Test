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
rt_uint8_t Standard_val; //���Ա�׼���
TestParameters_Type TestParameters_Structure[4];	//����ָ���Žṹ��,Ӧ�����޸�ָ��Ͳ���ʱ����ָ��

/**
 * HMI�������߳�
 * @param
 * @return
 * ���ڳ�ʼ���Լ��������̵߳Ŀ���
 */
void Master_thread_entry(void* parameter)
{
	u8 res;	
	
	/* Run Forever */
	while(1)
	{
		/* �ȴ�HMI���ڼ���̵߳Ĵ����ʼ� */
		if(rt_mb_recv(HMI_Response_mb,(rt_uint32_t*)HMI_Info,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("case %d",HMI_Info[0]);
			switch(HMI_Info[0])
			{
				/* ������ټ�⽻������ */
				case 0x01 : 
				{
					/* �������ٲ��Խ�����ʾ�߳� */
					HMI_FastTest_thread = rt_thread_create("HMI_1",HMI_FastTest_thread_entry, RT_NULL,1024,2,20);
					/* ��ת�����ٲ��Խ��� */
					HMI_File_Page(Page_fast_testing);
					/* ʹ�ܴ���3�ж���Ӧ���� */
					USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
					USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
					Current_HMIEvent = 0x01;
					rt_thread_startup(HMI_FastTest_thread);		//�������ٽ������߳�
				}break;
				
				/* �˳����ټ�⽻������ */
				case 0x02 : 
				{
					/* �رմ���3�ж���Ӧ���� */
					USART_ITConfig(USART3,USART_IT_IDLE,DISABLE);
					USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
					rt_thread_delete(HMI_FastTest_thread);	//ɾ�����ٽ������߳�
					HMI_File_Page(Page_main); 							//��ת��������
				}break;
				
				/* ��������ѡ�񽻻����� */
				case 0x03 : 
				{
					Entry_Code_Old=0x03;
					/* �����̣߳������б��߳� */
					HMI_SelectBatch_thread = rt_thread_create("HMI_Batch",HMI_SelectBatch_thread_entry, RT_NULL,1024,3,20);					
					HMI_File_Page(8);			//��ת���б���ʾ����
					rt_thread_startup(HMI_SelectBatch_thread);	//�����߳�
				}break;
				
				/* ѡ�����������������׼ѡ���߳� */
				case 0x04 :
				{
					Entry_Code_Old=0x04;
					//����ģʽѡ���߳�
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);
					rt_thread_delete(HMI_SelectBatch_thread);	//ɾ������ѡ���߳�
					HMI_File_Page(Page_test_selstand);	//��ת�����Ա�׼ѡ�����
					rt_thread_startup(HMI_SelectStandard_thread);	//�����߳�
				}break;
				/* ��������ѡ��ȡ��ѡ�� */
				case 0x05 :
				{	
					rt_thread_delete(HMI_SelectBatch_thread);	//ɾ������ѡ���߳�
					HMI_File_Page(Page_main);	//��ת�����������
				}break;
				/* ѡ��������׼����;���� */
				case 0x06 :
				{
					rt_thread_delete(HMI_SelectStandard_thread);	//ɾ������ѡ���߳�
					HMI_File_Page(Page_main);	//��ת�����������
				}break;
				/* ������������������д��SD�� */
				case 0x07 : 
				{
					Entry_Code_Old=0x07;
					/* ��ȡ����Ŀ¼���Ʋ�����д��SD���� */
					f_mount(&fat,"0",1);
					HMI_Creat_NewBatch();
					f_mount(NULL,"0",1);
					HMI_File_Page(Page_main);	//��ת�����Ա�׼ѡ�����
				}break;
				/* ��׼���ý�����ѡ���׼��� */
				case 0x0a : 
				{
					Entry_Code_Old=0x0a;
					//����ģʽѡ���߳�
					HMI_SelectStandard_thread = rt_thread_create("Standard",HMI_SelectStandard_thread_entry, RT_NULL,512,3,20);					
					HMI_File_Page(Page_standard_sel);	//��ת�����ò��Ա�׼ѡ�����
					rt_thread_startup(HMI_SelectStandard_thread);	//�����߳�					
				}break;
				/* ��׼���ý�����ѡ���׼����¼����� */
				case 0x0b : 
				{
					rt_thread_delete(HMI_SelectStandard_thread);
					HMI_File_Page(Page_standard);		//��ת�����Ա�׼���ý���
					#if	Thread_Debug
					logging_debug("Standard_val: %d",Standard_val);
					logging_debug("Vout=%d",TestParameters_Structure[Standard_val].Vout);
					logging_debug("Vout_Tolerance=%d",TestParameters_Structure[Standard_val].Vout_Tolerance);
					logging_debug("Cout_Max=%d",TestParameters_Structure[Standard_val].Cout_Max);
					logging_debug("V_Ripple=%d",TestParameters_Structure[Standard_val].V_Ripple);
					logging_debug("Efficiency=%d",TestParameters_Structure[Standard_val].Efficiency);
					logging_debug("Safety_Code=%d",TestParameters_Structure[Standard_val].Safety_Code);
					logging_debug("Quick_Charge=%d",TestParameters_Structure[Standard_val].Quick_Charge);	
					HMI_StandardPage_Show(&TestParameters_Structure[Standard_val]);	//��ʾ��ǰģʽ���Ա�׼
					#endif
				}break;
				/* ��׼���ý��������ñ�׼ */
				case 0x0c : 
				{
					/* ��ֹ�����Է�ֹ���Ŵ��ڽ��� */
					res=HMI_Standard_Atoi();	//��ȡ�����׼������װ�ؽ��ṹ��
					rt_enter_critical();	//�����ٽ���
					f_mount(&fat,"0:",1);	//���ع�����
					if(Modify_TestParameters(&TestParameters_Structure[Standard_val],Standard_val)) rt_kprintf("WriteData_Error!\r\n");
					f_mount(NULL,"0:",1);	//ע��������
					rt_exit_critical();		//�˳��ٽ���
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
					HMI_File_Page(Page_main);	//��ת��������
				}break;
				/* ���Կ������ý������ */
				case 0x0d	:
				{
					HMI_File_Page(12);	//��ת�����ؽ���
					HMI_TestLimit_Itoa();
				}break;
				/* ���Կ��������˳����� */
				case 0x0e	:
				{
					/* ��ֹ�����Է�ֹ���Ŵ��ڽ��� */
					rt_enter_critical();	//�����ٽ���
					HMI_TestLimit_Atoi(&HMI_TestLimit);
					rt_exit_critical();		//�˳��ٽ���
					HMI_File_Page(1);	//��ת��������
				}break;
				/* ����ʱ�����ý��� */
				case 0x0f	:
				{
					HMI_File_Page(17);	//��ת��ʱ�����
					HMI_RTC_Show();
				}break;
				/* �˳�ʱ�����ý��� */
				case 0x10	:
				{
					HMI_RTC_Atoi();
					HMI_File_Page(1);
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

/**
 * HMI�¼������߳�
 * @param
 * @return
 * ����״̬����HMI���� һ�����ض�ҳ���������������̣�����hmi���ڽ��������̿���
 */
void HMIMonitor_thread_entry(void* parameter)
{
	while(1)
	{
		/* �ȴ����յ�HMIϵͳ�¼���Ϣ */
		if(!USART_Solution(HMI_System_Type,HMI_Info)) 
		{
			/* ���յ�HMIϵͳ�¼���Ϣ,��������֪ͨ����߳� */
			rt_mb_send(HMI_Response_mb,HMI_Info[0]);
		}
		LED2=~LED2;
		rt_thread_delay(100);
	}
}

/**
 * ������ݲɼ��ص�����
 * @param
 * @return
 * �ɼ�USB�˿ڵĵ�Դ��ѹ/����/�Ʋ���ѹ
 * ʹ�������ʱ���ص�����
 */
void GetData_timerout(void* parameter)
{
	rt_enter_critical();	//�����ٽ���
	Get_Power_Val_All(&Global_Measured_Structure);
	rt_exit_critical();		//�˳��ٽ���
	/* �������ݵ�ַ������߳� */
	rt_mb_send(GetData_mb,(rt_uint32_t)&Global_ReadTimeData_structure);
}


/**
 * HMI���������¼������߳�
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
		/* �������ⰴ���¼����� */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("Receive Interface Event:%d",Event_Flag);
			if(Current_HMIEvent == 0x01)
			{
				/* ���յ�����յ��¼� */
				if(Event_Flag==1)
				{
					logging_debug("Detected QuickCharge...");
					/* �����п��Э������յ� */
					rt_enter_critical();	//�����ٽ���
					res=QuickCharge_Induction(0x0f);
					sprintf(str,"QC: %x",res);
					HMI_Print_Str("t8",str);
					rt_exit_critical();		//�˳��ٽ���
				}
				else if(Event_Flag==2)
				{
					logging_debug("Start OCP TEST...");
					/* ���й����������� */
					rt_enter_critical();	//�����ٽ���
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
					rt_exit_critical();		//�˳��ٽ���				
				}
			}
		}
		rt_thread_delay(500);
	}	
}



/**
 * ���ټ�������ʾ����
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
		HMI_Print_Str("t6",str);	//��ʾAC�����ѹ
		str[0]='\0';
		sprintf(str,"%.3f",Global_Measured_Structure.Measured_AC_Current);
		HMI_Print_Str("t7",str);	//��ʾAC�������
		str[0]='\0';
		sprintf(str,"%.3f",(Global_Measured_Structure.Measured_USB_Voltage)/1000);
		HMI_Print_Str("t9",str);	//��ʾDC�����ѹ
		str[0]='\0';
		sprintf(str,"%.3f",Global_Measured_Structure.Measured_AC_Current);
		HMI_Print_Str("t10",str);	//��ʾDC�������
		str[0]='\0';
		HMI_Print_Str("t11",str);	//��ʾת��Ч��
		str[0]='\0';
		sprintf(str,"%.3f",0.00f);
		HMI_Print_Str("t12",str);	//��ʾ�Ʋ���ѹ
		str[0]='\0';
		rt_thread_delay(200);
	}
}

/**
 * ����ѡ���б��߳�
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
	/* ��ʾĿ¼ */
	rt_enter_critical();	//�����ٽ���
	f_mount(&fat,"0:",1);	//���ع�����
	count=Scan_BatchDir(0,29,Str);
	f_mount(NULL,"0:",1);	//ע��������
	rt_exit_critical();		//�˳��ٽ���
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
		/* ���հ���ʱ�� */
		if(rt_mb_recv(Event_mb,(rt_uint32_t*)&Event_Flag,RT_WAITING_FOREVER)==RT_EOK)
		{
			logging_debug("Receive button! %d",Event_Flag);
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
					if((Page_Flag+1)*5<count)
						Page_Flag=Page_Flag+1;
				}break;
				/* ѡ����Ŀ1 */
				case 3:
				{
					Batch_val=Page_Flag;
					HMI_Info[0]=4;
					strcpy(Batch_str,Global_str[0]);
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ2 */
				case 4:
				{
					Batch_val=Page_Flag+1;
					strcpy(Batch_str,Global_str[1]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ3 */
				case 5:
				{
					Batch_val=Page_Flag+2;
					strcpy(Batch_str,Global_str[2]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ4 */
				case 6:
				{
					Batch_val=Page_Flag+3;
					strcpy(Batch_str,Global_str[3]);
					HMI_Info[0]=4;
					rt_mb_send(HMI_Response_mb,HMI_Info[0]);
					rt_thread_delay(1000);
				}break;
				/* ѡ����Ŀ5 */
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
				case 1: Standard_val=0;break;
				case 2: Standard_val=1;break;
				case 3: Standard_val=2;break;
				case 4: Standard_val=3;break;
				default:break;
			}
		}
		/*���������潻������*/
		if(Entry_Code_Old==0x04||Entry_Code_Old==0x07)
		{
			rt_thread_delay(100);
			HMI_File_Page(19);
		}
		/*���Ա�׼���ý�������*/
		else
		{
			HMI_Info[0]=0x0b;
			rt_mb_send(HMI_Response_mb,HMI_Info[0]);
		}
	}
}


  
/**
 * ϵͳָʾ����˸�߳�
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
 * APP���������
 * @param   
 * @return 
 * @brief 
 **/
void Main_entry(void)
{

	//Get_Measured_Offset();
	
	/* ��ȡ�洢���ڵ����ò��� */
	if(Poweron_ReadTestParameters())
		logging_error("Read set.dat ERROR!");
	else
		logging_debug("Read set.dat OK!");
	
	
	/* ����mailbox */
	HMI_Response_mb=rt_mb_create("HMI_mb", /* ������HMI_mb */
			   32, /* ÿ���ʼ��Ĵ�С��4�ֽ� */
			   RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ� */
	GetData_mb=rt_mb_create("Data_mb", /* ������HMI_mb */
			   32, /* ÿ���ʼ��Ĵ�С��4�ֽ� */
			   RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ� */			 
	Event_mb=rt_mb_create("Event_mb", /* ������HMI_mb */
			   32, /* ÿ���ʼ��Ĵ�С��4�ֽ� */
			   RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ� */
	
	/* ��鴴�������Ƿ�ɹ� */
	if(HMI_Response_mb==RT_NULL)
		logging_error("create GetData_mb failed \r\n");
	if(GetData_mb==RT_NULL)
		logging_error("create GetData_mb failed \r\n");	
	if(Event_mb==RT_NULL)
		logging_error("create Event_mb failed \r\n");
	
	/* create timer*/
	GetData_timer = rt_timer_create("timer1", /* ��ʱ��������timer1 */
														GetData_timerout, /* ��ʱʱ�ص��Ĵ�����*/
														RT_NULL, /* ��ʱ��������ڲ���*/
														500, /* ��ʱ���ȣ���OS TickΪ��λ����10��OS Tick */
														RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ��*/
	if(GetData_timer==RT_NULL)
		logging_error("create GetData_timer failed \r\n");
	
  /* ��ʱһ��ʱ��,�ȴ�HMI��Ļ��ʼ����� */
	delay_ms(800);
	/* ��ת������ҳ */
	logging_debug("Flip Page_Init");
 	HMI_File_Page(Page_Init);		 
				
	/* ���� led1��˸ �߳� */
	logging_debug("create thread sys_led");
  led_thread = rt_thread_create("led", //�߳�1��������t1 
							led_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							3, //�߳����ȼ�
							20);//ʱ��Ƭtick
	if (led_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(led_thread);
	else
		logging_error("create thread led1 error!");

	/* ���� HMI���ڼ�� �߳� */
	logging_debug("create thread HMIMonitor");
  HMIMonitor_thread = rt_thread_create("HMIMonitor", //�߳�1��������t1 
							HMIMonitor_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							1, //�߳����ȼ�
							20);//ʱ��Ƭtick
	if (HMIMonitor_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(HMIMonitor_thread);
	else
		logging_error("create thread HMIMonitor error!");
	
  /* ���� �����¼���� �߳� */
	logging_debug("create thread Processing");
  EventProcessing_thread = rt_thread_create("Process", //�߳�1��������t1 
							EventProcessing_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							512, //�̶߳�ջ��С
							1, //�߳����ȼ�
							20);//ʱ��Ƭtick
	if (EventProcessing_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
			rt_thread_startup(EventProcessing_thread);
	else
		logging_error("create thread HMIMonitor error!");
	
	/* ���� ������ �߳� */
	logging_debug("create thread Master");
  Master_thread = rt_thread_create("Master", //�߳�1��������t1 
							Master_thread_entry, RT_NULL, //�����thread1_entry��������RT_NULL 
							1024, //�̶߳�ջ��С
							3, //�߳����ȼ�
							20);//ʱ��Ƭtick
	if (Master_thread != RT_NULL) //�������߳̿��ƿ飬��������߳�
		rt_thread_startup(Master_thread);
	else
		logging_error("create thread Master error!");
		
}


