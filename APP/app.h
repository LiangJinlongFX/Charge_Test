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

/* ����ʹ�õ���ȫ�ֱ������� */
static ReadTimeData_Type ReadTimeData_structure;	//ʵʱ���ݴ洢�ṹ��
static char Global_str[10][10];	//�ַ�����ά��������
static char HMI_Info[100];	//HMI��Ϣ��������
static TestParameters_Type TestParameters_Structure;	//����ָ���Žṹ��,Ӧ�����޸�ָ��Ͳ���ʱ����ָ��
static rt_int16_t SysError_Code;	//ϵͳ�����������
static char Batch_str[10];	//������������
static rt_uint8_t Entry_Code_Old;	//������һ��ִ�еĽ��̴�����ʵ���̸߳���

/* ������ƿ� */
static struct rt_mailbox HMI_Response_mb;		//HMI������Ӧ����
static rt_mailbox_t GetData_mb=RT_NULL;					//���ݲɼ�����
static rt_mailbox_t Event_mb=RT_NULL;						//������������

/* ���ڷ��ʼ����ڴ�� */
static char mb_pool[32];
static rt_uint8_t Event_Flag;

static rt_uint8_t Batch_val;	//�������
static rt_uint8_t Standard_val; //���Ա�׼���

static rt_thread_t led1_thread=RT_NULL;
static rt_thread_t usb_thread=RT_NULL;
static rt_thread_t HMIMonitor_thread=RT_NULL;
static rt_thread_t Master_thread=RT_NULL;
static rt_thread_t CollectData_thread=RT_NULL;
static rt_thread_t HMI_FastTest_thread=RT_NULL;
static rt_thread_t HMI_ShowBatchList_thread=RT_NULL;
static rt_thread_t HMI_SelectBatch_thread=RT_NULL;
static rt_thread_t HMI_SelectStandard_thread=RT_NULL;	

void usb_thread_entry(void* parameter);						//USB�߳�
void led1_thread_entry(void* parameter);					//LED��˸�߳�
void HMIMonitor_thread_entry(void* parameter);		//HMIϵͳ��Ϣ��ؽ���
void Master_thread_entry(void* parameter);				//�߳̿������߳�
void CollectData_thread_entry(void* parameter);		//ʵʱ���ݲɼ�����
void HMI_FastTest_thread_entry(void* parameter);	//���ټ��HMI�����߳�
void HMI_ShowBatchList_thread_entry(void* parameter); //�����б���ʾ�����߳�
void HMI_SelectBatch_thread_entry(void* parameter);	//����ѡ���߳�
void HMI_SelectStandard_thread_entry(void* parameter); //���Ա�׼ѡ���߳�
void cpu_usage_idle_hook(void);
void Main_entry(void);



#endif

