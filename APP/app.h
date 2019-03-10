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
	* �����rt-thread��֧��
	*	����rt-thread msh�Ĵ�������
  ******************************************************************************
**/

#include <rtthread.h>
#include "CSV_Database.h"
#include "Data_Math.h"

#define Thread_Debug 1

/* ����ʹ�õ���ȫ�ֱ������� */
extern char Global_str[30][20];		//�ַ�����ά��������
extern rt_uint8_t Standard_val; 	//���Ա�׼���
extern TestParameters_Type TestParameters_Structure[4];	//����ָ���Žṹ��,Ӧ�����޸�ָ��Ͳ���ʱ����ָ��
static ReadTimeData_Type Global_ReadTimeData_structure;			//ʵʱ���ݴ洢�ṹ��
static MeasuredData_Type Global_Measured_Structure;	//ʵʱ������ݽṹ��
static char HMI_Info[100];				//HMI��Ϣ��������
static rt_int16_t SysError_Code;	//ϵͳ�����������
static char Batch_str[10];				//������������
static rt_uint8_t Entry_Code_Old;	//������һ��ִ�еĽ��̴�����ʵ���̸߳���
static rt_uint8_t Current_HMIEvent;	//��¼��ǰ��HMI�¼�
static rt_uint8_t Batch_val;			//�������

/* ������ƿ� */
static rt_mailbox_t HMI_Response_mb=RT_NULL;		//HMI������Ӧ����
static rt_mailbox_t GetData_mb=RT_NULL;					//���ݲɼ�����
static rt_mailbox_t Event_mb=RT_NULL;						//������������

/* �����ʱ�����ƿ� */
static rt_timer_t GetData_timer=RT_NULL;				//���ݲɼ����ڶ�ʱ��
static struct rt_timer timer2;

/* ���ڷ��ʼ����ڴ�� */
static char mb_pool[32];
static rt_uint8_t Event_Flag;


/* �߳̿��ƿ� */
static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;
static rt_thread_t CollectData_thread=RT_NULL;
static rt_thread_t HMI_FastTest_thread=RT_NULL;
static rt_thread_t HMI_SelectBatch_thread=RT_NULL;
static rt_thread_t HMI_SelectStandard_thread=RT_NULL;
static rt_thread_t EventProcessing_thread=RT_NULL;

/* �̺߳��� */
void led1_thread_entry(void* parameter);					//LED��˸�߳�
void HMIMonitor_thread_entry(void* parameter);		//HMIϵͳ��Ϣ��ؽ���
void Master_thread_entry(void* parameter);				//�߳̿������߳�
void CollectData_thread_entry(void* parameter);		//ʵʱ���ݲɼ�����
void HMI_FastTest_thread_entry(void* parameter);	//���ټ��HMI�����߳�
void HMI_SelectBatch_thread_entry(void* parameter);	//����ѡ���߳�
void HMI_SelectStandard_thread_entry(void* parameter); //���Ա�׼ѡ���߳�
void EventProcessing_thread_entry(void* parameter);			//���潻���¼������߳�
void cpu_usage_idle_hook(void);
void Main_entry(void);
/* �ص����� */
void GetData_timerout(void* parameter);		//���ݲɼ������ʱ���ص�����



#endif

